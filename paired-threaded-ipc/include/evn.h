#ifndef _EVN_H_
#define _EVN_H_

#include <sys/time.h> // struct timeval

// Network Stuff
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>

#include <ev.h>

#include "buffer-list.h"
#include "bool.h"

#define EVN_DEBUG 1

#ifndef EVN_DEBUG
  #if defined DEBUG
    #define EVN_DEBUG 1
  #else
    #define EVN_DEBUG 0
  #endif
#endif

#if EVN_DEBUG
  #include <stdio.h>
  #define evn_debug(...) printf("[EVN] " __VA_ARGS__)
  #define evn_debugs(...) puts("[EVN] " __VA_ARGS__)
#else
  #define evn_debug(...)
  #define evn_debugs(...)
#endif


struct evn_stream;
struct evn_server;
struct evn_exception;

#define EVN_SERVER_P struct evn_server* server
#define EVN_STREAM_P struct evn_stream* stream

// Server Callbacks
typedef void (evn_server_listen_cb)(EV_P_ struct evn_server* server);
typedef void (evn_server_connection_cb)(EV_P_ struct evn_server* server, struct evn_stream* stream);
typedef void (evn_server_close_cb)(EV_P_ struct evn_server* server);

// Client Callbacks
typedef void (evn_stream_connect_cb)(EV_P_ struct evn_stream* stream);
typedef void (evn_stream_secure_cb)(EV_P_ struct evn_stream* stream);
typedef void (evn_stream_data_cb)(EV_P_ struct evn_stream* stream, void* blob, int size);
typedef void (evn_stream_end_cb)(EV_P_ struct evn_stream* stream);
typedef void (evn_stream_timeout_cb)(EV_P_ struct evn_stream* stream);
typedef void (evn_stream_drain_cb)(EV_P_ struct evn_stream* stream);
typedef void (evn_stream_error_cb)(EV_P_ struct evn_stream* stream, struct evn_exception* error);
typedef void (evn_stream_close_cb)(EV_P_ struct evn_stream* stream, bool had_error);

struct evn_exception {
  int error_number;
  char message[256];
};

struct evn_server {
  ev_io io;
  int fd;
  EV_P;
  evn_server_listen_cb* listen;
  evn_server_connection_cb* connection;
  evn_server_close_cb* close;
  struct sockaddr_un socket;
  int socket_len;
  //array streams;
};

struct evn_stream {
  ev_io io;
  int fd;
  evn_stream_connect_cb* connect;
  evn_stream_secure_cb* secure;
  evn_stream_data_cb* data;
  evn_stream_end_cb* end;
  evn_stream_timeout_cb* timeout;
  evn_stream_drain_cb* drain;
  evn_stream_error_cb* error;
  evn_stream_close_cb* close;
  int index;
  bool oneshot;
  evn_bufferlist* bufferlist;
  struct evn_server* server;
  EV_P;
  char type;
};


int evn_set_nonblock(int fd);
struct evn_stream* evn_stream_create(int fd);
struct evn_server* evn_server_create(EV_P_ char* sock_path, int max_queue);

int evn_server_destroy(EV_P_ struct evn_server* server);
int evn_stream_destroy(EV_P_ struct evn_stream* stream);

void evn_server_connection_priv_cb(EV_P_ ev_io *w, int revents);
void evn_stream_read_priv_cb(EV_P_ ev_io *w, int revents);

int evn_server_listen(struct evn_server* server);
#endif
