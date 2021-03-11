#include "unity.h"
#include "cookie_helper.h"

#include <esp_http_server.h>

TEST_CASE("Test cookie string parsing", "[cookie helper]") {
    struct cookie {
      const char* cookie_str;
      const char* key;
      const char* value;
    };

    struct cookie cookies[] = {
      {"test=345; test2=hellow", "test", "345"},
      {"test=345; test2=hellow", "test2", "hellow"},
      {"session_=asefesfsie; cat=yesss", "session_", "asefesfsie"},
      {"session_=asefesfsie; cat=yesss; ", "cat", "yesss"},
      {"session_=asefesfsie", "session_", "asefesfsie"},
      {},
    };

    struct cookie* ct = cookies;

    while(ct->cookie_str != 0) {
      char buf[20];
      cookie_value(ct->cookie_str, ct->key, buf, sizeof(buf));
      TEST_ASSERT_EQUAL_STRING(ct->value, buf);
      ct++;
    }
}

TEST_CASE("Test cookie string key not found", "[cookie helper]") {
    char buf[20];
    TEST_ASSERT_EQUAL_INT(cookie_value("key1=value1; key2=value2", "key3", buf, sizeof(buf)), ESP_ERR_NOT_FOUND);
    TEST_ASSERT_EQUAL_INT(cookie_value("key1=value1; key2=value2", "key2", buf, sizeof(buf)), ESP_OK);
}

TEST_CASE("Test cookie string truncate", "[cookie helper]") {
    char buf[4];
    TEST_ASSERT_EQUAL_INT(cookie_value("key1=1234; key2=value2", "key1", buf, sizeof(buf)), ESP_ERR_HTTPD_RESULT_TRUNC);
    TEST_ASSERT_EQUAL_INT(cookie_value("key1=123; key2=value2", "key1", buf, sizeof(buf)), ESP_OK);
}
