#include "http.h"

#include <string.h>

void parseRequest(FeatherWeightRequest* request, char* request_buffer) {
  
  char* ptr = request_buffer;
  request->method = strtok_r(ptr, " ", &ptr);
  request->path = strtok_r(ptr, " ", &ptr);

}
