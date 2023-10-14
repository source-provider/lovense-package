#include <cstdint>
#include <cstddef>

#include "lmem.hpp"
#include "lstring.hpp"
#include "lgc.hpp"


unsigned int luaS_hash(const char* str, size_t len)
{
    // Note that this hashing algorithm is replicated in BytecodeBuilder.cpp, BytecodeBuilder::getStringHash
    unsigned int a = 0, b = 0;
    unsigned int h = unsigned(len);

    // hash prefix in 12b chunks (using aligned reads) with ARX based hash (LuaJIT v2.1, lookup3)
    // note that we stop at length<32 to maintain compatibility with Lua 5.1
    while (len >= 32)
    {
#define rol(x, s) ((x >> s) | (x << (32 - s)))
#define mix(u, v, w) a ^= h, a -= rol(h, u), b ^= a, b -= rol(a, v), h ^= b, h -= rol(b, w)

        // should compile into fast unaligned reads
        uint32_t block[3];
        memcpy(block, str, 12);

        a += block[0];
        b += block[1];
        h += block[2];
        mix(14, 11, 25);
        str += 12;
        len -= 12;

#undef mix
#undef rol
    }

    // original Lua 5.1 hash for compatibility (exact match when len<32)
    for (size_t i = len; i > 0; --i)
        h ^= (h << 5) + (h >> 2) + (uint8_t)str[i - 1];

    return h;
}

std::uint32_t newlstr(std::uint32_t roblox_state, const char* str, std::size_t l, unsigned int h)
{
    using namespace offsets::string;
    std::uint32_t global = offsets::lua_state::global.get(roblox_state);
    std::uint32_t tstring = luaM_newgco_(roblox_state, sizestring(l), offsets::lua_state::activememcat.get(roblox_state));

    offsets::gch::marked.set(tstring, luaC_white(global));
    offsets::gch::tt.set(tstring, LUA_TSTRING);
    offsets::gch::memcat.set(tstring, offsets::lua_state::activememcat.get(roblox_state));

    atom.set(tstring, ATOM_UNDEF);
    len.set(tstring, l);
    hash.set(tstring, h);

    memcpy(data.custom_typed_bind<void*>(tstring), str, l);
    data.custom_typed_bind<char*>(tstring)[l] = '\0';

    h = lmod(h, offsets::global::strt::size.get(global));
    next.set(tstring, offsets::global::strt::hash.get(global)[h]);
    offsets::global::strt::hash.get(global)[h] = tstring;
    *offsets::global::strt::nuse.typed_bind_ptr(global) += 1;

    return tstring;
}

std::uint32_t luaS_newlstr(std::uint32_t roblox_state, const char* str, std::size_t l)
{
    unsigned int h = luaS_hash(str, l);

    using namespace offsets::global::strt;
    std::uint32_t global = offsets::lua_state::global.get(roblox_state);
    std::uint32_t* hash_table = hash.get(global);

    for (std::uint32_t el = hash_table[lmod(h, size.get(global))]; el != NULL; el = offsets::string::next.get(el))
    {
        const char* data = offsets::string::data.custom_typed_bind<const char*>(el);

        if (offsets::string::len.get(el) == l && !memcmp(str, data, l))
        {
            if (isdead(global, el))
                changewhite(el);
            return el;
        }
    }

    return newlstr(roblox_state, str, l, h);
}