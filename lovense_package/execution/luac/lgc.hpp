#pragma once
#include "roblox/update.hpp"

#define resetbits(x, m) ((x) &= static_cast<std::uint8_t>(~(m)))
#define setbits(x, m) ((x) |= (m))
#define testbits(x, m) ((x) & (m))
#define bitmask(b) (1 << (b))
#define bit2mask(b1, b2) (bitmask(b1) | bitmask(b2))
#define l_setbit(x, b) setbits(x, bitmask(b))
#define resetbit(x, b) resetbits(x, bitmask(b))
#define testbit(x, b) testbits(x, bitmask(b))
#define set2bits(x, b1, b2) setbits(x, (bit2mask(b1, b2)))
#define reset2bits(x, b1, b2) resetbits(x, (bit2mask(b1, b2)))
#define test2bits(x, b1, b2) testbits(x, (bit2mask(b1, b2)))

#define bit2mask(b1, b2) (bitmask(b1) | bitmask(b2))

#define WHITE0BIT 0
#define WHITE1BIT 1
#define BLACKBIT 2
#define FIXEDBIT 3
#define WHITEBITS bit2mask(WHITE0BIT, WHITE1BIT)

#define iswhite(x) test2bits(offsets::gch::marked.get(x), WHITE0BIT, WHITE1BIT)
#define isblack(x) testbit(offsets::gch::marked.get(x), BLACKBIT)
#define isgray(x) (!testbits(offsets::gch::marked.get(x), WHITEBITS | bitmask(BLACKBIT)))
#define isfixed(x) testbit(offsets::gch::marked.get(x), FIXEDBIT)

#define changewhite(x) (*offsets::gch::marked.typed_bind_ptr(x) ^= WHITEBITS)
#define otherwhite(g) (offsets::global::currentwhite.get(g) ^ WHITEBITS)
#define isdead(g, v) ((offsets::gch::marked.get(v) & (WHITEBITS | bitmask(FIXEDBIT))) == (otherwhite(g) & WHITEBITS))

/*
** Possible states of the Garbage Collector
*/
#define GCSpause 0
#define GCSpropagate 1
#define GCSpropagateagain 2
#define GCSatomic 3
#define GCSsweep 4

#define THREAD_ACTIVEBIT 0   // thread is currently active
#define THREAD_SLEEPINGBIT 1 // thread is not executing and stack should not be modified

#define luaC_white(g) offsets::global::currentwhite.get(g) & WHITEBITS
#define luaC_threadsleeping(L) (testbit(offsets::lua_state::stackstate.get(L), THREAD_SLEEPINGBIT))

#define keepinvariant(g) (offsets::global::gcstate.get(g) == GCSpropagate || offsets::global::gcstate.get(g) == GCSpropagateagain || offsets::global::gcstate.get(g) == GCSatomic)

void luaC_initobj(std::uint32_t roblox_state, std::uint32_t obj, std::uint8_t tt);
void luaC_checkGC(std::uint32_t roblox_state);
void luaC_wakethread(std::uint32_t roblox_state);
void luaC_barrierf(std::uint32_t roblox_state, std::uint32_t o, std::uint32_t v);

#define luaC_init(a, b, c) luaC_initobj(a, b, c)

#define luaC_checkthreadsleep(L) \
    { \
        if (luaC_threadsleeping(L)) \
            luaC_wakethread(L); \
    }

#define luaC_barriert(L, t, v) \
    { \
        if (iscollectable(v) && isblack(t) && iswhite(gcvalue(v))) \
            rbx_luac_barriertable(L, t, gcvalue(v)); \
    }

#define luaC_objbarrier(L, p, o) \
    { \
        if (isblack(p) && iswhite(o)) \
            luaC_barrierf(L, p, o); \
    }

#define luaC_threadbarrier(L) \
    { \
        if (luaC_threadsleeping(L)) \
            luaC_wakethread(L); \
    }