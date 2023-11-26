#ifndef LRTMIDI_ERROR_HPP
#define LRTMIDI_ERROR_HPP

#include "util.h"

/* ============================================================================================ */
namespace lrtmidi {
namespace error {
/* ============================================================================================ */

struct handler_data {
    char*  buffer;
    size_t len;
};

void handle_error(handler_data* error_handler_data, const char* msg, size_t msglen);

/* ============================================================================================ */
} } // namespace lrtmidi::error
/* ============================================================================================ */


#endif // LRTMIDI_ERROR_HPP
