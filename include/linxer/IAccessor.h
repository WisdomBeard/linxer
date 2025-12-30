#pragma once

#include <optional>
#include <string>
#include <vector>

namespace linxer
{
    class IAccessor
    {
    public:
        virtual std::string get_line(int line_index, bool update = false) = 0;
        virtual std::optional< std::string > try_get_line(int line_index, bool update = false) = 0;
        virtual std::vector< std::string > get_lines(int from_line_index, int to_line_index, bool update = false) = 0;
        virtual std::vector< std::string > try_get_lines(int from_line_index, int to_line_index, bool update = false) = 0;
        virtual void update_index() = 0;

        virtual ~IAccessor() noexcept = default;
    };
}
