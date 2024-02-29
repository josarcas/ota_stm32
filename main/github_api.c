/*
 * github_api.c
 *
 *  Created on: 25 feb. 2024
 *      Author: JoseArratia
 */
/*INCLUDES**************************************************************************************/
#include "github_api.h"
#include "utility.h"

#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

/*FUNCTIONS*************************************************************************************/
esp_err_t git_save_content(char *owner, char*repo, char *path, char *filename)
{
	//char git_req[GIT_BUFF_SIZE];
    esp_tls_cfg_t tls_config = {
    	.crt_bundle_attach = esp_crt_bundle_attach,
    };

    char buff[GIT_BUFF_SIZE];
    int32_t r_len;
    int16_t index = -1;
    FILE *in = fopen(filename, "wb");

    if(in == NULL)
    	return ESP_FAIL;

    sprintf(buff, "https://" GIT_SERVER_NAME "/repos/%s/%s/contents/%s", owner, repo, path);

    esp_tls_t *tls = esp_tls_init();

    if (tls == NULL)
    {
    	fclose(in);
    	return ESP_FAIL;
    }
    if(esp_tls_conn_http_new_sync(buff, &tls_config, tls) != 1)
    {
    	esp_tls_conn_destroy(tls);
    	fclose(in);
    	return ESP_FAIL;
    }
    sprintf(buff, "GET https://" GIT_SERVER_NAME "/repos/%s/%s/contents/%s HTTP/1.1\r\n"
			  	  	 "Host: " GIT_SERVER_NAME "\r\n"
					 "User-Agent: espios/1.0 esp32\r\n"
					 "Accept: application/vnd.github.raw\r\n"
					 "Authorization: Bearer " GIT_TOKEN "\r\n"
					 "X-GitHub-Api-Version: 2022-11-28\r\n"
					 "\r\n", owner, repo, path);
    if(esp_tls_conn_write(tls, buff, strlen(buff)) != strlen(buff))
    {
    	esp_tls_conn_destroy(tls);
    	fclose(in);
    	return ESP_FAIL;
    }

    start_find_part("\n\r\n");

    do
    {
    	//memset(buff, 0, GIT_BUFF_SIZE);
    	r_len = esp_tls_conn_read(tls, buff, GIT_BUFF_SIZE-1);

        if (r_len == ESP_TLS_ERR_SSL_WANT_WRITE  || r_len == ESP_TLS_ERR_SSL_WANT_READ)
        	asm("nop");
        else if (r_len <= 0)
        	break;
        else
        {
        	if(index == -1)
        	{
        		if((index=str_find_part(buff, r_len)) != -1 && index<r_len)
        		{
    				if(fwrite(&buff[index], 1, r_len-index, in) != r_len-index)
    				{
    					esp_tls_conn_destroy(tls);
    					fclose(in);
    					return ESP_FAIL;
    				}
        		}
        	}
        	else
        	{
				if(fwrite(buff, 1, r_len, in) != r_len)
				{
					esp_tls_conn_destroy(tls);
					fclose(in);
					return ESP_FAIL;
				}
        	}
        }
    }while (1);

    esp_tls_conn_destroy(tls);
    fclose(in);

    return ESP_OK;

}

esp_err_t git_get_content(char *owner, char *path, char **in)
{
	return ESP_OK;
}



