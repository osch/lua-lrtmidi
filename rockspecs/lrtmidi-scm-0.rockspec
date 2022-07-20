package = "lrtmidi"
version = "scm-0"
source = {
  url = "https://github.com/osch/lua-lrtmidi/archive/master.zip",
  dir = "lua-lrtmidi-master",
}
description = {
  summary = "Lua binding for RtMidi",
  homepage = "https://github.com/osch/lua-lrtmidi",
  license = "MIT",
  detailed = [[
      Lua binding for RtMidi ( http://www.music.mcgill.ca/~gary/rtmidi/ ).
  ]],
}
dependencies = {
  "lua >= 5.1, <= 5.4",
}
build = {
  type = "builtin",
  modules = {
    lrtmidi = {
      libraries = {
        "rtmidi"
      },
      sources = { 
          "src/main.cpp",
          "src/midibase.cpp",
          "src/midiin.cpp",
          "src/midiout.cpp",
          "src/async_util.cpp",
          "src/error.cpp",
          "src/lrtmidi_compat.c"
      },
      defines = { "LRTMIDI_VERSION="..version:gsub("^(.*)-.-$", "%1") },
    },
  }
}