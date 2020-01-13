#include "route.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RouteTable* createRouteTable() {
  RouteTable* route_table = malloc(sizeof(route_table));

  route_table->routes = malloc(1 * sizeof(Route));
  route_table->size = 0;
  route_table->capacity = 1;

  return route_table;
}

int destroyRouteTable(RouteTable* route_table) {
  // free compiled regex patterns
  for (int i = 0; i < route_table->size; i++) {
    Route* route = &route_table->routes[i];
    regfree(&route->path_regex);
  }

  free(route_table->routes);
  free(route_table);

  return 0;
}

int registerRoute(RouteTable* route_table, const char* uncompiled_path_pattern, FeatherWeightHandler handler) {
  route_table->size += 1;

  // allocated more memory if necessary
  if (route_table->size > route_table->capacity) {
    route_table->capacity *= 2;
    route_table->routes = realloc(route_table->routes, route_table->capacity * sizeof(Route));
  }

  // pointer for convienience
  Route* new_route = &route_table->routes[route_table->size - 1];

  int compilation_status = regcomp(&new_route->path_regex, uncompiled_path_pattern, REG_EXTENDED);

  // if compilation failed
  if (compilation_status) {
    route_table->size -= 1;
    return -1;
  }

  new_route->handler = handler;

  return 0;
}

FILE* executeRoute(const RouteTable* routeTable, FeatherWeightRequest* request, FILE* response) {
  unsigned path_length = strlen(request->path);
  regmatch_t path_captures[FW_MAX_PATH_CAPTURES];
  const char* path_capture_strings[FW_MAX_PATH_CAPTURES];
  char* path_capture_buffer = malloc(FW_MAX_PATH_CAPTURES * path_length);

  request->path_captures = path_capture_strings;

  for (int i = 0; i < routeTable->size; i++) {
    Route* route = &routeTable->routes[i];

    int match_status = regexec(&route->path_regex, request->path, FW_MAX_PATH_CAPTURES, path_captures, 0);

    // if no match
    if (match_status)
      continue;

    for (unsigned m = 0; m < FW_MAX_PATH_CAPTURES; m++) {
      regmatch_t* capture = &path_captures[m];
      if (capture->rm_so == -1) {
        path_capture_strings[m] = NULL;
        break;
      }
      unsigned capture_length = capture->rm_eo - capture->rm_so;
      const char* capture_start = &request->path[capture->rm_so];
      char* string_start = &path_capture_buffer[m * FW_MAX_PATH_CAPTURES];
      char* string_end = string_start + capture_length;
      memcpy(string_start, capture_start, capture_length);
      *string_end = 0;
      path_capture_strings[m] = string_start;
    }

    response = route->handler(request, response);

    if (!response)
      break;
  }

  free(path_capture_buffer);

  return response;
}
