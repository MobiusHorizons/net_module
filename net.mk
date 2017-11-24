# dependencies for ./net.c
./net.o: ./net.c ../stream/stream.h ./socket.h ./tlssocket.h ../stream/stream.h

# dependencies for ../stream/stream.c
../stream/stream.o: ../stream/stream.c

# dependencies for ./socket.c
./socket.o: ./socket.c ../stream/stream.h ../stream/file.h ../stream/stream.h

# dependencies for ../stream/file.c
../stream/file.o: ../stream/file.c ../stream/stream.h ../stream/stream.h

# dependencies for ./tlssocket.c
LIBRESSL_INCLUDE_DIR ?= /usr/local/opt/libressl/include
LIBRESSL_LIB_DIR ?= /usr/local/opt/libressl/lib
CFLAGS += -I$(LIBRESSL_INCLUDE_DIR)
LDFLAGS += -L$(LIBRESSL_LIB_DIR) -ltls 
./tlssocket.o: ./tlssocket.c ../stream/stream.h ../stream/stream.h

net.a: ../stream/stream.o ../stream/file.o ./socket.o ./tlssocket.o ./net.o
	ar rcs $@ $^

CLEAN_net.a:
	rm -rf net.a  ../stream/stream.o ../stream/file.o ./socket.o ./tlssocket.o ./net.o
