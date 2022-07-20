#include "error.hpp"

using namespace lrtmidi;

void error::handle_error(error::handler_data* e, const char* msg, size_t msglen)
{
    if (!e->buffer) {
        e->buffer = (char*) malloc(msglen + 1);
        if (e->buffer) {
            memcpy(e->buffer, msg, msglen);
            e->buffer[msglen] = '\0';
            e->len = msglen;
        }
    } else {
        char* newB = (char*) realloc(e->buffer, e->len + 1 + msglen + 1);
        if (newB) {
            e->buffer = newB;
            e->buffer[e->len] = '\n';
            memcpy(e->buffer + e->len + 1, msg, msglen);
            e->buffer[e->len + 1 + msglen] = '\0';
            e->len += 1 + msglen;
        }
    }
}
