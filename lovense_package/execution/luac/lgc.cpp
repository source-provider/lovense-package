#include <cstdint>

#include "roblox/update.hpp"
#include "roblox/functions.hpp"

#include "lgc.hpp"

#define maskmarks (~(bitmask(BLACKBIT) | WHITEBITS))
#define black2gray(x) resetbit((*reinterpret_cast<std::uint8_t*>(offsets::gch::marked.bind(x))), BLACKBIT)
#define makewhite(g, x) (*offsets::gch::marked.typed_bind_ptr(x) = ((*offsets::gch::marked.typed_bind_ptr(x) & maskmarks) | luaC_white(g)))


void luaC_initobj(std::uint32_t roblox_state, std::uint32_t obj, std::uint8_t tt)
{
	std::uint32_t global = offsets::lua_state::global.get(roblox_state);

	offsets::gch::marked.set(obj, luaC_white(global));
	offsets::gch::tt.set(obj, tt);
	offsets::gch::memcat.set(obj, offsets::lua_state::activememcat.get(roblox_state));
}

void luaC_checkGC(std::uint32_t roblox_state)
{
	std::uint32_t global = offsets::lua_state::global.get(roblox_state);

	if (offsets::global::totalbytes.get(global) >= offsets::global::GCthreshold.get(global))
	{
		rbx_luac_step(roblox_state, true);
	}
}

void luaC_wakethread(std::uint32_t roblox_state)
{
	if (!luaC_threadsleeping(roblox_state))
		return;

	std::uint32_t g = offsets::lua_state::global.get(roblox_state);
	
	resetbit((*reinterpret_cast<std::uint8_t*>(offsets::lua_state::stackstate.bind(roblox_state))), THREAD_SLEEPINGBIT);

	if (keepinvariant(g))
	{
		offsets::lua_state::gclist.set(roblox_state, offsets::global::grayagain.get(g));
		offsets::global::grayagain.set(g, roblox_state);

		black2gray(roblox_state);
	}
}

void luaC_barrierf(std::uint32_t roblox_state, std::uint32_t o, std::uint32_t v)
{
	std::uint32_t g = offsets::lua_state::global.get(roblox_state);
	LUAU_ASSERT(isblack(o) && iswhite(v) && !isdead(g, v) && !isdead(g, o));
	LUAU_ASSERT(offsets::global::gcstate.get(g) != GCSpause);
	// must keep invariant?
	if (keepinvariant(g))
		rbx_reallymarkobject(g, v); // restore invariant
	else                        // don't mind
		makewhite(g, o);        // mark as white just to avoid other barriers
}