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
#define MAXBUF 8192

typedef struct sockaddr SA;

char * getExt(char * fileName){
        char * copyFileName = fileName;
        char * currentFound;
        
        while ((currentFound = strchr(copyFileName, '.')) != NULL){
                printf("passed first\n");
                char * token;
                if (strlen(currentFound) != 1){
                    token = strchr(currentFound, '.') + 1;
                    copyFileName = token;
                }
                
        }
        return copyFileName;

}

char* check_mime(char* ext){
    if ( strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0 )
        return "text/html";
    else if ( strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0)
        return "image/jpeg";
    else if ( strcmp(ext, "css") == 0 )
        return "text/css";
    else if ( strcmp(ext, "csv") == 0 )
        return "text/csv";
    else if ( strcmp(ext, "txt") == 0 )
        return "text/plain";
    else if ( strcmp(ext, "png") == 0 )
        return "image/png";
    else if ( strcmp(ext, "gif") == 0 )
        return "image/gif";
    
    else if ( strcmp(ext, "acc") == 0 )
        return "audio/acc";
    else if ( strcmp(ext, "abw") == 0 )
        return "application/x-abiword";
    else if ( strcmp(ext, "arc") == 0 )
        return "application/x-freearc";
    else if ( strcmp(ext, "avi") == 0 )
        return "video/x-msvideo";
    else if ( strcmp(ext, "azw") == 0 )
        return "application/vnd.amazon.ebook";
    else if ( strcmp(ext, "bin") == 0 )
        return "application/octet-stream";
    else if ( strcmp(ext, "bmp") == 0 )
        return "image/bmp";
    else if ( strcmp(ext, "bz") == 0 )
        return "application/x-bzip";
    else if ( strcmp(ext, "bz2") == 0 )
        return "application/x-bzip";
    else if ( strcmp(ext, "cda") == 0 )
        return "application/x-cda";
    else if ( strcmp(ext, "doc") == 0 )
        return "application/msword";
    else if ( strcmp(ext, "docx") == 0 )
        return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    else if ( strcmp(ext, "eot") == 0 )
        return "application/vnd.ms-fontobject";
    else if ( strcmp(ext, "epub") == 0 )
        return "application/epub+zip";
    else if ( strcmp(ext, "gz") == 0 )
        return "application/gzip";
    else if ( strcmp(ext, "ico") == 0 )
        return "image/vnd.microsoft.icon";
    else if ( strcmp(ext, "ics") == 0 )
        return "text/calendar";
    else if ( strcmp(ext, "jar") == 0 )
        return "application/java-archive";

    else if ( strcmp(ext, "js") == 0 )
        return "application/javascript";

    else if ( strcmp(ext, "json") == 0 )
        return "application/json";
    else if ( strcmp(ext, "jsonld") == 0 )
        return "application/ld+json";
    else if ( strcmp(ext, "mid") == 0 || strcmp(ext, "midi") == 0)
        return "audio/midi"; // "audio/x-midi"
    else if ( strcmp(ext, "mjs") == 0 )
        return "text/javascript";
    else if ( strcmp(ext, "mp3") == 0 )
        return "audio/mpeg";
    else if ( strcmp(ext, "mp4") == 0 )
        return "video/mp4";
    else if ( strcmp(ext, "mpeg") == 0 )
        return "video/mpeg";
    else if ( strcmp(ext, "mpkg") == 0 )
        return "application/vnd.apple.installer+xml";
    else if ( strcmp(ext, "odp") == 0 )
        return "application/vnd.oasis.opendocument.presentation";
    else if ( strcmp(ext, "ods") == 0 )
        return "vnd.oasis.opendocument.spreadsheet";
    else if ( strcmp(ext, "odt") == 0 )
        return "vnd.oasis.opendocument.txt";
    else if ( strcmp(ext, "oga") == 0 )
        return "audio/ogg";
    else if ( strcmp(ext, "ogv") == 0 )
        return "video/ogg";
    else if ( strcmp(ext, "ogx") == 0 )
        return "application/ogg";
    else if ( strcmp(ext, "opus") == 0 )
        return "audio/opus";
    else if ( strcmp(ext, "otf") == 0 )
        return "font/otf";
    else if ( strcmp(ext, "pdf") == 0 )
        return "application/pdf";
    else if ( strcmp(ext, "php") == 0 )
        return "application/x-httpd-php";
    else if ( strcmp(ext, "ppt") == 0 )
        return "application/vnd.ms-powerpoint";
    else if ( strcmp(ext, "pptx") == 0 )
        return "application/vnd.openxmlformats-officedocument.presentational.presentation";
    else if ( strcmp(ext, "rar") == 0 )
        return "application/vnd.rar";
    else if ( strcmp(ext, "rtf") == 0 )
        return "application/rtf";
    else if ( strcmp(ext, "sh") == 0 )
        return "application/x-sh";
    else if ( strcmp(ext, "svg") == 0 )
        return "image/svg+xml";
    else if ( strcmp(ext, "swf") == 0 )
        return "application/x-shockwave-flash";
    else if ( strcmp(ext, "tar") == 0 )
        return "application/x-tar";
    else if ( strcmp(ext, "tif") == 0 || strcmp(ext, "tiff") == 0 )
        return "image/tiff";
    else if ( strcmp(ext, "ts") == 0 )
        return "video/mp2t";
    else if ( strcmp(ext, "ttf") == 0 )
        return "font/ttf";
    else if ( strcmp(ext, "vsd") == 0 )
        return "application/vnd.visio";
    else if ( strcmp(ext, "wav") == 0 )
        return "audio/wav";
    else if ( strcmp(ext, "weba") == 0 )
        return "audio/webm";
    else if ( strcmp(ext, "webm") == 0 )
        return "video/webm";
    else if ( strcmp(ext, "webp") == 0 )
        return "image/webp";
    else if ( strcmp(ext, "woff") == 0 )
        return "font/woff";
    else if ( strcmp(ext, "woff2") == 0 )
        return "font/woff2";
    else if ( strcmp(ext, "xhtml") == 0 )
        return "application/xhtml+xml";
    else if ( strcmp(ext, "xls") == 0 )
        return "application/vnd.ms-exel";
    else if ( strcmp(ext, "xlsx") == 0 )
        return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    
    else if ( strcmp(ext, "xml") == 0 )
        return "application/xml"; // text/xml if human readable

    else if ( strcmp(ext, "xul") == 0 )
        return "application/vnd.mozilla.xul+xml";
    else if ( strcmp(ext, "zip") == 0 )
        return "application/zip";

    else if ( strcmp(ext, "3gp") == 0 )
        return "video/3gpp"; // audio/3gpp if doesnt contain video
    else if ( strcmp(ext, ".3g2") == 0 )
        return "video/3gpp2"; // audio/3gpp2 if doesnt contain video

    else if ( strcmp(ext, "7z") == 0 )
        return "application/x-7z-compressed";

    else{
        return "null";
    }

}


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

    printf("File location is: %s \n", loc);
    int fd = open( loc , O_RDONLY);

    if (fd < 0){
        sprintf(headr, 
                "HTTP/1.1 404 not found\r\n"
                "Server: ICWS\r\n"
                "Connection: close\r\n");
        write_all(connFd, headr, strlen(headr));
        return;
    }

    struct stat st;
    fstat(fd, &st);
    size_t filesize = st.st_size;
    if (filesize < 0){
        printf("Filesize Error\n");
        close(fd);
        return ;
    }
    // find extension
    char * ext = getExt(req_obj);
    char * mime = check_mime(ext);
    printf("mime: %s\n", mime);

    if ( strcmp(mime, "null")==0){
        sprintf(headr, 
                        "HTTP/1.1 400 Bad Request\r\n"
                        "Server: ICWS\r\n"
                        "Connection: close\r\n");
        write_all(connFd, headr , strlen(headr) );
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

    char buf[MAXBUF];
    ssize_t numRead;
    // printf("passed here\n");
    while ((numRead = read(fd, buf, MAXBUF)) > 0) {
        write_all(connFd, buf, numRead);
    }

    if ( (close(fd)) < 0 ){
        printf("Failed to close input file. Meh.\n");
    }    
}


void serve_http(int connFd, char* rootFol) {
    char buf[MAXBUF];
    char lineByline[MAXBUF];
    int readRet;
    int currentRead;


    while ((currentRead = read_line(connFd,lineByline,MAXBUF)) > 0){
            strcat(buf, lineByline);
            readRet += currentRead;
            if (strcmp(lineByline, "\r\n") == 0){
                    break;
            }
    }
    printf("LOG: %s\n", buf);
    Request *request = parse(buf,readRet,connFd);

    
    if (strcasecmp(request->http_version, "HTTP/1.1") == 0) {
            if (strcasecmp(request->http_method, "GET") == 0 &&
                            strcasecmp(request->http_version, "HTTP/1.1") == 0) {
                    printf("LOG from Get: %s\n", request->http_uri);
                    respond_get(connFd, rootFol, request->http_uri);
            }
            else if (strcasecmp(request->http_method, "HEAD") == 0 &&
                            strcasecmp(request->http_version, "HTTP/1.1") == 0) {
                    printf("LOG from HEAD: %s\n", request->http_uri);
                    respond_get(connFd, rootFol, request->http_uri);
            }
    }
    else {
        char headr[MAXBUF];
        if (strcmp(request->http_version, "HTTP/1.1")){
                sprintf(headr, 
                        "HTTP/1.1 505 HTTP version not supported\r\n"
                        "Server: ICWS\r\n"
                        "Connection: close\r\n");
        }
        else{
                sprintf(headr, 
                        "HTTP/1.1 501 not implemented\r\n"
                        "Server: ICWS\r\n"
                        "Connection: close\r\n");
        }
        write_all(connFd, headr, strlen(headr));
    }
    free(request->headers);
    free(request);
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
