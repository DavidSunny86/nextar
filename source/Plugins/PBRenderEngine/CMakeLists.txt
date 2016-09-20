##---------------------------------------------------
# Nextar build file.
# : Physically Based Rendering module
# Abhishek Dey
# 11/08/2015
##---------------------------------------------------

# Add definition
create_plugin("PBRenderEngine" "" "Preload" "${NEX_VERSION}" "engine" "false" "")
add_dependency(Core RenderEngine)
