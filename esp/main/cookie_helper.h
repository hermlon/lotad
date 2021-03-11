#ifndef COOKIE_HELPER_H
#define COOKIE_HELPER_H

#include <esp_err.h>

esp_err_t cookie_value(const char* cookie_str, const char* key, char* val, size_t val_size);
#endif
