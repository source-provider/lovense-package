#pragma once
#include "roblox/update.hpp"

// string size limit
#define MAXSSIZE (1 << 30)

// string atoms are not defined by default; the storage is 16-bit integer
#define ATOM_UNDEF -32768

#define sizestring(len) (offsets::string::data.get_offset() + len + 1)
#define lmod(s, size) ((cast_to(int, (s) & ((size)-1))))

// std::uint32_t newlstr(std::uint32_t roblox_state, const char* str, std::size_t l, unsigned int h);
unsigned int luaS_hash(const char* str, size_t len);

std::uint32_t luaS_newlstr(std::uint32_t roblox_state, const char* str, std::size_t l);
#define luaS_new(rbx_state, str) luaS_newlstr(rbx_state, str, strlen(str))