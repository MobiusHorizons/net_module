#ifndef _tlssocket_
#define _tlssocket_

#include "../stream/stream.h"

int tlssocket_type();
ssize_t tlssocket_read(void * ctx, void * buf, size_t nbyte, stream_error_t * error);
ssize_t tlssocket_write(void * ctx, const void * buf, size_t nbyte, stream_error_t * error);
ssize_t tlssocket_close(void * ctx, stream_error_t * error);
stream_t * tlssocket_connect(const char * host, int port);
int tlssocket_hangup(stream_t * conn);

#endif
