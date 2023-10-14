#include <intrin.h>

#include "roblox/update.hpp"
#include "roblox/functions.hpp"
#include "bypasses/bypasses.hpp"

#include "lapi.hpp"
#include "lgc.hpp"
#include "lfunc.hpp"
#include "lobject.hpp"
#include "ltable.hpp"
#include "lstring.hpp"
#include "lnumutils.hpp"





std::uint32_t getcurrenv(std::uint32_t roblox_state)
{
    if (offsets::lua_state::ci.get(roblox_state) == offsets::lua_state::base_ci.get(roblox_state))
        return offsets::lua_state::gt.get(roblox_state);
    else
        return offsets::closure::env.get(offsets::call_info::func.get(offsets::lua_state::ci.get(roblox_state))->value.gc);
}


static double xor_number(double in)
{
    __m128d sd = _mm_load_sd(&in);
    __m128d pd = _mm_load_pd(rbx_xor_key);
    __m128d result = _mm_xor_pd(sd, pd);
    return _mm_cvtsd_f64(result);
}



/*
** basic stack manipulation
*/

int lua_absindex(std::uint32_t roblox_state, int idx)
{
    return idx > 0 || lua_ispseudo(idx) ? idx : lua_gettop(roblox_state) + idx + 1;
}

void lua_pushvalue(std::uint32_t roblox_state, int idx)
{
    luaC_checkthreadsleep(roblox_state);
    TValue* o = rbx_index2addr(roblox_state, idx);
    setobj2s(roblox_state, offsets::lua_state::top.get(roblox_state), o);
    api_incr_top(roblox_state);
    return;
}

int lua_gettop(std::uint32_t roblox_state)
{
    return offsets::lua_state::top.get(roblox_state) - offsets::lua_state::base.get(roblox_state);
}

void lua_settop(std::uint32_t roblox_state, int idx)
{
    if (idx >= 0)
    {
        while (offsets::lua_state::top.get(roblox_state) < offsets::lua_state::base.get(roblox_state) + idx)
            setnilvalue(((*offsets::lua_state::top.typed_bind_ptr(roblox_state))++));
        *offsets::lua_state::top.typed_bind_ptr(roblox_state) = offsets::lua_state::base.get(roblox_state) + idx;
    }
    else
    {
        *offsets::lua_state::top.typed_bind_ptr(roblox_state) += idx + 1; // `subtract' index (index is negative)
    }
    return;
}

void lua_remove(std::uint32_t roblox_state, int idx)
{
    StkId p = rbx_index2addr(roblox_state, idx);
    while (++p < offsets::lua_state::top.get(roblox_state))
        setobjs2s(roblox_state, p - 1, p);
    *offsets::lua_state::top.typed_bind_ptr(roblox_state) -= 1;
    return;
}

void lua_insert(std::uint32_t roblox_state, int idx)
{
    luaC_checkthreadsleep(roblox_state);
    StkId p = rbx_index2addr(roblox_state, idx);

    for (StkId q = offsets::lua_state::top.get(roblox_state); q > p; q--)
        setobjs2s(roblox_state, q, q - 1);
    setobjs2s(roblox_state, p, offsets::lua_state::top.get(roblox_state));
    return;
}

/*
** access functions (stack -> C)
*/

int lua_type(std::uint32_t roblox_state, int idx)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    return ttype(o);
}

const char* lua_typename(std::uint32_t roblox_state, int t)
{
    return (t == LUA_TNONE) ? "no value" : rbx_luat_typenames[t];
}

int lua_iscfunction(std::uint32_t roblox_state, int idx)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    return iscfunction(o);
}

int lua_isLfunction(std::uint32_t roblox_state, int idx)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    return isLfunction(o);
}

int lua_isnumber(std::uint32_t roblox_state, int idx)
{
    TValue n;
    StkId o = rbx_index2addr(roblox_state, idx);
    return tonumber(o, &n);
}

int lua_isstring(std::uint32_t roblox_state, int idx)
{
    int t = lua_type(roblox_state, idx);
    return (t == LUA_TSTRING);
}

int lua_isuserdata(std::uint32_t roblox_state, int idx)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    return (ttisuserdata(o) || ttislightuserdata(o));
}

double lua_tonumberx(std::uint32_t roblox_state, int idx, int* isnum)
{
    TValue n;
    const TValue* o = rbx_index2addr(roblox_state, idx);
    if (tonumber(o, &n))
    {
        if (isnum)
            *isnum = 1;
        return nvalue(o);
    }
    else
    {
        if (isnum)
            *isnum = 0;
        return 0;
    }
}

int lua_tointegerx(std::uint32_t roblox_state, int idx, int* isnum)
{
    TValue n;
    const TValue* o = rbx_index2addr(roblox_state, idx);
    if (tonumber(o, &n))
    {
        int res;
        double num = nvalue(o);
        luai_num2int(res, num);
        if (isnum)
            *isnum = 1;
        return res;
    }
    else
    {
        if (isnum)
            *isnum = 0;
        return 0;
    }
}

unsigned lua_tounsignedx(std::uint32_t roblox_state, int idx, int* isnum)
{
    TValue n;
    const TValue* o = rbx_index2addr(roblox_state, idx);
    if (tonumber(o, &n))
    {
        unsigned res;
        double num = nvalue(o);
        luai_num2unsigned(res, num);
        if (isnum)
            *isnum = 1;
        return res;
    }
    else
    {
        if (isnum)
            *isnum = 0;
        return 0;
    }
}

int lua_toboolean(std::uint32_t roblox_state, int idx)
{
    const TValue* o = rbx_index2addr(roblox_state, idx);
    return !l_isfalse(o);
}

const char* lua_tolstring(std::uint32_t roblox_state, int idx, size_t* len)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    if (!ttisstring(o))
    {
        luaC_threadbarrier(roblox_state);
/*                                  NOT IMPLEMENTED, wouldn't be hard to implement but just annoying lol
        if (!luaV_tostring(L, o))
        { // conversion failed?
            if (len != NULL)
                *len = 0;
            return NULL;
        }
*/
        luaC_checkGC(roblox_state);
        o = rbx_index2addr(roblox_state, idx); // previous call may reallocate the stack
    }
    if (len != NULL)
        *len = offsets::string::len.get(tsvalue(o));
    return svalue(o);
}

const float* lua_tovector(std::uint32_t roblox_state, int idx)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    if (!ttisvector(o))
        return NULL;
    return vvalue(o);
}

int lua_objlen(std::uint32_t roblox_state, int idx)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    switch (ttype(o))
    {
    case LUA_TSTRING:
        return offsets::string::len.get(tsvalue(o));
    case LUA_TUSERDATA:
        return offsets::userdata::len.get(uvalue(o));
    case LUA_TTABLE:
        return -1; // NOT IMPLEMENTED
    default:
        return 0;
    }
}

lua_CFunction lua_tocfunction(std::uint32_t roblox_state, int idx)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    return (!iscfunction(o)) ? NULL : cast_to(lua_CFunction, offsets::closure::f.get(clvalue(o)));
}

std::uint32_t lua_tolightuserdata(std::uint32_t roblox_state, int idx)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    return (!ttislightuserdata(o)) ? NULL : pvalue(o);
}

std::uint32_t lua_touserdata(std::uint32_t roblox_state, int idx)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    if (ttisuserdata(o))
        return offsets::userdata::data.get(uvalue(o));
    else if (ttislightuserdata(o))
        return pvalue(o);
    else
        return NULL;
}

std::uint32_t lua_touserdatatagged(std::uint32_t roblox_state, int idx, int tag)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    return (ttisuserdata(o) && offsets::userdata::tag.get(uvalue(o)) == tag) ? offsets::userdata::data.get(uvalue(o)) : NULL;
}

int lua_userdatatag(std::uint32_t roblox_state, int idx)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    if (ttisuserdata(o))
        return offsets::userdata::tag.get(uvalue(o));
    return -1;
}

std::uint32_t lua_tothread(std::uint32_t roblox_state, int idx)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    return (!ttisthread(o)) ? NULL : thvalue(o);
}


std::uint32_t lua_topointer(std::uint32_t roblox_state, int idx)
{
    StkId o = rbx_index2addr(roblox_state, idx);
    switch (ttype(o))
    {
    case LUA_TTABLE:
        return hvalue(o);
    case LUA_TFUNCTION:
        return clvalue(o);
    case LUA_TTHREAD:
        return thvalue(o);
    case LUA_TUSERDATA:
        return offsets::userdata::data.get(uvalue(o));
    case LUA_TLIGHTUSERDATA:
        return o->value.gc;
    default:
        return NULL;
    }
}

/*
    Push functions (Your mom -> your dad)
*/

void lua_pushnil(std::uint32_t roblox_state)
{
    setnilvalue(offsets::lua_state::top.get(roblox_state));
    api_incr_top(roblox_state);
    return;
}

void lua_pushnumber(std::uint32_t roblox_state, double n)
{
    setnvalue(offsets::lua_state::top.get(roblox_state), n);
    api_incr_top(roblox_state);
    return;
}

void lua_pushinteger(std::uint32_t roblox_state, int n)
{
    setnvalue(offsets::lua_state::top.get(roblox_state), static_cast<double>(n));
    api_incr_top(roblox_state);
    return;
}

void lua_pushunsigned(std::uint32_t roblox_state, unsigned u)
{
    setnvalue(offsets::lua_state::top.get(roblox_state), static_cast<double>(u));
    api_incr_top(roblox_state);
    return;
}

void lua_pushlstring(std::uint32_t roblox_state, const char* s, std::size_t len)
{
    luaC_checkGC(roblox_state);
    luaC_checkthreadsleep(roblox_state);
    setsvalue2s(roblox_state, offsets::lua_state::top.get(roblox_state), luaS_newlstr(roblox_state, s, len));
    api_incr_top(roblox_state);
    return;
}

void lua_pushstring(std::uint32_t roblox_state, const char* s)
{
    if (s == NULL)
        lua_pushnil(roblox_state);
    else
        lua_pushlstring(roblox_state, s, strlen(s));
}

void lua_pushboolean(std::uint32_t roblox_state, int b)
{
    setbvalue(offsets::lua_state::top.get(roblox_state), (b != 0)); // ensure that true is 1
    api_incr_top(roblox_state);
    return;
}

void lua_pushlightuserdata(std::uint32_t roblox_state, void* p)
{
    setpvalue(offsets::lua_state::top.get(roblox_state), p);
    api_incr_top(roblox_state);
    return;
}

/*
    Continuations won't work since callcheck is a big funky, I can fix by adding 2 tags but cba
*/
void lua_pushcclosurek(std::uint32_t roblox_state, lua_CFunction fn, const char* debugname, int nup, lua_Continuation cont)
{
    luaC_checkGC(roblox_state);
    luaC_checkthreadsleep(roblox_state);

    std::uint32_t closure = luaF_newCclosure(roblox_state, nup, getcurrenv(roblox_state));

    offsets::closure::f.set(closure, rbx_callcheck_jmp_addy); // add callcheck bypass
    offsets::closure::cont.set(closure, 0);
    offsets::closure::debugname.set(closure, reinterpret_cast<std::uint32_t>(debugname));
    offsets::closure::isC.set(closure, bypasses::callcheck(fn));

    *reinterpret_cast<TValue**>(offsets::lua_state::top.bind(roblox_state)) -= nup;

    while (nup--)
        setobj2s(roblox_state, &offsets::closure::upvals.get(closure)[nup], offsets::lua_state::top.get(roblox_state) + nup);

    setclvalue(roblox_state, offsets::lua_state::top.get(roblox_state), closure);

    api_incr_top(roblox_state);

    return;
}

/*
    Get functions
    (Your mom -> your dad)
*/

int lua_gettable(std::uint32_t roblox_state, int idx)
{
    luaC_checkthreadsleep(roblox_state);
    StkId t = rbx_index2addr(roblox_state, idx);

    rbx_luav_gettable(roblox_state, t, offsets::lua_state::top.get(roblox_state) - 1, offsets::lua_state::top.get(roblox_state) - 1);
    return ttype(offsets::lua_state::top.get(roblox_state) - 1);
}

int lua_getfield(std::uint32_t roblox_state, int idx, const char* k)
{
    luaC_checkthreadsleep(roblox_state);
    TValue* t = rbx_index2addr(roblox_state, idx);

    TValue key;
    setsvalue(roblox_state, &key, luaS_new(roblox_state, k));
    rbx_luav_gettable(roblox_state, t, &key, offsets::lua_state::top.get(roblox_state));
    api_incr_top(roblox_state);

    return ttype(offsets::lua_state::top.get(roblox_state) - 1);
}

void lua_createtable(std::uint32_t roblox_state, int narray, int nrec)
{
    luaC_checkGC(roblox_state);
    luaC_checkthreadsleep(roblox_state);
    sethvalue(roblox_state, offsets::lua_state::top.get(roblox_state), luaH_new(roblox_state, narray, nrec));
    api_incr_top(roblox_state);
}

int lua_getreadonly(std::uint32_t roblox_state, int objindex)
{
    const TValue* o = rbx_index2addr(roblox_state, objindex);

    const std::uint32_t t = hvalue(o);

    return *offsets::table::readonly.typed_bind_ptr(t);
}

int lua_getmetatable(std::uint32_t roblox_state, int objindex)
{
    luaC_checkthreadsleep(roblox_state);
    std::uint32_t mt = NULL;
    const TValue* obj = rbx_index2addr(roblox_state, objindex);
    switch (ttype(obj))
    {
    case LUA_TTABLE:
        mt = offsets::table::metatable.get(hvalue(obj));
        break;
    case LUA_TUSERDATA:
        mt = offsets::userdata::metatable.get(uvalue(obj));
        break;
    default:
        mt = offsets::global::mt.get(offsets::lua_state::global.get(roblox_state))[ttype(obj)];
        break;
    }
    if (mt)
    {
        sethvalue(roblox_state, offsets::lua_state::top.get(roblox_state), mt);
        api_incr_top(roblox_state);
    }
    return mt != NULL;
}

std::uint8_t lua_getthreadidentity(std::uint32_t roblox_state)
{
    std::uint32_t identity_ptr = *reinterpret_cast<std::uint32_t*>(__readfsdword(0x2C)) + 0xC8;
    return *reinterpret_cast<std::uint8_t*>(identity_ptr);
}

/*
** set functions (stack -> Lua)
*/

void lua_settable(std::uint32_t roblox_state, int idx)
{
    StkId t = rbx_index2addr(roblox_state, idx);

    rbx_luav_settable(roblox_state, t, offsets::lua_state::top.get(roblox_state) - 2, offsets::lua_state::top.get(roblox_state) - 1);
    *offsets::lua_state::top.typed_bind_ptr(roblox_state) -= 2; // pop index and value
    return;
}

void lua_setfield(std::uint32_t roblox_state, int idx, const char* k)
{
    TValue* t = rbx_index2addr(roblox_state, idx);
    TValue key;

    setsvalue(roblox_state, &key, luaS_new(roblox_state, k));
    rbx_luav_settable(roblox_state, t, &key, offsets::lua_state::top.get(roblox_state) - 1);
    api_decr_top(roblox_state);
    return;
}

void lua_setreadonly(std::uint32_t roblox_state, int objindex, bool enabled)
{
    const TValue* o = rbx_index2addr(roblox_state, objindex);

    const std::uint32_t t = hvalue(o);

    *offsets::table::readonly.typed_bind_ptr(t) = enabled;
    return;
}

void lua_setthreadidentity(std::uint32_t roblox_state, std::uint8_t identity)
{
    std::uint32_t identity_ptr = *reinterpret_cast<std::uint32_t*>(__readfsdword(0x2C)) + 0xC8;

    *reinterpret_cast<std::uint8_t*>(identity_ptr) = identity;
    *offsets::extra_space::identity.typed_bind_ptr(offsets::lua_state::extra_space.get(roblox_state)) = identity;
}

/*
    Removed readonly checks because I don't really care lol
*/

int lua_setmetatable(std::uint32_t roblox_state, int objindex)
{
    TValue* obj = rbx_index2addr(roblox_state, objindex);

    std::uint32_t mt = NULL;
    if (!ttisnil(offsets::lua_state::top.get(roblox_state) - 1))
    {
        mt = hvalue(offsets::lua_state::top.get(roblox_state) - 1);
    }

    switch (ttype(obj))
    {
        case LUA_TTABLE:
        {
            offsets::table::metatable.set(hvalue(obj), mt);
            if (mt)
                luaC_objbarrier(roblox_state, hvalue(obj), mt);
            break;
        }
        case LUA_TUSERDATA:
        {
            offsets::userdata::metatable.set(uvalue(obj), mt);
            if (mt)
                luaC_objbarrier(roblox_state, uvalue(obj), mt);
            break;
        }
        default:
        {
            offsets::global::mt.get(offsets::lua_state::global.get(roblox_state))[ttype(obj)] = mt;
            break;
        }
    }

    api_decr_top(roblox_state);
    return 1;
}