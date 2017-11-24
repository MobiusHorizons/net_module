#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>

import stream     from "../stream/stream.module.c";
import file       from "../stream/file.module.c";

static int _type;

export int type() {
  if (_type == 0) {
    _type = stream.register("tlssocket");
  }

  return _type;
}

export
stream.t connect(const char * host, int port) {
	int sockfd;

	struct sockaddr_in serv_addr;
	struct hostent * h;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
    int e = errno;
    return stream.error((stream.t) {0}, e, strerror(e));
  }

	h = gethostbyname(host);
	if (h == NULL) {
    global.close(sockfd);

    int e = h_errno;
    return stream.error((stream.t) {0}, e, hstrerror(e));
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy(
		(char *)h->h_addr,
		(char*)&serv_addr.sin_addr.s_addr,
		h->h_length
	);

	serv_addr.sin_port = htons(port);
	if (global.connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    int e = errno;
    return stream.error((stream.t) {0}, e, strerror(e));
  }

  stream.t conn = file.new(sockfd);
  conn.type = type();
  return conn;
}

export int hangup(stream.t conn) {
  int fd = file.descriptor(conn);
  if (shutdown(fd, SHUT_RDWR) == 0) {
    return stream.close(conn);
  } else {
    int e = errno;
    stream.error(conn, e, strerror(e));
  }
  return -1;
}

