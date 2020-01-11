// Implements the routing tables for the server //
#include "featherweight.h"

#include <regex.h>

/*
 * Object for a route path and handler.
 */
typedef struct {
    regex_t path_regex;
    FeatherWeightHandler *handler;
} Route;

/*
 * The object that holds the created Routes.
 */
typedef struct {
    Route* routes;
    unsigned size;
    unsigned capacity;
} RouteTable;