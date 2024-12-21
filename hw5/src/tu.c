/*
 * TU: simulates a "telephone unit", which interfaces a client with the PBX.
 */
#include <stdlib.h>

#include "pbx.h"
#include "debug.h"

struct tu {
    int fd;
    TU_STATE state;
    int refs;
    int ext;
    TU *connected_to;
    pthread_mutex_t mutex;
};

void tu_print_state (TU *tu) {
    FILE *out = fdopen(tu->fd, "w");
    char *state = tu_state_names[tu->state];
    switch (tu->state) {
        case TU_ON_HOOK:
            fprintf(out, "%s %d\n", state, tu->ext);
            break;
        // case TU_RINGING:
        //     break;
        // case TU_DIAL_TONE:
        //     break;
        // case TU_RING_BACK:
        //     break;
        // case TU_BUSY_SIGNAL:
        //     break;
        case TU_CONNECTED:
            fprintf(out, "%s %d\n", state, tu->connected_to->ext);
            break;
        // case TU_ERROR:
        //     break;
        default:
            fprintf(out, "%s\n", state);
    }
    fflush(out);
    debug("finished notifying");
}

/*
 * Initialize a TU
 *
 * @param fd  The file descriptor of the underlying network connection.
 * @return  The TU, newly initialized and in the TU_ON_HOOK state, if initialization
 * was successful, otherwise NULL.
 */
TU *tu_init(int fd) {
    // TO BE IMPLEMENTED
    TU *res = malloc(sizeof(TU));
    res->fd = fd;
    res->state = TU_ON_HOOK;
    res->refs = 0;
    return res;
}

/*
 * Increment the reference count on a TU.
 *
 * @param tu  The TU whose reference count is to be incremented
 * @param reason  A string describing the reason why the count is being incremented
 * (for debugging purposes).
 */
void tu_ref(TU *tu, char *reason) {
    // TO BE IMPLEMENTED
    tu->refs += 1;
}

/*
 * Decrement the reference count on a TU, freeing it if the count becomes 0.
 *
 * @param tu  The TU whose reference count is to be decremented
 * @param reason  A string describing the reason why the count is being decremented
 * (for debugging purposes).
 */
void tu_unref(TU *tu, char *reason) {
    // TO BE IMPLEMENTED
    tu->refs -= 1;
}

/*
 * Get the file descriptor for the network connection underlying a TU.
 * This file descriptor should only be used by a server to read input from
 * the connection.  Output to the connection must only be performed within
 * the PBX functions.
 *
 * @param tu
 * @return the underlying file descriptor, if any, otherwise -1.
 */
int tu_fileno(TU *tu) {
    // TO BE IMPLEMENTED
    return tu->fd;
}

/*
 * Get the extension number for a TU.
 * This extension number is assigned by the PBX when a TU is registered
 * and it is used to identify a particular TU in calls to tu_dial().
 * The value returned might be the same as the value returned by tu_fileno(),
 * but is not necessarily so.
 *
 * @param tu
 * @return the extension number, if any, otherwise -1.
 */
int tu_extension(TU *tu) {
    // TO BE IMPLEMENTED
    return tu->ext;
}

/*
 * Set the extension number for a TU.
 * A notification is set to the client of the TU.
 * This function should be called at most once one any particular TU.
 *
 * @param tu  The TU whose extension is being set.
 */
int tu_set_extension(TU *tu, int ext) {
    // TO BE IMPLEMENTED
    tu->ext = ext;
    tu_print_state(tu);
    return 0;
}

/*
 * Initiate a call from a specified originating TU to a specified target TU.
 *   If the originating TU is not in the TU_DIAL_TONE state, then there is no effect.
 *   If the target TU is the same as the originating TU, then the TU transitions
 *     to the TU_BUSY_SIGNAL state.
 *   If the target TU already has a peer, or the target TU is not in the TU_ON_HOOK
 *     state, then the originating TU transitions to the TU_BUSY_SIGNAL state.
 *   Otherwise, the originating TU and the target TU are recorded as peers of each other
 *     (this causes the reference count of each of them to be incremented),
 *     the target TU transitions to the TU_RINGING state, and the originating TU
 *     transitions to the TU_RING_BACK state.
 *
 * In all cases, a notification of the resulting state of the originating TU is sent to
 * to the associated network client.  If the target TU has changed state, then its client
 * is also notified of its new state.
 *
 * If the caller of this function was unable to determine a target TU to be called,
 * it will pass NULL as the target TU.  In this case, the originating TU will transition
 * to the TU_ERROR state if it was in the TU_DIAL_TONE state, and there will be no
 * effect otherwise.  This situation is handled here, rather than in the caller,
 * because here we have knowledge of the current TU state and we do not want to introduce
 * the possibility of transitions to a TU_ERROR state from arbitrary other states,
 * especially in states where there could be a peer TU that would have to be dealt with.
 *
 * @param tu  The originating TU.
 * @param target  The target TU, or NULL if the caller of this function was unable to
 * identify a TU to be dialed.
 * @return 0 if successful, -1 if any error occurs that results in the originating
 * TU transitioning to the TU_ERROR state.
 */
int tu_dial(TU *tu, TU *target) {
    // TO BE IMPLEMENTED
    debug("start of tu_dial");
    if (tu->state == TU_DIAL_TONE) {
        if (!target)
            tu->state = TU_ERROR;
        else if (tu == target) {
            tu->state = TU_BUSY_SIGNAL;
            tu_print_state(tu);
        }
        else if (target->connected_to || target->state != TU_ON_HOOK) {
            tu->state = TU_BUSY_SIGNAL;
            tu_print_state(tu);
        }
        else {
            tu->connected_to = target;
            target->connected_to = tu;
            tu_ref(tu, "connected");
            tu_ref(target, "connected");
            target->state = TU_RINGING;
            tu->state = TU_RING_BACK;
            tu_print_state(tu);
            tu_print_state(target);
        }

    }
    else
        tu_print_state(tu);

    return 0;
}

/*
 * Take a TU receiver off-hook (i.e. pick up the handset).
 *   If the TU is in neither the TU_ON_HOOK state nor the TU_RINGING state,
 *     then there is no effect.
 *   If the TU is in the TU_ON_HOOK state, it goes to the TU_DIAL_TONE state.
 *   If the TU was in the TU_RINGING state, it goes to the TU_CONNECTED state,
 *     reflecting an answered call.  In this case, the calling TU simultaneously
 *     also transitions to the TU_CONNECTED state.
 *
 * In all cases, a notification of the resulting state of the specified TU is sent to
 * to the associated network client.  If a peer TU has changed state, then its client
 * is also notified of its new state.
 *
 * @param tu  The TU that is to be picked up.
 * @return 0 if successful, -1 if any error occurs that results in the originating
 * TU transitioning to the TU_ERROR state.
 */
int tu_pickup(TU *tu) {
    // TO BE IMPLEMENTED
    debug("start of tu_pickup");
    if (tu->state == TU_ON_HOOK) {
        tu->state = TU_DIAL_TONE;
        tu_print_state(tu);
    }
    else if (tu->state == TU_RINGING) {
        tu->state = TU_CONNECTED;
        tu->connected_to->state = TU_CONNECTED;
        tu_print_state(tu);
        tu_print_state(tu->connected_to);
    }
    return 0;
}

/*
 * Hang up a TU (i.e. replace the handset on the switchhook).
 *
 *   If the TU is in the TU_CONNECTED or TU_RINGING state, then it goes to the
 *     TU_ON_HOOK state.  In addition, in this case the peer TU (the one to which
 *     the call is currently connected) simultaneously transitions to the TU_DIAL_TONE
 *     state.
 *   If the TU was in the TU_RING_BACK state, then it goes to the TU_ON_HOOK state.
 *     In addition, in this case the calling TU (which is in the TU_RINGING state)
 *     simultaneously transitions to the TU_ON_HOOK state.
 *   If the TU was in the TU_DIAL_TONE, TU_BUSY_SIGNAL, or TU_ERROR state,
 *     then it goes to the TU_ON_HOOK state.
 *
 * In all cases, a notification of the resulting state of the specified TU is sent to
 * to the associated network client.  If a peer TU has changed state, then its client
 * is also notified of its new state.
 *
 * @param tu  The tu that is to be hung up.
 * @return 0 if successful, -1 if any error occurs that results in the originating
 * TU transitioning to the TU_ERROR state.
 */
int tu_hangup(TU *tu) {
    // TO BE IMPLEMENTED
    debug("start of tu_hangup");
    if (tu->state == TU_CONNECTED || tu->state == TU_RINGING || tu->state == TU_RING_BACK) {
        tu->connected_to->state = (tu->state == TU_RING_BACK ? TU_ON_HOOK:TU_DIAL_TONE);
        tu->connected_to->connected_to = NULL;
        tu_print_state(tu->connected_to);

        tu->connected_to = NULL;
        tu->state = TU_ON_HOOK;
    }
    else
        tu->state = TU_ON_HOOK;
    tu_print_state(tu);
    return 0;
}

/*
 * "Chat" over a connection.
 *
 * If the state of the TU is not TU_CONNECTED, then nothing is sent and -1 is returned.
 * Otherwise, the specified message is sent via the network connection to the peer TU.
 * In all cases, the states of the TUs are left unchanged and a notification containing
 * the current state is sent to the TU sending the chat.
 *
 * @param tu  The tu sending the chat.
 * @param msg  The message to be sent.
 * @return 0  If the chat was successfully sent, -1 if there is no call in progress
 * or some other error occurs.
 */
int tu_chat(TU *tu, char *msg) {
    // TO BE IMPLEMENTED
    debug ("startof tu_chat");
    if (tu->state != TU_CONNECTED) {
        fprintf(stderr, "TU not connected for chat\n");
        return -1;
    }

    int fd = tu->connected_to->fd;
    FILE *other_end = fdopen(fd, "w");
    fprintf(other_end, "CHAT %s\n", msg);
    fflush(other_end);

    return 0;
}
