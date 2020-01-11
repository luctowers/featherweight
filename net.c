#include "net.h"

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>



static const int sockopt = 1;

int createTCPListenerSocket(uint16_t port, int backlog_limit) {

  int sockfd;
  struct sockaddr_in address; 

  // create socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (!sockfd)
    return 0;

  // set socket options
  int setsockopt_status = setsockopt(
    sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
    &sockopt, sizeof(sockopt)
  );
  if (setsockopt_status) {
    close(sockfd);
    return 0;
  }

  // bind the socket
  address.sin_family = AF_INET; 
  address.sin_addr.s_addr = INADDR_ANY; 
  address.sin_port = htons(port);
  int bind_status = bind(sockfd, (struct sockaddr*) &address, sizeof(address));
  if (bind_status) {
    close(sockfd);
    return 0;
  }

  // start listening
  int listen_status = listen(sockfd, backlog_limit);
  if (listen_status) {
    close(sockfd);
    return 0;
  }

  return sockfd;

}

IncomingConnection waitForConnection(int listener_socket) {

  IncomingConnection connection;

  const socklen_t address_size = sizeof(connection.address);
  connection.socketfd = accept(listener_socket, (struct sockaddr*) &connection.address, (socklen_t*) &address_size);

  return connection;

}

ConnectionQueue* createConnectionQueue(unsigned size) {

  ConnectionQueue* queue = malloc(sizeof(ConnectionQueue));
  queue->connections = malloc(size*sizeof(IncomingConnection));
  queue->size = size;
  queue->count = 0;
  queue->head = 0;
  queue->tail = 0;
  return queue;

}



void connectionQueuePush(ConnectionQueue* queue, IncomingConnection connection) {

  queue->connections[queue->tail] = connection;
  if (++queue->tail == queue->size)
    queue->tail = 0;
  queue->count += 1;

}



IncomingConnection connectionQueuePop(ConnectionQueue* queue) {

  IncomingConnection connection = queue->connections[queue->head];
  if (++queue->head == queue->size)
    queue->head = 0;
  queue->count -= 1;
  return connection;

}



bool connectionQueueEmpty(ConnectionQueue* queue) {

  return queue->count == 0;

}



bool connectionQueueFull(ConnectionQueue* queue) {

  return queue->count == queue->size;

}



ConnectionQueue* destroyConnectionQueue(ConnectionQueue* queue) {

  while (queue->head != queue->tail) {

    close(queue->connections[queue->head].socketfd);

    if (++queue->head == queue->size)
      queue->head = 0;

  }

  free(queue->connections);
  free(queue);

}



void refuseConnection(IncomingConnection* connection) {

    close(connection->socketfd);

}
