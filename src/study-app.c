#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cgic.h"
#include "cJSON.h"
#include "my_curl_helper.h"

int cgiMain(int argc, char *argv[]) {
	char *data;
	cJSON *output, *object;
	char **query_string_key_value_pair = NULL;
	size_t query_string_key_value_count, i;

	// https://raw.githubusercontent.com/boutell/cgic/master/cgic.h
	cgiHeaderContentType("application/json; charset=utf-8");

	if (NULL == (output = cJSON_CreateObject())) {
		fprintf(cgiOut, "{\"status\":false,\"error\":\"cJSON_CreateObject\"}");
		return 0;
	}
	// https://github.com/DaveGamble/cJSON/blob/master/cJSON.h
	if (NULL == (cJSON_AddTrueToObject(output, "status"))) {
		fprintf(cgiOut, "{\"status\":false,\"error\":\"cJSON_AddTrueToObject\"}");
		cJSON_free(output);
		return 0;
	} 
	if (NULL == (object = cJSON_AddArrayToObject(output, "params"))) {
		fprintf(cgiOut, "{\"status\":false,\"error\":\"cJSON_AddArrayToObject\"}");
		cJSON_free(output);
		return 0;
	}
	
	if (cgiQueryString != NULL) {
		cJSON_AddStringToObject(output, "query_string", cgiQueryString);
		my_curl_parse_query_string(cgiQueryString, &query_string_key_value_pair, &query_string_key_value_count, 1);
		cJSON_AddNumberToObject(output, "params_number", query_string_key_value_count);
		for (i=0 ; i<query_string_key_value_count ; ++i) {
			cJSON_AddItemToArray(object, cJSON_CreateString( query_string_key_value_pair[i] ));
		}
		my_curl_reset_query_string(&query_string_key_value_pair, &query_string_key_value_count);
	}

	// /cgi-bin/study-app.cgi?a=1&b=123&c=%E6%88%91%E6%98%AF%E8%AA%B0
	// {"status":true,"params":["a=1","b=123","c=我是誰"],"query_string":"a=1&b=123&c=%E6%88%91%E6%98%AF%E8%AA%B0","params_number":3}

	//data = cJSON_Print(output);
	data = cJSON_PrintUnformatted(output);
	fprintf(cgiOut, "%s", data);
	free(data);

	cJSON_free(output);

	return 0;
}
