#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cgic.h"
#include "my_curl_helper.h"

int cgiMain(int argc, char *argv[]) {
	int ret;
	char tmp;
	struct my_curl_output_result result;
	//cgiHeaderContentType("application/json; charset=utf-8");
	cgiHeaderContentType("text/html; charset=utf-8");
	fprintf(cgiOut, "<html><body><pre>\n");
	fprintf(cgiOut, "== Server Info - Begin ==\n");
	showServerEnvInfo();
	fprintf(cgiOut, "-- Server Info - End --\n");

	fprintf(cgiOut, "\n\nUsing libcurl fire a request: https://dns.google/resolve?name=google.com\n\n");
	// https://developers.google.com/speed/public-dns/docs/doh/json
	if (0 == (ret = get_file_contents("https://dns.google/resolve?name=google.com", &result))) {
		fprintf(cgiOut, "== Response code: %ld ==\n", result.respsone_status_code);
		if (result.header_length > 0) {
			tmp = result.header[result.header_length];
			result.header[result.header_length] = '\0';
			fprintf(cgiOut, "== Response Header - Begin ==\n%s\n-- Response Header - End --\n", result.header);
			result.header[result.header_length] = tmp;
		}
		fprintf(cgiOut, "== Response Body - Begin == \n%s\n-- Response Body - End --\n", result.body);
		//fprintf(cgiOut, "Response:\n%s\n", result.raw);
	} else {
		fprintf(cgiOut, "error: %d", ret);
	}
	fprintf(cgiOut, "</pre></body></html>\n");
	my_curl_release_output(&result);
	return 0;
}
