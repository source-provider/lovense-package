#pragma once
#include <cstdint>

#define api_incr_top(L) \
    { \
        *offsets::lua_state::top.typed_bind_ptr(L) += 1; \
    }

#define api_decr_top(L) \
    { \
        *offsets::lua_state::top.typed_bind_ptr(L) -= 1; \
    }


// option for multiple returns in `lua_pcall' and `lua_call'
#define LUA_MULTRET (-1)

/*
** pseudo-indices
*/
#define LUA_REGISTRYINDEX (-10000)
#define LUA_ENVIRONINDEX (-10001)
#define LUA_GLOBALSINDEX (-10002)
#define lua_upvalueindex(i) (LUA_GLOBALSINDEX - (i))
#define lua_ispseudo(i) ((i) <= LUA_REGISTRYINDEX)

// thread status; 0 is OK
enum lua_Status
{
    LUA_OK = 0,
    LUA_YIELD,
    LUA_ERRRUN,
    LUA_ERRSYNTAX,
    LUA_ERRMEM,
    LUA_ERRERR,
    LUA_BREAK, // yielded for a debug breakpoint
};

typedef int (*lua_CFunction)(std::uint32_t roblox_state);
typedef int (*lua_Continuation)(std::uint32_t roblox_state, int status);

static double xor_number(double in);

/*
** basic stack manipulation
*/

int lua_absindex(std::uint32_t roblox_state, int idx);
int lua_gettop(std::uint32_t roblox_state);
void lua_settop(std::uint32_t roblox_state, int idx);
void lua_pushvalue(std::uint32_t roblox_state, int idx);
void lua_remove(std::uint32_t roblox_state, int idx);
void lua_insert(std::uint32_t roblox_state, int idx);

/*
** access functions (stack -> C)
*/

int lua_isnumber(std::uint32_t roblox_state, int idx);
int lua_isstring(std::uint32_t roblox_state, int idx);
int lua_iscfunction(std::uint32_t roblox_state, int idx);
int lua_isLfunction(std::uint32_t roblox_state, int idx);
int lua_isuserdata(std::uint32_t roblox_state, int idx);
int lua_type(std::uint32_t roblox_state, int idx);
const char* lua_typename(std::uint32_t roblox_state, int tp);

double lua_tonumberx(std::uint32_t roblox_state, int idx, int* isnum);
int lua_tointegerx(std::uint32_t roblox_state, int idx, int* isnum);
unsigned lua_tounsignedx(std::uint32_t roblox_state, int idx, int* isnum);
const float* lua_tovector(std::uint32_t roblox_state, int idx);
int lua_toboolean(std::uint32_t roblox_state, int idx);
const char* lua_tolstring(std::uint32_t roblox_state, int idx, size_t* len);
int lua_objlen(std::uint32_t roblox_state, int idx);
lua_CFunction lua_tocfunction(std::uint32_t roblox_state, int idx);
std::uint32_t lua_tolightuserdata(std::uint32_t roblox_state, int idx);
std::uint32_t lua_touserdata(std::uint32_t roblox_state, int idx);
std::uint32_t lua_touserdatatagged(std::uint32_t roblox_state, int idx, int tag);
int lua_userdatatag(std::uint32_t roblox_state, int idx);
std::uint32_t lua_tothread(std::uint32_t roblox_state, int idx);
std::uint32_t lua_topointer(std::uint32_t roblox_state, int idx);

/*
** push functions (C -> stack)
*/

void lua_pushnil(std::uint32_t roblox_state);
void lua_pushnumber(std::uint32_t roblox_state, double n);
void lua_pushinteger(std::uint32_t roblox_state, int n);
void lua_pushunsigned(std::uint32_t roblox_state, unsigned n);
void lua_pushlstring(std::uint32_t roblox_state, const char* s, size_t l);
void lua_pushstring(std::uint32_t roblox_state, const char* s);
void lua_pushcclosurek(std::uint32_t roblox_state, lua_CFunction fn, const char* debugname, int nup, lua_Continuation cont);
void lua_pushboolean(std::uint32_t roblox_state, int b);
void lua_pushlightuserdata(std::uint32_t roblox_state, void* p);

/*
** get functions (Lua -> stack)
*/

int lua_gettable(std::uint32_t roblox_state, int idx);
int lua_getfield(std::uint32_t roblox_state, int idx, const char* k);
void lua_createtable(std::uint32_t roblox_state, int narr, int nrec);
int lua_getreadonly(std::uint32_t roblox_state, int idx);
int lua_getmetatable(std::uint32_t roblox_state, int objindex);
std::uint8_t lua_getthreadidentity(std::uint32_t roblox_state);

/*
** set functions (stack -> Lua)
*/

void lua_setreadonly(std::uint32_t roblox_state, int idx, bool enabled);
void lua_settable(std::uint32_t roblox_state, int idx);
void lua_setfield(std::uint32_t roblox_state, int idx, const char* k);
int lua_setmetatable(std::uint32_t roblox_state, int objindex);
void lua_setthreadidentity(std::uint32_t roblox_state, std::uint8_t identity);

/*
** reference system, can be used to pin objects
*/

#define LUA_NOREF -1
#define LUA_REFNIL 0


/*
** ===============================================================
** some useful macros
** ===============================================================
*/

#define lua_tonumber(L, i) lua_tonumberx(L, i, NULL)
#define lua_tointeger(L, i) lua_tointegerx(L, i, NULL)
#define lua_tounsigned(L, i) lua_tounsignedx(L, i, NULL)

#define lua_pop(roblox_state, n) lua_settop(roblox_state, -(n)-1)

#define lua_newtable(roblox_state) lua_createtable(roblox_state, 0, 0)

#define lua_strlen(L, i) lua_objlen(L, (i))

#define lua_isfunction(L, n) (lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L, n) (lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L, n) (lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L, n) (lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L, n) (lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isvector(L, n) (lua_type(L, (n)) == LUA_TVECTOR)
#define lua_isthread(L, n) (lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L, n) (lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n) (lua_type(L, (n)) <= LUA_TNIL)

#define lua_pushliteral(L, s) lua_pushlstring(L, "" s, (sizeof(s) / sizeof(char)) - 1)
#define lua_pushcfunction(L, fn, debugname) lua_pushcclosurek(L, fn, debugname, 0, NULL)
#define lua_pushcclosure(L, fn, debugname, nup) lua_pushcclosurek(L, fn, debugname, nup, NULL)

#define lua_setglobal(L, s) lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_getglobal(L, s) lua_getfield(L, LUA_GLOBALSINDEX, (s))

#define lua_tostring(L, i) lua_tolstring(L, (i), NULL)