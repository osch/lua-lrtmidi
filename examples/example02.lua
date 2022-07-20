----------------------------------------------------------------------------------------------------
--[[
     A simple midi monitor. 
--]]
----------------------------------------------------------------------------------------------------

local format   = string.format
local nocurses = require("nocurses") -- https://github.com/osch/lua-nocurses
local carray   = require("carray")   -- https://github.com/osch/lua-carray
local mtmsg    = require("mtmsg")    -- https://github.com/osch/lua-mtmsg
local lrtmidi  = require("lrtmidi")

----------------------------------------------------------------------------------------------------

local function printbold(...)
    nocurses.setfontbold(true) print(...) nocurses.resetcolors()
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

local midiIn = lrtmidi.newMidiIn(midiApi, "example02.lua")

----------------------------------------------------------------------------------------------------

local midiBuffer = mtmsg.newbuffer()

midiBuffer:notifier(nocurses, ">", 0)

midiIn:setReceiver(midiBuffer)

----------------------------------------------------------------------------------------------------

local portId = choose("Other MIDI out", midiIn:getPortNames())

midiIn:openPort(portId, "MIDI Monitor")

----------------------------------------------------------------------------------------------------

local function parseMidiEvent(midiBytes)
    local b0, b1, b2 = midiBytes:get(1, 3)
    local status  = math.floor(b0 / 0x10)
    local channel = b0 % 0x10
    local v1, v2
    if 0x8 <= status  and status <= 0xB then
        v1 = b1
        v2 = b2
    elseif 0xC <= status and status <= 0xD then
        v1 = b1
    elseif 0xE == status then
        v1= (b1 + 128*b2) - 0x2000
    elseif 0xF == status then
        v1 = b1
    end
    return status, channel, v1, v2
end

local statusToString = {
    [0x8] = function(dt, c, v1, v2) return format("%8.3f %2d %-15s %3d %3d", dt, c, "Note Off",        v1, v2) end,
    [0x9] = function(dt, c, v1, v2) return format("%8.3f %2d %-15s %3d %3d", dt, c, "Note On",         v1, v2) end,
    [0xA] = function(dt, c, v1, v2) return format("%8.3f %2d %-15s %3d %3d", dt, c, "Poly Aftertouch", v1, v2) end,
    [0xB] = function(dt, c, v1, v2) return format("%8.3f %2d %-15s %3d %3d", dt, c, "Control Change",  v1, v2) end,

    [0xC] = function(dt, c, v1, v2) return format("%8.3f %2d %-15s %3d",     dt, c, "Program Change",  v1) end,
    [0xD] = function(dt, c, v1, v2) return format("%8.3f %2d %-15s %3d",     dt, c, "Chan Aftertouch", v1) end,
    [0xE] = function(dt, c, v1, v2) return format("%8.3f %2d %-15s %7d",     dt, c, "Pitch Bend",      v1) end,
    [0xF] = function(dt, c, v1, v2) return format("%8.3f %2d %-15s %3d",     dt, c, "System Message",  v1) end,
}

----------------------------------------------------------------------------------------------------

printbold("Monitoring midi events... (Press <Q> to Quit)")

local midiBytes = carray.new("uint8")

while true do
    local c = nocurses.getch() -- returns nil if new messages in midiBuffer
    repeat
        local deltaSeconds = midiBuffer:nextmsg(0, midiBytes)
        if deltaSeconds then
            local status, channel, v1, v2 = parseMidiEvent(midiBytes)
            local toString = statusToString[status]
            if toString then
                print(toString(deltaSeconds, channel + 1, v1, v2))
            end
        end
    until not dt
    if c then
        c = string.char(c)
        if c == "Q" or c == "q" then
            printbold("Quit.")
            break
        end
    end
end

----------------------------------------------------------------------------------------------------
