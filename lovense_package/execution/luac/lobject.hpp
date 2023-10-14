#pragma once
#include <cstdint>
#include <intrin.h>

#include "roblox/update.hpp"

#include "luaconf.hpp"

using Value = union
{
    std::uint32_t gc;
    void* p;
    double n;
    int b;
    float v[2]; // v[0], v[1] live here; v[2] lives in TValue::extra
};

using TValue = struct lua_TValue
{
    Value value;
    int extra[LUA_EXTRA_SIZE];
    int tt;
};


template<typename YIKES>
static inline YIKES shit_cast_to(auto value) // shitty wrapper to support all their garbage casts
{
    return *reinterpret_cast<YIKES*>(&value);
}

#define ttype(o) ((o)->tt)
#define cast_to(x, y) shit_cast_to<x>(y)


#define setobj(L, obj1, obj2) \
    { \
        const TValue* o2 = (obj2); \
        TValue* o1 = (obj1); \
        *o1 = *o2; \
    }

#define sethvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = x; \
        i_o->tt = LUA_TTABLE; \
    }

#define setsvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = x; \
        i_o->tt = LUA_TSTRING; \
    }

#define setbvalue(obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.b = (x); \
        i_o->tt = LUA_TBOOLEAN; \
    }

#define setpvalue(obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.p = (x); \
        i_o->tt = LUA_TLIGHTUSERDATA; \
    }

#define setclvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(std::uint32_t, (x)); \
        i_o->tt = LUA_TFUNCTION; \
    }

#define setnvalue(obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.n = (xor_number(x)); \
        i_o->tt = LUA_TNUMBER; \
    }

#define setthvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(std::uint32_t, (x)); \
        i_o->tt = LUA_TTHREAD; \
    }

#define sethvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(std::uint32_t, (x)); \
        i_o->tt = LUA_TTABLE; \
    }

#define setuvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(std::uint32_t, (x)); \
        i_o->tt = LUA_TUSERDATA; \
    }

/*
** different types of sets, according to destination
*/

// from stack to (same) stack
#define setobjs2s setobj
// to stack (not from same stack)
#define setobj2s setobj
#define setsvalue2s setsvalue
#define sethvalue2s sethvalue
#define setptvalue2s setptvalue
// from table to same table
#define setobjt2t setobj
// to table
#define setobj2t setobj
// to new object
#define setobj2n setobj
#define setsvalue2n setsvalue

#define setttype(obj, tt) (ttype(obj) = (tt))

#define iscollectable(o) (ttype(o) >= LUA_TSTRING)

typedef TValue* StkId; // index to stack elements


#define lmod(s, size) (cast_to(int, (s) & ((size)-1)))
#define twoto(x) ((int)(1 << (x)))
#define sizenode(t) (twoto((t)->lsizenode))



#define setnilvalue(obj) ((obj)->tt = LUA_TNIL)


#define ttisnil(o) (ttype(o) == LUA_TNIL)
#define ttisnumber(o) (ttype(o) == LUA_TNUMBER)
#define ttisstring(o) (ttype(o) == LUA_TSTRING)
#define ttistable(o) (ttype(o) == LUA_TTABLE)
#define ttisfunction(o) (ttype(o) == LUA_TFUNCTION)
#define ttisboolean(o) (ttype(o) == LUA_TBOOLEAN)
#define ttisuserdata(o) (ttype(o) == LUA_TUSERDATA)
#define ttisthread(o) (ttype(o) == LUA_TTHREAD)
#define ttislightuserdata(o) (ttype(o) == LUA_TLIGHTUSERDATA)
#define ttisvector(o) (ttype(o) == LUA_TVECTOR)


#define LUAU_ASSERT(expr) (void)sizeof(!!(expr))

#define check_exp(c, e) (LUAU_ASSERT(c), (e))
#define api_check(l, e) LUAU_ASSERT(e)

#define gcvalue(o) check_exp(iscollectable(o), (o)->value.gc)
#define pvalue(o) check_exp(ttislightuserdata(o), (o)->value.gc)
#define nvalue(o) check_exp(ttisnumber(o), xor_number((o)->value.n))
#define vvalue(o) check_exp(ttisvector(o), (o)->value.v)
#define tsvalue(o) check_exp(ttisstring(o), (o)->value.gc)
#define uvalue(o) check_exp(ttisuserdata(o), (o)->value.gc)
#define clvalue(o) check_exp(ttisfunction(o), (o)->value.gc)
#define hvalue(o) check_exp(ttistable(o), (o)->value.gc)
#define bvalue(o) check_exp(ttisboolean(o), (o)->value.b)
#define thvalue(o) check_exp(ttisthread(o), (o)->value.gc)

#define getstr(ts) offsets::string::data.custom_typed_bind<const char*>((ts))
#define svalue(o) getstr(tsvalue(o))

#define l_isfalse(o) (ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))

#define iscfunction(o) (ttype(o) == LUA_TFUNCTION && offsets::closure::isC.get(clvalue(o)))
#define isLfunction(o) (ttype(o) == LUA_TFUNCTION && !offsets::closure::isC.get(clvalue(o)))

/*
    Missing luaV_tonumber
*/
#define tonumber(o, n) (ttype(o) == LUA_TNUMBER != NULL)

#define registry(rbx_state) (offsets::global::registry.get(offsets::lua_state::global.get(rbx_state)))