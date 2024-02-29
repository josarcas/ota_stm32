/*
 * github_api.h
 *
 *  Created on: 25 feb. 2024
 *      Author: JoseArratia
 */

#ifndef MAIN_GITHUB_API_H_
#define MAIN_GITHUB_API_H_
/*INCLUDES*********************************************************************************/
#include "stdio.h"

#include "esp_err.h"

/*DEFINES**********************************************************************************/
#define GIT_SERVER_NAME 			"api.github.com"
#define GIT_PORT 					"443"
//#define GIT_BIN_URL				 	"https://api.github.com/repos/josarcas/core2lvlgl/contents/README.md"
#define GIT_TOKEN					"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"
#define GIT_BUFF_SIZE				1024


//Support api
#ifndef GIT_API_REST
#define GIT_API_REST
#define GIT_CONTENTS
#endif

/*PROTOTYPES********************************************************************************/
esp_err_t git_save_content(char *owner, char*repo, char *path, char *filename);
esp_err_t git_get_content(char *owner, char *path, char **in);


#endif /* MAIN_GITHUB_API_H_ */
