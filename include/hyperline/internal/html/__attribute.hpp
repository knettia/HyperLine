#pragma once

// std
#include <algorithm>

// hyperline
#include "../__format.hpp" // __hyperline_format
#include "../__funcexcept.hpp" // __throw_runtime_error

namespace hyperline::html::__1
{
	struct attribute
	{
	private:
		std::string __key;
		std::string __val;

		inline static void __validate_key(const std::string &key)
		{
			if (key.empty() || std::any_of(key.begin(), key.end(), ::isspace))
			{ __throw_runtime_error(__hyperline_format("Invalid attribute key \"{0}\": XML keys may not be empty or contain whitespaces", key)); }
		}
	public:
		inline attribute(const std::string &key, const std::string &value = "")
		{
			attribute::__validate_key(key);
			__key = key;
			__val = value;
		}

		// inline attribute(attribute&& other) noexcept: __key(std::move(other.__key)), __val(std::move(other.__val)) { }

		inline std::string &value()
		{ return __val; }

		inline const std::string &value() const
		{ return __val; }

		inline std::string &key()
		{ return __key; }

		inline const std::string &key() const
		{ return __key; }
	};
}