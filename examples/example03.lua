----------------------------------------------------------------------------------------------------
--[[
     Example for sending MIDI notes.
--]]
----------------------------------------------------------------------------------------------------

local nocurses  = require("nocurses") -- https://github.com/osch/lua-nocurses
local carray    = require("carray")   -- https://github.com/osch/lua-carray
local lrtmidi   = require("lrtmidi")

----------------------------------------------------------------------------------------------------

local format   = string.format
local function printbold(...) nocurses.setfontbold(true) print(...) nocurses.resetcolors() end

----------------------------------------------------------------------------------------------------

local MIDI_CHANNEL = 1

local function printHelp()
    printbold(format("Press keys c,d,e,f,g,a,b,C for playing MIDI notes on channel %s, q for Quit", MIDI_CHANNEL))
end

----------------------------------------------------------------------------------------------------

local function choose(title, list, printFilter, noneAllowed)
    printbold(format("List %s:", title))
    for i, p in ipairs(list) do
        if printFilter then
            local txt = printFilter(p)
            if txt then print(i, txt) end
        else
            print(i, p)
        end
    end
    if #list == 0 then
        print(format("No %s found", title))
        os.exit()
    end
    while true do
        io.write(format("Choose %s (1-%d%s): ", title, #list, noneAllowed and " or none" or ""))
        local inp = io.read()
        if inp == "q" then os.exit() end
        if inp ~= "" then
            local p = list[tonumber(inp)]
            if p then
                print()
                return tonumber(inp), p
            end
        elseif noneAllowed then
            return
        end
    end
end

----------------------------------------------------------------------------------------------------

local _, midiApi = choose("RtMidi API", lrtmidi.getCompiledApi())

local midiOut = lrtmidi.newMidiOut(midiApi, "example03.lua")

----------------------------------------------------------------------------------------------------


local portId = choose("Other MIDI in", midiOut:getPortNames())

midiOut:openPort(portId, "MIDI Out")

----------------------------------------------------------------------------------------------------

local notes = {
    c = 60,
    d = 62,
    e = 64,
    f = 65,
    g = 67,
    a = 69,
    b = 71,
    C = 72
}

local NOTE_OFF     = 0x8
local NOTE_ON      = 0x9

local midiBuffer = carray.new("uint8", 3)

local function sendMidi(b1, b2, b3)
    midiBuffer:set(1, b1, b2, b3)
    midiOut:sendMessage(midiBuffer)
end

local function playNote(n)
    sendMidi((NOTE_ON  * 0x10 + (MIDI_CHANNEL - 1)), n, 127)
    sendMidi((NOTE_OFF * 0x10 + (MIDI_CHANNEL - 1)), n, 0)
end

----------------------------------------------------------------------------------------------------

printHelp()

while true do
    local c = nocurses.getch()
    if c then
        c = string.char(c)
        if c == "Q" or c == "q" then
            printbold("Quit.")
            break
        else
            local n = notes[c]
            if n then
                playNote(n)
                print(format("playing note %s", c))
            else
                printHelp()
            end
        end
    end
end

----------------------------------------------------------------------------------------------------
