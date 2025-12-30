# linxer

Fast access to lines in a file.

## Build

```bash
conan build
```

## Use

In your `conanfile.py`:

```python
def requirements(self):
    self.requires("linxer/0.1.0")
```

In your `CMakeLists.txt`

```cmake
find_package(linxer CONFIG REQUIRED)

target_link_libraries(YOUR_TARGET linxer::linxer)
```

In your C++ source code:

```cpp
#include <linxer/linxer.h>
```

## Key concepts

`linxer` stands for _**lin**e inde**xer**_. The purpose of this library is to give a fast access to any line of a textual file. To do so:
* a read-only file descriptor is maintained open on the file
* an index keeps track of the position of each line in the file
* optionnally, the last N lines are buffered for even faster retrieval
* lines that are not in the buffer are retrieved by seeking the right position in the file and reading them from the disk

The index is maintained up to date, either by manually triggering its update, or by letting an internal refresh loop do it.

### Accessor creation

```cpp
auto file_path = "...";

// File accessor based on the defaut line indexer:
// - no automatic refresh of the index
// - no buffering of the last N logs
auto accessor = linxer::new_accessor(file_path);

// File accessor based on a line indexer with:
// - automatic refresh of the index, every 1500 ms
// - no buffering of the last N logs
auto accessor = linxer::new_accessor(file_path, 1500);

// File accessor based on a line indexer with:
// - no automatic refresh of the index
// - buffering of the last 200 logs
auto accessor = linxer::new_accessor(file_path, 0, 200);
```

### Index update

Updating the index means that the library will scan the file starting from its last position, up until the current end of the file.

```cpp
// Either via automatic refresh

// or explicit, manual update of the index
accessor->update_index();

// or when retrieving lines:
auto line = accessor->get_line(42, /*update=*/true);
```

### Line retrieval

Line retrieval exists in 4 different flavours:
| single-line | multi-line | If not found? |
|--|--|--|
| get_line | get_lines | Throws |
| try_get_line | try_get_lines | Returns empty optional/vector |

```cpp
// Retrieve a single line. Throw if not found.
try
{
    std::string line = accessor->get_line(42 /*, update=false*/);
    std::cout << line << std::endl;
}
catch (std::exception& e)
{
    // ...
}

// Retrieve a single line.
std::optional<std::string> line = accessor-try_>get_line(42 /*, update=false*/);
if (line)
{
    std::cout << *line << std::endl;
}
else
{
    // ...
}

// Retrieve several lines. Throw if not found.
try
{
    std::vector<std::string> lines = accessor->get_lines(42, 84 /*, update=false*/);
    for (auto& line : lines)
    {
        std::cout << line << std::endl;
    }
}
catch (std::exception& e)
{
    // ...
}

// Retrieve several lines.
std::vector<std::string> lines = accessor->try_get_lines(42, 84 /*, update=false*/);
for (auto& line : lines)
{
    std::cout << line << std::endl;
}
```

### Ranges

Ranges can be expressed in various ways. The following example illustrate the main concepts related to ranges.

Consider a file filled with 10 lines. Each line has 2 indexes: forward and backward.

| Line | Forward index | Backward index |
|:--:|:--:|:--:|
| `L01` | `0` | `-11` |
| `L02` | `1` | `-10` |
| `L03` | `2` | `-9` |
| `L04` | `3` | `-8` |
| `L05` | `4` | `-7` |
| `L06` | `5` | `-6` |
| `L07` | `6` | `-5` |
| `L08` | `7` | `-4` |
| `L09` | `8` | `-3` |
| `L10` | `9` | `-2` |
| `<END>` | `10` | `-1` |

Ranges can either be expressed as `[from-min ; to-max[` or as `]from-max ; to-min]`.

| From | To | Returned |
|:--:|:--:|:--:|
| `0` | `3` | `L01 L02 L03` |
| `-4` | `-1` | `L08 L09 L10` |
| `6` | `-1` | `L07 L08 L09 L10` |
| `3` | `0` | `L03 L02 L01` |
| `-1` | `-4` | `L10 L09 L08` |
| `-1` | `6` | `L10 L09 L08 L07` |
| `5` | `5` | ` ` |
