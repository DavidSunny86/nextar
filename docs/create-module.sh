cur_dir=$(pwd)
root_dir=${cur_dir}/..
src_dir=${root_dir}/source
fw_name=$1
mod_name=$2
module_path=${src_dir}/${fw_name}/${mod_name}

if [ $# -lt 1 ]; then
  echo "Usage: create-module <framework-name> [sub-module-name] <module-name>"
  echo "[sub-module-name] is only valid when framework is 'UnitTests', in which case do not use '.tst', just the first name should suffice."
else

	if [ "${fw_name}" = "UnitTests" ]; then
	  sub_mod_name=$2
	  sub_mod=$2.tst
	  mod_name=$3

	  module_path=${src_dir}/${fw_name}/${sub_mod}/${mod_name}
	  # check if sub module exists, if not we create a submodule and add it to list
	  if [ ! -d "${src_dir}/${fw_name}/${sub_mod}" ]; then
		mkdir "${src_dir}/${fw_name}/${sub_mod}"
		echo  "\n##---- SubModule: ${sub_mod} ----" >> ${src_dir}/${fw_name}/${sub_mod}/CMakeLists.txt
		echo  "\n##---- Created: $(date) ----" >> ${src_dir}/${fw_name}/${sub_mod}/CMakeLists.txt	
		echo  ""
		echo  "\nadd_module(${sub_mod_name})" >> ${src_dir}/${fw_name}/CMakeLists.txt
	  else
		echo  "\n# Added test ${mod_name}: $(date)" >> ${src_dir}/${fw_name}/${sub_mod}/CMakeLists.txt
		echo  ""
		echo  "\nadd_unit_test(${mod_name})" >> ${src_dir}/${fw_name}/${sub_mod}/CMakeLists.txt
	  fi
	  
	elif [ "${fw_name}" = "Plugins" ]; then
	  sed -i "## EndAdd/i add_module(${mod_name})" ${src_dir}/${fw_name}/CMakeLists.txt
	else
	  echo ""
	  echo "\nadd_module(${mod_name})" >> ${src_dir}/${fw_name}/CMakeLists.txt
	fi;

	mkdir ${module_path}
	mkdir ${module_path}/src
	mkdir ${module_path}/local_include
	echo "\n# Module created" >> ${module_path}/CMakeLists.txt
	echo "\n# Dated: $(date)" >> ${module_path}/CMakeLists.txt
	echo "\n#-------------------------------" >> ${module_path}/CMakeLists.txt
	echo "\n/**" >> ${module_path}/src/${mod_name}.cpp
	echo " * ${mod_name}.cpp" >> ${module_path}/src/${mod_name}.cpp
	echo " * Created on: $(date)" >> ${module_path}/src/${mod_name}.cpp
	echo " * @author: obhi" >> ${module_path}/src/${mod_name}.cpp
	echo " */\n" >> ${module_path}/src/${mod_name}.cpp
fi
# . gen-eclipse-project-files.sh

