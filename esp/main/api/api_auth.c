#include "api_auth.h"

#include <esp_log.h>
#include "mbedtls/md.h"

#include "cookie_helper.h"

static const char *TAG = "api auth";

#define MAX_SESSIONS = 10;

bool authenticated(struct cache* session_cache, httpd_req_t* req) {
  /* get session_id from reqest header */
  char* buf;
  size_t buf_len;

  buf_len = httpd_req_get_hdr_value_len(req, "Cookie") + 1;
  if(buf_len > 1) {
    buf = malloc(buf_len);
    if(httpd_req_get_hdr_value_str(req, "Cookie", buf, buf_len) == ESP_OK) {
      ESP_LOGI(TAG, "Found header => Cookie: %s", buf);
      char session_id[SESSION_ID_LENGTH];
      if(cookie_value(buf, "session_id", session_id, sizeof(session_id)) == ESP_OK) {
        ESP_LOGI(TAG, "session_id: %s", session_id);
        return contains_session(session_cache, session_id);
      }
    }
  }

  return false;
}

static void password_hash(char* input, unsigned char result[33]) {
  mbedtls_md_context_t ctx;
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char*) input, strlen(input));
  mbedtls_md_finish(&ctx, result);
  mbedtls_md_free(&ctx);
  result[32] = '\0';
}

bool password_check(char* password, unsigned char* real_pw_hash) {
  unsigned char pw_hash[33];
  password_hash(password, pw_hash);

  return strncmp(pw_hash, real_pw_hash, 32) == 0;
}
