#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "pbx.h"
#include "server.h"
#include "debug.h"

static void terminate(int status);
void handle_sighup ();

/*
 * "PBX" telephone exchange simulation.
 *
 * Usage: pbx <port>
 */
int main(int argc, char* argv[]){
    // Option processing should be performed here.
    // Option '-p <port>' is required in order to specify the port number
    // on which the server should listen.
    int port = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] != 'p') {
                fprintf(stderr, "bin/pbx: invalid option -- '%c'\n", argv[i][1]);
                fprintf(stderr, "Usage: bin/pbx -p <port>\n");
                exit(EXIT_FAILURE);
            }
            if (++i >= argc) {
                fprintf(stderr, "bin/pbx: option requires an argument -- 'p'\n");
                fprintf(stderr, "Usage: bin/pbx -p <port>\n");
                exit(EXIT_FAILURE);
            }
            port = atoi(argv[i]);
        }
        else {
            fprintf(stderr, "Usage: bin/pbx -p <port>\n");
            exit(EXIT_FAILURE);
        }
    }

    if (!port) {
        fprintf(stderr, "Usage: bin/pbx -p <port>\n");
        exit(EXIT_FAILURE);
    }

    // Perform required initialization of the PBX module.
    debug("Initializing PBX...");
    pbx = pbx_init();


    // TODO: Set up the server socket and enter a loop to accept connections
    // on this socket.  For each connection, a thread should be started to
    // run function pbx_client_service().  In addition, you should install
    // a SIGHUP handler, so that receipt of SIGHUP will perform a clean
    // shutdown of the server.
    struct sigaction sa;
    sa.sa_handler = &handle_sighup;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGHUP, &sa, NULL);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        debug("Error creating socket");
        return EXIT_FAILURE;
    }
    int sockopt = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) < 0)
        debug("Error setsockopt");
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port)
    };
    if (bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        debug("Error binding");
        fprintf(stderr, "%s\n", strerror(errno));
        close(listenfd);
        return EXIT_FAILURE;
    }
    if (listen(listenfd, PBX_MAX_EXTENSIONS) < 0) {
        debug ("Error listening");
        close(listenfd);
        return EXIT_FAILURE;
    }

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int *clientfd = malloc(sizeof(int));
        *clientfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_len);
        if (*clientfd < 0) {
            debug("Error accepting");
            free(clientfd);
            continue;
        }

        pthread_t th;
        if (pthread_create(&th, NULL, pbx_client_service, clientfd) != 0) {
            debug("Error creating thread");
            close(*clientfd);
            free(clientfd);
            continue;
        }

        pthread_detach(th);
    }

    fprintf(stderr, "You have to finish implementing main() "
	    "before the PBX server will function.\n");

    terminate(EXIT_FAILURE);
}

/*
 * Function called to cleanly shut down the server.
 */
static void terminate(int status) {
    debug("Shutting down PBX...");
    pbx_shutdown(pbx);
    debug("PBX server terminating");
    exit(status);
}

void handle_sighup () {
    fprintf(stderr, "SIGHUP handler triggered\n");
    terminate(EXIT_SUCCESS);
}