#ifndef FEATHERWEIGHT_H
#define FEATHERWEIGHT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/*
 * The object given to the FeatherWeightHandler that represents the HTTP request.
 * Has things like the clients IP address and parsed parameters from the path.
 */
typedef struct {

} FeatherWeightRequest;

/*
 * The handler for a route. Register a handler using a method like fwGet.
 * Corresponding handlers will be called in order of registration.
 */
typedef FILE* (*FeatherWeightHandler)(FeatherWeightRequest, FILE*);

/* The application, created with fwCreateApp and destroyed with fwDestroyApp. */
typedef struct {

} FeatherWeightApp;

FeatherWeightApp* fwCreateApp();
void fwGet(FeatherWeightApp* app, const char* path_regex, FeatherWeightHandler handler);
void fwListen(FeatherWeightApp* app, uint16_t port, unsigned thread_count, unsigned queue_size);
void fwDestroyApp(FeatherWeightApp* app);

#endif /* FEATHERWEIGHT_H */
