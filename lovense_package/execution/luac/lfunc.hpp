#pragma once

std::uint32_t luaF_newCclosure(std::uint32_t roblox_state, int nelems, std::uint32_t environment);

#define sizeCclosure(n) (32 + 16 * (n))