#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "../stream/stream.h"
#include "../stream/file.h"

static int _type;

int socket_type() {
  if (_type == 0) {
    _type = stream_register("socket");
  }

  return _type;
}

stream_t * socket_connect(const char * host, int port) {
	int sockfd;

	struct sockaddr_in serv_addr;
	struct hostent * h;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
    int e = errno;
    return stream_error(NULL, e, strerror(e));
  }

	h = gethostbyname(host);
	if (h == NULL) {
    close(sockfd);

    int e = h_errno;
    return stream_error(NULL, e, hstrerror(e));
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy(
		(char *)h->h_addr,
		(char*)&serv_addr.sin_addr.s_addr,
		h->h_length
	);

	serv_addr.sin_port = htons(port);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    int e = errno;
    return stream_error(NULL, e, strerror(e));
  }

  stream_t * conn = file_new(sockfd);
  conn->type = socket_type();
  return conn;
}

int socket_hangup(stream_t * conn) {
  int fd = file_descriptor(conn);
  if (shutdown(fd, SHUT_RDWR) == 0) {
    return stream_close(conn);
  } else {
    int e = errno;
    conn->error.code    = e;
    conn->error.message = strerror(e);
  }
  return -1;
}

