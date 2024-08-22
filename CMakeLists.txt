cmake_minimum_required(VERSION 3.18)

include(./cmake/WUPS.cmake)

set(TARGET "aRAMa")
project(${TARGET} LANGUAGES CXX ASM)

set(CMAKE_CXX_STANDARD 20)

# file(GLOB_RECURSE C_FILES src/*.c)
file(GLOB_RECURSE CPP_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS src/*.h)
file(GLOB_RECURSE ASM_FILES CONFIGURE_DEPENDS src/*.S)

set(WUMS ${DEVKITPRO}/wums)

add_executable(${TARGET}
    # ${C_FILES}
    ${CPP_FILES}
    ${HEADER_FILES}
    ${ASM_FILES}
    # src/link.ld
    # test.c
)

target_include_directories(${TARGET} PRIVATE
    # ${DEVKITPRO}/libogc/include
    ${DEVKITPRO}/portlibs/ppc/include
    ${WUMS}/include
)

find_library(LIBNOTIFY NAMES notifications HINTS ${WUMS}/lib REQUIRED)
find_library(LIBKERNEL NAMES kernel HINTS ${WUMS}/lib REQUIRED)

target_link_libraries(${TARGET} PRIVATE
    ${LIBNOTIFY}
    ${LIBKERNEL}
)

target_link_options(${TARGET} PRIVATE
    "-T${WUMS}/share/libkernel.ld"
)

# wut_create_rpx(${TARGET})

# wut_create_wuhb(${TARGET})

wups_create_plugin(${TARGET})
