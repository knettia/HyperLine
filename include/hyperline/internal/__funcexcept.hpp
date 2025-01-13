#pragma once

#include <stdexcept>

namespace hyperline::html::__1
{
	inline void __throw_runtime_error(const std::string &s)
	{ throw std::runtime_error(s); }
}