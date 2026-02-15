# Newlib support library for xv6
# This library provides the syscall stubs and runtime startup needed
# to use newlib's libc with xv6

set(NEWLIB_SYSCALLS_LIB xv6_newlib_syscalls)

# Get the toolchain sysroot for newlib headers
execute_process(
    COMMAND ${CMAKE_C_COMPILER} -print-sysroot
    OUTPUT_VARIABLE TOOLCHAIN_SYSROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
# If sysroot is empty, try to find include path from compiler
if(NOT TOOLCHAIN_SYSROOT)
    get_filename_component(TOOLCHAIN_BIN_DIR ${CMAKE_C_COMPILER} DIRECTORY)
    get_filename_component(TOOLCHAIN_DIR ${TOOLCHAIN_BIN_DIR} DIRECTORY)
    set(TOOLCHAIN_SYSROOT "${TOOLCHAIN_DIR}/riscv64-unknown-elf")
endif()

# Get GCC's built-in include path (for stddef.h, stdarg.h, etc.)
execute_process(
    COMMAND ${CMAKE_C_COMPILER} -print-file-name=include
    OUTPUT_VARIABLE GCC_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(USE_LOCAL_NEWLIB)
    set(NEWLIB_LOCAL_ROOT "${CMAKE_SOURCE_DIR}/user/v6-newlib" CACHE PATH "Local newlib source root")
    set(NEWLIB_INCLUDE_DIR "${NEWLIB_LOCAL_ROOT}/newlib/libc/include")
    set(NEWLIB_TOP_INCLUDE_DIR "${NEWLIB_LOCAL_ROOT}/include")
    if(NOT EXISTS "${NEWLIB_INCLUDE_DIR}/sys/stat.h")
        message(FATAL_ERROR "USE_LOCAL_NEWLIB is ON but local headers are missing: ${NEWLIB_INCLUDE_DIR}/sys/stat.h")
    endif()
else()
    set(NEWLIB_INCLUDE_DIR "${TOOLCHAIN_SYSROOT}/include")
    set(NEWLIB_TOP_INCLUDE_DIR "${TOOLCHAIN_SYSROOT}/include")
endif()

# Source files for newlib syscall stubs
set(NEWLIB_SYSCALLS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/newlib_syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/compat_ncurses.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pthread.c
)

# Assembly syscall stubs (kept local to user/libc for independence)
set(NEWLIB_ASM_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/usys.S
)

# CRT0 startup code
set(NEWLIB_CRT0_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/crt0.S
)

# Mark assembly files
set_source_files_properties(${NEWLIB_ASM_SRC} PROPERTIES LANGUAGE C)
set_source_files_properties(${NEWLIB_CRT0_SRC} PROPERTIES LANGUAGE C)

# Create the syscalls library
# Create the syscalls library (without crt0 - that's linked separately first)
add_library(${NEWLIB_SYSCALLS_LIB} STATIC
    ${NEWLIB_SYSCALLS_SRC}
    ${NEWLIB_ASM_SRC}
)

# Compile crt0.o separately so it can be linked first to ensure _start is at entry point
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/crt0.o
    COMMAND ${CMAKE_C_COMPILER}
            -nostdinc -ffreestanding
            -isystem ${GCC_INCLUDE_DIR}
            -isystem ${TOOLCHAIN_SYSROOT}/include
            -c ${CMAKE_CURRENT_SOURCE_DIR}/crt0.S
            -o ${CMAKE_CURRENT_BINARY_DIR}/crt0.o
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/crt0.S
    COMMENT "Building crt0.o for newlib programs"
)
add_custom_target(crt0_obj ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/crt0.o)

add_dependencies(${NEWLIB_SYSCALLS_LIB} crt0_obj)

# Clear inherited include directories - we want ONLY newlib headers for this library
set_target_properties(${NEWLIB_SYSCALLS_LIB} PROPERTIES
    INCLUDE_DIRECTORIES ""
)

# Use newlib headers ONLY for syscall stubs
# Note: We add our local sys/ directory first to provide sys/dirent.h
target_compile_options(${NEWLIB_SYSCALLS_LIB} PRIVATE
    -nostdinc
    -ffreestanding
    -fno-builtin
    -D_LARGEFILE_SOURCE=1
    -D_LARGEFILE64_SOURCE=1
    -D_FILE_OFFSET_BITS=64
    -D__LARGE64_FILES=1
    -D_POSIX_THREADS=200809L
    "SHELL:-I ${CMAKE_SOURCE_DIR}"
    "SHELL:-isystem ${CMAKE_CURRENT_SOURCE_DIR}"
    "SHELL:-isystem ${GCC_INCLUDE_DIR}"
    "SHELL:-isystem ${NEWLIB_TOP_INCLUDE_DIR}"
    "SHELL:-isystem ${NEWLIB_INCLUDE_DIR}"
)

# Build a standalone libpthread.a for ports/configure checks expecting -lpthread.
add_library(xv6_pthread STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/pthread.c
    ${NEWLIB_ASM_SRC}
)

set_target_properties(xv6_pthread PROPERTIES
    OUTPUT_NAME pthread
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    INCLUDE_DIRECTORIES ""
)

target_compile_options(xv6_pthread PRIVATE
    -nostdinc
    -ffreestanding
    -fno-builtin
    -D_POSIX_THREADS=200809L
    "SHELL:-I ${CMAKE_SOURCE_DIR}"
    "SHELL:-isystem ${CMAKE_CURRENT_SOURCE_DIR}"
    "SHELL:-isystem ${GCC_INCLUDE_DIR}"
    "SHELL:-isystem ${NEWLIB_TOP_INCLUDE_DIR}"
    "SHELL:-isystem ${NEWLIB_INCLUDE_DIR}"
)

add_dependencies(xv6_pthread crt0_obj)

# Export the include path for newlib programs (for sys/dirent.h, etc.)
set(NEWLIB_LIBC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
