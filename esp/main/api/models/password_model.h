#ifndef PASSWORD_MODEL_H
#define PASSWORD_MODEL_H

#include <stdbool.h>

bool password_check(char* password);

void password_set(char* password);

void password_get(char* pw_hash);

#endif
