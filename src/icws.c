#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <fcntl.h>
#include "pcsa_net.h"
#include "parse.h"


/* Rather arbitrary. In real life, be careful with buffer overflow */
#define MAXBUF 1024  

typedef struct sockaddr SA;

void respond_get(int connFd, char* rootFol, char* req_obj) {

    
    char loc[MAXBUF];                   // full file path
    char headr[MAXBUF];                 // buffer for header

    strcpy(loc, rootFol);               // loc = rootFol
    if (strcmp(req_obj, "/") == 0){     // if input == / then req_obj = /index
        req_obj = "/index.html";
    } 
    else if (req_obj[0] != '/'){      // add '/' if first char is not '/'
        strcat(loc, "/");   
    }
    strcat(loc, req_obj);
    printf(" File location is: %s \n", loc);
    int fd = open( loc , O_RDONLY);

    if (fd < 0){
        sprintf(headr, 
                "HTTP/1.1 404 not found\r\n"
                "Server: Micro\r\n"
                "Connection: close\r\n");
        write_all(connFd, headr, strlen(headr));
        return;
    }

    // ====================================
    struct stat st;
    fstat(fd, &st);
    size_t filesize = st.st_size;
    if (filesize < 0){
        printf("Filesize Error\n");
        close(fd);
        return ;
    }
    // ====================================
    // find extension
    char * ext = strchr(req_obj, '.') + 1;
    //printf("%s\n", ext);
    char * mime;
    
    // check extension
    if ( strcmp(ext, "html") == 0 )
        mime = "text/html";
    else if ( strcmp(ext, "jpg") == 0 )
        mime = "image/jpeg";
    else if ( strcmp(ext, "jpeg") == 0 )
        mime = "image/jpeg";
    else{
        mime = "null";
    }

    // ====================================
    if ( strcmp(mime, "null")==0){
        char * msg = "File type not supported\n";
        write_all(connFd, msg , strlen(msg) );
        close(fd);
        return;
    }
    sprintf(headr, 
            "HTTP/1.1 200 OK\r\n"
            "Server: Micro\r\n"
            "Connection: close\r\n"
            "Content-length: %lu\r\n"
            "Content-type: %s\r\n\r\n", filesize, mime);

    write_all(connFd, headr, strlen(headr));

    // ====================================

    char buf[filesize];
    ssize_t numRead;
    while ((numRead = read(fd, buf, MAXBUF)) > 0) {
        write_all(connFd, buf, numRead);
    }

    if ( (close(fd)) < 0 ){
        printf("Failed to close input file. Meh.\n");
    }    
}


void serve_http(int connFd, char* rootFol) {
    char buf[MAXBUF];

    int readRet = read(connFd, buf, MAXBUF);
    if (!readRet)
        return ;  /* Quit if we can't read the first line */

    printf("LOG: %s\n", buf);
    /* [METHOD] [URI] [HTTPVER] */
    // char method[MAXBUF], uri[MAXBUF], httpVer[MAXBUF];
    // sscanf(buf, "%s %s %s", method, uri, httpVer);
    // while (read_line(connFd, buf, MAXBUF)){
            // if (strcmp(buf, "\r\n") == 0){
                    // break;
            // }
            // printf("%s\n", buf);
    // }
    Request *request = parse(buf,readRet,connFd);

    if (strcasecmp(request->http_method, "GET") == 0 &&
        strcasecmp(request->http_version, "HTTP/1.1") == 0) {
        printf("LOG: %s\n", request->http_uri);
        respond_get(connFd, rootFol, request->http_uri);
    }
    else {
        printf("LOG: Unknown request\n");
    }
    
}

int main(int argc, char* argv[]) {
    int listenFd = open_listenfd(argv[1]);

    for (;;) {
        struct sockaddr_storage clientAddr;
        socklen_t clientLen = sizeof(struct sockaddr_storage);

        int connFd = accept(listenFd, (SA *) &clientAddr, &clientLen);
        if (connFd < 0) { fprintf(stderr, "Failed to accept\n"); continue; }

        char hostBuf[MAXBUF], svcBuf[MAXBUF];
        if (getnameinfo((SA *) &clientAddr, clientLen, 
                        hostBuf, MAXBUF, svcBuf, MAXBUF, 0)==0) 
            printf("Connection from %s:%s\n", hostBuf, svcBuf);
        else
            printf("Connection from ?UNKNOWN?\n");
                
        serve_http(connFd, argv[2]);
        close(connFd);
    }

    return 0;
}
