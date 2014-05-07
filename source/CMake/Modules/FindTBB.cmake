
if (NOT TBB_ROOT)
	message(FATAL_ERROR "TBB Root not set. Cant handle this!")
endif (NOT TBB_ROOT)

if (WIN32)
	if (NEX_ARCHITECTURE_NAME STREQUAL "x86")
		set(TBB_ARCH "ia32")
	elseif (NEX_ARCHITECTURE_NAME STREQUAL "x86_64")
		set(TBB_ARCH "intel64")
	else()
		message(FATAL_ERROR "TBB not supported in this architecture!")
	endif(NEX_ARCHITECTURE_NAME STREQUAL "x86")
	
	if (MSVC80)
        set(TBB_COMPILER "vc8")
    elseif (MSVC90)
        set(TBB_COMPILER "vc9")
	elseif (MSVC10)
		set(TBB_COMPILER "vc10")
	elseif (MSVC11)
		set(TBB_COMPILER "vc11")
	elseif (MSVC12)
		set(TBB_COMPILER "vc12")
    endif()
	set(TBB_LIB_SEARCH_DIR "${TBB_ROOT}/lib/${TBB_ARCH}/${TBB_COMPILER}")
 endif()

if (UNIX)
	message(FATAL_ERROR "TODO: Work on this!")
endif (UNIX)


set (TBB_FOUND "NO")
set (TBB_INC_SEARCH_DIR ${TBB_ROOT}/include)
find_path(TBB_INCLUDE_DIR
    tbb/task_scheduler_init.h
    PATHS ${TBB_INC_SEARCH_DIR}
)
mark_as_advanced(TBB_INCLUDE_DIR)

message("@TODO: Use tbb_preview? task group priority et. al.")
find_library(TBB_LIBNAME NAMES tbb PATHS ${TBB_LIB_SEARCH_DIR} NO_DEFAULT_PATH)
find_library(TBB_MALLOC_LIBNAME NAMES tbbmalloc PATHS ${TBB_LIB_SEARCH_DIR} NO_DEFAULT_PATH)
find_library(TBB_DEBUG_LIBNAME NAMES tbb_debug PATHS ${TBB_LIB_SEARCH_DIR} NO_DEFAULT_PATH)
find_library(TBB_MALLOC_DEBUG_LIBNAME NAMES tbbmalloc_debug PATHS ${TBB_LIB_SEARCH_DIR} NO_DEFAULT_PATH)

if (TBB_LIBNAME AND TBB_MALLOC_LIBNAME)
	set(TBB_LIBRARIES "${TBB_LIBNAME};${TBB_MALLOC_LIBNAME}")
	set(TBB_LIBRARIES_DEBUG "${TBB_DEBUG_LIBNAME};${TBB_MALLOC_DEBUG_LIBNAME}")
endif()

mark_as_advanced(TBB_LIBNAME)
mark_as_advanced(TBB_MALLOC_LIBNAME)
mark_as_advanced(TBB_DEBUG_LIBNAME)
mark_as_advanced(TBB_MALLOC_DEBUG_LIBNAME)
mark_as_advanced(TBB_LIBRARIES)

if (TBB_INCLUDE_DIR AND TBB_LIBRARIES)
	set (TBB_FOUND "YES")
endif()

# find_library(TBB_MALLOC_LIBRARY ${_TBB_LIB_MALLOC_NAME} ${TBB_LIBRARY_DIR} NO_DEFAULT_PATH)
# mark_as_advanced(TBB_LIBRARY TBB_MALLOC_LIBRARY)

# #-- Look for debug libraries
# find_library(TBB_LIBRARY_DEBUG        ${_TBB_LIB_DEBUG_NAME}        ${TBB_LIBRARY_DIR} NO_DEFAULT_PATH)
# find_library(TBB_MALLOC_LIBRARY_DEBUG ${_TBB_LIB_MALLOC_DEBUG_NAME} ${TBB_LIBRARY_DIR} NO_DEFAULT_PATH)
# mark_as_advanced(TBB_LIBRARY_DEBUG TBB_MALLOC_LIBRARY_DEBUG)



    # # has em64t/vc8   em64t/vc9
    # # has ia32/vc7.1  ia32/vc8   ia32/vc9
    # set(_TBB_DEFAULT_INSTALL_DIR "C:/Program Files/Intel/TBB" "C:/Program Files (x86)/Intel/TBB")
    # set(_TBB_LIB_NAME "tbb")
    # set(_TBB_LIB_MALLOC_NAME "${_TBB_LIB_NAME}malloc")
    # set(_TBB_LIB_DEBUG_NAME "${_TBB_LIB_NAME}_debug")
    # set(_TBB_LIB_MALLOC_DEBUG_NAME "${_TBB_LIB_MALLOC_NAME}_debug")
    # if (MSVC71)
        # set (_TBB_COMPILER "vc7.1")
    # endif(MSVC71)
    # if (MSVC80)
        # set(_TBB_COMPILER "vc8")
    # endif(MSVC80)
    # if (MSVC90)
        # set(_TBB_COMPILER "vc9")
    # endif(MSVC90)
    # if (NOT _TBB_COMPILER)
        # message("ERROR: TBB supports only VC 7.1, 8 and 9 compilers on Windows platforms.")
    # endif (NOT _TBB_COMPILER)
    # set(_TBB_ARCHITECTURE ${TBB_ARCHITECTURE})
# endif (WIN32)

# if (UNIX)
    # if (APPLE)
        # # MAC
        # set(_TBB_DEFAULT_INSTALL_DIR "/Library/Frameworks/Intel_TBB.framework/Versions")
        # # libs: libtbb.dylib, libtbbmalloc.dylib, *_debug
        # set(_TBB_LIB_NAME "tbb")
        # set(_TBB_LIB_MALLOC_NAME "${_TBB_LIB_NAME}malloc")
        # set(_TBB_LIB_DEBUG_NAME "${_TBB_LIB_NAME}_debug")
        # set(_TBB_LIB_MALLOC_DEBUG_NAME "${_TBB_LIB_MALLOC_NAME}_debug")
        # # has only one flavor: ia32/cc4.0.1_os10.4.9
        # set(_TBB_COMPILER "cc4.0.1_os10.4.9")
        # set(_TBB_ARCHITECTURE "ia32")
    # else (APPLE)
        # # LINUX
        # set(_TBB_DEFAULT_INSTALL_DIR "/opt/intel/tbb" "/usr/local/include" "/usr/include")
        # set(_TBB_LIB_NAME "tbb")
        # set(_TBB_LIB_MALLOC_NAME "${_TBB_LIB_NAME}malloc")
        # set(_TBB_LIB_DEBUG_NAME "${_TBB_LIB_NAME}_debug")
        # set(_TBB_LIB_MALLOC_DEBUG_NAME "${_TBB_LIB_MALLOC_NAME}_debug")
        # # has em64t/cc3.2.3_libc2.3.2_kernel2.4.21  em64t/cc3.3.3_libc2.3.3_kernel2.6.5  em64t/cc3.4.3_libc2.3.4_kernel2.6.9  em64t/cc4.1.0_libc2.4_kernel2.6.16.21
        # # has ia32/*
        # # has itanium/*
        # set(_TBB_COMPILER ${TBB_COMPILER})
        # set(_TBB_ARCHITECTURE ${TBB_ARCHITECTURE})
    # endif (APPLE)
# endif (UNIX)

# if (CMAKE_SYSTEM MATCHES "SunOS.*")
# # SUN
# # not yet supported
# # has em64t/cc3.4.3_kernel5.10
# # has ia32/*
# endif (CMAKE_SYSTEM MATCHES "SunOS.*")


# #-- Clear the public variables
# set (TBB_FOUND "NO")


# #-- Find TBB install dir and set ${_TBB_INSTALL_DIR} and cached ${TBB_INSTALL_DIR}
# # first: use CMake variable TBB_INSTALL_DIR
# if (TBB_INSTALL_DIR)
    # set (_TBB_INSTALL_DIR ${TBB_INSTALL_DIR})
# endif (TBB_INSTALL_DIR)
# # second: use environment variable
# if (NOT _TBB_INSTALL_DIR)
    # if (NOT "$ENV{TBB_INSTALL_DIR}" STREQUAL "")
        # set (_TBB_INSTALL_DIR $ENV{TBB_INSTALL_DIR})
    # endif (NOT "$ENV{TBB_INSTALL_DIR}" STREQUAL "")
    # # Intel recommends setting TBB21_INSTALL_DIR
    # if (NOT "$ENV{TBB21_INSTALL_DIR}" STREQUAL "")
        # set (_TBB_INSTALL_DIR $ENV{TBB21_INSTALL_DIR})
    # endif (NOT "$ENV{TBB21_INSTALL_DIR}" STREQUAL "")
# endif (NOT _TBB_INSTALL_DIR)
# # third: try to find path automatically
# if (NOT _TBB_INSTALL_DIR)
    # if (_TBB_DEFAULT_INSTALL_DIR)
        # set (_TBB_INSTALL_DIR $ENV{_TBB_DEFAULT_INSTALL_DIR})
    # endif (_TBB_DEFAULT_INSTALL_DIR)
# endif (NOT _TBB_INSTALL_DIR)
# # sanity check
# if (NOT _TBB_INSTALL_DIR)
    # message ("ERROR: TBB_INSTALL_DIR not found. ${_TBB_INSTALL_DIR}")
# else (NOT _TBB_INSTALL_DIR)
# # finally: set the cached CMake variable TBB_INSTALL_DIR
# if (NOT TBB_INSTALL_DIR)
    # set (TBB_INSTALL_DIR ${_TBB_INSTALL_DIR} CACHE PATH "Intel TBB install directory")
    # mark_as_advanced(TBB_INSTALL_DIR)
# endif (NOT TBB_INSTALL_DIR)


# #-- Look for include directory and set ${TBB_INCLUDE_DIR}
# set (TBB_INC_SEARCH_DIR ${_TBB_INSTALL_DIR}/include)
# find_path(TBB_INCLUDE_DIR
    # tbb/task_scheduler_init.h
    # PATHS ${TBB_INC_SEARCH_DIR}
# )
# mark_as_advanced(TBB_INCLUDE_DIR)


# #-- Look for libraries
# # GvdB: $ENV{TBB_ARCH_PLATFORM} is set by the build script tbbvars[.bat|.sh|.csh]
# if (NOT $ENV{TBB_ARCH_PLATFORM} STREQUAL "")
    # set (TBB_LIBRARY_DIR "${_TBB_INSTALL_DIR}/$ENV{TBB_ARCH_PLATFORM}/lib")
# else (NOT $ENV{TBB_ARCH_PLATFORM} STREQUAL "")
    # # HH: deprecated
    # message(STATUS "[Warning] FindTBB.cmake: The use of TBB_ARCHITECTURE and TBB_COMPILER is deprecated and may not be supported in future versions. Please set $ENV{TBB_ARCH_PLATFORM} (using tbbvars.[bat|csh|sh]).")
    # set (TBB_LIBRARY_DIR "${_TBB_INSTALL_DIR}/${_TBB_ARCHITECTURE}/${_TBB_COMPILER}/lib")
# endif (NOT $ENV{TBB_ARCH_PLATFORM} STREQUAL "")


# find_library(TBB_LIBRARY        ${_TBB_LIB_NAME}        ${TBB_LIBRARY_DIR} NO_DEFAULT_PATH)
# find_library(TBB_MALLOC_LIBRARY ${_TBB_LIB_MALLOC_NAME} ${TBB_LIBRARY_DIR} NO_DEFAULT_PATH)
# mark_as_advanced(TBB_LIBRARY TBB_MALLOC_LIBRARY)

# #-- Look for debug libraries
# find_library(TBB_LIBRARY_DEBUG        ${_TBB_LIB_DEBUG_NAME}        ${TBB_LIBRARY_DIR} NO_DEFAULT_PATH)
# find_library(TBB_MALLOC_LIBRARY_DEBUG ${_TBB_LIB_MALLOC_DEBUG_NAME} ${TBB_LIBRARY_DIR} NO_DEFAULT_PATH)
# mark_as_advanced(TBB_LIBRARY_DEBUG TBB_MALLOC_LIBRARY_DEBUG)


# if (TBB_INCLUDE_DIR)
    # if (TBB_LIBRARY)
        # set (TBB_FOUND "YES")
        # set (TBB_LIBRARIES ${TBB_LIBRARY} ${TBB_MALLOC_LIBRARY} ${TBB_LIBRARIES})
        # set (TBB_DEBUG_LIBRARIES ${TBB_LIBRARY_DEBUG} ${TBB_MALLOC_LIBRARY_DEBUG} ${TBB_DEBUG_LIBRARIES})
        # set (TBB_INCLUDE_DIRS ${TBB_INCLUDE_DIR} CACHE PATH "TBB include directory" FORCE)
        # set (TBB_LIBRARY_DIRS ${TBB_LIBRARY_DIR} CACHE PATH "TBB library directory" FORCE)
        # mark_as_advanced(TBB_INCLUDE_DIRS TBB_LIBRARY_DIRS TBB_LIBRARIES TBB_DEBUG_LIBRARIES)
        # message(STATUS "Found Intel TBB")
    # endif (TBB_LIBRARY)
# endif (TBB_INCLUDE_DIR)

# if (NOT TBB_FOUND)
    # message("ERROR: Intel TBB NOT found!")
    # message(STATUS "Looked for Threading Building Blocks in ${_TBB_INSTALL_DIR}")
    # # do only throw fatal, if this pkg is REQUIRED
    # if (TBB_FIND_REQUIRED)
        # message(FATAL_ERROR "Could NOT find TBB library.")
    # endif (TBB_FIND_REQUIRED)
# endif (NOT TBB_FOUND)

# endif (NOT _TBB_INSTALL_DIR)
