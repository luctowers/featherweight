#ifndef FEATHERWEIGHT_TYPES_H
#define FEATHERWEIGHT_TYPES_H

#include <netinet/in.h>
#include <regex.h>

typedef struct {
  int socketfd;
  struct sockaddr_in address;
} IncomingConnection;

typedef struct {
  IncomingConnection* connections;
  unsigned head, tail, size, count;
} ConnectionQueue;

typedef struct {
  const char* method;
  const char* path;
  const char** path_captures;
} FeatherWeightRequest;

typedef FILE* (*FeatherWeightHandler)(FeatherWeightRequest*, FILE*);

typedef struct {
  regex_t path_regex;
  FeatherWeightHandler handler;
} Route;

typedef struct {
  Route* routes;
  unsigned size;
  unsigned capacity;
} RouteTable;

typedef struct {
  RouteTable* routeTable;
} FeatherWeightApp;

#endif /* FEATHERWEIGHT_TYPES_H */
