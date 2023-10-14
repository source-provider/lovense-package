#pragma once

std::uint32_t luaM_newgco_(std::uint32_t roblox_state, std::size_t nsize, std::uint8_t memcat);
void luaM_getpagewalkinfo(std::uint32_t page, char** start, char** end, int* busyBlocks, int* blockSize);