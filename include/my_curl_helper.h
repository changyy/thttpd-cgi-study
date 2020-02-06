#ifndef __MY_CURL_HELPER__
#define __MY_CURL_HELPER__

#define MY_CURL_HELPER_STATE_IS_ERROR			-1
#define MY_CURL_HELPER_STATE_IS_OK			0

#define MY_CURL_HELPER_STATE_ERROR_NULL_URL		-2
#define MY_CURL_HELPER_STATE_ERROR_NULL_RESULT		-3
#define MY_CURL_HELPER_STATE_ERROR_INIT_MALLOC		1
#define MY_CURL_HELPER_STATE_ERROR_INIT_CURL		2
#define MY_CURL_HELPER_STATE_ERROR_MEMORY		3
#define MY_CURL_HELPER_STATE_ERROR_HEADER_NOT_FOUND	4
#define MY_CURL_HELPER_STATE_ERROR_UNKNOWN		5

#define MY_CURL_HELPER_MEMORY_STATE_ERROR		-3
#define MY_CURL_HELPER_MEMORY_STATE_RELEASE		-2
#define MY_CURL_HELPER_MEMORY_STATE_INIT		-1
#define MY_CURL_HELPER_MEMORY_STATE_OK			0

// my curl request handler
struct my_curl_output_result {
	char *raw;
	char *header;
	char *body;
	size_t raw_length;
	size_t header_length;
	size_t body_length;
	size_t respsone_status_code;
	size_t memory_status;
};

size_t my_curl_init_output(struct my_curl_output_result *s) {
	s->memory_status = MY_CURL_HELPER_MEMORY_STATE_INIT;
	s->respsone_status_code = 0;
	s->raw_length = 0;
	s->header_length = 0;
	s->body_length = 0;
	s->header = NULL;
	s->body = NULL;
	s->raw = malloc(s->raw_length+1);
	if (s->raw == NULL) {
		fprintf(stderr, "my_curl_init_output malloc() failed\n");
		s->memory_status = MY_CURL_HELPER_STATE_ERROR_INIT_MALLOC;
		return MY_CURL_HELPER_STATE_IS_ERROR;
	}
	s->memory_status = MY_CURL_HELPER_MEMORY_STATE_OK;
	s->raw[0] = '\0';
	return MY_CURL_HELPER_STATE_IS_OK;
}

size_t my_curl_release_output(struct my_curl_output_result *s) {
	if (s == NULL)
		return MY_CURL_HELPER_STATE_IS_ERROR;
	if (s->raw != NULL) {
		free(s->raw);
	}
	s->raw_length = 0;
	s->header_length = 0;
	s->body_length = 0;
	s->raw = NULL;
	s->header = NULL;
	s->body = NULL;
	s->respsone_status_code = 0;
	s->memory_status = MY_CURL_HELPER_MEMORY_STATE_RELEASE;
	return MY_CURL_HELPER_STATE_IS_OK;
}

size_t my_curl_write_func(void *raw, size_t size, size_t nmemb, struct my_curl_output_result *s) {
	size_t new_len = s->raw_length + size*nmemb;
	
	if (s->memory_status != MY_CURL_HELPER_MEMORY_STATE_OK)
		return size*nmemb;

	s->raw = realloc(s->raw, new_len+1);
	if (s->raw == NULL) {
		fprintf(stderr, "my_curl_write_func realloc() failed\n");
		s->memory_status = MY_CURL_HELPER_MEMORY_STATE_ERROR;
		return size*nmemb;;
	}
	memcpy(s->raw+s->raw_length, raw, size*nmemb);
	s->raw[new_len] = '\0';
	s->raw_length = new_len;
	return size*nmemb;
}

size_t get_file_contents(char *url, struct my_curl_output_result *result) {
	size_t ret = 0, checker;
	CURL *curl;
	CURLcode res;

	if (url == NULL) {
		return MY_CURL_HELPER_STATE_ERROR_NULL_URL;
	}
	if (result == NULL) {
		return MY_CURL_HELPER_STATE_ERROR_NULL_RESULT;
	}

	if (MY_CURL_HELPER_STATE_IS_OK != my_curl_init_output(result)) {
		return MY_CURL_HELPER_STATE_ERROR_INIT_MALLOC;
	}

	curl = curl_easy_init();
	if(!curl) {
		return MY_CURL_HELPER_STATE_ERROR_INIT_CURL;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_curl_write_func);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, result);
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	res = curl_easy_perform(curl);

	if (MY_CURL_HELPER_MEMORY_STATE_OK != result->memory_status) {
		ret = MY_CURL_HELPER_STATE_ERROR_MEMORY;
	} else if (CURLE_ABORTED_BY_CALLBACK != res) {
		ret = MY_CURL_HELPER_STATE_IS_OK;
		curl_easy_getinfo(curl, CURLINFO_HEADER_SIZE, &(result->header_length));
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &(result->respsone_status_code));
		if (result->header_length > 0) {
			result->header = result->raw;
			result->body = result->raw + result->header_length;
		} else {
			result->body = result->raw;
		}
	} else {
		ret = MY_CURL_HELPER_STATE_ERROR_UNKNOWN;
	}
	curl_easy_cleanup(curl);

	return ret;
}

void showServerEnvInfo() {
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
