#ifndef FEATHERWEIGHT_NET_H
#define FEATHERWEIGHT_NET_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "types.h"

int createTCPListenerSocket(uint16_t port, int backlog_limit);
IncomingConnection waitForConnection(int listener_socket);
void refuseConnection(IncomingConnection* connection);
unsigned readFromConnection(IncomingConnection* connection, void* buffer, unsigned max_size);
FILE* fileInterfaceFromConnection(IncomingConnection* connection);

ConnectionQueue* createConnectionQueue(unsigned size);
void connectionQueuePush(ConnectionQueue* queue, IncomingConnection connection);
IncomingConnection connectionQueuePop(ConnectionQueue* queue);
bool connectionQueueEmpty(ConnectionQueue* queue);
bool connectionQueueFull(ConnectionQueue* queue);
ConnectionQueue* destroyConnectionQueue(ConnectionQueue* queue);

#endif /* FEATHERWEIGHT_NET_H */
