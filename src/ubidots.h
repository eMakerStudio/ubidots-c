/*
 * Copyright (c) 2013 Ubidots.
 */

#ifndef _ubidots_h_
#define _ubidots_h_ 1

#define DEFAULT_BASE_URL "http://things.ubidots.com/api/v1.6"
#define STRLEN_BASE_URL 100
#define STRLEN_API_KEY  100
#define STRLEN_TOKEN    100

#define TIMESTAMP_NOW   -1

typedef struct UbidotsClient {
  char base_url[STRLEN_BASE_URL];
  char api_key[STRLEN_API_KEY];
  char token[STRLEN_TOKEN];
} UbidotsClient;

typedef struct UbidotsCollection {
  int n;
  int i;
  char  **variable_ids;
  float *values;
} UbidotsCollection;

UbidotsClient* ubidots_init(char *api_key);
UbidotsClient* ubidots_init_with_base_url(char *api_key, char *base_url);
int ubidots_save_value(UbidotsClient *client, char *variable_id, double value, long long timestamp);
void ubidots_cleanup(UbidotsClient *client);

UbidotsCollection* ubidots_collection_init(int n);
void ubidots_collection_add(UbidotsCollection *coll, char *variable_id, double value);
int ubidots_collection_save(UbidotsClient *client, UbidotsCollection *coll);
void ubidots_collection_cleanup(UbidotsCollection *coll);

#endif
