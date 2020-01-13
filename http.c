#include "http.h"

#include <string.h>

int parseRequest(FeatherWeightRequest* request, char* request_buffer) {
  char* ptr;

  if (!strtok_r(request_buffer, "\r\n\r\n", &ptr))  // mark end of header
    return -1;
  if (!(request->method = strtok_r(request_buffer, " ", &ptr)))  // get http method
    return -1;
  if (!(request->path = strtok_r(NULL, " ", &ptr)))  // get http path
    return -1;

  return 0;
}
