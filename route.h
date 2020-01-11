#ifndef FEATHERWEIGHT_ROUTE_H
#define FEATHERWEIGHT_ROUTE_H

#include "featherweight.h"

#include <regex.h>

/*
 * Object for a route path and handler.
 */
typedef struct {
    regex_t path_regex;
    FeatherWeightHandler handler;
} Route;

/*
 * The object that holds the created Routes.
 */
typedef struct {
    Route* routes;
    unsigned size;
    unsigned capacity;
} RouteTable;

RouteTable* createRouteTable();
int destroyRouteTable(RouteTable* table);
int registerRoute(RouteTable *routeTable, const char *uncompiledRouteRegex, FeatherWeightHandler handler);
int executeRoute(RouteTable* routeTable, FeatherWeightRequest* request, FILE* response);

#endif /* FEATHERWEIGHT_ROUTE_H */