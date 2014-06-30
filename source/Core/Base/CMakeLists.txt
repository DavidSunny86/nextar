##---------------------------------------------------
# Nextar build file.
# : libnextech configuration file.
# Abhishek Dey
# 5:17 PM 10/1/2011
##---------------------------------------------------

# Source list
#===========================================================

create_library()
if (CMAKE_DL_LIBS)
	add_external_dependency("" "${CMAKE_DL_LIBS}")
endif()

# zlib
if(NEX_OS MATCHES "Windows")
 if((NOT ZLIB_ROOT) AND (NOT ENV{ZLIB_ROOT}))
  message("-->> Defaulting to dependencies/zlib")
  set(ZLIB_ROOT ${NEX_DEPENDENCIES}/zlib)
  copy_binary_to_out(${ZLIB_ROOT}/bin)
 endif((NOT ZLIB_ROOT) AND (NOT ENV{ZLIB_ROOT}))
endif(NEX_OS MATCHES "Windows")

find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
	add_external_dependency("${ZLIB_INCLUDE_DIRS}" "${ZLIB_LIBRARIES}")
endif (ZLIB_FOUND)

if(NEX_USE_TBB)	
	if((NOT TBB_ROOT) AND (NOT ENV{TBB_ROOT}))
		message("-->> Defaulting to dependencies/tbb42")
		set(TBB_ROOT ${NEX_DEPENDENCIES}/tbb42)
	endif((NOT TBB_ROOT) AND (NOT ENV{TBB_ROOT}))
	
	find_package(TBB REQUIRED)
	if(TBB_FOUND)
		if(NEX_OS MATCHES "Windows")
			if (TBB_LIB_SEARCH_DIR)
				string(REPLACE "/lib/" "/bin/" TBB_BIN_SEARCH_DIR ${TBB_LIB_SEARCH_DIR})
				copy_binary_to_out(${TBB_BIN_SEARCH_DIR})
			endif()
		endif()
		if(NEX_DEBUG)
			set(TBB_LIBRARIES "${TBB_LIBRARIES_DEBUG}")
		endif()
		add_chained_external_dependency("${TBB_INCLUDE_DIR}" "${TBB_LIBRARIES}")
	endif (TBB_FOUND)
endif(NEX_USE_TBB)
## Platform specific dependencies

#===========================================================
# Linux
if ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
	# package requirements
	# X11
	find_package(X11 REQUIRED)
	if (X11_FOUND)
		message("Add X11 Dependency: ${X11_INCLUDE_DIR} ${X11_LIBRARIES}")
		add_external_dependency("${X11_INCLUDE_DIR}" "${X11_LIBRARIES}")
	endif (X11_FOUND)
endif ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )

