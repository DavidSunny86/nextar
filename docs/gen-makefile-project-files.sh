cur_dir=$(pwd)
root_dir=${cur_dir}/..
src_dir=${root_dir}/source
build_dir=${root_dir}/build
export FBX_DIR=~/fbx-sdk
cd $root_dir
rm -rf $build_dir 
mkdir $build_dir
cd $build_dir
cmake -DCMAKE_BUILD_TYPE:STRING="Debug" -DNEX_UNITY_BUILD=ON -Wdev -DCMAKE_CXX_COMPILER_ARG1=-std=c++11 -G "Unix Makefiles" $src_dir
cd $cur_dir

