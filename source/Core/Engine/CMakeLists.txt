##---------------------------------------------------
# Nextar build file.
# : libnextech configuration file.
# Abhishek Dey
# 5:17 PM 10/1/2011
##---------------------------------------------------

# Source list
#===========================================================
set(NEXCORE_ENGINE_SRC ${NEX_CORE}/Engine/src)

if(NEX_OS MATCHES "Windows")
 set(NEXTECH_PLATFORM_SRC ${NEXCORE_ENGINE_SRC}/platform/win32)
elseif(NEX_OS MATCHES "Linux")
 set(NEXTECH_PLATFORM_SRC ${NEXCORE_ENGINE_SRC}/platform/linux)
elseif(NEX_OS MATCHES "MacOSX")
 set(NEXTECH_PLATFORM_SRC ${NEXCORE_ENGINE_SRC}/platform/macosx)
endif(NEX_OS MATCHES "Windows")

file(GLOB NEX_SRC_LIST 
	${NEXCORE_ENGINE_SRC}/*.cpp
	${NEXTECH_PLATFORM_SRC}/*.cpp
	)

# Add definition

create_module(Engine Core ${NEX_SRC_LIST}) 
add_dependency(Engine Core Base Core)

