#include <stdbool.h>
#include <stdlib.h>

import stream     from "../stream/stream.module.c";
import socket     from "./socket.module.c";
import tls        from "./tlssocket.module.c";

export stream.t connect(const char * host, int port, bool use_tls) {
  if (use_tls){
    return tls.connect(host, port);
  } else {
    return socket.connect(host, port);
  }
}

export int hangup(stream.t conn) {
  int out;
  if (conn.type == tls.type()){
      return tls.hangup(conn);
  } else
  if (conn.type == socket.type()){
      return socket.hangup(conn);
  } else {
    stream.error(conn, -1 * conn.type, "Stream is not a network socket");
    return -1;
  }
}
