#pragma once
#include <cstdint>


/*
	ENCRYPTION REPRESENTED AS AN ALGEBRA EQUATION:
	SUBTRACTION A:
	// *reinterpret_cast<std::uintptr_t*>(val + offset) = new_value - (val + offset);
	goal: get b
	c = b - a
	+a    +a
	ANSWER:		c + a = b

	SUBTRACTION B:
	// *reinterpret_cast<std::uintptr_t*>(val + offset) = (val + offset) - new_value;
	goal: get a
	c = b - a
	+a    +a
	c + a = b
	-c      -c
	ANSWER: 	a = b - c
*/


// ONLY POINTERS WILL EVER BE ENCRYPTED, SO DON'T WORRY ABOUT ANY OTHER VALUES IN THE ENCRYPTION PROCESS.

namespace encryption
{
	enum class encryption_type
	{
		ENC_NONE,
		ENC_ADD, // *reinterpret_cast<std::uintptr_t*>(val + offset) = new_value + (val + offset);
		ENC_SUB1, // *reinterpret_cast<std::uintptr_t*>(val + offset) = new_value - (val + offset);
		ENC_SUB2, // *reinterpret_cast<std::uintptr_t*>(val + offset) = (val + offset) - new_value;
		ENC_XOR // *reinterpret_cast<std::uintptr_t*>(val + offset) = new_value ^ (val + offset);
	};

	/*
		Automatically casted to std::uint32_t since pointer encryption is ONLY for well.. pointers. It's impossible to have the same kind of functional encryption for any other type.
	*/
	template<typename T>
	constexpr T get(std::uintptr_t value, encryption_type t)
	{
		std::uint32_t val = 0;

		switch (t)
		{
		case encryption_type::ENC_NONE:
			return *reinterpret_cast<T*>(value);
			break;
		case encryption_type::ENC_ADD:
			val = *reinterpret_cast<std::uint32_t*>(value) - value;
			break;
		case encryption_type::ENC_SUB1:
			val = *reinterpret_cast<std::uint32_t*>(value) + value;
			break;
		case encryption_type::ENC_SUB2:
			val = value - *reinterpret_cast<std::uint32_t*>(value);
			break;
		case encryption_type::ENC_XOR:
			val = *reinterpret_cast<std::uint32_t*>(value) ^ value;
			break;
		}

		return *reinterpret_cast<T*>(&val);
	}

	constexpr void set(std::uint32_t value, std::uint32_t new_value, encryption_type t)
	{
		switch (t)
		{
		case encryption_type::ENC_NONE:
			*reinterpret_cast<std::uint32_t*>(value) = new_value;
			break;
		case encryption_type::ENC_ADD:
			*reinterpret_cast<std::uint32_t*>(value) = new_value + value;
			break;
		case encryption_type::ENC_SUB1:
			*reinterpret_cast<std::uint32_t*>(value) = new_value - value;
			break;
		case encryption_type::ENC_SUB2:
			*reinterpret_cast<std::uint32_t*>(value) = value - new_value;
			break;
		case encryption_type::ENC_XOR:
			*reinterpret_cast<std::uint32_t*>(value) = new_value ^ value;
			break;
		}
	}
}


template<typename T>
struct offset
{
private:
	const std::uint32_t offset_value = 0;
	const encryption::encryption_type enc = encryption::encryption_type::ENC_NONE;
public:
	offset(std::uint32_t value, encryption::encryption_type enc = encryption::encryption_type::ENC_NONE) : offset_value{ value }, enc{ enc } {};


	std::uint32_t bind(std::uint32_t base) // for when I want to directly access the raw ptr
	{
		return base + this->offset_value;
	}

	/*
		Warning: no encryptions will be done since you will be handling a raw pointer YOURSELF. You can manually use encryption library if u desire.
	*/

	template<typename OV>
	OV custom_typed_bind(std::uint32_t base)
	{
		return reinterpret_cast<OV>(base + this->offset_value);
	}

	T* typed_bind_ptr(std::uint32_t base)
	{
		return reinterpret_cast<T*>(base + this->offset_value);
	}

	T get(std::uint32_t base)
	{
		return encryption::get<T>(this->bind(base), enc);
	}

	std::uint32_t get_offset()
	{
		return this->offset_value;
	}

	void set(std::uint32_t base, std::uint32_t new_value)
	{
		encryption::set(this->bind(base), new_value, enc);
	}
};