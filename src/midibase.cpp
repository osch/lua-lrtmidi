#include <rtmidi/RtMidi.h>

#include "midibase.hpp"
#include "midiin.hpp"

#define RECEIVER_CAPI_IMPLEMENT_GET_CAPI 1
#include "receiver_capi.h"

using namespace lrtmidi;

void midibase::errorCallback(RtMidiError::Type type, const std::string& errorText, void* voidData)
{
    MidiBase* base = (MidiBase*) voidData;
    
    if (base->errorReceiverWriter) {
        const receiver_capi* capi      = base->errorReceiverCapi;
        receiver_object*     receiver  = base->errorReceiver;
        receiver_writer*     writer    = base->errorReceiverWriter;
        
        const char* t = "?";
        switch (type) {
            case RtMidiError::WARNING:           t = "WARNING";           break; // A non-critical error.
            case RtMidiError::DEBUG_WARNING:     t = "DEBUG_WARNING";     break; // A non-critical error which might be useful for debugging.
            case RtMidiError::UNSPECIFIED:       t = "UNSPECIFIED";       break; // The default, unspecified error type.
            case RtMidiError::NO_DEVICES_FOUND:  t = "NO_DEVICES_FOUND";  break; // No devices found on system.
            case RtMidiError::INVALID_DEVICE:    t = "INVALID_DEVICE";    break; // An invalid device ID was specified.
            case RtMidiError::MEMORY_ERROR:      t = "MEMORY_ERROR";      break; // An error occured during memory allocation.
            case RtMidiError::INVALID_PARAMETER: t = "INVALID_PARAMETER"; break; // An invalid parameter was specified to a function.
            case RtMidiError::INVALID_USE:       t = "INVALID_USE";       break; // The function was called incorrectly.
            case RtMidiError::DRIVER_ERROR:      t = "DRIVER_ERROR";      break; // A system driver error occured.
            case RtMidiError::SYSTEM_ERROR:      t = "SYSTEM_ERROR";      break; // A system error occured.
            case RtMidiError::THREAD_ERROR:      t = "THREAD_ERROR";      break; // A thread error occured.         
        }
        
        capi->addStringToWriter(writer, t, strlen(t));
        capi->addStringToWriter(writer, errorText.c_str(), errorText.size());
        int rc = capi->msgToReceiver(receiver, writer, false, false, NULL, NULL);
        if (rc != 0) {
            capi->clearWriter(writer);
        }
    }
}

void midibase::release(MidiBase* base)
{
    if (base->errorReceiverWriter) {
        base->errorReceiverCapi->freeWriter(base->errorReceiverWriter);
        base->errorReceiverWriter = NULL;
    }
    if (base->errorReceiver) {
        base->errorReceiverCapi->releaseReceiver(base->errorReceiver);
        base->errorReceiver     = NULL;
        base->errorReceiverCapi = NULL;
    }
}
