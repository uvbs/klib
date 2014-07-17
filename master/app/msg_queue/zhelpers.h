
#pragma once


static char *s_recv (void *socket) {
    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recvmsg (socket, &message, 0) <= 0)
        return (NULL);
    int size = zmq_msg_size (&message);
    char *string = (char*)malloc (size + 1);
    memcpy (string, zmq_msg_data (&message), size);
    zmq_msg_close (&message);
    string [size] = 0;
    return (string);
}

static int s_send (void *socket, char *string) {
    int rc;
    zmq_msg_t message;
    zmq_msg_init_size (&message, strlen (string));
    memcpy (zmq_msg_data (&message), string, strlen (string));
    rc = zmq_sendmsg (socket, &message, 0);
    zmq_msg_close (&message);
    return (rc);
}
