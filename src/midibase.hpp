#ifndef LRTMIDI_MIDIBASE_HPP
#define LRTMIDI_MIDIBASE_HPP

/* ============================================================================================ */
extern "C" {
/* ============================================================================================ */

#include "util.h"

struct receiver_capi;
struct receiver_object;
struct receiver_writer;

/* ============================================================================================ */
} // extern "C"
/* ============================================================================================ */

class RtMidi;

/* ============================================================================================ */
namespace lrtmidi {
/* ============================================================================================ */

enum Direction 
{
    IN,
    OUT
};

struct MidiBase
{
    Direction   direction;
    RtMidi*     api;
    bool        isPortOpen;

    const receiver_capi* errorReceiverCapi;
    receiver_object*     errorReceiver;
    receiver_writer*     errorReceiverWriter;
};

/* ============================================================================================ */
namespace midibase {
/* ============================================================================================ */

void errorCallback(RtMidiError::Type type, const std::string& errorText, void* voidData);

void release(MidiBase* base);

/* ============================================================================================ */
} } // namespace lrtmidi::midibase
/* ============================================================================================ */

#endif // LRTMIDI_MIDIBASE_HPP

