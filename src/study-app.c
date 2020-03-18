#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cgic.h"
#include "cJSON.h"

void reset_query_string(char ***key_value, int *key_value_number) {
	int i, cnt;
	if (key_value == NULL || (*key_value == NULL)) {
		if (key_value_number != NULL)
			*key_value_number = 0;
		return;
	}
	for ( i = 0, cnt = *key_value_number ; i < cnt ; ++i ) {
		if ((*key_value)[i]) {
			free((*key_value)[i]);
			(*key_value)[i] = NULL;
		}
	}
	if (key_value_number != NULL)
		*key_value_number = 0;
	free(*key_value);
	*key_value = NULL;
}

int parse_query_string(const char *query_string, char ***output_key_value, int *output_key_value_number, int urldecode) {
	int i, key_value_length;
	char *find_end, *find_equal, *decode;
	int query_string_length = 0;
	int detect_key_value_number = 0;
	int decode_length = 0;
	CURL *curl_resource = NULL;
	if (query_string == NULL || (query_string_length = strlen(query_string)) == 0)
		return -1;
	if (output_key_value == NULL || output_key_value_number == NULL)
		return -1;

	reset_query_string(output_key_value, output_key_value_number);
	for (i = 0 ; i < query_string_length ; ++i) {
		if (query_string[i] == '&')
			detect_key_value_number++;
	}
	if (detect_key_value_number == 0 && strchr(query_string, '=') == NULL)
		return 0;
	detect_key_value_number++;

	*output_key_value = (char**)malloc(detect_key_value_number* sizeof(char*));

	if (urldecode == 1) {
		curl_resource = curl_easy_init();
	}

	i = 0; 
	while(i < query_string_length) {
		find_equal = strchr(query_string + i, '=');
		if (find_equal == NULL)
			break;
		find_end = strchr(query_string + i, '&');
		if (find_end == NULL) {
			key_value_length = query_string_length - i;	// query_string_length - 1 -i + 1
		} else {
			key_value_length = query_string_length - strlen(find_end) - i;
		}
		if (key_value_length <= 0)
			break;
		(*output_key_value)[(*output_key_value_number)] = (char*)malloc(sizeof(char)*(key_value_length+1));
		memset((*output_key_value)[(*output_key_value_number)], 0, key_value_length+1);
		memcpy((*output_key_value)[(*output_key_value_number)], query_string + i, key_value_length);
		//(*output_key_value)[(*output_key_value_number)][key_value_length] = '\0';
		if (curl_resource != NULL) {
			decode = curl_easy_unescape(curl_resource, (*output_key_value)[(*output_key_value_number)], key_value_length, &decode_length);
			if (decode && decode_length > 0) {
				if (decode_length > key_value_length) {
					(*output_key_value)[(*output_key_value_number)] = (char*)realloc((*output_key_value)[(*output_key_value_number)], sizeof(char)*(decode_length+1));
					key_value_length = decode_length;
					memset((*output_key_value)[(*output_key_value_number)], 0, key_value_length+1);
				}	
				memcpy((*output_key_value)[(*output_key_value_number)], decode, decode_length);
				(*output_key_value)[(*output_key_value_number)][decode_length] = '\0';
				curl_free(decode);
			}
		}
		(*output_key_value_number)++;
		i += key_value_length + 1;
	}

	if (*output_key_value_number == 0) {
		free(*output_key_value);
		*output_key_value = NULL;
	}

	if (curl_resource != NULL)
		curl_free(curl_resource);

	return *output_key_value_number;
}

int cgiMain(int argc, char *argv[]) {
	char *data;
	cJSON *output, *object;
	char **query_string_key_value_pair = NULL;
	int query_string_key_value_count, i;

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
		parse_query_string(cgiQueryString, &query_string_key_value_pair, &query_string_key_value_count, 1);
		cJSON_AddNumberToObject(output, "params_number", query_string_key_value_count);
		for (i=0 ; i<query_string_key_value_count ; ++i) {
			cJSON_AddItemToArray(object, cJSON_CreateString( query_string_key_value_pair[i] ));
		}
		reset_query_string(&query_string_key_value_pair, &query_string_key_value_count);
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
