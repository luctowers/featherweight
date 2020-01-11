#ifndef FEATHERWEIGHT_TYPES_H
#define FEATHERWEIGHT_TYPES_H

#include <regex.h>
#include <netinet/in.h>

typedef struct {
  int socketfd;
  struct sockaddr_in address;
} IncomingConnection;

typedef struct {
  IncomingConnection* connections;
  unsigned head, tail, size, count;
} ConnectionQueue;


/*
 * The object given to the FeatherWeightHandler that represents the HTTP request.
 * Has things like the clients IP address and parsed parameters from the path.
 */
typedef struct {
  const char* method;
  const char* path;
} FeatherWeightRequest;

/*
 * The handler for a route. Register a handler using a method like fwGet.
 * Corresponding handlers will be called in order of registration.
 */
typedef FILE* (*FeatherWeightHandler)(FeatherWeightRequest*, FILE*);


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

/* The application, created with fwCreateApp and destroyed with fwDestroyApp. */
typedef struct {
    RouteTable* routeTable; 
} FeatherWeightApp;


#endif /* FEATHERWEIGHT_TYPES_H */