/*
 * json_utility.h
 *
 *  Created on: 1 abr. 2023
 *      Author: JoseArratia
 */

#ifndef MAIN_JSON_UTILITY_H_
#define MAIN_JSON_UTILITY_H_
/*INCLUDES*****************************************************************************************/
#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "esp_err.h"

/*DEFINES******************************************************************************************/
#define JSON_MAX_NAME_SIZE				50
#define JSON_MAX_VALUE_SIZE				128
#define JSON_MAX_STR					128

#define INT_TYPE						0
#define FLOAT_TYPE						1
#define STRING_TYPE						2
#define BOOL_TYPE						3

/*TYPEDEFS*****************************************************************************************/
typedef uint8_t jdata_t;

struct json{
	char *name;
	char *value;

	struct json *next;
};

typedef struct json *json_t;

/*PROTOTYPES***************************************************************************************/
esp_err_t str_to_json(json_t *list, char *data);
esp_err_t json_to_str(json_t list, char **data);
esp_err_t json_get_value(json_t list, char *name, void *value, jdata_t type);
void json_add_data(json_t *list, char *name, void *value, jdata_t type);
json_t json_locate(json_t list, char *name);
void json_delete(json_t *list);
void jstr_delete(char **data);

#endif /* MAIN_JSON_UTILITY_H_ */
