#include "roblox/update.hpp"


#define curr_func(roblox_state) (offsets::call_info::func.get(offsets::lua_state::ci.get(roblox_state)))