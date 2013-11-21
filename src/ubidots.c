/*
 * Ubidots C client.
 *
 * Copyright (c) 2013 Ubidots.
 */

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json/json.h>
#include <json/json_tokener.h>
#include "ubidots.h"


void crs_init(CurlRespString *s) {
  s->len = 0;
  s->ptr = malloc(1);
  
  if (s->ptr == NULL) {
    fprintf(stderr, "Call to malloc() failed.\n");
    exit(EXIT_FAILURE);
  }

  s->ptr[0] = '\0';
}


void crs_cleanup(CurlRespString *s) {
  free(s->ptr);
}


size_t curl_writefunc(void *ptr, size_t size, size_t nmemb, CurlRespString *s) {
  size_t new_len = s->len + size * nmemb;
  s->ptr = realloc(s->ptr, new_len + 1);

  if (s->ptr == NULL) {
    fprintf(stderr, "Call to realloc() failed.\n");
    exit(EXIT_FAILURE);
  }

  memcpy(s->ptr + s->len, ptr, size * nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size * nmemb;
}


/**
 * Returns a char* containing a new token for the given api key and base url.
 * If there was an error, returns NULL.
 */
char* get_token(char* api_key, char* base_url) {
  CURL *curl;
  CURLcode res;
  struct curl_slist *chunk;
  char auth_token_url[100];
  char custom_header[100];
  CurlRespString resp;

  sprintf(auth_token_url, "%s/auth/token", base_url);
  sprintf(custom_header, "X-UBIDOTS-APIKEY: %s", api_key);
  chunk = curl_slist_append(NULL, custom_header);
  crs_init(&resp);

  // Setup CURL
  curl = curl_easy_init();

  if (! curl)
    return NULL;

  curl_easy_setopt(curl, CURLOPT_URL, auth_token_url);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_writefunc);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

  // Execute CURL  
  res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
	    curl_easy_strerror(res));

    curl_easy_cleanup(curl);
    curl_slist_free_all(chunk);
    crs_cleanup(&resp);

    return NULL;
  }

  // Extract token from JSON
  json_object *jobj = json_tokener_parse(resp.ptr);

  if (jobj == NULL) {
    fprintf(stderr, "Parsing of JSON response from %s failed.\n", auth_token_url);
    curl_easy_cleanup(curl);
    curl_slist_free_all(chunk);
    crs_cleanup(&resp);
    return NULL;
  }

  json_object *jtok = json_object_object_get(jobj, "token");
  char *token = (char*)json_object_get_string(jtok);

  // Cleanup
  curl_easy_cleanup(curl);
  curl_slist_free_all(chunk);
  crs_cleanup(&resp);

  return token;
}


/**
 * Initialize a Ubidots session. This is most likely the first Ubidots
 * library function you will call. Returns a pointer to a UbidotsClient
 * struct. If there was an error, returns NULL.
 */
UbidotsClient* ubidots_init(char *api_key) {
  return ubidots_init_with_base_url(api_key, DEFAULT_BASE_URL);
}


UbidotsClient* ubidots_init_with_base_url(char *api_key, char *base_url) {
  char *token = get_token(api_key, base_url);

  if (token == NULL) {
    return NULL;
  }

  UbidotsClient *client = malloc(sizeof(UbidotsClient));  
  strncpy(client->base_url, base_url, STRLEN_BASE_URL);
  strncpy(client->api_key, api_key, STRLEN_API_KEY);
  strncpy(client->token, token, STRLEN_TOKEN);

  free(token);
  
  return client;
}


/**
 * End a ubidots session. After calling this function with UbidotsClient* client,
 * no more functions may be calls with it.
 */
void ubidots_cleanup(UbidotsClient *client) {
  free(client);
}


int main() {
  UbidotsClient *client = ubidots_init("74ccf3b7957fe38e3382c9fd107d70870edbb462");
  return 0;
}
