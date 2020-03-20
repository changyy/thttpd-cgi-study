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

#endif
