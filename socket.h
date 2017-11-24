#ifndef _socket_
#define _socket_

#include "../stream/stream.h"

int socket_type();
stream_t socket_connect(const char * host, int port);
int socket_hangup(stream_t conn);

#endif
