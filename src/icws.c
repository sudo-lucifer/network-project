#define _GNU_SOURCE
#include <strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
#include<unistd.h>
#include<signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <getopt.h>
#include <poll.h>
#include "pcsa_net.h"
#include "parse.h"

// Collaborator List (In-case anyone needed someone's name):
//      - Thanawin Boonpojanasoontorn  (6280163)
//      - Vanessa Rujipatanakul (6280204)
//      - Sujinna Phutatham (6181279)


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
        char * svcBuf;
        int connFd;
} threadContent;

char * dirName;
char * port;

// for thread pool. not thread save but fix by using mutex
threadContent JobQueue[MAXQ];
int JobCount = 0;
// read only variables
int threadNum = 5;
int timeout = 0;

pthread_mutex_t mutexQueue;
// for parse
pthread_mutex_t parseLock;
// conditional variables
pthread_cond_t condQueue;

int is_sigint = 0;


//cgi 
char * path_cgi;
static char * inferiorCmd;

void fail_exit(char *msg) { fprintf(stderr, "%s\n", msg); exit(-1); }


void set_environment(Request * request, char * svcBuf){
    int content_length = 0;
    int content_type = 0;    
    int http_accept = 0;
    int http_referer = 0;
    int http_accept_encoding = 0;
    int http_accept_language = 0;
    int http_accpet_charset = 0;
    int http_host = 0;
    int http_cookie = 0;
    int http_user_agent = 0;
    int http_connection = 0;

    // check each given header and set environment variable if the variable is not set
    for (int i = 0; i < request->header_count; i++){
        char* header_name = request->headers[i].header_name;
        char* header_value = request->headers[i].header_value;
        if (strcasecmp(header_name, "CONTENT-LENGTH") == 0 && !content_length){
            content_length = 1;
            setenv("CONTENT_LENGTH", header_value, 1);
        } else if (strcasecmp(header_name, "CONTENT-TYPE") == 0 && !content_type){
            content_type = 1;
            setenv("CONTENT_TYPE", header_value, 1);
        } else if (strcasecmp(header_name, "ACCEPT") == 0 && !http_accept){
            http_accept = 1;
            setenv("HTTP_ACCEPT", header_value, 1);
        } else if (strcasecmp(header_name, "REFERER") == 0 && !http_referer){
            http_referer = 1;
            setenv("HTTP_REFERER", header_value, 1);
        } else if (strcasecmp(header_name, "ACCEPT-ENCODING") == 0 && !http_accept_encoding){
            http_accept_encoding = 1;
            setenv("HTTP_ACCEPT_ENCODING", header_value, 1);
        } else if (strcasecmp(header_name, "ACCEPT-LANGUAGE") == 0 && !http_accept_language){
            http_accept_language = 1;
            setenv("HTTP_ACCEPT_LANGUAGE", header_value, 1);
        } else if (strcasecmp(header_name, "ACCEPT-CHARSET") == 0 && !http_accpet_charset){
            http_accpet_charset = 1;
            setenv("HTTP_ACCEPT_CHARSET", header_value, 1);
        } else if (strcasecmp(header_name, "HOST") == 0 && !http_host){
            http_host = 1;
            setenv("HTTP_HOST", header_value, 1);
        } else if (strcasecmp(header_name, "COOKIE") == 0 && !http_cookie){
            http_cookie = 1;
            setenv("HTTP_COOKIE", header_value, 1);
        } else if (strcasecmp(header_name, "USER-AGENT") == 0 && !http_user_agent){
            http_user_agent = 1;
            setenv("HTTP_USER_AGENT", header_value, 1);
        } else if (strcasecmp(header_name, "CONNECTION") == 0 && !http_connection){
            http_connection = 1;
            setenv("HTTP_CONNECTION", header_value, 1);
        }
    }

    // get the query string from the request uri if it exists
    char* query_string = strchr(request->http_uri, '?');
    if (query_string != NULL){
        setenv("QUERY_STRING", query_string + 1, 1);
    }

    // get the path info from the request uri
    char path_info[MAXBUF];
    for (int i = 0; i < strlen(request->http_uri); i++){
        if (request->http_uri[i] != '?'){
            path_info[i] = request->http_uri[i];
            continue;
        }
        break;
    } 

    // set other environment variables
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("PATH_INFO", path_info, 1);
    setenv("REQUEST_METHOD", request->http_method, 1);
    setenv("REQUEST_URI", request->http_uri, 1);
    setenv("SCRIPT_NAME", path_cgi, 1);
    setenv("SERVER_PORT", port, 1);
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
    setenv("SERVER_SOFTWARE", "ICWS", 1);
    setenv("REMOTE_ADDR", svcBuf, 1);
}

void cgi_program(Request * request, int connFd, char * svcBuf, char * message) {
    int c2pFds[2]; /* Child to parent pipe */
    int p2cFds[2]; /* Parent to child pipe */

    struct sigaction sa;

    if (pipe(c2pFds) < 0) fail_exit("c2p pipe failed.");
    if (pipe(p2cFds) < 0) fail_exit("p2c pipe failed.");

    int pid = fork();

    if (pid < 0) fail_exit("Fork failed.");
    if (pid == 0) { /* Child - set up the conduit & run inferior cmd */
        sa.sa_handler = SIG_DFL;
        sigaction(SIGINT, &sa, NULL);


        set_environment(request, svcBuf);

        /* Wire pipe's incoming to child's stdin */
        /* First, close the unused direction. */
        if (close(p2cFds[1]) < 0) fail_exit("failed to close p2c[1]");
        if (p2cFds[0] != STDIN_FILENO) {
            if (dup2(p2cFds[0], STDIN_FILENO) < 0)
                fail_exit("dup2 stdin failed.");
            if (close(p2cFds[0]) < 0)
                fail_exit("close p2c[0] failed.");
        }

        /* Wire child's stdout to pipe's outgoing */
        /* But first, close the unused direction */
        if (close(c2pFds[0]) < 0) fail_exit("failed to close c2p[0]");
        if (c2pFds[1] != STDOUT_FILENO) {
            if (dup2(c2pFds[1], STDOUT_FILENO) < 0)
                fail_exit("dup2 stdin failed.");
            if (close(c2pFds[1]) < 0)
                fail_exit("close pipeFd[0] failed.");
        }

        char* inferiorArgv[] = {inferiorCmd, NULL};
        if (execvpe(inferiorArgv[0], inferiorArgv, environ) < 0)
            fail_exit("exec failed.");
    }
    else { /* Parent - send a random message */
        /* Close the write direction in parent's incoming */
        if (close(c2pFds[1]) < 0) fail_exit("failed to close c2p[1]");

        /* Close the read direction in parent's outgoing */
        if (close(p2cFds[0]) < 0) fail_exit("failed to close p2c[0]");

        // char *message = "OMGWTFBBQ\n";
        // printf("Message Body: %s, %d\n", message, (int) strlen(message));
        /* Write a message to the child - replace with write_all as necessary */
        write(p2cFds[1], message, strlen(message));
        /* Close this end, done writing. */
        if (close(p2cFds[1]) < 0) fail_exit("close p2c[01] failed.");

        char buf[MAXBUF+1];
        ssize_t numRead;
        /* Begin reading from the child */
        while ((numRead = read(c2pFds[0], buf, MAXBUF))>0) {
            write_all(connFd, buf, numRead);
            memset(buf,0,MAXBUF+1);
        }
        /* Close this end, done reading. */
        if (close(c2pFds[0]) < 0) fail_exit("close c2p[01] failed.");

        /* Wait for child termination & reap */
        int status;

        if (waitpid(pid, &status, 0) < 0) fail_exit("waitpid failed.");
    }
}

void sigint_handler(int signum){
    is_sigint = 1;
}

char * getAliveHeader(int status){
    if (!status){ return strdup("close");}
    else{ return strdup("keep-alive"); }
}


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

void respond_get(int connFd, char* req_obj, int isHEAD, int alive) {
    char * cuurentDate = getCurrentTime();
    char loc[MAXBUF];                   
    char headr[MAXBUF];                
    char * aliveHeader = getAliveHeader(alive);

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
                "Connection: %s\r\n"
                "Date: %s\r\n\r\n", aliveHeader,cuurentDate);
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
                        "Connection: %s\r\n"
                        "Date: %s\r\n\r\n", aliveHeader, cuurentDate);
        write_all(connFd, headr , strlen(headr) );
        free(aliveHeader);
        free(cuurentDate);
        close(fd);
        return;
    }

    sprintf(headr, 
            "HTTP/1.1 200 OK\r\n"
            "Server: ICWS\r\n"
            "Connection: %s\r\n"
            "Content-length: %lu\r\n"
            "Content-type: %s\r\n"
            "Last modified date: %s"
            "Date: %s\r\n\r\n", aliveHeader, filesize, mime, modTime, cuurentDate);

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
    free(cuurentDate);
    // free(ext);
    // free(mime);
    free(aliveHeader);
}

void serve_http(int connFd, char * svcBuf) {
    char * currentDate = getCurrentTime();
    int bufSize = MAXBUF;
    char * buf = malloc(sizeof(char) * bufSize);
    char lineByline[MAXBUF];
    int readRet = 0;
    int pret;
    int currentRead;
    int keep_alive = 1;
    int shouldRead = 1;

    struct pollfd fds[1];

    // read request
    while (keep_alive){
        if (shouldRead){
            memset(buf, 0, bufSize);
            readRet = 0;
            memset(lineByline, 0, MAXBUF);
        }
        fds[0].fd = connFd;
        fds[0].events = 0;
        fds[0].events |= POLLIN;

        int timeOutConvert = timeout * 1000;

        pret = poll(fds,1,timeOutConvert);


        if (!pret){
            char headr[MAXBUF];
            printf("%sLOG:%s %sTime out%s\n", PURPLE,RESET,RED,RESET);
            sprintf(headr, 
                            "HTTP/1.1 408 Connection Time out\r\n"
                            "Server: ICWS\r\n"
                            "Connection: close\r\n"
                            "Date: %s\r\n\r\n", currentDate);
            write_all(connFd, headr,strlen(headr));
            break;
        }
        else if (pret < 0){
            printf("%sLOG:%s %spoll() fail%s\n", PURPLE,RESET,RED,RESET);
            break;
        }
        else{
            // read all request
            while((currentRead = read(connFd, lineByline, MAXBUF)) > 0){
                // if size per one persistant request is bigger than 8192 byte, make buffer bigger
                if (readRet + currentRead > bufSize){
                    bufSize += currentRead + 1;
                    buf = realloc(buf, sizeof(char) * bufSize);
                }
                strcat(buf, lineByline);
                readRet += currentRead;
                // check end of 
                if (readRet >= 4)
                {
                    char checkCarraigeReturnAndNewLine[5];
                    memset(checkCarraigeReturnAndNewLine, 0, 4);
                    for (int i = readRet - 4; i < readRet; i++)
                    {
                        if (buf[i] == '\r') { strcat(checkCarraigeReturnAndNewLine, "\r"); }
                        if (buf[i] == '\n'){ strcat(checkCarraigeReturnAndNewLine, "\n"); }
                    }
                    if (!strcmp(checkCarraigeReturnAndNewLine, "\r\n\r\n")){ break; }
                    memset(checkCarraigeReturnAndNewLine, 0, 5);
                }
                memset(lineByline, 0, MAXBUF);
            }
            char * ptr = strstr(buf, "\r\n\r\n");
            int message_size = MAXBUF;
            char * message_body = malloc(sizeof(char) * message_size);
            memset(message_body, 0, message_size);
            int isSkip = 0;
            int found_content_length = 0;
            int needToReadBody = 0;
            while (strcmp(ptr, "") != 0){
                // at most one thread can use parse function at a time
                pthread_mutex_lock(&parseLock);
                Request *request = parse(buf, readRet, connFd);
                pthread_mutex_unlock(&parseLock);
                // close read function after sending request if there is no header "Connection: keep-alive"
                keep_alive = 0;
                if (request != NULL)
                {
                    for (int i = 0; i < request->header_count; i++)
                    {
                        if (strcasecmp(request->headers[i].header_name, "content-length") == 0){
                            int content_length = atoi(request->headers[i].header_value);
                            found_content_length = content_length;
                            // ptr = strstr(buf, "\r\n\r\n");
                            char *save_buf = strdup(ptr + 4);
                            // have request but no request body
                            if (strcasecmp(save_buf, "") == 0){
                                free(request->headers);
                                free(request);
                                shouldRead = 0;
                                needToReadBody = 1;
                                keep_alive = 1;
                                break;
                            }
                            else{
                                isSkip = 1;
                                int content_length = atoi(request->headers[i].header_value);
                                found_content_length = content_length;
                                strncpy(message_body, save_buf, content_length);
                                // char *save_big_buf = strdup(buf);
                                memset(buf, 0, bufSize);
                                strcpy(buf, save_buf + content_length);
                                readRet = strlen( save_buf + content_length);
                                free(save_buf);
                                // free(save_big_buf);
                            }
                            
                        }
                        if (strcasecmp(request->headers[i].header_name, "connection") == 0)
                        {
                            if (strcasecmp(request->headers[i].header_value, "keep-alive") == 0)
                            {
                                keep_alive = 1;
                                break;
                            }
                        }
                    }
                }
                // printf("Message: %s\nSize: %d\n", message_body, message_size);
                if (needToReadBody){
                    // free(alive);
                    free(message_body);
                    message_size = 0;
                    break;
                }
                else{
                    shouldRead = 1;
                }
                // for response header
                char *alive = getAliveHeader(keep_alive);
                char headr[MAXBUF];
                if (request == NULL)
                {
                    sprintf(headr,
                            "HTTP/1.1 400 bad request\r\n"
                            "Server: ICWS\r\n"
                            "Connection: %s\r\n"
                            "Date: %s\r\n\r\n",
                            alive, currentDate);
                    write_all(connFd, headr, strlen(headr));
                    free(alive);
                    free(request);
                    break;
                }
                else if (strcasecmp(request->http_version, "HTTP/1.1") && strcasecmp(request->http_version, "HTTP/1.0"))
                {
                    sprintf(headr,
                            "HTTP/1.1 505 HTTP version not supported\r\n"
                            "Server: ICWS\r\n"
                            "Connection: %s\r\n"
                            "Date: %s\r\n\r\n",
                            alive, currentDate);
                    write_all(connFd, headr, strlen(headr));
                }
                else if (strcasecmp(request->http_method, "GET") == 0)
                {
                    if (strstr(request->http_uri, "/cgi/") != NULL){
                        cgi_program(request, connFd, svcBuf, message_body);
                    }
                    else{
                        respond_get(connFd, request->http_uri, 0, keep_alive);
                    }
                }
                else if (strcasecmp(request->http_method, "HEAD") == 0)
                {
                    if (strstr(request->http_uri, "/cgi/") != NULL){
                        cgi_program(request,connFd, svcBuf, message_body);
                    }
                    else{
                        respond_get(connFd, request->http_uri, 1, keep_alive);
                    }
                }
                else if (strcasecmp(request->http_method, "POST") == 0){
                    if (strstr(request->http_uri, "/cgi/") != NULL && found_content_length){
                        cgi_program(request,connFd, svcBuf, message_body);
                    }
                    else{
                        sprintf(headr,
                                "HTTP/1.1 400 bad request\r\n"
                                "Server: ICWS\r\n"
                                "Connection: %s\r\n"
                                "Date: %s\r\n\r\n",
                                alive, currentDate);
                        write_all(connFd, headr, strlen(headr));
                    }
                }
                else
                {
                    sprintf(headr,
                            "HTTP/1.1 501 not implemented\r\n"
                            "Server: ICWS\r\n"
                            "Connection: %s\r\n"
                            "Date: %s\r\n\r\n",
                            alive, currentDate);
                    write_all(connFd, headr, strlen(headr));
                }
                free(request->headers);
                free(request);
                // deep copy pointer
                if (!isSkip){
                    char *temp = strdup(ptr);
                    memset(buf, 0, bufSize);
                    strcat(buf, temp + 4);
                    ptr = strstr(buf, "\r\n\r\n");
                    free(temp);
                }
                free(alive);
                free(message_body);
                message_size = 0;
                if (ptr == NULL){
                    ptr = "";
                }
            }
        }
    }
    free(buf);
    free(currentDate);
}

void * startThread(void* args){
    pthread_detach(pthread_self());
    while(1){
        pthread_mutex_lock(&mutexQueue);
        while(JobCount == 0){
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        threadContent request = JobQueue[0];
        for (int i = 0; i < JobCount - 1; i++){
            JobQueue[i] = JobQueue[i + 1];
        }
        JobCount--;
        pthread_mutex_unlock(&mutexQueue);
        if (request.connFd == -1000){
            break;
        }
        serve_http(request.connFd, request.svcBuf);
        close(request.connFd);
        free(request.svcBuf);

    }
    return NULL;
    // pthread_exit;
}

void addContent(int connFd, char * svcBuf){
    pthread_mutex_lock(&mutexQueue);
    threadContent job;
    job.connFd = connFd;
    job.svcBuf = strdup(svcBuf);
    // memcpy(&job.clientAddr, &clientAddr, sizeof(struct sockaddr_storage));
    // need handle queue is full
    JobQueue[JobCount] = job;
    JobCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}



int main(int argc, char* argv[]) {
        struct sigaction new_action, old_action;

        new_action.sa_handler = sigint_handler;
        sigemptyset(&new_action.sa_mask);
        new_action.sa_flags = 0;

        sigaction(SIGINT, NULL, &old_action);
        if (old_action.sa_handler != SIG_IGN)
        {
            sigaction(SIGINT, &new_action, NULL);
        }

        signal(SIGPIPE, SIG_IGN);


        int listenFd = 0;
        // initialize mutex
        pthread_mutex_init(&mutexQueue, NULL);
        pthread_cond_init(&condQueue, NULL);
        pthread_mutex_init(&parseLock, NULL);
        static struct option long_options[] = {
            {"port", 1, 0, '0'},
            {"root", 1, 0, '1'},
            {"numThreads", 1, 0, '2'},
            {"timeout", 1, 0, '3'},
            {"cgiHandler", 1, 0, '4'}
        };
        int c;
        int option_index = 0;
        while (1)
        {
            c = getopt_long(argc, argv, "01234", long_options, &option_index);
            if (c == -1)
            {
                break;
            }
            // int this_option_optind = optind ? optind : 1;
            switch (c)
            {
            case '0':
                listenFd = open_listenfd(argv[2]);
                port = strdup(argv[2]);
                break;
            case '1':
                dirName = argv[4];
                break;
            case '2':
                threadNum = atoi(argv[6]);
                break;
            case '3':
                timeout = atoi(argv[8]);
                break;
            case '4':
                path_cgi = strdup(argv[10]);
                inferiorCmd = argv[10];
                break;
            default:
                printf("Missing argument %s\n", long_options[option_index].name);
                return 0;
            }
        }

        pthread_t thread[threadNum];
        // spawn n threads
        for (int i = 0; i < threadNum; i++)
        {
            if (pthread_create(&thread[i], NULL, &startThread, NULL))
            {
                printf("%sFail to create thread at %d%s\n", RED, i, RESET);
            }
            sleep(0.5);
        }

        printf("%sServer boosted%s\n", PURPLE,RESET);

        for (;;) {
                struct sockaddr_storage clientAddr;
                socklen_t clientLen = sizeof(struct sockaddr_storage);

                int connFd = accept(listenFd, (SA *) &clientAddr, &clientLen);

                if (connFd < 0) {
                    // printf("passed\n"); 
                    if (is_sigint){
                        break;
                    }
                    fprintf(stderr, "Failed to accept\n"); 
                    continue; 
                }

                char hostBuf[MAXBUF], svcBuf[MAXBUF];
                if (getnameinfo((SA *) &clientAddr, clientLen, 
                                        hostBuf, MAXBUF, svcBuf, MAXBUF, 0)!=0) 
                        printf("%sConnection from ?UNKNOWN?%s\n", PURPLE, RESET);

                serve_http(connFd, svcBuf);
                close(connFd);
                continue;
                addContent(connFd, svcBuf);
        }
        
        // printf("SIGINT: Join thread: %d\n", threadNum);
        char * poisonBuf = "\0";
        // join all thread and free mutex
        for (int i = 0; i < threadNum; i++) {
            addContent(-1000,poisonBuf);
        }
        // printf("Done kill thread\n");
        pthread_mutex_destroy(&mutexQueue);
        pthread_mutex_destroy(&parseLock);
        // condQueue->conQueue.__data.__wrefs = 0;
        pthread_cond_destroy(&condQueue);
        free(port);
        free(path_cgi);

        return 0;
}
