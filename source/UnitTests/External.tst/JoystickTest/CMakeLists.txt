
#---
if(${NEX_OS} MATCHES "Linux")
	create_ut(TRUE)
	add_dependency(Core Base)
endif(${NEX_OS} MATCHES "Linux")


