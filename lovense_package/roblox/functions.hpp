#pragma once
#include "update.hpp"
#include "bypasses/bypasses.hpp"
#include "scheduler/scheduler.hpp"


// Roblox functions

using rbx_output_t = void(__fastcall*)(std::int16_t code, const char*);
static auto rbx_output = reinterpret_cast<rbx_output_t>(rbx_output_addy);

using rbx_scheduler_t = scheduler_t * (*)();
static rbx_scheduler_t rbx_scheduler = reinterpret_cast<rbx_scheduler_t>(rbx_scheduler_addy);

// Technically not a Roblox function but I'll make it this anyways
using rbx_deserialize_t = std::uint32_t(__fastcall*)(std::uint32_t rbx_state, const char* chunk_name, const char* bytecode, std::size_t bytecode_len, std::int32_t environment_index);
static auto rbx_deserialize = bypasses::retcheck<rbx_deserialize_t, std::uint32_t>(rbx_deserialize_addy);

using rbx_spawn_t = void(__cdecl*)(std::uint32_t rbx_state);
static auto rbx_spawn = bypasses::retcheck<rbx_spawn_t>(rbx_spawn_addy);

// "InvalidInstance"
using rbx_reflection_push_instance_t = void(__stdcall*)(std::uint32_t roblox_state, std::uint32_t reflection_obj);
static auto rbx_reflection_push_instance = bypasses::retcheck<rbx_reflection_push_instance_t>(rbx_reflection_push_instance_addy);

// luaC_* functions

using rbx_luac_step_t = void(__fastcall*)(std::uint32_t rbx_state, bool val);
static auto rbx_luac_step = bypasses::retcheck<rbx_luac_step_t>(rbx_luac_step_addy);

using rbx_luac_barriertable_t = void(__fastcall*)(std::uint32_t rbx_state, std::uint32_t t, std::uint32_t v);
static rbx_luac_barriertable_t rbx_luac_barriertable = reinterpret_cast<rbx_luac_barriertable_t>(rbx_luac_barriertable_addy);

using rbx_reallymarkobject_t = void(__fastcall*)(std::uint32_t global, std::uint32_t o);
static rbx_reallymarkobject_t rbx_reallymarkobject = reinterpret_cast<rbx_reallymarkobject_t>(rbx_reallymarkobject_addy);

// luaV_* functions

using rbx_luav_gettable_t = void(__fastcall*)(std::uint32_t rbx_state, TValue* t, TValue* key, TValue* val);
static auto rbx_luav_gettable = bypasses::retcheck<rbx_luav_gettable_t>(rbx_luav_gettable_addy);

using rbx_luav_settable_t = void(__fastcall*)(std::uint32_t rbx_state, TValue* t, TValue* key, TValue* val);
static auto rbx_luav_settable = bypasses::retcheck<rbx_luav_settable_t>(rbx_luav_settable_addy);

// luaM_* functions

using rbx_luam_visitgco_t = void(__fastcall*)(std::uint32_t rbx_state, void* context, bool(__cdecl*)(void* context, std::uint32_t current_page, std::uint32_t gco));
static auto rbx_luam_visitgco = bypasses::retcheck<rbx_luam_visitgco_t>(rbx_luam_visitgco_addy);

// misc functions

using rbx_index2addr_t = TValue * (__fastcall*)(std::uint32_t rbx_state, std::int32_t idx);
static auto rbx_index2addr = reinterpret_cast<rbx_index2addr_t>(rbx_index2addr_addy);