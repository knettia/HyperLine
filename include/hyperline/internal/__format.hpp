#pragma once

// TODO: handle function aliases instead of macros
#ifndef HYPERLINE_FORMAT_IMPLEMENTATION
#include <format> 
#define __hyperline_format std::format
#else
#include <fmt/format.h>
#define __hyperline_format fmt::format
#endif