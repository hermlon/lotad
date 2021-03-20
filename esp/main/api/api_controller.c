#include "api_controller.h"

#include <esp_log.h>
#include <sys/param.h>
#include "cJSON.h"

#include "api_auth.h"
#include "api_server.h"
#include "models/password_model.h"

static const char *TAG = "api controller";

static bool auth(httpd_req_t* req) {
  if(!authenticated(((api_server_context_t*) req->user_ctx)->session_cache, req)) {
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, "Invalid or no session cookie.");
    return false;
  }
  return true;
}

static cJSON* parse_post_data(httpd_req_t* req) {
  int total_len = req->content_len;
  int cur_len = 0;
  char* buf = ((api_server_context_t*)(req->user_ctx))->scratch;
  int received = 0;
  if(total_len >= SCRATCH_BUFSIZE) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too log");
    return NULL;
  }
  while(cur_len < total_len) {
    received = httpd_req_recv(req, buf + cur_len, total_len);
    if(received <= 0) {
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "failed receiving post content");
      return NULL;
    }
    cur_len += received;
  }
  buf[total_len] = '\0';
  return cJSON_Parse(buf);
}

static esp_err_t password_post_handler(httpd_req_t* req) {
  //if(!auth(req)) return ESP_FAIL;

  cJSON* data = parse_post_data(req);
  if(data == NULL) return ESP_FAIL;

  esp_err_t result = ESP_OK;
  cJSON* old_password = cJSON_GetObjectItemCaseSensitive(data, "old_password");
  if(cJSON_IsString(old_password) && old_password->valuestring != NULL) {
    if(password_check(old_password->valuestring)) {
      cJSON* new_password = cJSON_GetObjectItemCaseSensitive(data, "new_password");
      if(cJSON_IsString(new_password) && new_password->valuestring != NULL) {
        password_set(new_password->valuestring);
        httpd_resp_sendstr(req, "Successfully set new password");
      }
      else {
        result = ESP_FAIL;
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Missing or malformed param new_password.");
      }
    }
    else {
      result = ESP_FAIL;
      httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, "Wrong password.");
    }
  }
  else {
    result = ESP_FAIL;
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Missing or malformed param old_password.");
  }

  cJSON_Delete(data);
  return result;
}

void api_controller_init(httpd_handle_t* server) {
  /* URI handlers */
  httpd_uri_t password_post = {
    .uri = "/password",
    .method = HTTP_POST,
    .handler = password_post_handler,
    .user_ctx = httpd_get_global_user_ctx(*server)
  };
  httpd_register_uri_handler(*server, &password_post);
}
