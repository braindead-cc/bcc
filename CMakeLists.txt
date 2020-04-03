cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set (BCC_TARGET  "bcc")
set (BCC_VERSION "0.2.0")
set (BCC_SOURCES
	src/emitc.c
	src/emit_rust.c
	src/main.c
	src/comp.c
	src/debugger.c
	src/interp.c
	src/opt-nloops.c
	src/opt-sloops.c
	src/opt-squash.c
	src/parser.c
	src/prepare.c
	src/status.c
	src/terminfo.c
	src/util.c
	src/warn.c
)
set (BCC_LINK_LIBRARIES
	${BCC_LINK_LIBRARIES}
	ncurses
)
set (BCC_INCLUDE_DIRECTORIES
	include
	include/ccommon
)

set (CMAKE_C_STANDARD 99)
set (CMAKE_C_STANDARD_REQUIRED True)

project (${BCC_TARGET} VERSION ${BCC_VERSION} LANGUAGES C)

message ("-- Building ${BCC_TARGET} version ${BCC_VERSION}")
message ("     System: ${CMAKE_SYSTEM}")
message ("     C compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")

execute_process (COMMAND date "+%Y-%m-%d %H%M"
	OUTPUT_VARIABLE BUILDDATE
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_executable (${BCC_TARGET} ${BCC_SOURCES})
target_link_libraries (${BCC_TARGET} ${BCC_LINK_LIBRARIES})
find_package (Curses REQUIRED)
target_include_directories (${BCC_TARGET} PUBLIC
	${CURSES_INCLUDE_DIR}
	./include
	./include/ccommon
	./src
)

include (CheckCCompilerFlag)
check_c_compiler_flag("-Wimplicit-fallthrough=2" HAS_IMPLICIT_FALLTHROUGH)
if (HAS_IMPLICIT_FALLTHROUGH)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wimplicit-fallthrough=2")
endif()
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -fdiagnostics-color=auto \
-fmax-errors=32 -Wall -Wpedantic -Wextra -Wold-style-definition \
-Wmissing-prototypes -Winit-self -Wfloat-equal -Wstrict-prototypes \
-Wredundant-decls -Wendif-labels -Wstrict-aliasing=2 -Woverflow -Wformat=2 \
-Wmissing-include-dirs -Wno-trigraphs -Wno-format-nonliteral")

# unset this for non-fancy spinners
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSPINNER_FANCY")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBUILDDATE=\"\\\"${BUILDDATE}\\\"\"")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVERSION=\\\"${BCC_VERSION}\\\"")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMEMTYPE=u8")

set (CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG}   -O0 -ggdb")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -march=native")
set (CMAKE_EXE_LINKER_FLAGS_RELEASE
	"${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto -s")

install (TARGETS ${BCC_TARGET} DESTINATION bin)
