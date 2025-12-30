cmake_minimum_required(VERSION 3.15)
project(linxer
        LANGUAGES "CXX"
        VERSION "0.1.1"
        DESCRIPTION "Fast access to lines in a file."
)


add_library(linxer src/linxer.cpp src/Accessor.cpp src/StringRingBuffer.cpp)
target_include_directories(linxer PUBLIC include)


set(PUBLIC_HEADERS
    "include/linxer/linxer.h"
    "include/linxer/IAccessor.h"
)
set_target_properties(linxer PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
install(TARGETS linxer)
install(DIRECTORY include/
    DESTINATION include
    PATTERN "CMakeLists.txt" EXCLUDE
)