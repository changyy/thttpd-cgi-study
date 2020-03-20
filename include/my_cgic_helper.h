#ifndef __MY_CGIC_HELPER__
#define __MY_CGIC_HELPER__

#include "cgic.h"

void my_cgic_api_output(const char* data, const char* callback) {
	fprintf(cgiOut, "Cache-Control: no-cache, no-store, must-revalidate, max-age=0\r\n");
	// https://raw.githubusercontent.com/boutell/cgic/master/cgic.h
	//cgiHeaderContentType(callback == NULL ? "application/json; charset=UTF-8" : "application/javascript; charset=UTF-8");
	if (data != NULL) {
		if (callback == NULL) {
			cgiHeaderContentType("application/json; charset=UTF-8");
			fprintf(cgiOut, "%s", data);
		} else {
			cgiHeaderContentType("application/javascript; charset=UTF-8");
			fprintf(cgiOut, "%s(%s)", callback, data);
		}
	} else {
		if (callback == NULL) {
			cgiHeaderContentType("application/json; charset=UTF-8");
			fprintf(cgiOut, "{}");
		} else {	
			cgiHeaderContentType("application/javascript; charset=UTF-8");
			fprintf(cgiOut, "%s({})", callback);
		}
	}
}

void my_cgic_show_http_env() {
	char HTTP_EVNS[][64] = {
		"PATH",
		"LD_LIBRARY_PATH",
		"SERVER_SOFTWARE",
		"SERVER_NAME",
		"GATEWAY_INTERFACE",
		"SERVER_PROTOCOL",
		"SERVER_PORT",
		"REQUEST_METHOD",
		"PATH_INFO",
		"PATH_TRANSLATED",
		"SCRIPT_NAME",
		"QUERY_STRING",
		"REMOTE_ADDR",
		"HTTP_REFERER",
		"HTTP_USER_AGENT",
		"HTTP_ACCEPT",
		"HTTP_ACCEPT_ENCODING",
		"HTTP_ACCEPT_LANGUAGE",
		"HTTP_COOKIE",
		"CONTENT_TYPE",
		"HTTP_HOST",
		"CONTENT_LENGTH",
		"REMOTE_USER",
		"AUTH_TYPE",
		"TZ",
		"CGI_PATTERN",
	};
	int i, total;
	total = sizeof(HTTP_EVNS)/sizeof(HTTP_EVNS[0]);
	for(i = 0; i < total; ++i )
        	printf("%s=%s\n", HTTP_EVNS[i], getenv(HTTP_EVNS[i]));
}

#endif
