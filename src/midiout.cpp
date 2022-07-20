#include <rtmidi/RtMidi.h>

#include "midibase.hpp"
#include "midiout.hpp"
#include "main.hpp"

#define RECEIVER_CAPI_IMPLEMENT_GET_CAPI 1
#include "receiver_capi.h"

#define CARRAY_CAPI_IMPLEMENT_GET_CAPI 1
#include "carray_capi.h"

using namespace lrtmidi;

/* ============================================================================================ */

const char* const LRTMIDI_MIDIOUT_CLASS_NAME = "lrtmidi.MidiOut";

/* ============================================================================================ */

static void rtMidiCallback(double deltaSeconds, std::vector<unsigned char>* message, void* voidData)
{
    MidiOutUserData* udata = (MidiOutUserData*) voidData;
    if (udata->receiverWriter) {
        const receiver_capi* capi      = udata->receiverCapi;
        receiver_object*     receiver  = udata->receiver;
        receiver_writer*     writer    = udata->receiverWriter;
        
        capi->addNumberToWriter(writer, deltaSeconds);
        size_t size = message->size();
        unsigned char* ptr = (unsigned char*) capi->addArrayToWriter(writer, RECEIVER_UCHAR, size);
        int rc = -1;
        if (ptr) {
            memcpy(ptr, message->data(), size);
            rc = capi->msgToReceiver(receiver, writer, false, false, NULL, NULL);
        }
        if (rc != 0) {
            capi->clearWriter(writer);
        }
    }
}

/* ============================================================================================ */
extern "C" {
/* ============================================================================================ */

static void setupMidiOutMeta(lua_State* L);

static int pushMidiOutMeta(lua_State* L)
{
    if (luaL_newmetatable(L, LRTMIDI_MIDIOUT_CLASS_NAME)) {
        setupMidiOutMeta(L);
    }
    return 1;
}

/* ============================================================================================ */

static MidiOutUserData* checkMidiOutUdata(lua_State* L, int arg)
{
    MidiOutUserData* udata = (MidiOutUserData*) luaL_checkudata(L, arg, LRTMIDI_MIDIOUT_CLASS_NAME);
    if (!udata->base.api) {
        luaL_argerror(L, arg, "invalid midiOut object");
        return NULL;
    }
    return udata;
}

/* ============================================================================================ */

static MidiOutUserData* checkMidiOutUdataOpen(lua_State* L, int arg, bool shouldBeOpen)
{
    MidiOutUserData* udata = checkMidiOutUdata(L, arg);
    if (shouldBeOpen && !udata->base.isPortOpen) {
        luaL_argerror(L, arg, "port has not been opened");
    }
    else if (!shouldBeOpen && udata->base.isPortOpen) {
        luaL_argerror(L, arg, "port is already open");
    }
    return udata;
}

/* ============================================================================================ */

static int MidiOut_new(lua_State* L)
{
    try {
        int arg = 1;
        int lastArg = lua_gettop(L);
    
        RtMidi::Api apiType = RtMidi::UNSPECIFIED;
        if (lua_isnil(L, arg)) {
            arg += 1;
        } else if (arg <= lastArg && lua_type(L, arg) == LUA_TSTRING) {
            const char* apiTypeName = luaL_checkstring(L, arg);
            apiType = RtMidi::getCompiledApiByName(apiTypeName);
            if (apiType == RtMidi::UNSPECIFIED) {
                return luaL_argerror(L, arg, "unknown RtMidi API");
            }
            arg += 1;
        }
        const char* clientName = NULL;
        if (arg <= lastArg && lua_type(L, arg) == LUA_TSTRING) {
            clientName = lua_tostring(L, arg++);
        }
        const receiver_capi* receiverCapi = NULL;
        receiver_object*     receiver     = NULL;
        if (arg <= lastArg) {
            int versErr = 0;
            receiverCapi = receiver_get_capi(L, arg, &versErr);
            if (receiverCapi) {
                receiver = receiverCapi->toReceiver(L, arg);
                ++arg;
            } else {
                if (versErr) {
                    return luaL_argerror(L, arg, "receiver capi version mismatch");
                }
            }
            if (!receiver) {
                return luaL_argerror(L, arg, "expected receiver object");
            }
        }
        MidiOutUserData* udata = (MidiOutUserData*) lua_newuserdata(L, sizeof(MidiOutUserData));
        memset(udata, 0, sizeof(MidiOutUserData));              /* -> udata */
        
        pushMidiOutMeta(L);                                  /* -> udata, meta */
        lua_setmetatable(L, -2);                                /* -> udata */
        udata->className = LRTMIDI_MIDIOUT_CLASS_NAME;
        if (receiver) {
            udata->receiverCapi = receiverCapi;
            udata->receiver     = receiver;
            receiverCapi->retainReceiver(receiver);
        }
        if (clientName) {
            udata->api = new RtMidiOut(apiType, clientName);
        } else {
            udata->api = new RtMidiOut(apiType);
        }
        if (!udata->api) {
            return luaL_error(L, "cannot create RtMidiOut object");
        }
        udata->base.api = udata->api;
        
        return 1;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiOut_release(lua_State* L)
{
    try {
        MidiOutUserData* udata = (MidiOutUserData*)luaL_checkudata(L, 1, LRTMIDI_MIDIOUT_CLASS_NAME);

        if (udata->receiverWriter) {
            udata->receiverCapi->freeWriter(udata->receiverWriter);
            udata->receiverWriter = NULL;
        }
        if (udata->receiver) {
            udata->receiverCapi->releaseReceiver(udata->receiver);
            udata->receiver     = NULL;
            udata->receiverCapi = NULL;
        }
        
        midibase::release(&udata->base);
        
        if (udata->api) {
            delete udata->api;
            udata->api      = NULL;
            udata->base.api = NULL;
        }

        if (!udata->messageBuffer) {
            delete udata->messageBuffer;;
            udata->messageBuffer = NULL;
        }

        return 0;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiOut_toString(lua_State* L)
{
    MidiOutUserData* udata = (MidiOutUserData*) luaL_checkudata(L, 1, LRTMIDI_MIDIOUT_CLASS_NAME);
    lua_pushfstring(L, "%s: %p", LRTMIDI_MIDIOUT_CLASS_NAME, udata);
    return 1;
}

/* ============================================================================================ */

static int MidiOut_getCurrentApi(lua_State* L)
{
    try {
        MidiOutUserData* udata = checkMidiOutUdata(L, 1);
        lua_pushstring(L, RtMidi::getApiName(udata->api->getCurrentApi()).c_str());
        return 1;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiOut_getPortCount(lua_State* L)
{
    try {
        MidiOutUserData* udata = checkMidiOutUdata(L, 1);
        lua_pushinteger(L, udata->api->getPortCount());
        return 1;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiOut_getPortName(lua_State* L)
{
    try {
        MidiOutUserData* udata = checkMidiOutUdata(L, 1);
        lua_Integer portNumber = luaL_checkinteger(L, 2);
        if (portNumber < 1 || portNumber > udata->api->getPortCount()) {
            return luaL_argerror(L, 2, "invalid portNumber");
        }
        lua_pushstring(L, udata->api->getPortName(portNumber - 1).c_str());
        return 1;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiOut_getPortNames(lua_State* L)
{
    try {
        MidiOutUserData* udata = checkMidiOutUdata(L, 1);
        lua_newtable(L);   // -> list
        for (int i = 0; i < udata->api->getPortCount(); ++i) {
            lua_pushstring(L, udata->api->getPortName(i).c_str()); // -> list, name
            lua_rawseti(L, -2, i+1); // -> list
        }
        return 1;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiOut_openPort(lua_State* L)
{
    try {
        MidiOutUserData* udata = checkMidiOutUdataOpen(L, 1, false);
        lua_Integer portNumber = luaL_checkinteger(L, 2);
        if (portNumber < 1 || portNumber > udata->api->getPortCount()) {
            return luaL_argerror(L, 2, "invalid portNumber");
        }
        const char* portName = NULL;
        if (!lua_isnoneornil(L, 3)) {
            portName = luaL_checkstring(L, 3);
        }
        if (portName) {
            udata->api->openPort(portNumber-1, portName);
        } else {
            udata->api->openPort(portNumber-1);
        }
        udata->base.isPortOpen = true;
        return 0;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiOut_openVirtualPort(lua_State* L)
{
    try {
        MidiOutUserData* udata = checkMidiOutUdataOpen(L, 1, false);
        const char* portName = NULL;
        if (!lua_isnoneornil(L, 2)) {
            portName = luaL_checkstring(L, 2);
        }
        if (portName) {
            udata->api->openVirtualPort(portName);
        } else {
            udata->api->openVirtualPort();
        }
        udata->base.isPortOpen = true;
        return 0;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiOut_closePort(lua_State* L)
{
    try {
        MidiOutUserData* udata = checkMidiOutUdataOpen(L, 1, true);
        udata->api->closePort();
        udata->base.isPortOpen = false;
        return 0;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiOut_sendMessage(lua_State* L)
{
    try {
        int arg = 1;
        MidiOutUserData* udata = checkMidiOutUdataOpen(L, arg++, true);

        int errorReason;
        const carray_capi* carrayCapi = carray_get_capi(L, arg, &errorReason);
        if (carrayCapi) {
            carray_info info;
            const carray* a = carrayCapi->toReadableCarray(L, arg, &info);
            if (!a || info.elementSize != 1) {
                return luaL_argerror(L, arg, "byte carray expected");
            }
            if (info.elementCount > 0) {
                const unsigned char* ptr = (const unsigned char*) carrayCapi->getReadableElementPtr(a, 0, info.elementCount);
                udata->api->sendMessage(ptr, info.elementCount);
            }
            return 0;
        }
        else if (errorReason == 1) {
            return luaL_argerror(L, arg, "carray version mismatch");
        } 
        else if (lua_type(L, arg) == LUA_TSTRING) {
            size_t len;
            const unsigned char* ptr = (const unsigned char*) lua_tolstring(L, arg, &len);
            if (len > 0) {
                udata->api->sendMessage(ptr, len);
            }
            return 0;
        }
        else {
            return luaL_argerror(L, arg, "carray or string expected");
        }
    }
    catch (...) { return lrtmidi::handleException(L); }
}



/* ============================================================================================ */

static int MidiOut_setErrorReceiver(lua_State* L)
{
    try {
        int arg = 1;
        MidiOutUserData* udata = checkMidiOutUdataOpen(L, arg++, false);

        int versErr = 0;
        const receiver_capi* receiverCapi = receiver_get_capi(L, arg, &versErr);
        receiver_object*     receiver     = NULL;
        if (receiverCapi) {
            receiver = receiverCapi->toReceiver(L, arg);
            ++arg;
        } else {
            if (versErr) {
                return luaL_argerror(L, arg, "receiver capi version mismatch");
            }
        }
        if (!receiver) {
            return luaL_argerror(L, arg, "expected receiver object");
        }
        
        if (udata->base.errorReceiverWriter) {
            udata->base.errorReceiverCapi->freeWriter(udata->base.errorReceiverWriter);
            udata->base.errorReceiverWriter = NULL;
        }
        if (udata->base.errorReceiver) {
            udata->base.errorReceiverCapi->releaseReceiver(udata->base.errorReceiver);
            udata->base.errorReceiver     = NULL;
            udata->base.errorReceiverCapi = NULL;
        }
        udata->base.errorReceiverCapi = receiverCapi;
        udata->base.errorReceiver     = receiver;
        receiverCapi->retainReceiver(receiver);
        
        udata->base.errorReceiverWriter = receiverCapi->newWriter(2048, 2);
        
        udata->api->setErrorCallback(midibase::errorCallback, &udata->base);
        
        return 0;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static const luaL_Reg MidiOutMethods[] = 
{
    { "getCurrentApi",           MidiOut_getCurrentApi    },
    { "getPortCount",            MidiOut_getPortCount     },
    { "getPortName",             MidiOut_getPortName      },
    { "getPortNames",            MidiOut_getPortNames     },
    { "openPort",                MidiOut_openPort         },
    { "openVirtualPort",         MidiOut_openVirtualPort  },
    { "closePort",               MidiOut_closePort        },
    { "close",                   MidiOut_release          },
    { "sendMessage",             MidiOut_sendMessage      },
    { "setErrorReceiver",        MidiOut_setErrorReceiver },
    { NULL,                      NULL } /* sentinel */
};

static const luaL_Reg MidiOutMetaMethods[] = 
{
    { "__gc",       MidiOut_release  },
    { "__tostring", MidiOut_toString },

    { NULL,       NULL } /* sentinel */
};

static const luaL_Reg ModuleFunctions[] = 
{
    { "newMidiOut", MidiOut_new  },
    { NULL,        NULL } /* sentinel */
};

/* ============================================================================================ */

static void setupMidiOutMeta(lua_State* L)
{                                                      /* -> meta */
    lua_pushstring(L, LRTMIDI_MIDIOUT_CLASS_NAME);        /* -> meta, className */
    lua_setfield(L, -2, "__metatable");                /* -> meta */

    luaL_setfuncs(L, MidiOutMetaMethods, 0);       /* -> meta */
    
    lua_newtable(L);                                   /* -> meta, RtMidiClass */
    luaL_setfuncs(L, MidiOutMethods, 0);           /* -> meta, RtMidiClass */
    lua_setfield (L, -2, "__index");                   /* -> meta */
}


/* ============================================================================================ */

int lrtmidi_midiout_init_module(lua_State* L, int module)
{
    if (luaL_newmetatable(L, LRTMIDI_MIDIOUT_CLASS_NAME)) {
        setupMidiOutMeta(L);
    }
    lua_pop(L, 1);
    
    lua_pushvalue(L, module);
        luaL_setfuncs(L, ModuleFunctions, 0);
    lua_pop(L, 1);
    
    return 0;
}

/* ============================================================================================ */
} // extern "C"
/* ============================================================================================ */
