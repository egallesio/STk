/*
 * This file is based on a contribution of David Tolpin (dvd@pizza.msk.su)
 *
 * Bugs correction (conversion between host and network byte order) by
 * Marc Furrer (Marc.Furrer@di.epfl.ch)
 *
 * Reworked  by Erick Gallesio for 2.2 release. Some additions and simplifications
 * (I hope). 
 * 
 * Win32 support by Caleb Deupree <cdeupree@erinet.com>
 *
 *      Modified on: 27-Apr-1999 12:54 (Steve Pruitt <steve@pruitt.net>)
 * Last file update:  3-Sep-1999 19:46 (eg)
 *
 * For Win32 support the following changes were made:
 *
 *    1) re-arraged includes to remove define errors
 *
 *    2) added define for CLOSESOCKET      
 *
 *    3) Removed calls tcl_DeleteFileHandler and tclDeleteFileHandler
 *
 *    4) added new portable primitives: initialize-client-socket,
 *            accept-connection, close-connection, socket-handle, 
 *            socket-recv, socket-send and socket-peek
 */


#ifdef WIN32
#    define CLOSESOCKET  closesocket
#    include <fcntl.h>
#    include "stk.h"
#    include "stk.h"
#    define BAD_SOCKET(s) ((s) == INVALID_SOCKET)
#    ifndef _O_WRONLY
#      define _O_WRONLY O_WRONLY
#    endif
#    ifndef _O_RDONLY
#      define _O_RDONLY O_RDONLY
#    endif 
#else 
#  define CLOSESOCKET  close
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <memory.h>
#  include "stk.h"
#  define BAD_SOCKET(s) ((s) < 0)
#endif

#include <errno.h>

struct socket_type {
  int portnum;
  SCM hostname, hostip;
  int fd;
  SCM input, output;
  SCM ready_event;
};

static int tc_socket;

#define SOCKET(x)   ((struct socket_type*)(x->storage_as.extension.data))
#define LSOCKET(x)  (x->storage_as.extension.data)
#define SOCKETP(x)  (TYPEP(x,tc_socket))
#define NSOCKETP(x) (NTYPEP(x,tc_socket))

/******************************************************************************
 *
 *   U t i l i t i e s
 *
 ******************************************************************************/

static void system_error(char *who)
{
  char buffer[512]; /* should suffice */
  
  sprintf(buffer, "%s: %s", who, strerror(errno));
  Err(buffer, NIL);
}

static void socket_error(char *who, char *message, SCM object)
{
  char buffer[512]; /* should suffice */
  
  sprintf(buffer, "%s: %s", who, message);
  Err(buffer, object);
}

static void set_socket_io_ports(int s, SCM sock, char *who)
{
  int t, port;
  size_t len;
  char *hostname, *fname;
  FILE *fs= NULL, *ft = NULL;  /* initialization for GCC! */
  char buffer[200];

#ifdef WIN32
  {
    int r;

    if (((t = _open_osfhandle(s, _O_WRONLY)) == -1) || 
	((r = _open_osfhandle(s, _O_RDONLY)) == -1)) {
      sprintf(buffer, "%s: cannot open osfhandle", who);
      Err(buffer, NIL);
    }
    
    if(!((fs = fdopen(r, "r")) && (ft = fdopen(t, "w")))) {
      sprintf(buffer, "%s: cannot create socket io ports", who);
      Err(buffer, NIL);
    }
  }
#else
  t = dup(s); /* duplicate handles so that we are able to access one 
		 socket channel via two scheme ports */

  if (t == -1) {
    sprintf(buffer, "%s: cannot duplicate io port", who);
    Err(buffer,  STk_makeinteger(errno));
  }

  if(!((fs = fdopen(s, "r")) && (ft = fdopen(t, "w")))) {
    sprintf(buffer, "%s: cannot create socket io ports", who);
    Err(buffer, NIL);
  }
#endif

  port     = SOCKET(sock)->portnum;
  hostname = CHARS(SOCKET(sock)->hostname);
  len      = strlen(hostname) + 20;
  fname    = (char*) must_malloc(len);
  sprintf(fname, "%s:%d", hostname, port);

  /* Create input port */
  SOCKET(sock)->input = STk_Cfile2port(fname, fs, tc_iport, 0);

  /* Create output port */
  SOCKET(sock)->output = STk_Cfile2port(strdup(fname), ft, tc_oport, 0);
}

/******************************************************************************
 *
 *   m a k e - c l i e n t - s o c k e t
 *
 ******************************************************************************/
#ifdef WIN32
/* initialize_client_socket is identical to make_client_socket */
/* except that it does not call "set_socket_io_ports" */
static PRIMITIVE initialize_client_socket(SCM hostname, SCM port);
    
static PRIMITIVE make_client_socket(SCM hostname, SCM port) 
{ 
 SCM sock;
 sock = initialize_client_socket(hostname, port);
 set_socket_io_ports(SOCKET(sock)->fd, sock, "make-client-socket");
 return sock;
}
  
static PRIMITIVE initialize_client_socket(SCM hostname, SCM port)
#else
static PRIMITIVE make_client_socket(SCM hostname, SCM port)
#endif
{
  char str[] = "make-client-socket";
  struct hostent *hp;
  struct sockaddr_in server;
  int s;
  SCM z;

  /* Verify arguments */
  if(NSTRINGP(hostname)) 
    socket_error(str, "bad hostname", hostname);
  if(NINTEGERP(port))
    socket_error(str, "bad port number", port);

  /* Locate the host IP address */
  if ((hp=gethostbyname(CHARS(hostname))) == NULL)
    socket_error(str, "unknown or misspelled host name", hostname);

  /* Get a socket */
  if (BAD_SOCKET(s=socket(AF_INET,SOCK_STREAM,0)))
    socket_error(str, "cannot create socket", NIL);
  
  /* Setup a connect address */
  memset(&server, 0, sizeof(server));
  memcpy((char*)&server.sin_addr, hp->h_addr, hp->h_length);
  server.sin_family = AF_INET;
  server.sin_port   = htons(INTEGER(port));

  /* Try to connect */
  if (connect(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
    CLOSESOCKET(s);
    system_error(str);
  }

  /* Create a new Scheme socket object */
  NEWCELL(z, tc_socket);
  LSOCKET(z) = (struct socket_type*) 
    				must_malloc(sizeof (struct socket_type));

  SOCKET(z)->portnum	 = ntohs(server.sin_port); /* Query true value */
  SOCKET(z)->hostname    = STk_makestring((char *) hp->h_name);
  SOCKET(z)->hostip      = STk_makestring((char *) inet_ntoa(server.sin_addr));
  SOCKET(z)->fd 	 = s;
  SOCKET(z)->input 	 = Ntruth;
  SOCKET(z)->output 	 = Ntruth;
  SOCKET(z)->ready_event = Ntruth;
#ifndef WIN32
  set_socket_io_ports(s, z, str);
#endif
  return z;
}

/******************************************************************************
 *
 *   m a k e - s e r v e r - s o c k e t
 *
 ******************************************************************************/

static PRIMITIVE make_server_socket(SCM port)
{
  char msg[] = "make-server-socket";
  struct sockaddr_in sin;
  int s, portnum, len;
  SCM z;

  /* Determine port to use */
  portnum = (port == UNBOUND) ? 0 : STk_integer_value(port);
  if (portnum < 0)  Err("make-server-socket: bad port number", port);

  /* Create a socket */
  if (BAD_SOCKET(s = socket(AF_INET, SOCK_STREAM, 0)))
    Err("Cannot create socket", NIL);
  
  /* Bind the socket to a name */
  sin.sin_family      = AF_INET;
  sin.sin_port 	      = htons(portnum);
  sin.sin_addr.s_addr = INADDR_ANY;

  if (bind(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
    CLOSESOCKET(s);
    system_error(msg);
  }

  /* Query the socket name (permits to get the true socket number if 0 was given */
  len = sizeof(sin);
  if (getsockname(s, (struct sockaddr *) &sin, (int *) &len) < 0) {
    CLOSESOCKET(s);
    system_error(msg);
  }

  /* Indicate that we are ready to listen */
  if (listen(s, 5) < 0) {
    CLOSESOCKET(s);
    system_error(msg);
  }

  /* Now we can create the socket object */
  NEWCELL(z, tc_socket);
  LSOCKET(z) = (struct socket_type*) 
    				must_malloc(sizeof (struct socket_type));
  SOCKET(z)->portnum      = ntohs(sin.sin_port);
  SOCKET(z)->hostname     = Ntruth;
  SOCKET(z)->hostip	  = Ntruth;
  SOCKET(z)->fd	 	  = s;
  SOCKET(z)->input	  = Ntruth;
  SOCKET(z)->output	  = Ntruth;
  SOCKET(z)->ready_event  = Ntruth;

  return z;
}

/******************************************************************************
 *
 *   s o c k e t - a c c e p t - c o n n e c t i o n
 *
 ******************************************************************************/
#ifdef WIN32
/* accept_connection is identical to socket_accept_connection */
/* except that it does not call "set_socket_io_ports" and */
/* it also returns the accepted socket as a SCM integer */
static PRIMITIVE accept_connection(SCM sock);
  
static PRIMITIVE socket_accept_connection(SCM sock) 
{ 
 int new_s;
 new_s = INTEGER(accept_connection(sock));
 set_socket_io_ports(new_s , sock, "socket-accept-connection");
 return UNDEFINED;
}
  
static PRIMITIVE accept_connection(SCM sock)
#else
static PRIMITIVE socket_accept_connection(SCM sock) 
#endif
{
  char *s;
  char str[]= "socket-accept-connection";
  struct sockaddr_in sin;
  struct hostent *host;
  int len = sizeof(sin);
  int new_s;

  if (NSOCKETP(sock)) 
    socket_error(str, "bad socket", sock);

  if (BAD_SOCKET(new_s = accept(SOCKET(sock)->fd, (struct sockaddr *) &sin, &len)))
    system_error(str);
  
  /* Set the client info (if possible its name, otherwise its IP number) */
  host = gethostbyaddr((char *) &sin.sin_addr, sizeof(sin.sin_addr), AF_INET);
  s    = (char *) inet_ntoa(sin.sin_addr);
  
  SOCKET(sock)->hostip   = STk_makestring(s);
  SOCKET(sock)->hostname = STk_makestring(host? (char*) (host->h_name): s);

#ifdef WIN32
  return STk_makeinteger(new_s);
#else
  set_socket_io_ports(new_s, sock, str);
  return UNDEFINED;
#endif
}

/******************************************************************************
 *
 *   w h e n - s o c k e t - r e a d y 
 *
 ******************************************************************************/
#ifdef WIN32
static PRIMITIVE when_socket_ready(SCM s, SCM closure)
{
  /* Removal of tcl_DeleteFileHandler and tclDeleteFileHandler */ 
  /* in Tcl/Tk 8.0 make when_socket_ready impossible on Win32. */
  /* Will be available again in Tcl/Tk version 8.1    FIXME    */
  Err("when-socket-ready: cannot be used with Win32", NIL);
  return UNDEFINED;
}
#else
static void apply_socket_closure(SCM closure)
{
  Apply0(closure);
}
 
static PRIMITIVE when_socket_ready(SCM s, SCM closure)
{
  int fd;

  if (NSOCKETP(s))
    Err("when-socket-ready: bad socket", s);
  
  if (closure == UNBOUND) {
    /* Return the current handler closure */
    return SOCKET(s)->ready_event;
  }
  
  fd = SOCKET(s)->fd;
 
  if (closure == Ntruth) {
    Tcl_DeleteFileHandler(fd);    
    SOCKET(s)->ready_event = Ntruth;
  }
  else {
    if (STk_procedurep(closure) == Ntruth) 
      Err("when-socket-ready: bad closure", closure);

    Tcl_CreateFileHandler(fd, TCL_READABLE, (Tcl_FileProc *) apply_socket_closure, 
			  (ClientData) closure);
    SOCKET(s)->ready_event = closure;
  }
  return UNDEFINED;
}
#endif

static PRIMITIVE buggy_handler(SCM s, SCM closure)
{
  Err("when-socket-ready: cannot be used with snow", NIL);
  return UNDEFINED;
}

/******************************************************************************
 *
 *   s o c k e t - s h u t d o w n
 *
 ******************************************************************************/

static PRIMITIVE socket_shutdown(SCM sock, SCM close_socket)
{
  if (close_socket == UNBOUND) close_socket = Truth;

  if (NSOCKETP(sock)) 	       Err("socket-shutdown: bad socket", sock);
  if (NBOOLEANP(close_socket)) Err("socket-shutdown: bad boolean", close_socket);

  if (close_socket == Truth && SOCKET(sock)->fd > 0) {
    int fd = SOCKET(sock)->fd;

#ifndef WIN32
    if (!STk_snow_is_running)
      /* We cannot use #ifdef USE_TK here to have the same socket.so
       * for both snow and stk. So we have to test if we are running 
       * snow dynamically
       */
      Tcl_DeleteFileHandler(fd);
#endif

    /* close(fd); */
    shutdown(fd, 2);
    SOCKET(sock)->fd = -1;
  }
 
  /* 
   * Warning: input and output can have already be garbaged :if the socket is 
   * no more used, the input and output are not marked as used and can
   * (eventually) be released before the call to shutdown (through free_socket)
   * be done. One way could be to just set SOCKET(sock)->{in|out}put to #t
   * and wait that next GC frees the ports if not already down. However,
   * this will really disconnect the peer when the GC occurs rather than when
   * the call to shutdown is done. This is not important if this function 
   * is called by the GC, but could be annoying when it is called by the user
   */

  if (INP(SOCKET(sock)->input)) {
    STk_close_port(SOCKET(sock)->input);
    SOCKET(sock)->input = Ntruth;
  }
  if (OUTP(SOCKET(sock)->output)) {
    STk_close_port(SOCKET(sock)->output);
    SOCKET(sock)->output = Ntruth;
  }

  return UNDEFINED;
}

/******************************************************************************
 *
 *   O t h e r   s o c k e t   p r i m i t i v e s
 *
 ******************************************************************************/

static PRIMITIVE socketp(SCM sock)
{
  return SOCKETP(sock)? Truth: Ntruth;
}

static PRIMITIVE socket_port_number(SCM sock)
{
  if (NSOCKETP(sock)) Err("socket-port-number: bad socket", sock);
  return STk_makeinteger(SOCKET(sock)->portnum);
}

static PRIMITIVE socket_input(SCM sock)
{
  if (NSOCKETP(sock)) Err("socket-input: bad socket", sock);
  return SOCKET(sock)->input;
}

static PRIMITIVE socket_output(SCM sock)
{
  if (NSOCKETP(sock)) Err("socket-output: bad socket", sock);
  return SOCKET(sock)->output;
}

static PRIMITIVE socket_hostname(SCM sock)
{
  if (NSOCKETP(sock)) Err("socket-hostname: bad socket", sock);
  return SOCKET(sock)->hostname;
}

static PRIMITIVE socket_host_address(SCM sock)
{
  if (NSOCKETP(sock)) Err("socket-host-address: bad socket", sock);
  return SOCKET(sock)->hostip;
}

static PRIMITIVE socket_downp(SCM sock)
{
  if (NSOCKETP(sock)) Err("socket-down?: bad socket", sock);
  return (SOCKET(sock)->fd == -1) ? Truth: Ntruth;
}

static PRIMITIVE socket_dup(SCM socket)
{
  SCM z;
  int new_fd;
#ifdef WIN32
  HANDLE process;
#endif

  if (NSOCKETP(socket)) Err("socket-dup: bad socket", socket);

#ifdef WIN32
  process = GetCurrentProcess();
  if (!DuplicateHandle(process, 
		       (HANDLE) SOCKET(socket)->fd, process,
		       (HANDLE*) &new_fd,
		       0, 
		       TRUE, 
		       DUPLICATE_SAME_ACCESS))
    Err("socket-dup: cannot duplicate socket", STk_makeinteger(GetLastError()));
#else
  if ((new_fd=dup(SOCKET(socket)->fd)) < 0)
    Err("socket-dup: cannot duplicate socket", socket);
#endif
  
  NEWCELL(z, tc_socket);
  LSOCKET(z) = (struct socket_type*) must_malloc(sizeof (struct socket_type));

  *SOCKET(z) = *SOCKET(socket);
  SOCKET(z)->fd = new_fd;  

  return z;
}


static PRIMITIVE socket_local_addr(SCM sock)
{
  struct sockaddr_in sin;
  int len = sizeof(sin);

  if (NSOCKETP(sock)) Err("socket-local-address: bad socket", sock);

  if (getsockname(SOCKET(sock)->fd, (struct sockaddr *) &sin, &len))
    Err("socket-local-address: cannot get socket name", sock);

  return STk_makestring((char *) inet_ntoa(sin.sin_addr));
}
  

/******************************************************************************/


static void mark_socket(SCM sock)
{
  STk_gc_mark(SOCKET(sock)->hostname);
  STk_gc_mark(SOCKET(sock)->hostip);
  STk_gc_mark(SOCKET(sock)->input);
  STk_gc_mark(SOCKET(sock)->output);
  STk_gc_mark(SOCKET(sock)->ready_event);
}

static void free_socket(SCM sock)
{
  socket_shutdown(sock, Truth);
  free(SOCKET(sock));
}

static void displ_socket(SCM sock, SCM port, int mode)
{
  struct socket_type *s = SOCKET(sock);

  sprintf(STk_tkbuffer, "#[socket %s %d]", 
	  		(s->hostname == Ntruth)?"*none*": CHARS(s->hostname),
	  		s->portnum);
  Puts(STk_tkbuffer, port);
}

static STk_extended_scheme_type socket_type = {
  "socket",		/* name */
  0, 			/* is_procp */
  mark_socket, 		/* gc_mark_fct */
  free_socket,		/* gc_free_fct */
  NULL,			/* apply_fct */
  displ_socket,		/* display_fct */
  NULL			/* compare function */
};

#ifdef WIN32
/*******************************************************************************
 *
 * These primitives allow WIN32 apps to work when the 
 * socket io-ports are not available. By adding Primitives
 * to winsock functions "send" and "recv", sockets can be
 * operated on directly. 
 *
 ******************************************************************************/

static PRIMITIVE close_connection(SCM connection)
{
  if (NINTEGERP(connection)) 
	Err("close_connection: bad connection", connection);
  CLOSESOCKET(INTEGER(connection));
  return UNDEFINED;
}

/******************************************************************************/

static PRIMITIVE socket_handle(SCM sock)
{
  if (NSOCKETP(sock)) Err("socket-handle: bad socket", sock);
  return STk_makeinteger(SOCKET(sock)->fd);
}

/******************************************************************************/

static PRIMITIVE socket_peek(SCM fd, SCM num_chars) {
 int i, len;
 SCM chars;
 char *s;
 len = INTEGER(num_chars);
 chars = STk_makestrg(len+1, NULL);
 s = chars->storage_as.string.data;
 i = recv(INTEGER(fd), s, len, MSG_PEEK);
 return chars;
}

/******************************************************************************/

static PRIMITIVE socket_recv(SCM fd, SCM buffer) {
 int len, result;
 char *s;
 s = buffer->storage_as.string.data;
 len = buffer->storage_as.string.dim - 1;
 result = recv(INTEGER(fd), s, len, 0);
 if (result <= 0) return STk_eof_object;
 return STk_makeinteger(result);
}

/******************************************************************************/

static PRIMITIVE socket_send(SCM fd, SCM buffer, SCM num_chars) {
 int len, result;
 char *s;
 len = INTEGER(num_chars);
 s = buffer->storage_as.string.data;
 result = send(INTEGER(fd), s, len, 0);
 return STk_makeinteger(result);
}
#endif

/******************************************************************************/

PRIMITIVE STk_init_socket(void)
{
#ifdef WIN32
  int rc;
  WSADATA wsadata;
  int optionValue = SO_OPENTYPE;
    
  if ((rc = WSAStartup(MAKEWORD(1,1), &wsadata)) != 0) {
    Err("Socket initialisation failed", STk_makeinteger(rc));
  }

  /* All sockets have to be synchronous to be readable with stdio. */
  if ((rc = setsockopt(INVALID_SOCKET, SOL_SOCKET, SO_OPENTYPE, 
                       (char *)&optionValue, sizeof(optionValue))) != NO_ERROR) {
    /* failed for some reason... */
    Err("Synchronous socket option setting failed", 
	STk_makeinteger(WSAGetLastError()));
  }

  /* new primitives for send and recv without using stdio */
  STk_add_new_primitive("accept-connection", tc_subr_1, accept_connection);
  STk_add_new_primitive("close-connection", tc_subr_1, close_connection);
  STk_add_new_primitive("socket-handle", tc_subr_1, socket_handle);
  STk_add_new_primitive("socket-peek", tc_subr_2, socket_peek);
  STk_add_new_primitive("socket-recv", tc_subr_2, socket_recv);
  STk_add_new_primitive("socket-send", tc_subr_3, socket_send);
  STk_add_new_primitive("initialize-client-socket", tc_subr_2, 
					initialize_client_socket);
#endif

  tc_socket = STk_add_new_type(&socket_type);

  STk_add_new_primitive("make-client-socket",  tc_subr_2,      make_client_socket);
  STk_add_new_primitive("make-server-socket",  tc_subr_0_or_1, make_server_socket);
  STk_add_new_primitive("socket-accept-connection",
		    		               tc_subr_1, socket_accept_connection);
  STk_add_new_primitive("socket?",	       tc_subr_1,      socketp);
  STk_add_new_primitive("socket-port-number",  tc_subr_1,      socket_port_number);
  STk_add_new_primitive("socket-input",        tc_subr_1,      socket_input);
  STk_add_new_primitive("socket-output",       tc_subr_1,      socket_output);
  STk_add_new_primitive("socket-host-name",    tc_subr_1,      socket_hostname);
  STk_add_new_primitive("socket-host-address", tc_subr_1,      socket_host_address);
  STk_add_new_primitive("socket-shutdown",     tc_subr_1_or_2, socket_shutdown);
  STk_add_new_primitive("socket-down?",        tc_subr_1,      socket_downp);
  STk_add_new_primitive("socket-local-address",tc_subr_1,      socket_local_addr);
  STk_add_new_primitive("socket-dup", 	       tc_subr_1,      socket_dup);

  STk_add_new_primitive("when-socket-ready",   tc_subr_1_or_2, 
			(STk_snow_is_running)? buggy_handler:  when_socket_ready);

  return UNDEFINED;
}
