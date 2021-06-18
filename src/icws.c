#include <strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
#include<unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "pcsa_net.h"
#include "parse.h"


/* Rather arbitrary. In real life, be careful with buffer overflow */
#define MAXBUF 8192
#define MAXQ 256

// define color in shell
#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define PURPLE "\033[0;35m"
#define RESET "\e[0m"

typedef struct sockaddr SA;
typedef struct survival_bag {
        struct sockaddr_storage clientAddr;
        int connFd;
} threadContent;

char * dirName;

// for thread pool;
threadContent JobQueue[MAXQ];
int JobCount = 0;
int threadNum;

pthread_mutex_t mutexQueue;
// for parse
pthread_mutex_t parseLock;
// conditional variables
pthread_cond_t condQueue;


char * getCurrentTime(){
        time_t rawTime;
        time(&rawTime);
        return strdup(asctime(localtime(&rawTime)));
}

char * getExt(char * fileName){
        char * copyFileName = fileName;
        char * currentFound;
        
        while ((currentFound = strchr(copyFileName, '.')) != NULL){
                // printf("passed first\n");
                char * token;
                if (strlen(currentFound) != 1){
                    token = strchr(currentFound, '.') + 1;
                    copyFileName = token;
                }
                
        }
        return copyFileName;

}

char * check_mime(char* ext){
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
// excecute
void respond_get(int connFd, char* req_obj, int isHEAD) {
    char * cuurentDate = getCurrentTime();
    char loc[MAXBUF];                   
    char headr[MAXBUF];                

    strcpy(loc, dirName);               
    if (strcmp(req_obj, "/") == 0){     
        req_obj = "/index.html";
    } 
    else if (req_obj[0] != '/'){      
        strcat(loc, "/");   
    }
    strcat(loc, req_obj);

    int fd = open( loc , O_RDONLY);

    if (fd < 0){
        sprintf(headr, 
                "HTTP/1.1 404 not found\r\n"
                "Server: ICWS\r\n"
                "Connection: close\r\n"
                "Date: %s\r\n\r\n", cuurentDate);
        write_all(connFd, headr, strlen(headr));
        return;
    }

    struct stat attr;
    fstat(fd, &attr);
    size_t filesize = attr.st_size;
    char * modTime = ctime(&attr.st_mtime);

    // find extension
    char * ext = getExt(req_obj);
    char * mime = check_mime(ext);
    // printf("mime: %s\n", mime);

    if (filesize < 0 || strcmp(mime, "null") == 0){
        sprintf(headr, 
                        "HTTP/1.1 400 Bad Request\r\n"
                        "Server: ICWS\r\n"
                        "Connection: close\r\n"
                        "Date: %s\r\n\r\n", cuurentDate);
        write_all(connFd, headr , strlen(headr) );
        close(fd);
        return;
    }

    sprintf(headr, 
            "HTTP/1.1 200 OK\r\n"
            "Server: Micro\r\n"
            "Connection: close\r\n"
            "Content-length: %lu\r\n"
            "Content-type: %s\r\n"
            "Last modified date: %s"
            "Date: %s\r\n\r\n", filesize, mime, modTime, cuurentDate);

    write_all(connFd, headr, strlen(headr));


    if (!isHEAD){
            char buf[MAXBUF];
            ssize_t numRead;
            while ((numRead = read(fd, buf, MAXBUF)) > 0) {
                    write_all(connFd, buf, numRead);
            }
    }

    if ( (close(fd)) < 0 ){
        printf("Failed to close input file.\n");
    }    
}


void serve_http(int connFd) {
    char * currentDate = getCurrentTime();
    char buf[MAXBUF];
    char lineByline[MAXBUF];
    memset(buf, 0, MAXBUF);
    memset(lineByline, 0, MAXBUF);
    int readRet = 0;
    int currentRead;

    // read request
    while ((currentRead = read(connFd, lineByline, MAXBUF)) > 0){
        strcat(buf,lineByline);
        readRet += currentRead;
        if (readRet >= 4){
            char checkCarraigeReturnAndNewLine[4];
            memset(checkCarraigeReturnAndNewLine,0,4);
            for (int i = readRet - 4; i < readRet; i++){
                if (buf[i] == '\r')
                {
                    strcat(checkCarraigeReturnAndNewLine, "\r");
                }
                if (buf[i] == '\n')
                {
                    strcat(checkCarraigeReturnAndNewLine, "\n");
                }
            }
            if (!strcmp(checkCarraigeReturnAndNewLine, "\r\n\r\n")){
                break;
            }
            memset(checkCarraigeReturnAndNewLine,0,4);
        }
        memset(lineByline,0,MAXBUF);
    }

    printf("%sLOG:%s %s%s%s\n", PURPLE, RESET, BLUE,buf,RESET);
    Request *request = parse(buf,readRet,connFd);

    char headr[MAXBUF];

    if (request == NULL){
            sprintf(headr, 
                            "HTTP/1.1 400 bad request\r\n"
                            "Server: ICWS\r\n"
                            "Connection: close\r\n"
                            "Date: %s\r\n\r\n", currentDate);
            write_all(connFd, headr,strlen(headr));
            free(request);
            return;
    }
    else if (strcasecmp(request->http_version, "HTTP/1.1")){
            sprintf(headr, 
                            "HTTP/1.1 505 HTTP version not supported\r\n"
                            "Server: ICWS\r\n"
                            "Connection: close\r\n"
                            "Date: %s\r\n\r\n", currentDate);
            write_all(connFd, headr,strlen(headr));
    }
    else if (strcasecmp(request->http_method, "GET") == 0){
            printf("%sLOG:%s %sGET method matched%s\n", PURPLE,RESET,BLUE,RESET);
            respond_get(connFd, request->http_uri, 0);
    }
    else if (strcasecmp(request->http_method, "HEAD") == 0){
            printf("%sLOG:%s %sHEAD method matched%s\n", PURPLE,RESET,BLUE,RESET);
            respond_get(connFd, request->http_uri, 1);
    }
    else {
            sprintf(headr, 
                            "HTTP/1.1 501 not implemented\r\n"
                            "Server: ICWS\r\n"
                            "Connection: close\r\n"
                            "Date: %s\r\n\r\n", currentDate);
            write_all(connFd, headr,strlen(headr));
    }
    free(request->headers);
    free(request);
}

void excecute(int connFd){
    printf("%sExcecute: Receive connfd:%s %s%d%s\n", PURPLE,RESET,GREEN,connFd,RESET);
    close(connFd);
}

void * startThread(void* args){
    while(1){
        pthread_mutex_lock(&mutexQueue);
        while(JobCount == 0){
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        threadContent request = JobQueue[0];
        printf("Thread %d starts working\n", *((int*) args));
        // push content up
        for (int i = 0; i < JobCount - 1; i++){
            JobQueue[i] = JobQueue[i + 1];
        }
        JobCount--;
        printf("Job removed\n");
        pthread_mutex_unlock(&mutexQueue);
        // serve_http(request.connFd);
        excecute(request.connFd);
        close(request.connFd);

    }

}

void addContent(int connFd, struct sockaddr_storage clientAddr){
    pthread_mutex_lock(&mutexQueue);
    threadContent job;
    job.connFd = connFd;
    memcpy(&job.clientAddr, &clientAddr, sizeof(struct sockaddr_storage));
    // need handle queue is full
    JobQueue[JobCount] = job;
    JobCount++;
    printf("Job added from client, JobCount: %d, confd: %d\n", JobCount, job.connFd);
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}



int main(int argc, char* argv[]) {
        int listenFd;
        // initialize mutex
        pthread_mutex_init(&mutexQueue, NULL);
        pthread_cond_init(&condQueue, NULL);
        pthread_mutex_init(&parseLock, NULL);
        if (argc >= 3){ listenFd = open_listenfd(argv[2]); }
        else { printf("%sNo port specified%s\n",RED,RESET); }

        if (argc >= 5){ dirName = argv[4]; }
        else{ dirName = "./"; }

        // initialize thread pool
        if (argc >= 7){ printf("passed\n"); threadNum = atoi(argv[6]);  }
        else{ threadNum = 5; }

        pthread_t thread[threadNum];
        for (int i = 0; i < threadNum; i++)
        {
            printf("Thread %d is created and waiting\n", i);
            if (pthread_create(&thread[i], NULL, &startThread, (void *) &i))
            {
                printf("%sFail to create thread at %d%s\n", RED, i, RESET);
            }
        }

        for (;;) {

                struct sockaddr_storage clientAddr;
                socklen_t clientLen = sizeof(struct sockaddr_storage);

                int connFd = accept(listenFd, (SA *) &clientAddr, &clientLen);

                if (connFd < 0) { fprintf(stderr, "Failed to accept\n"); continue; }

                printf("\n%s===========================================================%s\n", CYAN,RESET);
                char hostBuf[MAXBUF], svcBuf[MAXBUF];
                if (getnameinfo((SA *) &clientAddr, clientLen, 
                                        hostBuf, MAXBUF, svcBuf, MAXBUF, 0)==0) 
                        printf("%sConnection from%s %s%s:%s%s\n", PURPLE, RESET, GREEN,hostBuf, svcBuf, RESET);
                else
                        printf("%sConnection from ?UNKNOWN?%s\n", PURPLE, RESET);

                addContent(connFd, clientAddr);
                printf("%sLOG:%s %sContinue geting request%s\n", PURPLE, RESET,GREEN,RESET);
                printf("\n%s===========================================================%s\n", CYAN,RESET);
        }

        for (int i = 0; i < threadNum; i++) {
            if (pthread_join(thread[i], NULL) != 0) {
                perror("Failed to join the thread\n");
            }
        }
        pthread_mutex_destroy(&mutexQueue);
        pthread_cond_destroy(&condQueue);

        return 0;
}
