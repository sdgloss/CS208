#include "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <time.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define _UNIX03_SOURCE


int socket(int domain, int type, int protocol);

int bind(int socket, const struct sockaddr *address,
       socklen_t address_len);

int setenv(const char *var_name, const char *new_value, int change_flag);

int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
void parse_uri(char *uri, char *filename, char *cgiargs, int* port);
void clienterror(int fd, char *cause, char *errnum,char *shortmsg, char *longmsg);
void serve_dynamic(int fd, char *filename, char *cgiargs);


/* suggested entry and cache structs for Part II */
typedef struct cache_entry {
    char *uri;
    char *item;
    struct cache_entry *next;
    size_t size;
} cache_entry_t;

typedef struct {
    cache_entry_t *head;
    size_t total_size;
} cache_t;


// For Part II: a global variable to point to the in-memory cache
cache_t *cache;

/* print out the contents of the cache */
void cache_print() {
    cache_entry_t *cur = cache->head;
    printf("current cache: (%zd)\n", cache->total_size);
    while(cur) {
        printf("%s (%zd)\n", cur->uri, cur->size);
        cur = cur->next;
    }
}

/* initialize the global cache variable (allocate memory and initialize fields) */
void cache_init() {
    cache_t *cache = malloc(sizeof(cache_entry_t));
    cache->head = NULL;
    cache->total_size = 0;
}

/* deallocate the entire cache (all the entries and the cache global variable) */
void cache_free() {
  cache_entry_t *holder = cache->head;
  while(holder != NULL){
        cache_entry_t *next = holder->next;  
        free(holder);
        holder = next;
  }
  free(cache);
}

/* search cache for an entry with a matching uri 
 * return a pointer to the matching entry or NULL if no matching entry is found
*/
cache_entry_t* cache_lookup(char *uri) {
    cache_entry_t *holder = cache->head;
    while (holder != NULL){
        if (holder->uri == uri){
          return holder;
        }
        holder = holder->next;   
    }
    return NULL;
}

/* insert a new entry at the head of the cache */
void cache_insert(char *uri, char *item, size_t size) {
    cache_entry_t *holder = malloc(sizeof(cache_entry_t));
    holder->uri =  uri;
    holder->item = item;
    holder->size = size;
    holder->next = cache->head;
    cache->head = holder;
    cache->total_size += sizeof(cache_entry_t);
}



/* gets a URI and a port. If no port, returns null. Updates the URI and returns the port. */
char* remove_port(char* uri){  
  const char cole[2] = ":";
  if (strstr(":", uri) == 0){
    char* holder = uri;
    char *token = strtok(holder, cole);
    uri = token;
    token = strtok(NULL, cole);
    return token;
    }
  return "80";
}

/* Implement this function for Part I
 * For Part III, you may need to change the parameter and return type of handle_request
 */

/* our attempt at caching is included in this function (commented out). It is not working properly, but we made some progress. */

void* handle_request(int connfd) {
    //we realized that the static and dynamic did not matter the way that we did it so we deleted them but that may cause more issues in compiling down the line. 
    pthread_detach(pthread_self());
    int port = 80;
    int opened;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE], header[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio, srio; //reads connfd.


    // Read request line and headers 
    Rio_readinitb(&rio, connfd);
    if (!Rio_readlineb(&rio, buf, MAXLINE)){
        return 0;}
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version); 
    if (strcasecmp(method, "GET")) {
        clienterror(connfd, method, "501", "Not Implemented", "Tiny does not implement this method");
        return 0;
    }

    
    /*cache_entry_t* lookup = cache_lookup(uri);
    if(lookup == NULL){
        cache_insert(uri, )*/
    parse_uri(uri, filename, cgiargs, &port); 
    build_header(header,cgiargs);
    char portStr[100];
    sprintf(portStr, "%d", port);
    opened = Open_clientfd(filename, portStr);
        
   /* }
    else{
        serve client using cache
        cache_entry_t* cache_info = cache_lookup(uri);
        get info from cache instead
        char* filename = cache_info->item;
        serve_dynamic or serve_static?
    } */


    Rio_readinitb(&srio,opened);
    Rio_writen(opened,header,strlen(header));

    size_t n;
    while((n=Rio_readlineb(&srio,buf,MAXLINE))!=0)
    {
        
        Rio_writen(connfd,buf,n);
    }
    Close(opened);
    return 0;
    }


/* given the URI and the filepath builds a system output */
void build_header(char *header,char *cgiargs){
    char request[MAXLINE],hold[MAXLINE];
    const char *get = "GET %s HTTP/1.0\r\n"; 
    const char *conn = "Connection: close\r\n";
    const char *proxy = "Proxy-Connection: close\r\n";
    const char *end = "\r\n";
    
    /*request line*/
    sprintf(request,get,cgiargs); //don't understand why this can't be apart of below sprintf but it seg fault if it is. 
    
    sprintf(header,"%s%s%s%s%s%s%s",request, conn, proxy, hold, end);

    return;
}

    
/*parse URI into filename and CGI args return 0 if dynamic content, 1 if static */

//we completely overhauled this method because the given code didn't work for us. found out that dynamic v static didn't matter. 
void parse_uri(char *uri, char *filename, char *cgiargs, int *port){
    char* ptr;
    char* ptr2;
    
    ptr = strstr(uri,"//");
    if(ptr == NULL){
        ptr = uri; //means there's no http so just start at beginning. 
    }
    else{
        ptr = ptr + 2; //move over by two
    }
    
    ptr2 = strstr(ptr,":");
    
    if(ptr2 != NULL){ //has a port
        *ptr2 = '\0'; //delimeter for sscanf. 
        ptr2 = ptr2 + 1; //moves the pointer over 1 spot. 
        sscanf(ptr2,"%d%s",port,cgiargs);
    }
    else{ //no port
        ptr2 = strstr(ptr,"/");
        if(ptr2!=NULL){
            *ptr2 = '/'; //delimeter. 
            sscanf(ptr2,"%s",cgiargs);
        }
    }
    
    sscanf(ptr,"%s",filename); //moves ptr into filename. 
    return;
}


/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Tiny Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}
/* $end clienterror */


int main(int argc, char **argv){
    long listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    cache_init();
    while (1){
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE,
                    port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);
        pthread_t tid;
        Pthread_create(&tid, NULL, handle_request, (void *)connfd);
    }
    cache_free();
    return 0;
}
