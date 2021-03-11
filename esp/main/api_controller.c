#include "api_controller.h"

#include <esp_log.h>
#include <sys/param.h>

static const char *TAG = "api controller";

static esp_err_t password_post_handler(httpd_req_t* req) {
  char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t password_post = {
  .uri = "/password",
  .method = HTTP_POST,
  .handler = password_post_handler,
  .user_ctx = NULL
};

void api_controller_register_uri_handlers(httpd_handle_t* server) {
  httpd_register_uri_handler(*server, &password_post);
}
