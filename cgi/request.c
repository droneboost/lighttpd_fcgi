#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

/* some of the HTTP variables we are interest in */
#define MAX_HTTP_VARS (30)
char* http_var_names[MAX_HTTP_VARS] = {
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

#define MSG_QUEEN_NAME "/request_send_queue"
#define EMPTY_REQUEST_MSG "Content-type: text/plain\r\n\r\nempty request string\n"
#define EMPTY_API_NAME_MSG "Content-type: text/plain\r\n\r\nempty api name\n"
#define MAX_REQ_PARAMS (10)
#define MAX_REQ_LEN (1024)
#define MAX_RESP_LEN (1024)
static mqd_t q_send;
static mqd_t q_read;

void handle_set_request(char* params[], FCGX_Request* request)
{
    char resp[MAX_RESP_LEN] = {0};

    if (0 == strcmp(params[1], "wifisetting")) {
        if (params[2] != NULL && params[4] != NULL && strlen(params[2]) != 0 && strlen(params[4]) != 0) {  // ssid & key are not empty
            sprintf(resp, "%s:%s:%s", params[2], params[3], params[4]);;
            mq_send(q_send, resp, strlen(resp), 1);
            sprintf(resp, "Content-type: text/plain\r\n\r\n&s:%s:%s:%s:%s\n", params[0], params[1], params[2], params[3], params[4]);
        }
        else {
            sprintf(resp, "Content-type: text/plain\r\n\r\nwifi setting error\n");
        }
    }
    else if (0 == strcmp(params[1], "xxxxx")) {
        sprintf(resp, "Content-type: text/plain\r\n\r\nNot implemented yet!\n");
    }
    else {
        sprintf(resp, "Content-type: text/plain\r\n\r\nunsupported!\n");
    }
    FCGX_PutStr(resp, strlen(resp), request->out);
}

void handle_get_request(char* params[], FCGX_Request* request)
{
}

int main(void)
{
    int i = 0;
    int content_length = 0;
    char* v = NULL;
    char* params[MAX_REQ_PARAMS] = {NULL}; //0: API name, 1 ~ n: api's parameters
                                           //api=set: p1: wifi, p2: ssid, p3: encrypt, p4: key
    char req[MAX_REQ_LEN] = {0};
    FCGX_Request request;
    struct mq_attr mqattr;

    if (FCGX_Init()){
        printf("FCGX_Init");
        return -1;
    }

    if (0 != FCGX_InitRequest(&request, 0, 0)){
        printf("FCGX_InitRequest");
        FCGX_Finish();
        return -1;
    }

    mqattr.mq_maxmsg = 10;
    mqattr.mq_msgsize = 1024;
    q_send = mq_open(MSG_QUEEN_NAME, O_WRONLY | O_NONBLOCK | O_CREAT, S_IRUSR | S_IWUSR, &mqattr);

    while (0 == FCGX_Accept_r(&request)) {
        for (i = 0; i < MAX_REQ_PARAMS; i++) params[i] = NULL;
        memset(req,  0, MAX_REQ_LEN);
        v = FCGX_GetParam("CONTENT_LENGTH", request.envp);
        content_length = strtol(v, &v, 10);
        if (content_length <= 0) {
            FCGX_PutStr(EMPTY_REQUEST_MSG, strlen(EMPTY_REQUEST_MSG), request.out);
            continue;
        }

        FCGX_GetStr(req, content_length, request.in);
        v = strtok(req, ",");
        if (v == NULL) {
            FCGX_PutStr(EMPTY_API_NAME_MSG, strlen(EMPTY_API_NAME_MSG), request.out);
            continue;
        }

        i = 0;
        while (v != NULL) {
            params[i] = v;
            v = strtok (NULL, ",");
            i++;
        }

        if (0 == strcmp(params[0], "set")) {
            handle_set_request(params, &request);
        }
        else if (0 == strcmp(params[0], "get")) {
            handle_get_request(params, &request);
        }
        continue;
#ifdef DEBUG_FCGI
        char resp[MAX_RESP_LEN] = {0};
        for (i = 0; i < MAX_HTTP_VARS; ++i) {
            v = FCGX_GetParam(http_var_names[i], request.envp);
            if (v == NULL){
                sprintf(resp, "%s: \n", http_var_names[i]);
            }else{
                sprintf(resp, "%s: %s\n", http_var_names[i], v);
            }
            FCGX_PutStr(resp, strlen(resp), request.out);
        }
#endif
    }

    mq_close(q_send);
    FCGX_Finish();
    return 0;
}
