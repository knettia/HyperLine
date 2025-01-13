#pragma once

#define __hyperline_convertible_template(__source_type, __target_type) typename std::enable_if<std::is_convertible<__source_type, __target_type>::value>::type* = nullptr