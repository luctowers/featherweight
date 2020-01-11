#ifndef FEATHERWEIGHT_H
#define FEATHERWEIGHT_H

#define FW_MAX_REQUEST_SIZE 8192

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "types.h"

bool featherWeightDebug;

FeatherWeightApp* fwCreateApp();
void fwGet(FeatherWeightApp* app, const char* path_regex, FeatherWeightHandler handler);
void fwListen(FeatherWeightApp* app, uint16_t port, unsigned thread_count, unsigned queue_size);
void fwDestroyApp(FeatherWeightApp* app);

#endif /* FEATHERWEIGHT_H */
