#ifndef FEATHERWEIGHT_ROUTE_H
#define FEATHERWEIGHT_ROUTE_H

#include <regex.h>
#include "featherweight.h"

#include "types.h"

RouteTable* createRouteTable();
int destroyRouteTable(RouteTable* route_table);
int registerRoute(RouteTable* route_table, const char* uncompiled_path_pattern, FeatherWeightHandler handler);
FILE* executeRoute(const RouteTable* route_table, FeatherWeightRequest* request, FILE* response);

#endif /* FEATHERWEIGHT_ROUTE_H */
