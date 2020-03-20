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

void my_curl_reset_query_string(char ***key_value, size_t *key_value_number) {
	size_t i, cnt;
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

size_t my_curl_parse_query_string(const char *query_string, char ***output_key_value, size_t *output_key_value_number, size_t urldecode) {
	size_t i, key_value_length;
	char *find_end, *find_equal, *decode;
	size_t query_string_length = 0;
	size_t detect_key_value_number = 0;
	int decode_length = 0;
	CURL *curl_resource = NULL;
	if (query_string == NULL || (query_string_length = strlen(query_string)) == 0)
		return -1;
	if (output_key_value == NULL || output_key_value_number == NULL)
		return -1;

	my_curl_reset_query_string(output_key_value, output_key_value_number);
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

#endif
