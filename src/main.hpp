#ifndef LRTMIDI_MAIN_HPP
#define LRTMIDI_MAIN_HPP

#include "util.h"

/* ============================================================================================ */
extern "C" {
/* ============================================================================================ */

DLL_PUBLIC int luaopen_lrtmidi(lua_State* L);

/* ============================================================================================ */
} // extern "C"
/* ============================================================================================ */

/* ============================================================================================ */
namespace lrtmidi {
/* ============================================================================================ */

int handleException(lua_State* L);

const char* quoteString(lua_State* L, const char* s);
const char* quoteLString(lua_State* L, const char* s, size_t len);

bool log_errorV(const char* fmt, va_list args);
bool log_infoV(const char* fmt, va_list args);

void log_error(const char* fmt, ...);
void log_info(const char* fmt, ...);

/* ============================================================================================ */
} // namespace lrtmidi
/* ============================================================================================ */

#endif // LRTMIDI_MAIN_HPP
