#include <cstdint>
#include <string>

#include "roblox/update.hpp"


std::uint32_t luaM_newgco_(std::uint32_t roblox_state, std::size_t nsize, std::uint8_t memcat)
{
	std::uint32_t global = offsets::lua_state::global.get(roblox_state);
	std::uint32_t block = offsets::global::frealloc.get(global)(offsets::global::ud.get(global), NULL, NULL, nsize);

	ZeroMemory(reinterpret_cast<void*>(block), nsize);

	*reinterpret_cast<std::size_t*>(offsets::global::totalbytes.bind(global)) += nsize;
	*reinterpret_cast<std::size_t*>(offsets::global::memcatbytes.bind(global) + 4 * memcat) += nsize;

	return block;
}


// Caution: may or may not be wrong, I am not sure if I implemented it correctly.
void luaM_getpagewalkinfo(std::uint32_t page, char** start, char** end, int* busyBlocks, int* blockSize)
{
    int blockCount = (offsets::page::pagesize.get(page) - offsets::page::data.get_offset()) / offsets::page::blocksize.get(page);

    char* data = reinterpret_cast<char*>(offsets::page::data.bind(page)); // silences ubsan when indexing page->data

    *start = data + offsets::page::freenext.get(page) + offsets::page::blocksize.get(page);
    *end = data + blockCount * offsets::page::blocksize.get(page);
    *busyBlocks = offsets::page::busyblocks.get(page);
    *blockSize = offsets::page::blocksize.get(page);
}