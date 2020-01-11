#ifndef FEATHERWEIGHT_ROUTE_H
#define FEATHERWEIGHT_ROUTE_H

#include <regex.h>
#include "featherweight.h"

#include "types.h"

RouteTable* createRouteTable();
int destroyRouteTable(RouteTable* table);
int registerRoute(RouteTable *routeTable, const char *uncompiledRouteRegex, FeatherWeightHandler handler);
FILE* executeRoute(RouteTable* routeTable, FeatherWeightRequest* request, FILE* response);

#endif /* FEATHERWEIGHT_ROUTE_H */