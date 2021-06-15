set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

macro(print_config name)
    message(STATUS "---config of ${name}: ${${name}}")
endmacro()

print_config(CMAKE_CXX_STANDARD)
print_config(CMAKE_CXX_COMPILER)
print_config(CMAKE_CXX_COMPILER_ID)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wpedantic)
add_compile_options(-Wunused-parameter)
add_compile_options(-Wshadow)
add_compile_options(-Wnon-virtual-dtor)
add_compile_options(-Woverloaded-virtual)
add_compile_options(-Wignored-qualifiers)

