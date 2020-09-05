project(moeExceptions)

add_library(
        moeExceptions
        MoeExceptions.hpp
)
set_target_properties(moeExceptions PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(moeExceptions PUBLIC ../Exceptions)