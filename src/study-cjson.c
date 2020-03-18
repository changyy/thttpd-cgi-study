#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cgic.h"
#include "cJSON.h"

int cgiMain(int argc, char *argv[]) {
	char *data;
	cJSON *output, *object;

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
	if (NULL == (object = cJSON_AddArrayToObject(output, "data"))) {
		fprintf(cgiOut, "{\"status\":false,\"error\":\"cJSON_AddArrayToObject\"}");
		cJSON_free(output);
		return 0;
	}
	cJSON_AddItemToArray(object, cJSON_CreateString("Hello"));
	cJSON_AddItemToArray(object, cJSON_CreateString("World"));

	// {"status":true,"data":["Hello","World"]}

	//data = cJSON_Print(output);
	data = cJSON_PrintUnformatted(output);
	fprintf(cgiOut, "%s", data);
	free(data);

	//
	// {"status":false,"data":["Hello","World"]}
	//
	//cJSON_DeleteItemFromObject(output, "status");
	//cJSON_AddFalseToObject(output, "status");
	//data = cJSON_PrintUnformatted(output);
	//fprintf(cgiOut, "%s", data);
	//free(data);

	cJSON_free(output);

	return 0;
}
