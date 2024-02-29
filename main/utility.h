/*
 * utility.h
 *
 *  Created on: 17 dic. 2022
 *      Author: JoseArratia
 */

#ifndef MAIN_UTILITY_H_
#define MAIN_UTILITY_H_
/*INCLUDES**********************************************************************************************/
#include "stdint.h"

/*DEFINES***********************************************************************************************/


/*PROTOTYPES********************************************************************************************/
//STRING OPERATIONS
/*	@brief	Search char on string
*/
int str_find_c(char *str1, char c);
/*	@brief	Search char on string until index
*/
int str_find_c_s(char *str1, char c, int index);

void str_rmv_n(char *str, uint16_t offset, uint16_t len);
void str_clear(char *str, uint16_t size);
void str_copy_by_index(char *str1, char *str2, uint16_t index);
void str_copy(char *str1, char str2);
void str_add(char *str1, char *str2);
int str_cpy_c(char *str1, char *str2, char c);

void start_find_part(char *str);
int str_find_part(char *str1, uint16_t len);
void stop_find_part();

//
char *base64_encode(char *data, size_t input_length, size_t *output_length);

uint16_t crc_16 (uint8_t *data, uint16_t len);

#endif /* MAIN_UTILITY_H_ */
