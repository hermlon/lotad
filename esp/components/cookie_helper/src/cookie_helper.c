#include "cookie_helper.h"

#include <sys/param.h>
#include <esp_http_server.h>

/* Inspired by httpd_query_key_value */
esp_err_t cookie_value(const char* cookie_str, const char* key, char* val, size_t val_size) {
  if(cookie_str == NULL || key == NULL || val == NULL) {
    return ESP_ERR_INVALID_ARG;
  }

  const char* cookie_ptr = cookie_str;
  const size_t buf_len = val_size;

  while(strlen(cookie_ptr)) {
    /* Search for '=' character, else parameter is invalid */
    const char* val_ptr = strchr(cookie_ptr, '=');
    if(!val_ptr) {
      /* no '=' found */
      break;
    }
    size_t offset = val_ptr - cookie_ptr;

    /* length doesn't match or length matches, but content doesn't */
    if((offset != strlen(key)) || strncasecmp(cookie_ptr, key, offset)) {
      cookie_ptr = strchr(val_ptr, ';');
      /* no further cookies */
      if(!cookie_ptr) {
        break;
      }
      /* skip the space following the semicolon */
      cookie_ptr += 2;
      continue;
    }

    /* the cookie name matched! */

    /* Locate start of next cookie, set val_ptr after : */
    cookie_ptr = strchr(++val_ptr, ';');

    /* could be the last cookie, go until the end of the cookies string */
    if(!cookie_ptr) {
      cookie_ptr = val_ptr + strlen(val_ptr);
    }

    /* update value length, including one byte for null */
    val_size = cookie_ptr - val_ptr + 1;

    /* copy value to the caller's buffer, will add NULL terminator */
    strlcpy(val, val_ptr, MIN(val_size, buf_len));

    /* if buffer length is smaller than needed, return truncation error */
    if(buf_len < val_size) {
      return ESP_ERR_HTTPD_RESULT_TRUNC;
    }
    return ESP_OK;
  }
  return ESP_ERR_NOT_FOUND;
}
