




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <tls.h>

#include "../stream/stream.h"
static int _type;

int tlssocket_type() {
  if (_type == 0) {
    _type = stream_register("tlssocket");
  }

  return _type;
}

typedef struct {
  struct tls        * ctx;
  struct tls_config * cfg;
} state_t;


ssize_t tlssocket_read(void * ctx, void * buf, size_t nbyte, stream_error_t * error) {
  state_t * state = (state_t *) ctx;
  int e = tls_read(state->ctx, buf, nbyte);
  if (e < 0 && error != NULL) {
    error->code    = e;
    error->message = tls_error(state->ctx);
  }
  return e;
}

ssize_t tlssocket_write(void * ctx, const void * buf, size_t nbyte, stream_error_t * error) {
  state_t * state = (state_t *) ctx;
  int e = tls_write(state->ctx, buf, nbyte);
  if (e < 0 && error != NULL) {
    error->code    = e;
    error->message = tls_error(state->ctx);
  }
  return e;
}

ssize_t tlssocket_close(void * ctx, stream_error_t * error) {
  state_t *state = (state_t *) ctx;
  int e = tls_close(state->ctx);
  if (e < 0 && error != NULL) {
    error->code    = e;
    error->message = tls_error(state->ctx);
    return e;
  }

  tls_free(state->ctx);
  tls_config_free(state->cfg);
  free(state);
  return 0;
}

stream_t * tlssocket_connect(const char * host, int port) {
	struct tls_config * cfg = NULL;
	struct tls        * ctx = NULL;

	if (tls_init() != 0) {
    int e = errno;
    return stream_error(NULL, e, strerror(e));
  }

	if ((cfg = tls_config_new()) == NULL) {
    int e = errno;
    return stream_error(NULL, e, strerror(e));
  }

  /* TODO: add API for config changes */

	if ((ctx = tls_client()) == NULL) {
    int e = errno;
    return stream_error(NULL, e, strerror(e));
  }

	if (tls_configure(ctx, cfg) != 0) {
    return stream_error(NULL, 2, tls_error(ctx));
  }
  char port_ch[10];
  sprintf(port_ch, "%d", port);
	if (tls_connect(ctx, host, port_ch) != 0){
    return stream_error(NULL, 2, tls_error(ctx));
  }

  state_t * state = malloc(sizeof(state_t));
  state->cfg = cfg;
  state->ctx = ctx;

  stream_t * s = malloc(sizeof(stream_t));

  s->ctx   = state;
  s->read  = tlssocket_read;
  s->write = tlssocket_write;
  s->pipe  = NULL;
  s->close = tlssocket_close;
  s->type  = tlssocket_type();

  s->error.code    = 0;
  s->error.message = NULL;

  return s;
}

int tlssocket_hangup(stream_t * conn) {
  return stream_close(conn);
}
