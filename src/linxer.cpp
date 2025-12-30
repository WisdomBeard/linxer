#include "linxer/linxer.h"
#include "linxer/Accessor.h"

using namespace std;

namespace linxer
{
    unique_ptr< IAccessor > new_accessor(const string& file_path, unsigned int refresh_period_ms, int buffer_size)
    {
        unique_ptr< IAccessor > new_accessor = make_unique< Accessor >(file_path, refresh_period_ms, buffer_size);
        return new_accessor;
    }
}
