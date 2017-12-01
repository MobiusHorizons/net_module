build set default LIBRESSL_INCLUDE_DIR "/usr/local/opt/libressl/include";
build set default LIBRESSL_LIB_DIR     "/usr/local/opt/libressl/lib";
build append CFLAGS  "-I$(LIBRESSL_INCLUDE_DIR)";
build append LDFLAGS "-L$(LIBRESSL_LIB_DIR) -ltls ";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <tls.h>

import stream     from "../stream/stream.module.c";
static int _type;

export int type() {
  if (_type == 0) {
    _type = stream.register("tlssocket");
  }

  return _type;
}

typedef struct {
  struct tls        * ctx;
  struct tls_config * cfg;
} state_t;


export ssize_t read(void * ctx, void * buf, size_t nbyte, stream.error_t * error) {
  state_t * state = (state_t *) ctx;
  int e = tls_read(state->ctx, buf, nbyte);
  if (e < 0 && error != NULL) {
    error->code    = e;
    error->message = tls_error(state->ctx);
  }
  return e;
}

export ssize_t write(void * ctx, const void * buf, size_t nbyte, stream.error_t * error) {
  state_t * state = (state_t *) ctx;
  int e = tls_write(state->ctx, buf, nbyte);
  if (e < 0 && error != NULL) {
    error->code    = e;
    error->message = tls_error(state->ctx);
  }
  return e;
}

export ssize_t close(void * ctx, stream.error_t * error) {
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

export
stream.t * connect(const char * host, int port) {
	struct tls_config * cfg = NULL;
	struct tls        * ctx = NULL;

	if (tls_init() != 0) {
    int e = errno;
    return stream.error(NULL, e, strerror(e));
  }

	if ((cfg = tls_config_new()) == NULL) {
    int e = errno;
    return stream.error(NULL, e, strerror(e));
  }

	/* set root certificate (CA) */
	if (tls_config_set_ca_file(cfg, "root.pem") != 0){
    int e = errno;
    return stream.error(NULL, e, strerror(e));
  }

	if ((ctx = tls_client()) == NULL) {
    int e = errno;
    return stream.error(NULL, e, strerror(e));
  }

	if (tls_configure(ctx, cfg) != 0) {
    return stream.error(NULL, 2, tls_error(ctx));
  }
  char port_ch[10];
  sprintf(port_ch, "%d", port);
	if (tls_connect(ctx, host, port_ch) != 0){
    return stream.error(NULL, 2, tls_error(ctx));
  }

  state_t * state = malloc(sizeof(state_t));
  state->cfg = cfg;
  state->ctx = ctx;

  stream.t * s = malloc(sizeof(stream.t));

  s->ctx   = state;
  s->read  = read;
  s->write = write;
  s->pipe  = NULL;
  s->close = close;
  s->type  = type();

  s->error.code    = 0;
  s->error.message = NULL;

  return s;
}

export int hangup(stream.t * conn) {
  return stream.close(conn);
}
