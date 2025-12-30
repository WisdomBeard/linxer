#pragma once

#include <memory>
#include <string>

#include "linxer/IAccessor.h"

#ifdef _WIN32
    #define LINXER_EXPORT __declspec(dllexport)
#else
    #define LINXER_EXPORT
#endif

namespace linxer
{
    LINXER_EXPORT std::unique_ptr< IAccessor > new_accessor(const std::string& file_path, unsigned int refresh_period_ms = 0, int buffer_size = 0);
}
