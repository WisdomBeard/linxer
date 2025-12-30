#pragma once

#include <atomic>
#include <deque>
#include <fstream>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>

#include "linxer/IAccessor.h"
#include "linxer/StringRingBuffer.h"

namespace linxer
{
    class Accessor : public IAccessor
    {
    // Sub-types
    protected:
        using Index = std::deque< size_t >;

    // Attributes
    protected:
        std::ifstream stream;
        std::thread refresh_thread;
        std::mutex mtx;
        std::atomic_bool stop;
        Index index;
        StringRingBuffer buffer;
    
    // methods
    protected:
        void refresh_loop(unsigned int refresh_period_ms);
        std::string _safe_get_line(int line_index, bool update);
        std::string _get_line(int line_index, bool update);
        std::optional< std::string > _try_get_line(int line_index, bool update);
        std::vector< std::string > _get_lines(int from_line_index, int to_line_index, bool update);
        std::vector< std::string > _try_get_lines(int from_line_index, int to_line_index, bool update);
        void _update_index() noexcept;

    public:
        Accessor() = default;
        Accessor(const std::string& file_path, unsigned int refresh_period_ms, int buffer_size);
        Accessor(const Accessor& other) = delete;
        Accessor(Accessor&& other) = default;
        Accessor& operator=(const Accessor& other) = delete;
        Accessor& operator=(Accessor&& other) = default;

        virtual ~Accessor() noexcept;

        // IAccessor - start
        virtual std::string get_line(int line_index, bool update = false) override;
        virtual std::optional< std::string > try_get_line(int line_index, bool update = false) override;
        virtual std::vector< std::string > get_lines(int from_line_index, int to_line_index, bool update = false) override;
        virtual std::vector< std::string > try_get_lines(int from_line_index, int to_line_index, bool update = false) override;
        virtual void update_index() noexcept override;
        // IAccessor - end
    };
}
