##---------------------------------------------------
# Nextar build file.
# : FX Compiler plugin configuration for nextar shaders.
# Abhishek Dey
# 5:56 PM 9/25/2011
##---------------------------------------------------
# Add plugin
create_plugin("MaterialLoader" "" "Preload" "${NEX_VERSION}" "engine" "false" "")
add_dependency(Core Engine)

