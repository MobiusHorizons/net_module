
#include <stdbool.h>

#include <stdlib.h>

#include "../stream/stream.h"
#include "./socket.h"
#include "./tlssocket.h"

stream_t * net_connect(const char * host, int port, bool use_tls) {
  if (use_tls){
    return tlssocket_connect(host, port);
  } else {
    return socket_connect(host, port);
  }
}

int net_hangup(stream_t * conn) {
  int out;
  if (conn->type == tlssocket_type()){
      return tlssocket_hangup(conn);
  } else
  if (conn->type == socket_type()){
      return socket_hangup(conn);
  } else {
    stream_error(conn, -1 * conn->type, "Stream is not a network socket");
    return -1;
  }
}
