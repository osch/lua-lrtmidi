# lrtmidi Documentation

<!-- ---------------------------------------------------------------------------------------- -->
##   Contents
<!-- ---------------------------------------------------------------------------------------- -->

   * [Overview](#overview)
   * [Module Functions](#module-functions)
        * [lrtmidi.getCompiledApi()](#lrtmidi_getCompiledApi)
        * [lrtmidi.newMidiIn()](#lrtmidi_newMidiIn)
        * [lrtmidi.newMidiOut()](#lrtmidi_newMidiOut)
   * [MidiIn Methods](#midiin-methods)
        * [midiin:getCurrentApi()](#midiin_getCurrentApi)
        * [midiin:getPortCount()](#midiin_getPortCount)
        * [midiin:getPortName()](#midiin_getPortName)
        * [midiin:getPortNames()](#midiin_getPortNames)
        * [midiin:openPort()](#midiin_openPort)
        * [midiin:openVirtualPort()](#midiin_openVirtualPort)
        * [midiin:ignoreTypes()](#midiin_ignoreTypes)
        * [midiin:setReceiver()](#midiin_setReceiver)
        * [midiin:cancelReceiver()](#midiin_cancelReceiver)
        * [midiin:getMessage()](#midiin_getMessage)
        * [midiin:closePort()](#midiin_closePort)
        * [midiin:close()](#midiin_close)
        * [midiin:setErrorReceiver()](#midiin_setErrorReceiver)
   * [MidiOut Methods](#midiout-methods)
        * [midiout:getCurrentApi()](#midiout_getCurrentApi)
        * [midiout:getPortCount()](#midiout_getPortCount)
        * [midiout:getPortName()](#midiout_getPortName)
        * [midiout:getPortNames()](#midiout_getPortNames)
        * [midiout:openPort()](#midiout_openPort)
        * [midiout:openVirtualPort()](#midiout_openVirtualPort)
        * [midiout:sendMessage()](#midiout_sendMessage)
        * [midiout:closePort()](#midiout_closePort)
        * [midiout:close()](#midiout_close)
        * [midiout:setErrorReceiver()](#midiout_setErrorReceiver)


<!-- ---------------------------------------------------------------------------------------- -->
##   Overview
<!-- ---------------------------------------------------------------------------------------- -->
   
*lrtmidi* is a Lua binding for [RtMidi].

This binding enables [Lua] scripting code to use [MidiIn](#lrtmidi_newMidiIn) and 
[MidiOut](#lrtmidi_newMidiOut) objects for connecting to [MIDI] ports and to send or 
receive [MIDI] event data.

This module can be installed from Luarocks: https://luarocks.org/modules/osch/lrtmidi.

The module name to require is *"lrtmidi"*:

```
local lrtmidi = require("lrtmidi")
```

<!-- ---------------------------------------------------------------------------------------- -->
##   Module Functions
<!-- ---------------------------------------------------------------------------------------- -->

* <a id="lrtmidi_getCompiledApi">**`  lrtmidi.getCompiledApi()
  `**</a>
  
  Returns a string list of possible audio API names that can be used
  for creating a new controller object with [lrtmidi.new()](#lrtmidi_new). 
  
  Possible API names for [RtMidi] 5.0.0 are: **"core"** (*CoreMidi*), **"alsa"** (*ALSA*), 
  **"jack"** (*Jack*), **"winmm"** (*Windows MultiMedia*) or **"web"** (*Web MIDI API*).

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="lrtmidi_newMidiIn">**`  lrtmidi.newMidiIn([apiName][, clientName])
  `**</a>
  
  Creates a new *MidiIn* object for receiving MIDI events from a MIDI OUT port.
  
  * *apiName* - optional string or *nil*: name of the MIDI API that is used with 
                this *MidiIn* object. If not given, the default order of use is 
                *"core"*, *"alsa"*, *"jack"*, *"winmm"*, *"web"*. 
                For possible values see: [lrtmidi.getCompiledApi()](#lrtmidi_getCompiledApi)
  
  * *clientName* - optional string client name. This will be used to group the ports that 
                   are created by the application. 
                   
  See also [MidiIn Methods](#midiin-methods).
  
<!-- ---------------------------------------------------------------------------------------- -->

* <a id="lrtmidi_newMidiOut">**`  lrtmidi.newMidiOut([apiName][, clientName])
  `**</a>
  
  Creates a new *MidiOut* object for sending MIDI events to a MIDI IN port.
  
  * *apiName* - optional string or *nil*: name of the MIDI API that is used with 
                this *MidiOut* object. If not given, the default order of use is 
                *"core"*, *"alsa"*, *"jack"*, *"winmm"*, *"web"*. 
                For possible values see: [lrtmidi.getCompiledApi()](#lrtmidi_getCompiledApi)
  
  * *clientName* - optional string client name. This will be used to group the ports that 
                   are created by the application. 
  
  See also [MidiOut Methods](#midiout-methods).
  
<!-- ---------------------------------------------------------------------------------------- -->
##   MidiIn Methods
<!-- ---------------------------------------------------------------------------------------- -->

*MidiIn* objects are created by invoking the function
[lrtmidi.newMidiIn()](#lrtmidi_newMidiIn). A *MidiIn* object can be used to receive MIDI events
from a MIDI Output port.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_getCurrentApi">**`      midiin:getCurrentApi()
  `** </a>

  Returns the MIDI API name used with this *MidiIn* object. For possible values 
  see [lrtmidi.getCompiledApi()](#lrtmidi_getCompiledApi).

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_getPortCount">**`     midiin:getPortCount()
  `** </a>

  Returns the number of available MIDI Output ports that can be used for MIDI input by connecting
  this *MidiIn* object using the method [midiin:openPort()](#midiin_openPort).

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_getPortName">**`     midiin:getPortName(portNumber)
  `** </a>

  Returns as string value the name of the specified MIDI port.
  
  *portNumber* - integer value, it must be
                 1 <= *portNumber* <= [midiin:getPortCount()](#midiin_getPortCount).

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_getPortNames">**`    midiin:getPortNames()
  `** </a>

  Returns a list of all port names. The list contains for each port the port name
  as string value at the list position of the corresponding port number.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_openPort">**`     midiin:openPort(portNumber)
  `** </a>

  A *MidiIn* object can handle one MIDI input connection for receiving MIDI events. This methods
  connects the *MidiIn* object to the MIDI OUT port specified by the given *portNumber*.

  * *portNumber* - integer value, it must be
                   1 <= *portNumber* <= [midiin:getPortCount()](#midiin_getPortCount).

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_openVirtualPort">**`     midiin:openVirtualPort([portName])
  `** </a>

  A *MidiIn* object can handle one MIDI input connection for receiving MIDI events. This methods
  creates a virtual input port to allow software connections to this *MidiIn* 
  object.

  This method creates a virtual MIDI input port to which other software applications can connect. 
  This type of functionality is currently only supported by the Macintosh OS-X, any JACK, and 
  Linux ALSA APIs (the function returns an error for the other APIs).  

  * *portName* - optional string value, a name for the virtual port that is visible for 
                 other applications.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_ignoreTypes">**`     midiin:ignoreTypes(types)
  `** </a>

  Specify whether certain MIDI message types should be queued or ignored during input.

  * *types* - a Lua table with optional boolean entries for the keys *"midiSysex"*, 
              *"midiTime"* and *"midiSense"*.

  By default, MIDI timing and active sensing messages are ignored during message input because 
  of their relative high data rates. MIDI sysex messages are ignored by default as well. 
  Table entry values of *true* imply that the respective message type will be ignored.
  Missing entries are assumed to be have the value *true*, i.e. default is to ignore
  these message types.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_setReceiver">**`     midiin:setReceiver(receiver)
  `** </a>
  
  Sets a receiver object for receiving asynchronously MIDI event data.
  
  * *receiver* - receiver object for MIDI events, must implement the [Receiver C API], 
                 e.g. a [mtmsg] buffer.

  The receiver object receivers for each MIDI event a message with two arguments:
    - the time difference to the previous MIDI event as float value in seconds
    - the MIDI event bytes, an [carray] of  8-bit integer values.

  While not absolutely necessary, it is best to set the receiver before opening a 
  MIDI port to avoid leaving some messages in the queue.
    
  See also [example02.lua](../examples/example02.lua).

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_cancelReceiver">**`     midiin:cancelReceiver()
  `** </a>
  
  Disables the receiver that was set with the method [midiin:setReceiver()](#midiin_setReceiver).
  The receiver object will no longer receive MIDI events.
  
  This method throws an error if there is active receiver and the *MidiIn* object
  is connected to a MIDI port.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_getMessage">**`         midiin:getMessage([buffer])
  `** </a>

  Obtains next MIDI message if available. Returns immediately whether a new message
  is available or not.
  
  * *buffer* - optional [carray] object of 8-bit integer values that is filled by this
               method with the MIDI message bytes.
               If not given, a new [carray] object will be created if a MIDI message 
               is available.
  
  If a MIDI message is available, this method returns two values:
  * the event-delta time in seconds as  float value
  * the event data as [carray] object of  8-bit integer values 
    (re-using *buffer* argument if given)
  
  If no MIDI message is availabe, this methods returns immediately without any return values.
  
  This methods throws an error if a receiver was set with the method
  [midiin:setReceiver()](#midiin_setReceiver) or if the *MidiIn* object is not connected
  to a MIDI port.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_closePort">**`     midiin:closePort()
  `** </a>
  
  Disconnects the *MidIn* object from the MIDI port. Afterwards the *MidiIn* object can be used 
  to connect to another port using [midiin:openPort()](#midiin_openPort) or 
  [midiin:openVirtualPort()](#midiin:openVirtualPort).

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_close">**`        midiin:close()
  `** </a>
  
  Closes the *MidIn* object and releases underlying resources. Afterwards the *MidiIn* 
  object becomes invalid and cannot be used again. 
  
  *MidiIn* objects are subject to garbage collection, i.e. the *close* method is implicitly 
  called if the object becomes garbage collected.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiin_setErrorReceiver">**`     midiin:setErrorReceiver(receiver)
  `** </a>

  * *receiver* - receiver object for error/debug information, must implement the [Receiver C API], 
                 e.g. a [mtmsg] buffer.

  The receiver object receives for each error/debug information a message with two
  arguments:
  
  * error type as string value, possible values: *"WARNING"*, *"DEBUG_WARNING"*, *"UNSPECIFIED"*, 
    *"NO_DEVICES_FOUND"*, *"INVALID_DEVICE"*, *"MEMORY_ERROR"*, *"INVALID_PARAMETER"*, *"INVALID_USE"*, 
    *"DRIVER_ERROR"*, *"SYSTEM_ERROR"*, *"THREAD_ERROR"*.
  * error message as string value
  
  This method must be called before the *MidiIn* object is connected to a MIDI port, i.e.
  before [midiin:openPort()](#midiin_openPort) or 
  [midiin:openVirtualPort()](#midiin:openVirtualPort) is called.

<!-- ---------------------------------------------------------------------------------------- -->
##   MidiOut Methods
<!-- ---------------------------------------------------------------------------------------- -->

*MidiOut* objects are created by invoking the function
[lrtmidi.newMidiOut()](#lrtmidi_newMidiOut). A *MidiOut* object can be used to send MIDI events
to a MIDI Input port.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiout_getCurrentApi">**`      midiout:getCurrentApi()
  `** </a>

  Returns the MIDI API name used with this *MidiOut* object. For possible values 
  see [lrtmidi.getCompiledApi()](#lrtmidi_getCompiledApi).


<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiout_getPortCount">**`     midiout:getPortCount()
  `** </a>

  Returns the number of available MIDI Input ports that can be used for MIDI output by connecting
  this *MidiOut* object using the method [midiout:openPort()](#midiout_openPort).

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiout_getPortName">**`     midiout:getPortName(portNumber)
  `** </a>

  Returns as string value the name of the specified MIDI port.
  
  *portNumber* - integer value, it must be
                 1 <= *portNumber* <= [midiout:getPortCount()](#midiout_getPortCount).

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiout_getPortNames">**`    midiout:getPortNames()
  `** </a>

  Returns a list of all port names. The list contains for each port the port name
  as string value at the list position of the corresponding port number.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiout_openPort">**`     midiout:openPort(portNumber)
  `** </a>

  A *MidiOut* object can handle one MIDI output connection for sending MIDI events. This methods
  connects the *MidiOut* object to the MIDI IN port specified by the given *portNumber*.

  * *portNumber* - integer value, it must be
                   1 <= *portNumber* <= [midiout:getPortCount()](#midiout_getPortCount).

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiout_openVirtualPort">**`     midiout:openVirtualPort([portName])
  `** </a>

  A *MidiOut* object can handle one MIDI output connection for sending MIDI events. This methods
  creates a virtual output port to allow software connections to this *MidiOut* 
  object.

  This method creates a virtual MIDI output port to which other software applications can connect. 
  This type of functionality is currently only supported by the Macintosh OS-X, any JACK, and 
  Linux ALSA APIs (the function returns an error for the other APIs).  

  * *portName* - optional string value, a name for the virtual port that is visible for 
                 other applications.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiout_sendMessage">**`         midiout:sendMessage(message)
  `** </a>

  Sends a MIDI message.
  
  * *message* - string or [carray] object of 8-bit integer values
                containing the MIDI message bytes to be sent.
  
  See also [example03.lua](../examples/example03.lua).

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiout_closePort">**`     midiout:closePort()
  `** </a>
  
  Disconnects the *MidiOut* object from the MIDI port. Afterwards the *MidiOut* object can be used 
  to connect to another port using [midiout:openPort()](#midiout_openPort) or 
  [midiout:openVirtualPort()](#midiout:openVirtualPort).

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiout_close">**`        midiout:close()
  `** </a>
  
  Closes the *MidiOut* object and releases underlying resources. Afterwards the *MidiOut* 
  object becomes invalid and cannot be used again. 
  
  *MidiOut* objects are subject to garbage collection, i.e. the *close* method is implicitly 
  called if the object becomes garbage collected.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="midiout_setErrorReceiver">**`     midiout:setErrorReceiver(receiver)
  `** </a>

  * *receiver* - receiver object for error/debug information, must implement the [Receiver C API], 
                 e.g. a [mtmsg] buffer.

  The receiver object receives for each error/debug information a message with two
  arguments:
  
  * error type as string value, possible values: *"WARNING"*, *"DEBUG_WARNING"*, *"UNSPECIFIED"*, 
    *"NO_DEVICES_FOUND"*, *"INVALID_DEVICE"*, *"MEMORY_ERROR"*, *"INVALID_PARAMETER"*, *"INVALID_USE"*, 
    *"DRIVER_ERROR"*, *"SYSTEM_ERROR"*, *"THREAD_ERROR"*.
  * error message as string value
  
  This method must be called before the *MidiOut* object is connected to a MIDI port, i.e.
  before [midiout:openPort()](#midiout_openPort) or 
  [midiout:openVirtualPort()](#midiout:openVirtualPort) is called.

<!-- ---------------------------------------------------------------------------------------- -->

End of document.

<!-- ---------------------------------------------------------------------------------------- -->

[RtMidi]:         https://github.com/thestk/rtmidi
[Lua]:            https://www.lua.org
[MIDI]:           https://www.midi.org/
[Receiver C API]: https://github.com/lua-capis/lua-receiver-capi
[mtmsg]:          https://github.com/osch/lua-mtmsg#mtmsg
[carray]:         https://github.com/osch/lua-carray

