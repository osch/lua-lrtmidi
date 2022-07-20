----------------------------------------------------------------------------------------------------
--[[
     This example lists all compiled [RtMidi] APIs and for each API it displays all 
     available MIDI ports. 
--]]
----------------------------------------------------------------------------------------------------

local inspect  = require("inspect")   -- https://luarocks.org/modules/kikito/inspect
local lrtmidi  = require("lrtmidi")


print("---------------------------------------------------------------------------------------------")

local apis = lrtmidi.getCompiledApi()

print("Compiled APIS:", inspect(apis))

for _, api in ipairs(apis) do
    print("-----------------------------------------------------------------------------------------")
    print("API: ", api)
    local ok, rslt = pcall(function()
        local controller = lrtmidi.newMidiIn(api)
        return controller
    end)
    if ok then
        print()
        print("Available MIDI Ports that can be used as Input:")
        for i, n in ipairs(rslt:getPortNames()) do
            print(i, n)
        end
    else
        print("Error", rslt)
    end
end

print("---------------------------------------------------------------------------------------------")
