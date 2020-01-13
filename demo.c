#include <string.h>

#include "featherweight.h"

#define PORT 8080
#define THREAD_COUNT 64
#define QUEUE_SIZE 256

FILE* rootHandler(FeatherWeightRequest* request, FILE* response) {
  fprintf(response, "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n");
  fputs("this is the root page", response);
  fclose(response);
  return NULL;
}

FILE* reverseHandler(FeatherWeightRequest* request, FILE* response) {
  fprintf(response, "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n");
  fputs("reversed: ", response);
  const char* string = request->path_captures[1];
  for (int i = strlen(string) - 1; i >= 0; i--)
    fputc(string[i], response);
  fclose(response);
  return NULL;
}

FILE* notFoundHandler(FeatherWeightRequest* request, FILE* response) {
  fprintf(response, "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
  fputs("not Found ;) eat dicks", response);
  fclose(response);
  return NULL;
}

int main() {
  FeatherWeightApp* app = fwCreateApp();
  fwGet(app, "^/$", rootHandler);
  fwGet(app, "^/reverse/([A-Za-z_0-9]*)$", reverseHandler);
  fwGet(app, ".*", notFoundHandler);
  fwListen(app, PORT, THREAD_COUNT, QUEUE_SIZE);
  fwDestroyApp(app);
  return 0;
}
