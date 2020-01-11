#include "route.h"

#include <stdlib.h>
#include <stdio.h>

RouteTable* createRouteTable() {
    RouteTable* table = malloc(sizeof(RouteTable)); 
    
    table->routes = malloc(1*sizeof(Route));
    table->size = 0;
    table->capacity = 1;

    return table;
}

int destroyRouteTable(RouteTable* table) {
    
    // Free the regex
    for(int i = 0; i < routeTable->size; i++) {
        Route* route = &routeTable->routes[i];
        
        regfree(&route->path_regex);
    }
 
    // Free tables and routes
    free(table->routes);
    free(table);

    return 0;
}

int registerRoute(RouteTable *routeTable, const char *uncompiledRouteRegex, FeatherWeightHandler handler) {
    routeTable->size += 1;

    if (routeTable->size > routeTable->capacity) {
        routeTable->capacity *= 2;
        routeTable->routes = realloc(routeTable->routes, routeTable->capacity*sizeof(Route));
    }

    Route* newRoute = &routeTable->routes[routeTable->size-1];

    int compStatus = regcomp(&newRoute->path_regex, uncompiledRouteRegex, REG_EXTENDED);

    if (compStatus) { 
        routeTable->size -= 1;
        return -1;
    }

    newRoute->handler = handler;

    return 0;
}

FILE* executeRoute(RouteTable* routeTable, FeatherWeightRequest* request, FILE* response) {

    for(int i = 0; i < routeTable->size; i++) {
        Route* route = &routeTable->routes[i];
        
        int regexStatus = regexec(&route->path_regex, request->path, 0, NULL, 0);

        if (regexStatus)
            continue; 
        
        response = route->handler(request, response);

        if (!response)
            break;        
    }

    return response;
}