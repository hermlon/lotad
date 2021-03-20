#include "password_model.h"

#include <string.h>
#include "mbedtls/md.h"
#include "nvs.h"

#include "watering_storage.h"

static void password_hash(char* input, char result[33]) {
  mbedtls_md_context_t ctx;
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char*) input, strlen(input));
  mbedtls_md_finish(&ctx, (unsigned char*) result);
  mbedtls_md_free(&ctx);
  result[32] = '\0';
}

static bool password_compare(char* password, char* real_pw_hash) {
  char pw_hash[33];
  password_hash(password, pw_hash);

  return strncmp(pw_hash, real_pw_hash, 32) == 0;
}

bool password_check(char* password) {
  char real_pw_hash[33];
  password_get(real_pw_hash);
  return password_compare(password, real_pw_hash);
}

void password_get(char* pw_hash) {
  /* read from nvs */
  nvs_handle_t my_handle;
  size_t length = 33;
  ESP_ERROR_CHECK(nvs_open(STORE_NAMESPACE, NVS_READONLY, &my_handle));
  ESP_ERROR_CHECK(nvs_get_str(my_handle, STORE_PASSWORD_HASH, pw_hash, &length));
  nvs_close(my_handle);
}

void password_set(char* password) {
  char pw_hash[33];
  password_hash(password, pw_hash);
  nvs_handle_t my_handle;
  ESP_ERROR_CHECK(nvs_open(STORE_NAMESPACE, NVS_READWRITE, &my_handle));
  ESP_ERROR_CHECK(nvs_set_str(my_handle, STORE_PASSWORD_HASH, pw_hash));
  ESP_ERROR_CHECK(nvs_commit(my_handle));
  nvs_close(my_handle);
}
