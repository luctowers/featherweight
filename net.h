#ifndef FEATHERWEIGHT_NET_H
#define FEATHERWEIGHT_NET_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

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
void destroyConnectionQueue(ConnectionQueue* queue);

#endif /* FEATHERWEIGHT_NET_H */
