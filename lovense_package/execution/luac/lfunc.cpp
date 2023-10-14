#include "roblox/update.hpp"

#include "lmem.hpp"
#include "lfunc.hpp"
#include "lgc.hpp"

std::uint32_t luaF_newCclosure(std::uint32_t roblox_state, int nelems, std::uint32_t environment)
{
	using namespace offsets::closure;

	std::uint32_t closure = luaM_newgco_(roblox_state, sizeCclosure(nelems), offsets::lua_state::activememcat.get(roblox_state));
	luaC_init(roblox_state, closure, LUA_TFUNCTION);

	isC.set(closure, 1);
	env.set(closure, environment);
	nupvalues.set(closure, nelems);
	stacksize.set(closure, LUA_MINSTACK);
	preload.set(closure, 0);
	f.set(closure, 0);
	cont.set(closure, 0);
	debugname.set(closure, 0);

	return closure;
}