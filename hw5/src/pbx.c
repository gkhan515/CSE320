/*
 * PBX: simulates a Private Branch Exchange.
 */
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>

#include "pbx.h"
#include "debug.h"

struct pbx {
    TU **clients;
    int ext[PBX_MAX_EXTENSIONS];
    size_t num_clients;
    pthread_mutex_t mutex;
};

/*
 * Initialize a new PBX.
 *
 * @&turn the newly initialized PBX, or NULL if initialization fails.
 */
PBX *pbx_init() {
    pbx = malloc(sizeof(struct pbx));
    pbx->clients = malloc(PBX_MAX_EXTENSIONS*sizeof(struct tu*));
    if (!pbx->clients) {
        debug("malloc failed in pbx_init");
        free(pbx);
        return NULL;
    }
    for (int i = 0; i < PBX_MAX_EXTENSIONS; i++) {
        pbx->clients[i] = NULL;
        pbx->ext[i] = -1;
    }
    pbx->num_clients = 0;
    pthread_mutex_init(&(pbx->mutex), NULL);
    return pbx;
}

/*
 * Shut down a pbx, shutting down all network connections, waiting for all server
 * threads to terminate, and freeing all associated resources.
 * If there are any registered extensions, the associated network connections are
 * shut down, which will cause the server threads to terminate.
 * Once all the server threads have terminated, any remaining resources associated
 * with the PBX are freed.  The PBX object itself is freed, and should not be used again.
 *
 * @param pbx  The PBX to be shut down.
 */
void pbx_shutdown(PBX *pbx) {
    pthread_mutex_lock(&(pbx->mutex));
    TU **clients = pbx->clients;
    for (int i = 0; i < PBX_MAX_EXTENSIONS; i++)
        if (clients[i])
            shutdown(tu_extension(clients[i]), SHUT_RDWR);

    while (pbx->num_clients > 0)
        usleep(50000);

    pthread_mutex_unlock(&(pbx->mutex));
    pthread_mutex_destroy(&(pbx->mutex));
    free(pbx);
}

/*
 * Register a telephone unit with a PBX at a specified extension number.
 * This amounts to "plugging a telephone unit into the PBX".
 * The TU is initialized to the TU_ON_HOOK state.
 * The reference count of the TU is increased and the PBX retains this reference
 *for as long as the TU remains registered.
 * A notification of the assigned extension number is sent to the underlying network
 * client.
 *
 * @param pbx  The PBX registry.
 * @param tu  The TU to be registered.
 * @param ext  The extension number on which the TU is to be registered.
 * @return 0 if registration succeeds, otherwise -1.
 */
int pbx_register(PBX *pbx, TU *tu, int ext) {
    pthread_mutex_lock(&(pbx->mutex));
    TU **clients = pbx->clients;
    for (int i = 0; i < PBX_MAX_EXTENSIONS; i++) {
        if (!(clients[i])) {
            clients[i] = tu;
            pbx->ext[i] = ext;
            pbx->num_clients += 1;
            pthread_mutex_unlock(&(pbx->mutex));
            tu_ref(tu, "Registered");
            tu_set_extension(tu, ext);
            //notification
            // FILE *client = fdopen(tu_fileno(tu), "w");
            // fprintf(client, "%d\n", pbx->ext[i]);
            // fflush(client);
            // fclose(client);
            return 0;
        }
    }
    pthread_mutex_unlock(&(pbx->mutex));
    debug("didn't register for some reason");
    return -1;
}

/*
 * Unregister a TU from a PBX.
 * This amounts to "unplugging a telephone unit from the PBX".
 * The TU is disassociated from its extension number.
 * Then a hangup operation is performed on the TU to cancel any
 * call that might be in progress.
 * Finally, the reference held by the PBX to the TU is released.
 *
 * @param pbx  The PBX.
 * @param tu  The TU to be unregistered.
 * @return 0 if unregistration succeeds, otherwise -1.
 */
int pbx_unregister(PBX *pbx, TU *tu) {
    // TO BE IMPLEMENTED
    pthread_mutex_lock(&(pbx->mutex));
    TU **clients = pbx->clients;
    for (int i = 0; i < PBX_MAX_EXTENSIONS; i++) {
        if (clients[i] == tu) {
            clients[i] = NULL;
            pbx->ext[i] = -1;
            pbx->num_clients -= 1;
            pthread_mutex_unlock(&(pbx->mutex));
            tu_hangup(tu);
            tu_unref(tu, "Unregistered");
            free(tu);
            return 0;
        }
    }
    pthread_mutex_unlock(&(pbx->mutex));
    debug("didn't unregister for some reason");
    return -1;
}

/*
 * Use the PBX to initiate a call from a specified TU to a specified extension.
 *
 * @param pbx  The PBX registry.
 * @param tu  The TU that is initiating the call.
 * @param ext  The extension number to be called.
 * @return 0 if dialing succeeds, otherwise -1.
 */
int pbx_dial(PBX *pbx, TU *tu, int ext) {
    // TO BE IMPLEMENTED
    debug("start of pbx_dial");
    pthread_mutex_lock(&(pbx->mutex));
    for (int i = 0; i < PBX_MAX_EXTENSIONS; i++) {
        if (pbx->ext[i] == ext) {
            TU *calling = pbx->clients[i];
            pthread_mutex_unlock(&(pbx->mutex));
            tu_dial(tu, calling);
            return 0;
        }
    }
    pthread_mutex_unlock(&(pbx->mutex));
    debug("dial failed");
    return -1;
}
