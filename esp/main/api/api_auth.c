#include "api_auth.h"

#include <esp_log.h>

#include "cookie_helper.h"

static const char *TAG = "api auth";

// 10 active sessions at a time should be enough for this application
static uint64_t active_sessions[10];

bool authenticated(httpd_req_t* req) {
  // get session_id from reqest header
  char* buf;
  size_t buf_len;

  buf_len = httpd_req_get_hdr_value_len(req, "Cookie") + 1;
  if(buf_len > 1) {
    buf = malloc(buf_len);
    if(httpd_req_get_hdr_value_str(req, "Cookie", buf, buf_len) == ESP_OK) {
      ESP_LOGI(TAG, "Found header => Cookie: %s", buf);
      char session_id[5];
      ESP_ERROR_CHECK(cookie_value(buf, "session_id", session_id, sizeof(session_id)));
      ESP_LOGI(TAG, "session_id: %s", session_id);
    }
  }
  return true;
}
