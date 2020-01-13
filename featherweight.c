#include "featherweight.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http.h"
#include "net.h"
#include "route.h"

FeatherWeightApp* fwCreateApp() {
  FeatherWeightApp* app = malloc(sizeof(FeatherWeightApp));
  if (!app)
    return NULL;

  app->routeTable = createRouteTable();
  if (!app->routeTable) {
    free(app);
    return NULL;
  }

  return app;
}

void fwDestroyApp(FeatherWeightApp* app) {
  destroyRouteTable(app->routeTable);
  free(app);
}

int fwGet(FeatherWeightApp* app, const char* path_pattern, FeatherWeightHandler handler) {
  return registerRoute(app->routeTable, path_pattern, handler);
}

struct WorkerParameters {
  pthread_mutex_t mutex;
  pthread_cond_t condition;
  ConnectionQueue* connection_queue;
  RouteTable* routeTable;
  bool shouldTerminate;
};

static void* workerTask(void* argument);

int fwListen(FeatherWeightApp* app, uint16_t port, unsigned thread_count, unsigned queue_size) {
  int return_value = 0;
  unsigned threads_created = 0;

  struct WorkerParameters workerParameters;
  workerParameters.routeTable = app->routeTable;
  workerParameters.shouldTerminate = false;

  workerParameters.connection_queue = createConnectionQueue(queue_size);
  if (!workerParameters.connection_queue)
    return -1;

  if (pthread_mutex_init(&workerParameters.mutex, NULL)) {
    return_value = -1;
    goto cleanup1;
  }

  if (pthread_cond_init(&workerParameters.condition, NULL)) {
    return_value = -1;
    goto cleanup2;
  }

  pthread_t* thread_pool = malloc(thread_count * sizeof(pthread_t));
  if (!thread_pool) {
    return_value = -1;
    goto cleanup3;
  }

  for (; threads_created < thread_count; threads_created++) {
    if (pthread_create(&thread_pool[threads_created], 0, workerTask, &workerParameters)) {
      return_value = -1;
      goto cleanup4;
    }
  }

  int listener_socket = createTCPListenerSocket(port, queue_size);
  if (listener_socket == -1) {
    return_value = -1;
    goto cleanup5;
  }

  // loop until terminated
  while (true) {
    // flag that determines whether connection will be refused or not
    bool shouldConnect;

    // wait for a new connection
    // notice that this is outside of lock
    IncomingConnection connection = waitForConnection(listener_socket);

    pthread_mutex_lock(&workerParameters.mutex);  // ACQUIRE LOCK

    if (workerParameters.shouldTerminate) {
      pthread_mutex_unlock(&workerParameters.mutex);  // RELEASE LOCK IF TERMINATED
      break;
    }

    if (!connectionQueueFull(workerParameters.connection_queue)) {
      connectionQueuePush(workerParameters.connection_queue, connection);
      shouldConnect = true;
    } else {
      shouldConnect = false;
    }

    pthread_mutex_unlock(&workerParameters.mutex);  // RELEASE LOCK

    // signal worker or refuse connect
    if (shouldConnect)
      pthread_cond_signal(&workerParameters.condition);
    else
      refuseConnection(&connection);
  }

  // prevent any new connections
  close(listener_socket);

cleanup5:
  pthread_mutex_lock(&workerParameters.mutex);
  workerParameters.shouldTerminate = true;
  pthread_mutex_unlock(&workerParameters.mutex);
  pthread_cond_broadcast(&workerParameters.condition);
  for (unsigned t = 0; t < threads_created; t++) {
    pthread_join(thread_pool[t], NULL);
  }

cleanup4:
  free(thread_pool);

cleanup3:
  pthread_cond_destroy(&workerParameters.condition);

cleanup2:
  pthread_mutex_destroy(&workerParameters.mutex);

cleanup1:
  destroyConnectionQueue(workerParameters.connection_queue);

  return return_value;
}

static void* workerTask(void* argument) {
  // cast the void pointer argument to the proper type
  struct WorkerParameters* parameters = argument;

  while (true) {
    IncomingConnection connection;

    pthread_mutex_lock(&parameters->mutex);  // ACQUIRE LOCK

    if (parameters->shouldTerminate) {
      pthread_mutex_unlock(&parameters->mutex);  // RELEASE LOCK IF TERMINATED
      return NULL;
    }

    // loop until there is a new connection to handle
    while (connectionQueueEmpty(parameters->connection_queue)) {
      // RELEASE LOCK AND SLEEP UNTIL CONDITIONAL SIGNALLED
      pthread_cond_wait(&parameters->condition, &parameters->mutex);
      // REACQUIRE LOCK ON WAKE

      if (parameters->shouldTerminate) {
        pthread_mutex_unlock(&parameters->mutex);  // RELEASE LOCK IF TERMINATED
        return NULL;
      }
    }

    // get the new connection
    connection = connectionQueuePop(parameters->connection_queue);

    pthread_mutex_unlock(&parameters->mutex);  // RELEASE LOCK

    // read request
    FeatherWeightRequest request;
    char* request_buffer = malloc(FW_MAX_REQUEST_SIZE + 1);
    if (!request_buffer)
      continue;
    unsigned readCount = readFromConnection(&connection, request_buffer, FW_MAX_REQUEST_SIZE);
    request_buffer[readCount] = 0;  // place a null terminator at the end

    int parse_status = parseRequest(&request, request_buffer);
    if (!parse_status) {
      // only get requests for now
      if (strcmp(request.method, "GET")) {
        free(request_buffer);
        refuseConnection(&connection);
        return NULL;
      }

      // repond to request
      FILE* response = fileInterfaceFromConnection(&connection);
      response = executeRoute(parameters->routeTable, &request, response);
      if (response)
        fclose(response);
    }

    free(request_buffer);
  }
}
