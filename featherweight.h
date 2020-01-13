#ifndef FEATHERWEIGHT_H
#define FEATHERWEIGHT_H

#define FW_MAX_REQUEST_SIZE 8192
#define FW_MAX_PATH_CAPTURES 8

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "types.h"

FeatherWeightApp* fwCreateApp();
void fwDestroyApp(FeatherWeightApp* app);
int fwGet(FeatherWeightApp* app, const char* path_pattern, FeatherWeightHandler handler);
int fwListen(FeatherWeightApp* app, uint16_t port, unsigned thread_count, unsigned queue_size);

#endif /* FEATHERWEIGHT_H */
