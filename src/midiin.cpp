#include <rtmidi/RtMidi.h>

#include "midibase.hpp"
#include "midiin.hpp"
#include "main.hpp"

#define RECEIVER_CAPI_IMPLEMENT_GET_CAPI 1
#include "receiver_capi.h"

#define CARRAY_CAPI_IMPLEMENT_REQUIRE_CAPI 1
#define CARRAY_CAPI_IMPLEMENT_GET_CAPI 1
#include "carray_capi.h"

using namespace lrtmidi;

/* ============================================================================================ */

const char* const LRTMIDI_MIDIIN_CLASS_NAME = "lrtmidi.MidiIn";

/* ============================================================================================ */

static void rtMidiCallback(double deltaSeconds, std::vector<unsigned char>* message, void* voidData)
{
    MidiInUserData* udata = (MidiInUserData*) voidData;
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

static void setupMidiInMeta(lua_State* L);

static int pushMidiInMeta(lua_State* L)
{
    if (luaL_newmetatable(L, LRTMIDI_MIDIIN_CLASS_NAME)) {
        setupMidiInMeta(L);
    }
    return 1;
}

/* ============================================================================================ */

static MidiInUserData* checkMidiInUdata(lua_State* L, int arg)
{
    MidiInUserData* udata = (MidiInUserData*) luaL_checkudata(L, arg, LRTMIDI_MIDIIN_CLASS_NAME);
    if (!udata->base.api) {
        luaL_argerror(L, arg, "invalid midiIn object");
        return NULL;
    }
    return udata;
}

/* ============================================================================================ */

static MidiInUserData* checkMidiInUdataOpen(lua_State* L, int arg, bool shouldBeOpen)
{
    MidiInUserData* udata = checkMidiInUdata(L, arg);
    if (shouldBeOpen && !udata->base.isPortOpen) {
        luaL_argerror(L, arg, "port has not been opened");
    }
    else if (!shouldBeOpen && udata->base.isPortOpen) {
        luaL_argerror(L, arg, "port is already open");
    }
    return udata;
}

/* ============================================================================================ */

static int MidiIn_new(lua_State* L)
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
        MidiInUserData* udata = (MidiInUserData*) lua_newuserdata(L, sizeof(MidiInUserData));
        memset(udata, 0, sizeof(MidiInUserData));              /* -> udata */
        
        pushMidiInMeta(L);                                  /* -> udata, meta */
        lua_setmetatable(L, -2);                                /* -> udata */
        udata->className = LRTMIDI_MIDIIN_CLASS_NAME;
        if (receiver) {
            udata->receiverCapi = receiverCapi;
            udata->receiver     = receiver;
            receiverCapi->retainReceiver(receiver);
        }
        if (clientName) {
            udata->api = new RtMidiIn(apiType, clientName);
        } else {
            udata->api = new RtMidiIn(apiType);
        }
        if (!udata->api) {
            return luaL_error(L, "cannot create RtMidiIn object");
        }
        udata->base.api = udata->api;
        
        return 1;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiIn_release(lua_State* L)
{
    try {
        MidiInUserData* udata = (MidiInUserData*)luaL_checkudata(L, 1, LRTMIDI_MIDIIN_CLASS_NAME);

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

static int MidiIn_toString(lua_State* L)
{
    MidiInUserData* udata = (MidiInUserData*) luaL_checkudata(L, 1, LRTMIDI_MIDIIN_CLASS_NAME);
    lua_pushfstring(L, "%s: %p", LRTMIDI_MIDIIN_CLASS_NAME, udata);
    return 1;
}

/* ============================================================================================ */

static int MidiIn_getCurrentApi(lua_State* L)
{
    try {
        MidiInUserData* udata = checkMidiInUdata(L, 1);
        lua_pushstring(L, RtMidi::getApiName(udata->api->getCurrentApi()).c_str());
        return 1;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiIn_getPortCount(lua_State* L)
{
    try {
        MidiInUserData* udata = checkMidiInUdata(L, 1);
        lua_pushinteger(L, udata->api->getPortCount());
        return 1;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiIn_getPortName(lua_State* L)
{
    try {
        MidiInUserData* udata = checkMidiInUdata(L, 1);
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

static int MidiIn_getPortNames(lua_State* L)
{
    try {
        MidiInUserData* udata = checkMidiInUdata(L, 1);
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

static int MidiIn_openPort(lua_State* L)
{
    try {
        MidiInUserData* udata = checkMidiInUdataOpen(L, 1, false);
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

static int MidiIn_openVirtualPort(lua_State* L)
{
    try {
        MidiInUserData* udata = checkMidiInUdataOpen(L, 1, false);
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

static int MidiIn_closePort(lua_State* L)
{
    try {
        MidiInUserData* udata = checkMidiInUdataOpen(L, 1, true);
        udata->api->closePort();
        udata->base.isPortOpen = false;
        return 0;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiIn_ignoreTypes(lua_State* L)
{
    try {
        MidiInUserData* udata = checkMidiInUdata(L, 1);
        int arg = 2;
        luaL_checktype(L, 2, LUA_TTABLE);
        bool midiSysex = true;
        bool midiTime  = true;
        bool midiSense = true;
        lua_pushnil(L);                 /* -> nil */
        while (lua_next(L, arg)) {      /* -> key, value */
            if (lua_type(L, -2) != LUA_TSTRING) {
                return luaL_argerror(L, arg, 
                                     lua_pushfstring(L, "got table key of type %s, but string expected", 
                                                     lua_typename(L, lua_type(L, -2))));
            }
            if (lua_type(L, -1) != LUA_TBOOLEAN) {
                return luaL_argerror(L, arg, 
                                     lua_pushfstring(L, "got table value of type %s, but boolean expected", 
                                                     lua_typename(L, lua_type(L, -1))));
            }
            const char* key = lua_tostring(L, -2);
            bool        val = lua_toboolean(L, -1);
            if (strcmp(key, "midiSysex") == 0) {
                midiSysex = val;
            }
            else if (strcmp(key, "midiTime") == 0) {
                midiTime = val;
            }
            else if (strcmp(key, "midiSense") == 0) {
                midiSense = val;
            }
            else {
                return luaL_argerror(L, arg, 
                                     lua_pushfstring(L, "unexpected table key '%s'", 
                                                     key));
            }
            lua_pop(L, 1);              /* -> key */
        }                               /* -> */
        udata->api->ignoreTypes(midiSysex, midiTime, midiSense);
        return 0;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiIn_setReceiver(lua_State* L)
{
    try {
        MidiInUserData* udata = checkMidiInUdata(L, 1);
        
        int versErr = 0;
        const receiver_capi* receiverCapi = receiver_get_capi(L, 2, &versErr);
        receiver_object*     receiver     = NULL;
        if (receiverCapi) {
            receiver = receiverCapi->toReceiver(L, 2);
        } else {
            if (versErr) {
                return luaL_argerror(L, 2, "receiver capi version mismatch");
            }
        }
        if (!receiver) {
            return luaL_argerror(L, 2, "expected receiver object");
        }
        
        if (udata->receiverWriter) {
            return luaL_argerror(L, 1, "receiver was already set");
        }

        if (udata->receiver) {
            udata->receiverCapi->releaseReceiver(udata->receiver);
            udata->receiver     = NULL;
            udata->receiverCapi = NULL;
        }
        udata->receiverCapi = receiverCapi;
        udata->receiver     = receiver;
        receiverCapi->retainReceiver(receiver);
        
        udata->receiverWriter = receiverCapi->newWriter(2048, 2);
        
        if (!udata->receiverWriter) {
            return luaL_error(L, "error setting receiver");
        }
        
        udata->api->setCallback(rtMidiCallback, udata);
        udata->receiverCallbackActive = true;
        
        return 0;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiIn_cancelReceiver(lua_State* L)
{
    try {
        MidiInUserData* udata = checkMidiInUdataOpen(L, 1, false);

        if (udata->receiverCallbackActive) {
            udata->api->cancelCallback();
            udata->receiverCallbackActive = false;
        }

        if (udata->receiverWriter) {
            udata->receiverCapi->freeWriter(udata->receiverWriter);
            udata->receiverWriter = NULL;
        }
        if (udata->receiver) {
            udata->receiverCapi->releaseReceiver(udata->receiver);
            udata->receiver     = NULL;
            udata->receiverCapi = NULL;
        }
        
        return 0;
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiIn_getMessage(lua_State* L)
{
    try {
        int arg = 1;
        MidiInUserData* udata = checkMidiInUdataOpen(L, arg++, true);
        
        if (udata->receiverWriter) {
            return luaL_argerror(L, 1, "receiver is active");
        }

        if (!udata->messageBuffer) {
            udata->messageBuffer = new std::vector<unsigned char>();
        }
        
        double dt = udata->api->getMessage(udata->messageBuffer);
        size_t size = udata->messageBuffer->size();
        
        unsigned char* dest = NULL;
        
        if (!lua_isnoneornil(L, arg)) {
            int errorReason;
            const carray_capi* carrayCapi = carray_get_capi(L, arg, &errorReason);
            if (carrayCapi) {
                carray_info info;
                carray* a = carrayCapi->toWritableCarray(L, arg, &info);
                if (a && info.elementSize == 1) {
                    dest = (unsigned char*) carrayCapi->resizeCarray(a, size, 0);
                }
                if (size > 0 && !dest) {
                    return luaL_argerror(L, arg, "writable byte carray expected");
                }
            } else if (errorReason == 1) {
                return luaL_argerror(L, arg, "carray version mismatch");
            } else {
                return luaL_argerror(L, arg, "carray expected");
            }
            lua_settop(L, arg);
        }
        else {
            const carray_capi* carrayCapi = carray_require_capi(L);
            if (size > 0) {
                carrayCapi->newCarray(L, CARRAY_UCHAR, CARRAY_DEFAULT, size, (void**) &dest);
            }
        }
        if (size == 0) {
            return 0;
        } 
        else {
            memcpy(dest, udata->messageBuffer->data(), size);
        
            lua_pushnumber(L, dt);  /* -> carray, dt */
            lua_insert(L, -2);     /* -> dt, carray */
            return 2;
        }
    }
    catch (...) { return lrtmidi::handleException(L); }
}

/* ============================================================================================ */

static int MidiIn_setErrorReceiver(lua_State* L)
{
    try {
        int arg = 1;
        MidiInUserData* udata = checkMidiInUdataOpen(L, arg++, false);

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

static const luaL_Reg MidiInMethods[] = 
{
    { "getCurrentApi",           MidiIn_getCurrentApi    },
    { "getPortCount",            MidiIn_getPortCount     },
    { "getPortName",             MidiIn_getPortName      },
    { "getPortNames",            MidiIn_getPortNames     },
    { "openPort",                MidiIn_openPort         },
    { "openVirtualPort",         MidiIn_openVirtualPort  },
    { "closePort",               MidiIn_closePort        },
    { "close",                   MidiIn_release          },
    { "ignoreTypes",             MidiIn_ignoreTypes      },
    { "setReceiver",             MidiIn_setReceiver      },
    { "cancelReceiver",          MidiIn_cancelReceiver   },
    { "getMessage",              MidiIn_getMessage       },
    { "setErrorReceiver",        MidiIn_setErrorReceiver },
    { NULL,                      NULL } /* sentinel */
};

static const luaL_Reg MidiInMetaMethods[] = 
{
    { "__gc",       MidiIn_release  },
    { "__tostring", MidiIn_toString },

    { NULL,       NULL } /* sentinel */
};

static const luaL_Reg ModuleFunctions[] = 
{
    { "newMidiIn", MidiIn_new  },
    { NULL,        NULL } /* sentinel */
};

/* ============================================================================================ */

static void setupMidiInMeta(lua_State* L)
{                                                      /* -> meta */
    lua_pushstring(L, LRTMIDI_MIDIIN_CLASS_NAME);        /* -> meta, className */
    lua_setfield(L, -2, "__metatable");                /* -> meta */

    luaL_setfuncs(L, MidiInMetaMethods, 0);       /* -> meta */
    
    lua_newtable(L);                                   /* -> meta, RtMidiClass */
    luaL_setfuncs(L, MidiInMethods, 0);           /* -> meta, RtMidiClass */
    lua_setfield (L, -2, "__index");                   /* -> meta */
}


/* ============================================================================================ */

int lrtmidi_midiin_init_module(lua_State* L, int module)
{
    if (luaL_newmetatable(L, LRTMIDI_MIDIIN_CLASS_NAME)) {
        setupMidiInMeta(L);
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
