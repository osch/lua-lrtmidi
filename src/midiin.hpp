#ifndef LRTMIDI_MIDIIN_HPP
#define LRTMIDI_MIDIIN_HPP

#include "midibase.hpp"

/* ============================================================================================ */
extern "C" {
/* ============================================================================================ */

#include "util.h"

int lrtmidi_midiin_init_module(lua_State* L, int module);

struct receiver_capi;
struct receiver_object;
struct receiver_writer;

/* ============================================================================================ */
} // extern "C"
/* ============================================================================================ */

class RtMidiIn;

extern const char* const LRTMIDI_MIDIIN_CLASS_NAME;

/* ============================================================================================ */
namespace lrtmidi {
/* ============================================================================================ */

struct MidiInUserData
{
    const char*          className;
    MidiBase             base;
    RtMidiIn*            api;

    const receiver_capi* receiverCapi;
    receiver_object*     receiver;
    receiver_writer*     receiverWriter;
    bool                 receiverCallbackActive;
    
    std::vector<unsigned char>* messageBuffer;
};

/* ============================================================================================ */
} // namespace lrtmidi
/* ============================================================================================ */

#endif // LRTMIDI_MIDIIN_HPP
