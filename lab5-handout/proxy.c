#include "csapp.h"

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
    
}

/* deallocate the entire cache (all the entries and the cache global variable) */
void cache_free() {

}

/* search cache for an entry with a matching uri 
 * return a pointer to the matching entry or NULL if no matching entry is found
*/
cache_entry_t* cache_lookup(char *uri) {
    return NULL
}

/* insert a new entry at the head of the cache */
void cache_insert(char *uri, char *item, size_t size) {
    
}

/* Implement this function for Part I
 * For Part III, you may need to change the parameter and return type of handle_request
 */
void handle_request(int connfd) {
    // read and parse request
    // hint: the initial part of the doit function in tiny/tiny.c may be a good starting point


    // parse URI for hostname, port, and filename, then open a socket on that port and hostname
    

    // send request to server as we read it from client (think echo)
    // a request consists of a GET line and zero or more headers
    // a blank line indicates the end of the request headers
    // remember: line endings/blank lines are "\r\n"
    

    // read the response from the server
    // remember: response headers, then a blank line, then the response body
}

int main(int argc, char **argv) {
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE,
                    port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);

        // For Part III, replace this with code that creates and detaches a thread
        // or otherwise handles the request concurrently
        handle_request(connfd);
    }
}

