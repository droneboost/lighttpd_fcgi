#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

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

#define MSG_QUEEN_NAME "/request_send_queue"
static mqd_t q_send;
static mqd_t q_read;

int main(void)
{
    int count = 0;
    int i = 0;
    int content_length = 0;
    char* v = NULL;
    char* settings[3] = {NULL}; //0: ssid, 1: encrypt, 2: key
    char out_str[2048] = {0};
    char in_str[2048] = {0};
    char wpa_passphrase_cmd[256] = {0};
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

    while (FCGX_Accept_r(&request) == 0) {
        settings[0] = settings[1] = settings[2] = NULL;
        memset(in_str,  0, 2048);
        memset(out_str, 0, 2048);
        v = FCGX_GetParam("CONTENT_LENGTH", request.envp);
        content_length = strtol(v, &v, 10);

        // read request strings
        FCGX_GetStr(in_str, content_length, request.in);
        v = strtok (in_str, ",");
        while (v != NULL) {
            settings[i] = v;
            v = strtok (NULL, ",");
            i++;
        }
        if (settings[0] != NULL && settings[2] != NULL &&
            strlen(settings[0]) != 0 && strlen(settings[2]) != 0) {
            //sprintf(wpa_passphrase_cmd, "/usr/bin/wpa_passphrase %s %s >> /etc/wpa_supplicant.conf", settings[0], settings[2]);
            //system(wpa_passphrase_cmd);
            //system("/sbin/ifdown wlan0");
            //system("/sbin/ifup wlan0")
            sprintf(out_str, "%s:%s:%s", settings[0], settings[2], settings[1]);;
            mq_send(q_send, out_str, strlen(out_str), 1);
            sprintf(out_str, "Content-type: text/plain\r\n\r\n%s:%s:%s\n", settings[0], settings[2], settings[1]);
            FCGX_PutStr(out_str, strlen(out_str), request.out);
        }
        else {
            sprintf(out_str, "Content-type: text/plain\r\n\r\nwifi setting error\n");
            FCGX_PutStr(out_str, strlen(out_str), request.out);
        }
        for (i = 0; i < MAX_VARS; ++i) {
            v = FCGX_GetParam(vars[i], request.envp);
            if (v == NULL){
                sprintf(out_str, "%s: \n", vars[i]);

            }else{
                sprintf(out_str, "%s: %s\n", vars[i], v);
            }
            //printf(out_str);
            //FCGX_PutStr(out_str, strlen(out_str), request.out);
        }
    }

    mq_close(q_send);
    FCGX_Finish();
    return 0;
}
