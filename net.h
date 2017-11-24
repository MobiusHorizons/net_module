#ifndef _net_
#define _net_

#include "../stream/stream.h"

stream_t net_connect(const char * host, int port, bool use_tls);
int net_hangup(stream_t conn);

#endif
