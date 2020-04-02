cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set (BCC_TARGET  "bcc")
set (BCC_VERSION "0.2.0")
set (BCC_SOURCES
	src/emitc.c
	src/emit_rust.c
	src/lbf.c
	src/lbfc.c
	src/lbfd.c
	src/lbfi.c
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
	./include
	./include/ccommon
)

project (${BCC_TARGET} VERSION ${BCC_VERSION} LANGUAGES C)

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

function (add_cflag flag)
	check_c_compiler_flag('${flag}' "has_${flag}")
	if ("has_${flag}")
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
	endif()
endfunction(add_cflag)

add_cflag (-Wold-style-definition)
add_cflag (-Wmissing-prototypes)
add_cflag (-Winit-self)
add_cflag (-Wfloat-equal)
add_cflag (-Wstrict-prototypes)
add_cflag (-Wredundant-decls)
add_cflag (-Wimplicit-fallthrough=2)
add_cflag (-Wendif-labels)
add_cflag (-Wstrict-aliasing=2)
add_cflag (-Woverflow)
add_cflag (-Wformat=2)
add_cflag (-Wmissing-include-dirs)

# trigraphs are fun!
# "if IsErr(err) ??!??! panic(err);" :^)
add_cflag (-Wno-trigraphs)

# see src/util.c
add_cflag (-Wno-format-nonliteral)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBUILDDATE=\"\\\"${BUILDDATE}\\\"\"")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVERSION=\\\"${BCC_VERSION}\\\"")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMEMTYPE=u8")

# unset this for non-fancy spinners
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSPINNER_FANCY")

set (CMAKE_C_STANDARD 99)
set (CMAKE_C_STANDARD_REQUIRED True)

install (TARGETS ${BCC_TARGET} DESTINATION bin)
