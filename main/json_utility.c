/*
 * json_utility.c
 *
 *  Created on: 1 abr. 2023
 *      Author: JoseArratia
 */
/*INCLUDES*****************************************************************************************/
#include "json_utility.h"
#include "utility.h"
#include "stdlib.h"

/*GLOBAL VARIABLES*********************************************************************************/

/*PROTOTYPES***************************************************************************************/
static json_t json_create_node(json_t *list);

/*FUNCTIONS****************************************************************************************/
static json_t json_create_node(json_t *list)
{
	json_t aux = (*list);

	if(aux == NULL)
	{
		(*list) = aux = (json_t) malloc(sizeof(struct json));
		if(aux == NULL) return NULL;
	}
	else
	{
		while(aux->next != NULL)
			aux = aux->next;
		aux->next = (json_t) malloc(sizeof(struct json));
		if(aux == NULL) return NULL;
		aux = aux->next;
	}
	aux->next = NULL;
	aux->name = NULL;
	aux->value = NULL;
	return aux;
}


esp_err_t str_to_json(json_t *list, char *data)
{
#warning "Only use json type int, float or string"
	size_t len;
	json_t aux;

	if(data == NULL || (len=strlen(data))==0 || data[0] != '{')
		return ESP_FAIL;

	size_t i=1;
	int j;

	while(i<len)
	{
		if(data[i] == '"')
		{
			i++;
			aux = json_create_node(list);
			if(aux == NULL)
			{
				json_delete(list);
				return ESP_FAIL;
			}
			j = str_find_c(&(data[i]), '"');
			if(j == -1)
				return ESP_FAIL;
			else
			{
				aux->name = (char *)malloc(sizeof(char)*j);
				if(aux->name == NULL)
				{
					json_delete(list);
					return ESP_FAIL;
				}

				memset(aux->name, 0, j);
				str_cpy_c(aux->name, &(data[i]), '"');
			}
			i+=j+1;
			if(data[i] != ':')
				return ESP_FAIL;
			i++;
			j = str_find_c(&(data[i]), ',');

			if(j == -1)
			{
				j = str_find_c(&(data[i]), '}');
				if(j == -1)
					return ESP_FAIL;
				else
				{
					aux->value = (char *)malloc(sizeof(char)*j);
					if(aux->value == NULL)
					{
						json_delete(list);
						return ESP_FAIL;
					}
					memset(aux->value, 0, j);
					str_cpy_c(aux->value, &(data[i]), '}');
				}
			}
			else
			{
				aux->value = (char *)malloc(sizeof(char)*j);
				if(aux->value == NULL)
				{
					json_delete(list);
					return ESP_FAIL;
				}
				memset(aux->value, 0, j);
				str_cpy_c(aux->value, &(data[i]), ',');
			}

			i+=j+1;
		}
	}

	return ESP_OK;
}

esp_err_t json_to_str(json_t list, char **data)
{
	json_t aux = list;
	uint16_t jindex=1;

	*data = malloc(JSON_MAX_STR);
	if(*data == NULL)
		return ESP_FAIL;

	str_clear(*data, JSON_MAX_STR);
	str_add(*data, "{");

	while(aux != NULL && jindex<JSON_MAX_STR-2)
	{
		str_add(*data, "\"");
		str_add(*data, aux->name);
		str_add(*data, "\":");
		str_add(*data, aux->value);
		if(aux->next != NULL)
			str_add(*data, ",");
		jindex+= strlen(aux->name) + strlen(aux->value) + 4;
		aux = aux->next;
	}
	str_add(*data, "}");
	return ESP_OK;
}

esp_err_t json_get_value(json_t list, char *name, void *value, jdata_t type)
{
	json_t aux = json_locate(list, name);

	switch(type)
	{
	case INT_TYPE:
		int *d = value;
		*d = atoi(aux->value);
		break;
	case FLOAT_TYPE:
		float *f = value;
		*f = atof(aux->value);
		break;
	case STRING_TYPE:
		memset(value, 0, strlen(value));
		str_cpy_c(value, &(aux->value[1]), '"');
		break;
	case BOOL_TYPE:
		uint8_t *u8 = value;
		*u8 = strcmp(aux->value, "true")==0? 1 : 0;
		break;
	default:
		return ESP_FAIL;
		break;
	}

	return ESP_OK;
}

void json_add_data(json_t *list, char *name, void *value, jdata_t type)
{
#warning "Only use json type int, float, string of boolean"
	json_t aux;
	char *aux_str;

	aux = json_create_node(list);

	//aux->name = name;
	aux->name = malloc(strlen(name)+1);
	if(aux->name == NULL)return;

	//strncpy(aux->name, name, strlen(name));
	//aux->name[strlen(name)] = '\0';
	strcpy(aux->name, name);

	aux_str = malloc(JSON_MAX_VALUE_SIZE);
	if(aux_str == NULL)return;

	switch(type)
	{
	case INT_TYPE:
		int *d = value;
		sprintf(aux_str, "%d", *d);
		aux->value = malloc(strlen(aux_str)+1);
		strcpy(aux->value, aux_str);
		break;
	case FLOAT_TYPE:
		float *f = value;
		sprintf(aux_str, "%.1f", *f);
		aux->value = malloc(strlen(aux_str)+1);
		if(aux->value == NULL)break;
		strcpy(aux->value, aux_str);
		break;
	case STRING_TYPE:
		char *str = value;
		aux->value = malloc(strlen(str)+3);
		if(aux->value == NULL)break;
		str_clear(aux->value, strlen(str)+3);
		str_add(aux->value, "\"");
		str_add(aux->value, str);
		str_add(aux->value, "\"");
		break;
	case BOOL_TYPE:
		uint8_t *u8 = value;
		sprintf(aux_str, "%s", *u8? "true" : "false");
		aux->value = malloc(strlen(aux_str)+1);
		if(aux->value == NULL)break;
		strcpy(aux->value, aux_str);
		break;
	default:
		break;
	}

	free(aux_str);

}

json_t json_locate(json_t list, char *name)
{
	json_t aux;
	for(aux = list; aux!= NULL; aux = aux->next)
	{
		if(strcmp(name, aux->name) == 0)
			return aux;
	}
	return aux;
}
void json_delete(json_t *list)
{
	json_t aux = (*list);
	json_t aux1;

	while(aux != NULL)//for(aux = (*list); aux!= NULL; aux = aux->next)
	{
		if(aux->name != NULL)
			free(aux->name);
		if(aux->value != NULL)
			free(aux->value);
		aux1 = aux;
		aux = aux->next;
		free(aux1);
	}
}

void jstr_delete(char **data)
{
	if(data != NULL)
		free(*data);
}



