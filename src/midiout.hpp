#ifndef LRTMIDI_MIDIOUT_HPP
#define LRTMIDI_MIDIOUT_HPP

#include "util.h"
#include "midibase.hpp"

/* ============================================================================================ */
extern "C" {
/* ============================================================================================ */

int lrtmidi_midiout_init_module(lua_State* L, int module);

struct receiver_capi;
struct receiver_object;
struct receiver_writer;

/* ============================================================================================ */
} // extern "C"
/* ============================================================================================ */

class RtMidiOut;

extern const char* const LRTMIDI_MIDIOUT_CLASS_NAME;

/* ============================================================================================ */
namespace lrtmidi {
/* ============================================================================================ */

struct MidiOutUserData
{
    const char*          className;
    MidiBase             base;
    RtMidiOut*            api;

    const receiver_capi* receiverCapi;
    receiver_object*     receiver;
    receiver_writer*     receiverWriter;
    
    std::vector<unsigned char>* messageBuffer;
};

/* ============================================================================================ */
} // namespace lrtmidi
/* ============================================================================================ */

#endif // LRTMIDI_MIDIOUT_HPP
