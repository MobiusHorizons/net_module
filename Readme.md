# net

The network module provides an interface for setting up and tearing down secure and unsecure sockets.


# Installation

To install, add this repository as a submodule of your project under a `deps` directory.
Then the module can be imported into your project as seen below.

```
import net from "deps/net/net.module.c"
```

This library depends on the [stream](http://github.com/MobiusHorizons/stream_module), which can be installed in the same way.

## LibreSSL

This library uses `libtls` provided by **LibreSSL**. The default settings of the library look for this library in the usual places as well as under **HOMEBREW** for OSX. If you have installed the library in a non-standard place, or if it can't be found, you can set the following variables with the correct locations

```sh
LIBRESSL_INCLUDE_DIR  # sets the include directory for finding LibreSSL
LIBRESSL_LIB_DIR      # sets the lib directory for finding LibreSSL
```

## root.pem

At the current time, this library expects the file `root.pem` to be available in the working
directory to run the program. This is clearly not ideal, and will be updated in the future. The version
of the file included with the library was downloaded from [firefox's certificates](https://hg.mozilla.org/releases/mozilla-release/raw-file/default/security/nss/lib/ckfw/builtins/certdata.txt).

# API

The API is super simple, it consists of two functions:

## `connect`
> `stream.t connect(const char * host, int port, bool use_tls)`

This function allows you to open a new connection by just providing a `host` and `port`.
You can easily choose to use `tls` or raw native sockets just based on the `use_tls` boolean flag.

The function returns a [stream](https://github.com/MobiusHorizons/stream_module) that can be used to both read and write on the connection. Any errors that occur in the network stack are exposed through the error handling on the stream so for example you can check for a failed connection like so:

```c
// error-test.module.c
package "main";

#include <stdio.h>
#include <stdbool.h>

import stream from "deps/stream/stream.module.c";
import net    from "deps/net/net.module.c";

int main(){
  stream.t conn = net.connect("invalid.example.com", 1234, false);
  if (conn.error.code != 0){
    fprintf(stderr, "Error Connecting: %s\n", conn.error.message);
    return -1;
  }

  // ... if it hasn't failed we could use our connection
}
```

which would return the error: `Error Connecting: Unknown host`


## `hangup`
> `int net_hangup(stream.t conn)`

This function will shutdown the connection and close out the underlying file pointers.
Any errors which occur will cause a non-zero return code, and set the `message` and `code` fields of the the stream's error struct.
