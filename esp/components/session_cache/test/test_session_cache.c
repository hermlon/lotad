#include "unity.h"
#include "session_cache.h"
#include "freertos/FreeRTOS.h"

TEST_CASE("Test cache creation and free", "[session cache]") {
  int memory = xPortGetFreeHeapSize();
  struct cache* cache = create_cache(10, 2);
  free_cache(cache);
  TEST_ASSERT_EQUAL_INT(xPortGetFreeHeapSize(), memory);
}

TEST_CASE("Test new session", "[session cache]") {
  int memory = xPortGetFreeHeapSize();
  struct cache* cache = create_cache(10, 3);
  char* session_id = new_session(cache);
  printf("%s\n", session_id);
  for(int i = 0; i < 500; i ++) {
    new_session(cache);
  }
  print_cache(cache);
  free_cache(cache);
  TEST_ASSERT_EQUAL_INT(xPortGetFreeHeapSize(), memory);
}
