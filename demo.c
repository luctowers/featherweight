#include "featherweight.h"

#define PORT 8080
#define THREAD_COUNT 16
#define QUEUE_SIZE 256

FILE* rootHandler(FeatherWeightRequest* request, FILE* response) {

  fprintf(response, "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\nrequested path: %s", request->path);
  fclose(response);
  return NULL;

}

FILE* handler404(FeatherWeightRequest* request, FILE* response) {

  fprintf(response, "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\nNot Found ;) eat dicks");
  fclose(response);
  return NULL;

}

int main() {

  featherWeightDebug = true;

  FeatherWeightApp* app = fwCreateApp();

  fwGet(app, "^/$", rootHandler);

  fwGet(app, ".*", handler404);

  fwListen(app, PORT, THREAD_COUNT, QUEUE_SIZE);

  fwDestroyApp(app);

  return 0;

}
