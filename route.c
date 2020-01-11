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
    
    free(table->routes);
    free(table);

    return 0;
}

int registerRoute(RouteTable *routeTable, const char *uncompiledRouteRegex, const FeatherWeightHandler handler) {
    routeTable->size += 1;

    if (routeTable->size > routeTable->capacity) {
        routeTable->capacity *= 2;
        realloc(routeTable->routes, routeTable->capacity*sizeof(Route));
    }

    Route* newRoute = &routeTable->routes[routeTable->size-1];

    int compStatus = regcomp(&newRoute->path_regex, uncompiledRouteRegex, REG_EXTENDED);

    if (compStatus) { 
        routeTable->size -= 1;
        return -1;
    }

    return 0;
}

int findRoute(RouteTable *routeTable, char *routeName) {

}

int main() {

    // Create a handler for testing //

    

    // Create table //
    RouteTable* table = createRouteTable();
    printf("Size: %i\n", table->size);
    printf("Capacity: %i\n", table->capacity);

    // Create a //
    registerRoute(table, "/");
    // Delete the table //

    destroyRouteTable(table);
}