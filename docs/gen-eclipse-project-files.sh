cur_dir=$(pwd)
root_dir=${cur_dir}/..
src_dir=${root_dir}/source
build_dir=${root_dir}/build
cd $root_dir
mkdir $build_dir
cd $build_dir
cmake -DCMAKE_BUILD_TYPE:STRING="Debug" -Wdev -G "Eclipse CDT4 - Unix Makefiles" $src_dir -DCMAKE_CXX_COMPILER_ARG1=-std=c++11
cd $cur_dir

