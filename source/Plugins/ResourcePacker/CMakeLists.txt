# Module created
# Dated: Sat Feb 13 17:59:01 IST 2016
#-------------------------------
# Todo This plugin should be in project context, but we need to implement
# 1. StringID as struct and save strings to files/along with id.
# 1.1 StringID can have project_id.string_id as string id/a full proof way of loading correct strings from files before they are compiled.
# 2. Save and load feature for renderscripts.
create_plugin("ResourcePacker" "" "Custom" "${NEX_VERSION}" "project" "false" "ResourcePackerService:Default")
add_dependency(Core NeoCmd)
add_dependency(Core RenderEngine)
add_dependency(Core ProjectModel)
