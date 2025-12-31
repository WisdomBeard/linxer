#include <chrono>
#include <sstream>
#include <stdexcept>

#include "linxer/Accessor.h"

using namespace std;

namespace linxer
{
    Accessor::Accessor(const string& file_path, unsigned int refresh_period_ms, int buffer_size)
        : stream(file_path, ios_base::in)
        , refresh_thread()
        , mtx()
        , stop(false)
        , index()
        , buffer(buffer_size)
    {
        if (!stream)
        {
            stringstream error_msg;
            error_msg << "Failed to open '" << file_path << "'";
            throw invalid_argument{error_msg.str()};
        }

        index.push_back(0);
        _update_index();
        if (refresh_period_ms > 0)
        {
            refresh_thread = thread{&Accessor::refresh_loop, this, refresh_period_ms};
        }
    }
    Accessor::~Accessor() noexcept
    {
        stop = true;
        if (refresh_thread.joinable())
        {
            refresh_thread.join();
        }
    }

    void Accessor::refresh_loop(unsigned int refresh_period_ms)
    {
        chrono::milliseconds period{refresh_period_ms};
        for (auto next = chrono::system_clock::now() + period ; !stop ; next += period)
        {
            {
                scoped_lock sl{mtx};
                _update_index();
            }
            this_thread::sleep_until(next);
        }
    }

    // IAccessor - start
    
    string Accessor::get_line(int line_index, bool update)
    {
        scoped_lock sl{mtx};
        return _safe_get_line(line_index, update);
    }

    string Accessor::_safe_get_line(int line_index, bool update)
    {
        auto index_size = static_cast< int >(index.size()) - 1;
        stringstream error_msg;
        error_msg << "Invalid index " << line_index << " : ";

        if (line_index < -index_size - 1)
        {
            error_msg << "negative index too big (< -" << (index_size+1) << ")";
            throw invalid_argument{error_msg.str()};
        }
        if (line_index > index_size)
        {
            error_msg << "positive index too big (> " << index_size << ")";
            throw invalid_argument{error_msg.str()};
        }
        return _get_line(line_index, update);
    }
    string Accessor::_get_line(int line_index, bool update)
    {
        if (update)
        {
            _update_index();
        }

        auto* possible_line = buffer.peek(line_index);
        if (possible_line)
        {
            return *possible_line;
        }

        stream.clear();

        auto start = index.at(line_index);
        auto end   = index.at(line_index+1);
        auto cur   = stream.tellg();
        stream.seekg(start);
        string line;
        getline(stream, line);
        stream.seekg(cur);
        return line;
    }

    optional< string > Accessor::try_get_line(int line_index, bool update)
    {
        scoped_lock sl{mtx};
        return _try_get_line(line_index, update);
    }

    optional< string > Accessor::_try_get_line(int line_index, bool update)
    {
        optional< string > possible_string;

        try
        {
            possible_string = _safe_get_line(line_index, update);
        }
        catch (...) {} // nothing to do

        return possible_string;
    }

    vector< string > Accessor::get_lines(int from_line_index, int to_line_index, bool update)
    {
        scoped_lock sl{mtx};
        return _get_lines(from_line_index, to_line_index, update);
    }

    vector< string > Accessor::_get_lines(int from_line_index, int to_line_index, bool update)
    {
        stringstream error_msg;
        error_msg << "Invalid range [" << from_line_index << "," << to_line_index << "[ : ";

        if (update)
        {
            _update_index();
        }

        // Make all the index positive, then change boundaries
        auto index_size = static_cast< int >(index.size()) - 1;

        if (from_line_index < 0)
        {
            from_line_index += index_size + 1;
            if (from_line_index <= 0)
            {
                error_msg << "negative from-index too big (< -" << (index_size+1) << ")";
                throw invalid_argument{error_msg.str()};
            }
        }
        if (from_line_index > index_size)
        {
            error_msg << "positive from-index too big (> " << index_size << ")";
            throw invalid_argument{error_msg.str()};
        }

        if (to_line_index < 0)
        {
            to_line_index += index_size + 1;
            if (to_line_index < 0)
            {
                error_msg << "negative to-index too big (< -" << (index_size+1) << ")";
                throw invalid_argument{error_msg.str()};
            }
        }
        if (to_line_index > index_size)
        {
            error_msg << "positive to-index too big (> " << index_size << ")";
            throw invalid_argument{error_msg.str()};
        }

        // Reversed ranges are accepted, it reverse the order of insertion in the result list
        auto reverse = to_line_index < from_line_index;
        if (reverse)
        {
            int swap = to_line_index;
            to_line_index = from_line_index;
            from_line_index = swap;
        }
        int range_len = to_line_index - from_line_index;
        int incr = 1;

        vector< string > lines;
        lines.reserve(range_len);
        for (int index = from_line_index ; index < to_line_index ; index += incr)
        {
            if (reverse)
            {
                lines.emplace(lines.begin(), _get_line(index, false));
            }
            else
            {
                lines.emplace_back(_get_line(index, false));
            }
        }

        return lines;
    }

    vector< string > Accessor::try_get_lines(int from_line_index, int to_line_index, bool update)
    {
        scoped_lock sl{mtx};
        return _try_get_lines(from_line_index, to_line_index, update);
    }

    vector< string > Accessor::_try_get_lines(int from_line_index, int to_line_index, bool update)
    {
        vector< string > possible_strings;

        try
        {
            possible_strings = _get_lines(from_line_index, to_line_index, update);
        }
        catch (...) {} // nothing to do

        return possible_strings;
    }
    
    void Accessor::update_index() noexcept
    {
        scoped_lock sl{mtx};
        _update_index();
    }

    void Accessor::_update_index() noexcept
    {
        stream.clear();

        for (string line ; getline(stream, line) ; )
        {
            if (!index.empty() && index.back() == static_cast< ifstream::pos_type >(-1))
            {
                index.back() = stream.tellg();
                buffer.append_back(line);
            }
            else
            {
                index.push_back(stream.tellg());
                buffer.push_back(move(line));
            }
        }
    }

    // IAccessor - end
}
