#!/bin/bash
## system repack script

#get current dir
curdir=$(pwd)
getonlydir=$(dirname $0)
actualdir=$curdir/$getonlydir
cd $actualdir
LOCAL_DIR=$(pwd)
echo $LOCAL_DIR

COLOR_GREEN="\e[32m"
COLOR_END="\e[0m"
COLOR_RED="\e[31m"


function getabspath {
	# $1: the relative path
	# echo "inside getabspath\n"
	# echo "input: $1 \n"
	currentdir=$(pwd)
	filename=$(basename $1)
	dir1=$(dirname $1)
	cd $dir1
	abspath=$(pwd)
	cd $currentdir
	# echo "$abspath/$filename"
	echo "$abspath/$filename"
}



# locate mkuserimage.sh file
#DEFAULT_TOOL_PATH=$LOCAL_DIR/../../mos-200/android/out/host/linux-x86/bin/mkuserimg.sh
DEFAULT_TOOL_PATH=/home/aka/mos-200/bin/mkuserimg.sh


DEFAULT_TARGET_SYSTEM_DIR=/home/aka/ext2/share/musio2.0.0/userdebug_rc8/result/system
DEFAULT_BOARDCONFIGMK_PATH=$LOCAL_DIR/../android/device/nexell/musio/BoardConfig.mk




# convert to abs path
DEFAULT_TOOL_PATH=$(getabspath $DEFAULT_TOOL_PATH)
DEFAULT_TARGET_SYSTEM_DIR=$(getabspath $DEFAULT_TARGET_SYSTEM_DIR)
DEFAULT_BOARDCONFIGMK_PATH=$(getabspath $DEFAULT_BOARDCONFIGMK_PATH)



####
echo "enter mkuserimage.sh location(default: $DEFAULT_TOOL_PATH) :"
read tool_path

# set to default value if no input.
if [ -z $tool_path ]; then
	tool_path=$DEFAULT_TOOL_PATH
fi


# check if it exists.
if [ -f $tool_path ]; then
	echo -e "${COLOR_GREEN}it exists${COLOR_END}"
else
	echo -e "${COLOR_RED}does not exist. abort${COLOR_END}"
	exit 1
fi

## if the tool exists, in the directory there should also be make_ext4fs.
## check if it exists

TOOL_DIR=$(dirname $tool_path)

if [ ! -f $TOOL_DIR/make_ext4fs ]; then
	echo -e "${COLOR_RED}make_ext4fs not found. abort${COLOR_END}"
	exit 1
fi

## update PATH  to included this
# echo $PATH
PATH=$TOOL_DIR:$PATH



#####
echo ""
echo "enter target system directory(default: $DEFAULT_TARGET_SYSTEM_DIR) :"
read target_system_dir

if [ -z $target_system_dir ]; then
	target_system_dir=$DEFAULT_TARGET_SYSTEM_DIR
fi

if [ -d $target_system_dir ]; then
	echo -e "${COLOR_GREEN}it exists${COLOR_END}"
else
	echo -e "${COLOR_RED}does not exist. abort${COLOR_END}"
	exit 1
fi

#define saveimgdir along with it.
# it is practically the dir above it. the so called "result" dir.
saveimgdir=$target_system_dir/..

# check if file_context file exists
DEFAULT_FILECONTEXT_PATH=$(getabspath $target_system_dir/../root/file_contexts)

filecontext_path=""
if [ -f $DEFAULT_FILECONTEXT_PATH ]; then
	echo -e "${COLOR_GREEN}file_context detected with default_method. will use this file.${COLOR_END}"
	filecontext_path=$DEFAULT_FILECONTEXT_PATH
else
	echo -e "${COLOR_RED}file_context not dected with default method. please enter the exact location:${COLOR_END}"
	read filecontext_path

	# check if the input path is valid

	if [ -f $filecontext_path ]; then

		echo -e "${COLOR_GREEN}file_context exists.${COLOR_END}"
	else
		echo -e "${COLOR_RED}file_context does not exist. abort${COLOR_END}"
		exit 1
	fi


fi

####
echo ""
echo "enter BoardConfig.mk location(default : $DEFAULT_BOARDCONFIGMK_PATH) :"
read boardconfigmk_path

if [ -z $boardconfigmk_path ]; then
	boardconfigmk_path=$DEFAULT_BOARDCONFIGMK_PATH
fi

if [ -f $boardconfigmk_path ]; then
	echo -e "${COLOR_GREEN}it exists${COLOR_END}"
else
	echo -e "${COLOR_RED}does not exist. abort${COLOR_END}"
	exit 1
fi


## basic settings finished. move on to execution.


board_name=musio
partition_name=system

# change it to uppercase letters
partition_name_upper=$(echo ${partition_name} | tr '[[:lower:]]' '[[:upper:]]')

# fetch partition size
partition_size=$(grep "BOARD_${partition_name_upper}IMAGE_PARTITION_SIZE" ${boardconfigmk_path} | awk '{print $3}')


echo "partition name: ${partition_name}, partition name upper: ${partition_name_upper}, partition_size: ${partition_size}"

# change all the relative paths to absolute path because we are going to give it to the docker
# file. 

tool_path=$(getabspath $tool_path)
target_system_dir=$(getabspath $target_system_dir)
saveimgdir=$(getabspath $saveimgdir)
filecontext_path=$(getabspath $filecontext_path)

savesystempath=$(getabspath ${saveimgdir}/${partition_name}.img)



if [ ${partition_name} == "system" ]; then
    # when building system.img, we need to reference file_contexts.
    ${tool_path} -s ${target_system_dir} $savesystempath ext4 ${partition_name} ${partition_size} ${filecontext_path}
    
else
    
	# if its not making system.img, then we don't need to reference the file contexts. 
    $tool_path -s $target_system_dir $savesystempath ext4 ${partition_name} ${partition_size}
fi
