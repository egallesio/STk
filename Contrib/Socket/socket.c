/*
 * This file is a contribution of David Tolpin (dvd@pizza.msk.su)
 * It is an implementation of BSD-INET sockets and is known to run on 
 * Solaris 1 and Linux.
 *
 * (prepare-server-socket portnum) 
 *  bound socket to a local address. Returns an object of type socket-handle
 *
 * (release-server-socket! handle) 
 * close server socket (created by prepare-server-socket
 * 
 * (socket-handle? handle) 
 * returns truth if the handle is of type socket-handle, false otherwise
 * 
 * (listen-socket! handle) 
 * listen for connection requests 
 * 
 * (accept-connection handle) 
 * returns a new socket in response to a detected connection request,
 * the return value is a list of two ports, 
 *      - (car sp) is opened for reading, 
 *	- (cadr sp) - for writing
 * 
 * (open-client-socket hostname portnum) 
 * connect to a socket on a remote machine, returns the same data structure
 * as the function described above
 *
 * (shutdown-connection! skt)							
 * shutdown socket, the mode of shutting down is determined according to
 * the mode of the port (read or write)
 */

#include "stk.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

PRIMITIVE prepare_server_socket(SCM portnum);
PRIMITIVE release_server_socket(SCM handle);
PRIMITIVE socket_handlep(SCM handle);
PRIMITIVE listen_socket(SCM handle);
PRIMITIVE accept_connection(SCM handle);
PRIMITIVE open_client_socket(SCM hostname, SCM portnum);
PRIMITIVE shutdown_connection(SCM skt);

/*
: stk_socket.c,v 1.4 1994/06/26 19:14:55 dvd Exp dvd $
*/

/*
: stk_socket.c,v $
 * Revision 1.4  1994/06/26  19:14:55  dvd
 * *** empty log message ***
 *
 * Revision 1.3  1994/06/26  18:55:27  dvd
 * Verbose error reporting is added
 *
*/ 

#ifdef __sun__
extern char *sys_errlist[];
#endif

struct socket_handle {
	int portnum;
	char *hostname;
	int handle;
};

static int tc_sockhandle;

static void free_sockhandle(SCM handle);
static void mark_sockhandle(SCM handle);
static void displ_sockhandle(SCM x, FILE *f, int mode);

static extended_scheme_type sockhandle_type = {
	"sockhandle",		/* name */
	0, 			/* is_procp */
	mark_sockhandle, 	/* gc_mark_fct */
	free_sockhandle,	/* gc_free_fct */
	NULL,			/* apply_fct */
	displ_sockhandle	/* display_fct */
};


#define SOCKHANDLE(x)   ((struct socket_handle*)(x->storage_as.extension.data))
#define LSOCKHANDLE(x)  (x->storage_as.extension.data)
#define SOCKHANDLEP(x)  (TYPEP(x,tc_sockhandle))
#define NSOCKHANDLEP(x) (NTYPEP(x,tc_sockhandle))

void mark_sockhandle(SCM handle)
{
}

void free_sockhandle(SCM handle)
{
	struct socket_handle *sh;
	sh = SOCKHANDLE(handle);
	if(sh->hostname) free(sh->hostname);
	close(sh->handle);
	free(sh);
	LSOCKHANDLE(handle) = NULL;
}

void displ_sockhandle(SCM handle, FILE *f, int mode)
{
	struct socket_handle *sh;
	sh = SOCKHANDLE(handle);
	sprintf(tkbuffer, "#[socket-handle %s %i]", sh->hostname, sh->portnum);
	Puts(tkbuffer,f);
}
	

static SCM makesp(int s, char *hn, int portnum)
{
	int t;
	int hnlen;
	FILE *fs, *ft;
	SCM zs, zt;
	long flag;
	
	flag = no_interrupt(1);

	t = dup(s); /* duplicate handles so that we are able to access one socket channel */
		    /* via two scheme ports */
	if(!((fs = fdopen(s, "r")) && (ft = fdopen(s, "w"))))
		err("internal(makesp): cannot create ports", NIL);
	NEWCELL(zs, tc_iport); 
	NEWCELL(zt, tc_oport);
	zs->storage_as.port.f = fs; setbuf(fs, NULL);  /* unbuffered input/output */
	zt->storage_as.port.f = ft; setbuf(ft, NULL);
	zs->storage_as.port.name = (char*)must_malloc((hnlen = strlen(hn))+16);
	sprintf(zs->storage_as.port.name, "%s:%i(r)", hn, portnum);
	zt->storage_as.port.name = (char*)must_malloc((hnlen = strlen(hn))+16);
	sprintf(zt->storage_as.port.name, "%s:%i(w)", hn, portnum);
	
	no_interrupt(flag);
	return(cons(zs, cons(zt, NIL)));
}	

PRIMITIVE prepare_server_socket(SCM portnum)
{
	struct sockaddr_in sin;
	int s;
	long flag;
	SCM ys;

	if(NINTEGERP(portnum)) 
		err("not a port number", portnum);
	sin.sin_port = INTEGER(portnum);
	sin.sin_addr.s_addr = INADDR_ANY;
	if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err(sys_errlist[errno], portnum);
	if(bind(s, (struct sockaddr*)&sin, sizeof sin) < 0)
		switch(errno) {
		case EADDRINUSE:
		case EADDRNOTAVAIL: {
			SCM errcode;
			NEWCELL(errcode, tc_integer);
			SET_INTEGER(errcode, errno);
			return errcode;
		}
		break;
		default: err(sys_errlist[errno], portnum); 
		}
	/* now we're ready to create the object */
	NEWCELL(ys, tc_sockhandle);
	LSOCKHANDLE(ys) = (struct socket_handle*)must_malloc(sizeof (struct socket_handle));
	SOCKHANDLE(ys)->portnum = sin.sin_port;
	SOCKHANDLE(ys)->hostname = (char*)must_malloc(strlen("localhost")+1);
	strcpy(SOCKHANDLE(ys)->hostname, "localhost");
	SOCKHANDLE(ys)->handle = s;

	return ys;
}


PRIMITIVE release_server_socket(SCM handle)
{
	if(NSOCKHANDLEP(handle)) err("not a socket handle", handle);
	close(SOCKHANDLE(handle)->handle);
	return UNDEFINED;
}

PRIMITIVE socket_handlep(SCM handle)
{
	return SOCKHANDLEP(handle)? truth: ntruth;
}

PRIMITIVE listen_socket(SCM handle)
{
	if(NSOCKHANDLEP(handle)) 
		err("not a socket handle", handle);	
	if(listen(SOCKHANDLE(handle)->handle, 5) < 0) 
		err(sys_errlist[errno], handle); 
	return UNDEFINED;
}

PRIMITIVE accept_connection(SCM handle)
{
	int s;

	if(NSOCKHANDLEP(handle))
		err("not a socket handle", handle);	
	if((s = accept(SOCKHANDLE(handle)->handle, NULL, NULL)) < 0)
		err(sys_errlist[errno], handle); 
	return makesp(s, SOCKHANDLE(handle)->hostname, SOCKHANDLE(handle)->portnum);
}

PRIMITIVE open_client_socket(SCM hostname, SCM portnum)
{
	char *hn;
	struct hostent *hp;
	struct sockaddr_in server;
	int s;

	if(NSTRINGP(hostname)) err("bad hostname", hostname);
	if(NINTEGERP(portnum)) err("bad port number", portnum);
	hp = gethostbyname(hn = CHARS(hostname));
	if(!hp) err("unknown or misspelled host name", hostname);
	bzero((char*)&server,sizeof server);
	bcopy(hp->h_addr,(char*)&server.sin_addr, hp->h_length);
	server.sin_family = hp->h_addrtype;
	server.sin_port = INTEGER(portnum);
	if((s = socket(AF_INET,SOCK_STREAM,0)) < 0) 
		err(sys_errlist[errno], NIL);
	if(connect(s, (struct sockaddr *)&server, sizeof server) < 0)
		switch(errno) {
		case EADDRINUSE:
		case EADDRNOTAVAIL:
		case ETIMEDOUT:
		case ECONNREFUSED: {
			SCM errcode;
			NEWCELL(errcode, tc_integer);
			SET_INTEGER(errcode, errno);
			return errcode;
		}
		break;
		default: err(sys_errlist[errno], NIL);
		}
	return makesp(s, hn, server.sin_port);
}

PRIMITIVE shutdown_connection(SCM skt)
{
	if(NIPORTP(skt) && NOPORTP(skt)) 
		err("not a port", skt);
	if(shutdown(fileno(skt->storage_as.port.f), IPORTP(skt)?0:1) < 0) 
		err(sys_errlist[errno], NIL);
	return UNDEFINED;
}

void init_socket(void)
{
	tc_sockhandle = add_new_type(&sockhandle_type);

	add_new_primitive("prepare-server-socket", tc_subr_1,		prepare_server_socket);
	add_new_primitive("release-server-socket!", tc_subr_1,		release_server_socket);
	add_new_primitive("socket-handle?",	   tc_subr_1,		socket_handlep);
	add_new_primitive("listen-socket!",        tc_subr_1,		listen_socket);
	add_new_primitive("accept-connection",     tc_subr_1,		accept_connection);
	add_new_primitive("open-client-socket",    tc_subr_2,		open_client_socket);
	add_new_primitive("shutdown-connection!",  tc_subr_1,		shutdown_connection);
};

