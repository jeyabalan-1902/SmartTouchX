#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_app.h"

esp_err_t wifi_handler(httpd_req_t *req);
void start_http_server();
void start_softap();


#endif