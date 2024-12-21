/*
 * "PBX" server module.
 * Manages interaction with a client telephone unit (TU).
 */
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "pbx.h"
#include "server.h"

/*
 * Thread function for the thread that handles interaction with a client TU.
 * This is called after a network connection has been made via the main server
 * thread and a new thread has been created to handle the connection.
 */
void *pbx_client_service(void *arg) {
    debug("starting pbx_client_service");
    int clientfd = *(int*)arg;
    free(arg);

    TU *tu = tu_init(clientfd);
    pbx_register(pbx, tu, clientfd);

    FILE *messagep = fdopen(clientfd, "r");
    debug("starting service loop");
    while (1) {
        debug("reading message");
        char c;
        char *message = malloc(2);
        int i = 0;
        size_t capacity = 2;
        while ((c = fgetc(messagep)) != EOF && c != '\n') {
            debug("c = %c", c);
            message[i] = c;
            i++;
            if (i == capacity) {
                capacity *= 2;
                message = realloc(message, capacity);
            }
        }
        message[i-1] = '\0';
        debug("should have read entire message: %s", message);

        debug("parsing message");

        char *command = strtok(message, " ");

        if (strcmp(command, "pickup") == 0) {
            debug("pickup");
            tu_pickup(tu);
        } else
        if (strcmp(command, "hangup") == 0) {
            debug("hangup");
            tu_hangup(tu);
        } else
        if (strcmp(command, "dial") == 0) {
            debug("dial");
            char *ext_string = strtok(NULL, "");
            debug("dialing %s", ext_string);
            if (ext_string != NULL) {
                int ext = atoi(ext_string);
                pbx_dial(pbx, tu, ext);
            }
            else
                debug("Dial failed");
        } else
        if (strcmp(command, "chat") == 0) {
            debug("chat");
            char *chat_msg = strtok(NULL, "");
            debug("chatting %s", chat_msg);
            tu_chat(tu, chat_msg);
        }

        free(message);
    }

    fclose(messagep);
    close(clientfd);
    return NULL;
}
