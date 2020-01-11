#include "featherweight.h"

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "net.h"
#include "http.h"

bool featherWeightDebug = false;

FeatherWeightApp* fwCreateApp() {

  FeatherWeightApp* app = malloc(sizeof(FeatherWeightApp));
  return app;

}



void fwDestroyApp(FeatherWeightApp* app) {

  free(app);

}



void fwGet(FeatherWeightApp* app, const char* path_regex, FeatherWeightHandler handler) { }



struct WorkerParameters {
  pthread_mutex_t mutex;
  pthread_cond_t condition;
  ConnectionQueue* connection_queue;
  bool shouldTerminate;
};

static void* workerTask(void* argument);


void fwListen(FeatherWeightApp* app, uint16_t port, unsigned thread_count, unsigned queue_size) {

  // initialise all worker params
  struct WorkerParameters workerParameters;
  workerParameters.connection_queue = createConnectionQueue(queue_size);
  workerParameters.shouldTerminate = false;
  pthread_mutex_init(&workerParameters.mutex, NULL);
  pthread_cond_init(&workerParameters.condition, NULL);

  // create worker thread pool
  pthread_t* thread_pool = malloc(thread_count*sizeof(pthread_t));
  for (unsigned t = 0; t < thread_count; t++)
    pthread_create(&thread_pool[t], 0, workerTask, &workerParameters);

  // create the socket that will listen for new connections
  int listener_socket = createTCPListenerSocket(port, queue_size);
  if (listener_socket == -1) {
    fprintf(stderr, "Failed to create listener socket on port %d\n", port);
    return;
  }
  printf("Listening on port %d...\n", port);

  // loop until terminated
  while (true) {

    // flag that determines whether connection will be refused or not
    bool shouldConnect;

    // wait for a new connection
    // notice that this is outside of lock
    IncomingConnection connection = waitForConnection(listener_socket);

    pthread_mutex_lock(&workerParameters.mutex); // ACQUIRE LOCK

    if (workerParameters.shouldTerminate) {
      pthread_mutex_unlock(&workerParameters.mutex); // RELEASE LOCK IF TERMINATED
      break;
    }

    if (!connectionQueueFull(workerParameters.connection_queue)) {
      connectionQueuePush(workerParameters.connection_queue, connection);
      shouldConnect = true;
    }
    else {
      shouldConnect = false;
    }

    pthread_mutex_unlock(&workerParameters.mutex); // RELEASE LOCK

    // signal worker or refuse connect
    if (shouldConnect)
      pthread_cond_signal(&workerParameters.condition);
    else
      refuseConnection(&connection);

  }

  // begin termination

  // prevent any new connections
  close(listener_socket);

  // tell workers to off themselves
  pthread_mutex_lock(&workerParameters.mutex);  
  workerParameters.shouldTerminate = true;
  pthread_mutex_unlock(&workerParameters.mutex);
  pthread_cond_broadcast(&workerParameters.condition);

  // join the workers and free the pool once they have all terminated
  for (unsigned t = 0; t < thread_count; t++) {
    pthread_join(thread_pool[t], NULL);
  }
  free(thread_pool);

  // get rid of the connection queue now that all workers are dead
  destroyConnectionQueue(workerParameters.connection_queue);

}



static void* workerTask(void* argument) {

  // cast the void pointer argument to the proper type
  struct WorkerParameters* parameters = argument;
  
  while (true) {

    IncomingConnection connection;
    
    pthread_mutex_lock(&parameters->mutex); // ACQUIRE LOCK

    if (parameters->shouldTerminate) {
      pthread_mutex_unlock(&parameters->mutex); // RELEASE LOCK IF TERMINATED
      return NULL;
    }

    // loop until there is a new connection to handle
    while (connectionQueueEmpty(parameters->connection_queue)) {

      // RELEASE LOCK AND SLEEP UNTIL CONDITIONAL SIGNALLED
      pthread_cond_wait(&parameters->condition, &parameters->mutex);
      // REACQUIRE LOCK ON WAKE

      if (parameters->shouldTerminate) {
        pthread_mutex_unlock(&parameters->mutex); // RELEASE LOCK IF TERMINATED
        return NULL;
      }

    }

    // get the new connection
    connection = connectionQueuePop(parameters->connection_queue);

    pthread_mutex_unlock(&parameters->mutex);  // RELEASE LOCK

    if(featherWeightDebug)
      printf("New request on thread: %u\n", (unsigned) pthread_self());

    // respond to the connection
    const char* message = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\nHello, world!";
    send(connection.socketfd, message, strlen(message), 0);
    close(connection.socketfd);

    // read request
    FeatherWeightRequest request;
    char* request_buffer = malloc(FW_MAX_REQUEST_SIZE+1);
    unsigned readCount = readFromConnection(&connection, request_buffer, FW_MAX_REQUEST_SIZE);
    request_buffer[readCount] = 0; // place a null terminator
    parseRequest(&request, request_buffer);

    // only get requests for now
    if (strcmp(request.method, "GET")) {
      free(request_buffer);
      refuseConnection(&connection);
      return NULL;
    }

    // respond to request
    FILE* response = fileInterfaceFromConnection(&connection);
    fprintf(response, "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\nrequested path: %s", request.path);
    fclose(response);

    // free request buffer
    free(request_buffer);

  }

}
