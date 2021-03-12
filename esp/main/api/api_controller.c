#include "api_controller.h"

#include <esp_log.h>
#include <sys/param.h>

#include "api_auth.h"
#include "session_cache.h"

static const char *TAG = "api controller";

#define SESSION_BLOCKS 10
#define SESSION_BLOCKSIZE 2
static struct cache* session_cache;

static bool auth(httpd_req_t* req) {
  if(!authenticated(session_cache, req)) {
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, "Invalid or no session cookie.");
    return false;
  }
  return true;
}

static esp_err_t password_post_handler(httpd_req_t* req) {
  if(!auth(req)) return ESP_OK;

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


void api_controller_init(httpd_handle_t* server) {
  httpd_register_uri_handler(*server, &password_post);

  session_cache = create_cache(SESSION_BLOCKS, SESSION_BLOCKSIZE);
}

void api_controller_deinit() {
  free_cache(session_cache);
}
