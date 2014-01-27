cur_dir=$(pwd)
root_dir=${cur_dir}/..
src_dir=${root_dir}/source
build_dir=${root_dir}/build
cd $root_dir/..
svn update
svn status | grep '?' | sed 's/?//g' | xargs svn add
svn status | grep '!' | sed 's/!//g' | xargs svn rm
mkdir $build_dir
cd $build_dir
cmake -DCMAKE_BUILD_TYPE:STRING="Debug" -Wdev -G "Eclipse CDT4 - Unix Makefiles" $src_dir
cd $cur_dir
