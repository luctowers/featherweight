#include "featherweight.h"

#define PORT 8080
#define THREAD_COUNT 16
#define QUEUE_SIZE 256

FILE* rootHandler(FeatherWeightRequest* request, FILE* response) {

  return NULL; // do not call any more handlers

}

int main() {

  FeatherWeightApp* app = fwCreateApp();

  fwGet(app, "^/$", rootHandler);

  fwListen(app, PORT, THREAD_COUNT, QUEUE_SIZE);

  fwDestroyApp(app);

  return 0;

}
