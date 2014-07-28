##---------------------------------------------------
# Nextar build file.
# : FX Compiler plugin configuration for nextar shaders.
# Abhishek Dey
# 5:56 PM 9/25/2011
##---------------------------------------------------
# Add plugin
create_plugin("MaterialCompiler" "" "Lifetime" "${NEX_VERSION}" "false")
add_dependency(Core ProductModel)

