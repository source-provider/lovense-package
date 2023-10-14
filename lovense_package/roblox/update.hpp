#pragma once
#include <Windows.h>
#include <cstdint>
#include <algorithm>
#include <string>

#include "encryptions.hpp"
#include "execution/luac/lobject.hpp"



const std::uint32_t base = reinterpret_cast<std::uint32_t>(GetModuleHandle(NULL));


// Roblox functions

const std::uint32_t rbx_output_addy = base + 0x2EFF60;

const std::uint32_t rbx_scheduler_addy = base + 0xF2DEC0;

const std::uint32_t rbx_deserialize_addy = base + 0x14A8C70;

const std::uint32_t rbx_spawn_addy = base + 0x4773A0;

const std::uint32_t rbx_reflection_push_instance_addy = base + 0x0054CCF0;

// luaC_* functions

const std::uint32_t rbx_luac_step_addy = base + 0x14DE210;

const std::uint32_t rbx_luac_barriertable_addy = base + 0x14DE620;

const std::uint32_t rbx_reallymarkobject_addy = base + 0x014DC3C0;

// luaV_* functions

const std::uint32_t rbx_luav_gettable_addy = base + 0x14FAB70;

const std::uint32_t rbx_luav_settable_addy = base + 0x14FB120;

// luaM_* functions

const std::uint32_t rbx_luam_visitgco_addy = base + 0x01502F60;

// bypass stuff

const std::uint32_t retcheck_addy = base + 0x01CCE2BE;

// Must use the FindWindowA hook for these
// v0 = off_20F22CC("Kernel32");
const std::uint32_t rbx_callcheck_data_addy = base + 0x398EEE0;
const std::uint32_t rbx_callcheck_jmp_addy = base + 0x002F5F47;

// misc functions

const std::uint32_t rbx_index2addr_addy = base + 0x14A5B30;

const std::uint32_t dummynode = base + 0x21F02A8;

static double* rbx_xor_key = reinterpret_cast<double*>(base + 0x3760A30);

static const char** rbx_luat_typenames = reinterpret_cast<const char**>(base + 0x21F02C8);


static void dump_types()
{
	bool userdata = false;

	std::printf("enum lua_Type\n{\n");
	std::printf("LUA_TNONE = -1,\n");
	for (std::uint32_t i = 0; i < 10; ++i)
	{
		std::string name{ "LUA_T" };
		if (!strcmp(rbx_luat_typenames[i], "userdata"))
			name += ((userdata = !userdata) ? "lightuserdata" : "userdata");
		else
			name += rbx_luat_typenames[i];

		std::transform(name.begin(), name.end(), name.begin(), toupper);

		if (i % 9)
			name += ",";

		std::printf("%s\n", name.c_str());
	}
	std::printf("};\n");
}

enum lua_Type
{
	LUA_TNONE = -1,
	LUA_TNIL,
	LUA_TBOOLEAN,
	LUA_TVECTOR,
	LUA_TLIGHTUSERDATA,
	LUA_TNUMBER,
	LUA_TSTRING,
	LUA_TTABLE,
	LUA_TFUNCTION,
	LUA_TTHREAD,
	LUA_TUSERDATA
};

namespace offsets
{
	namespace script_context
	{
		static offset<std::uint32_t> lua_state{ 0x124, encryption::encryption_type::ENC_SUB1 };
	}

	namespace lua_state
	{
		static offset<TValue*> top{ 0xC };
		static offset<TValue*> base{ 0x10 };

		static offset<std::uint32_t> extra_space{ 0x48 }; // dont ever change
		static offset<std::uint32_t> global{ 0x8, encryption::encryption_type::ENC_ADD };
		static offset<std::uint8_t> activememcat{ 0x4 }; // dont ever change

		static offset<std::uint32_t> ci{ 0x18 };
		static offset<std::uint32_t> base_ci{ 0x24 };
		static offset<std::uint32_t> gt{ 0x3C };
		static offset<std::uint8_t> stackstate{ 0x5 };
		static offset<std::uint32_t> gclist{ 0x38 };
	}

	namespace call_info
	{
		static offset<TValue*> func{ 0x4 };
	};

	namespace global
	{
		using frealloc_t = std::uint32_t(__cdecl*)(std::uint32_t, std::uint32_t, std::uint32_t, std::size_t);

		/*
			These are inside global still, so use global
		*/
		namespace strt
		{
			static offset<std::size_t> nuse{ 0x8 };
			static offset<std::uint32_t*> hash{ 0x4 };
			static offset<std::size_t> size{ 0x0 };
		};

		static offset<frealloc_t> frealloc{ 0xC }; // dont ever change
		static offset<std::uint32_t> ud{ 0x10 }; // dont ever change
		static offset<std::size_t> totalbytes{ 0x38 };
		static offset<std::size_t> GCthreshold{ 0x30 };
		static offset<std::size_t> memcatbytes{ 0x144 };
		static offset<std::uint8_t> currentwhite{ 0x14 };
		static offset<std::uint8_t> gcstate{ 0x15 }; // hasnt changed in a bit, might change tho (not enough data from my side)
		static offset<std::uint32_t> grayagain{ 0x20 };
		static offset<TValue*> registry{ 0x610 };
		static offset<std::uint32_t*> mt{ 0x590 };
		static offset<std::uint32_t> allgcopages{ 0x13C };
	}

	namespace page
	{
		static offset<std::uint32_t> gcolistnext{ 0x0 };
		static offset<std::uint32_t> pagesize{ 0x10 };
		static offset<std::uint32_t> blocksize{ 0x14 };
		static offset<std::uint32_t> busyblocks{ 0x20 };
		static offset<std::uint32_t> data{ 0x28 };
		static offset<std::uint32_t> freenext{ 0x1C };
	}

	namespace gch
	{
		static offset<std::uint8_t> marked{ 0x2 };
		static offset<std::uint8_t> tt{ 0x1 };
		static offset<std::uint8_t> memcat{ 0x0 };
	}

	namespace closure
	{
		static offset<std::uint8_t> isC{ 0x3 };
		static offset<std::uint8_t> nupvalues{ 0x4 };
		static offset<std::uint8_t> stacksize{ 0x5 };
		static offset<std::uint8_t> preload{ 0x6 };


		static offset<std::uint32_t> env{ 0xC }; // doesn't change
		static offset<std::uint32_t> f{ 0x10, encryption::encryption_type::ENC_ADD }; // encryption changes, not offset
		static offset<std::uint32_t> cont{ 0x14, encryption::encryption_type::ENC_ADD }; // encryption changes, not offset
		static offset<const char*> debugname{ 0x18, encryption::encryption_type::ENC_SUB1 }; // encryption changes, not offset
		static offset<TValue*> upvals{ 0x20 }; // doesnt change
	}

	namespace table
	{
		static offset<bool> readonly{ 0x7 };
		static offset<std::uint32_t> node{ 0x1C, encryption::encryption_type::ENC_ADD };
		static offset<std::uint32_t> lsizenode{ 0x6 }; // dont need update

		static offset<std::uint32_t> array{ 0x14, encryption::encryption_type::ENC_ADD };
		static offset<std::uint32_t> sizearray{ 0x8 }; // dont need update

		static offset<std::uint32_t> metatable{ 0x10, encryption::encryption_type::ENC_ADD };
		static offset<std::uint32_t> gclist{ 0x18 };
	}

	namespace userdata
	{
		static offset<std::uint8_t> tag{ 0x3 };
		static offset<std::size_t> len{ 0x4 };
		static offset<std::uint32_t> metatable{ 0x8, encryption::encryption_type::ENC_SUB2 };
		static offset<std::uint32_t> data{ 0x10 };
	}

	namespace proto
	{
		static offset<std::size_t> sizek{0x30};
		static offset<TValue*> k{ 0x8, encryption::encryption_type::ENC_ADD };
	}

	namespace string
	{
		static offset<std::uint16_t> atom{ 0x4 };
		static offset<std::uint32_t> next{ 0x8 };
		static offset<std::uint32_t> hash{ 0xC, encryption::encryption_type::ENC_SUB1 }; // offset doesnt change, encryption does
		static offset<std::size_t> len{ 0x10, encryption::encryption_type::ENC_SUB2 }; // offset doesnt change, encryption does

		/*
			Don't dereference. Use typed bind.
		*/
		static offset<const char*> data{ 0x14 }; // doesnt change
	}

	namespace extra_space
	{
		static offset<std::uint8_t> identity{ 0x18 }; // dont ever change
		static offset<std::uint32_t> calling_script{ 0x30 };
	}
}


/*
	IMPORTANT UPDATING TIPS FROM FISHY:
	1) If you get confused about an encryption, reference the encryptions.hpp file, I document it well.
	2) Not every single offset will change but that doesn't mean you shouldn't check them all.
	3) You CANNOT figure out an encryption if it's setter has a value of 0, they all mathematically fall back to an equation with multiple outcomes. (Example: debugname on a function without one)
	4) Although your offsets might not crash you at first, that doesn't mean they're not incorrect. They'll crash you sometime later and it'll be a pain in the ass to find ;3
	5) Don't fuck with the lua api if you don't want problems, everything you need to update is inside update.hpp, that's for the WHOLE source.
*/