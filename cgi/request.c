#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>

/* some of the HTTP variables we are interest in */
#define MAX_VARS 30
char* vars[MAX_VARS] = {
    "DOCUMENT_ROOT",
    "GATEWAY_INTERFACE",
    "HTTP_ACCEPT",
    "HTTP_ACCEPT_ENCODING",
    "HTTP_ACCEPT_LANGUAGE",
    "HTTP_CACHE_CONTROL",
    "HTTP_CONNECTION",
    "HTTP_HOST",
    "HTTP_PRAGMA",
    "HTTP_RANGE",
    "HTTP_REFERER",
    "HTTP_TE",
    "HTTP_USER_AGENT",
    "HTTP_X_FORWARDED_FOR",
    "PATH",
    "QUERY_STRING",
    "REMOTE_ADDR",
    "REMOTE_HOST",
    "REMOTE_PORT",
    "REQUEST_METHOD",
    "REQUEST_URI",
    "SCRIPT_FILENAME",
    "SCRIPT_NAME",
    "SERVER_ADDR",
    "SERVER_ADMIN",
    "SERVER_NAME",
    "SERVER_PORT",
    "SERVER_PROTOCOL",
    "SERVER_SIGNATURE",
    "SERVER_SOFTWARE"
};

int main(void)
{
    int count = 0;
    int i = 0;
    int content_length = 0;
    char* v = NULL;
    char* settings[3] = {NULL}; //0: ssid, 1: encrypt, 2: key
    char out_str[2048] = {0};
    char in_str[2048] = {0};
    FCGX_Request request;

    if (FCGX_Init()){
          printf("FCGX_Init");
          return -1;
    }

    if (0 != FCGX_InitRequest(&request, 0, 0)){
          printf("FCGX_InitRequest");
          FCGX_Finish();
          return -1;
    }

//    while (FCGI_Accept() >= 0) {
      while (FCGX_Accept_r(&request) == 0) {
        //printf("Content-type: text/plain\r\n\r\n"
        //       "Request number %d\n", ++count);
        v = FCGX_GetParam("CONTENT_LENGTH", request.envp);
        content_length = strtol(v, &v, 10);

        // read request strings
        FCGX_GetStr(in_str, content_length, request.in);
        //printf(in_str);

        v = strtok (in_str, ",");
        while (v != NULL) {
          //printf ("%s\n", v);
          settings[i] = v;
          v = strtok (NULL, ",");
          i++;
        }

        // Output Response
        sprintf(out_str, "Content-type: text/plain\r\n\r\n%s:%s:%s\n", settings[0], settings[1], settings[2]);
        FCGX_PutStr(out_str, strlen(out_str), request.out);
        for (i = 0; i < MAX_VARS; ++i) {
            //v = getenv(vars[i]);
              v = FCGX_GetParam(vars[i], request.envp);
            if (v == NULL){
                //printf("%s: \n", vars[i]);
                sprintf(out_str, "%s: \n", vars[i]);

            }else{
                //printf("%s: %s\n", vars[i], v);
                sprintf(out_str, "%s: %s\n", vars[i], v);
            }
            // FCGX_PutStr(out_str, strlen(out_str), request.out);
            printf(out_str);
        }
    }

    FCGX_Finish();
    return 0;
}
