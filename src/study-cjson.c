#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cgic.h"
#include "cJSON.h"

int cgiMain(int argc, char *argv[]) {
	char *data;
	cJSON *output;

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
	//data = cJSON_Print(output);
	data = cJSON_PrintUnformatted(output);
	fprintf(cgiOut, "%s", data);

	free(data);
	cJSON_free(output);

	return 0;
}
