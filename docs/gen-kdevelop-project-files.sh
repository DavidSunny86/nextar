cur_dir=$(pwd)
root_dir=${cur_dir}/..
src_dir=${root_dir}/source
build_dir=${root_dir}/build
export FBX_DIR=~/fbx-sdk
cd $root_dir
rm -rf $build_dir 
mkdir $build_dir
cd $build_dir
cmake -DCMAKE_BUILD_TYPE:STRING="Debug" -DNEX_UNITY_BUILD=ON -Wdev -G "KDevelop3" $src_dir -DCMAKE_CXX_COMPILER_ARG1=-std=c++11
cd $cur_dir

