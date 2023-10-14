#include <iostream>
#include <cstdint>

#include "roblox/update.hpp"

#include "lmem.hpp"
#include "lgc.hpp"


std::uint32_t luaH_new(std::uint32_t roblox_state, int narray, int nhash)
{
	std::uint32_t t = luaM_newgco_(roblox_state, 32, offsets::lua_state::activememcat.get(roblox_state));
	luaC_init(roblox_state, t, LUA_TTABLE);

	offsets::table::node.set(t, dummynode);
	offsets::table::array.set(t, 0);
	offsets::table::metatable.set(t, 0);

	if (narray > 0)
	{
		std::printf("Sorry! narray is currently not supported ATM. [luaH_new] - ltable.cpp\n");
		return 0;
	}

	if (nhash > 0)
	{
		std::printf("Sorry! nhash is currently not supported ATM. [luaH_new] - ltable.cpp\n");
		return 0;
	}

	return t;
}