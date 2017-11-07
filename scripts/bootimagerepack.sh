# this script is for repacking boot.img
# we assume that the 'root' and 'boot' directory already exists

# need to locate
# - mkinitramfs.sh
# - root and boot directory
# - make_ext4 related bin files

DEFAULT_MKINITRAMFS_SH_PATH="android/device/nexell/tools/mkinitramfs.sh"
DEFAULT_TARGET_DIR="result/temp"
DEFAULT_TOOL_PATH="android/out/host/linux-x86/bin/mkuserimg.sh"
DEFAULT_BOARDCONFIGMK_PATH="android/device/nexell/musio/BoardConfig.mk"


echo "=== boot.img repack ==="

echo ">> path to mkinitramfs.sh (default: $DEFAULT_MKINITRAMFS_SH_PATH ):"
read mkinitramfs_sh_path

if [ -z $mkinitramfs_sh_path ]; then
	mkinitramfs_sh_path=$DEFAULT_MKINITRAMFS_SH_PATH
fi

if [ ! -f $mkinitramfs_sh_path ]; then
	echo "file doesn't exist. abort"
	exit 1
else
	echo "exists"
fi


echo ">> path to directory where 'root' and 'boot' dir exists (default: $DEFAULT_TARGET_DIR)"
read target_dir

if [ -z $target_dir ]; then
	target_dir=$DEFAULT_TARGET_DIR
fi

if [ ! -d $target_dir ]; then
	echo "dir doesn't exist. abort"
	exit 1
else
	echo "exists"
fi

if [ ! -d $target_dir/root ]; then
	echo "root dir doesn't exist. abort"
	exit 1
fi

if [ ! -d $target_dir/boot ]; then
	echo "boot dir doesn't exist. abort"
	exit 1
fi


echo ">> enter mkuserimage.sh location(default: $DEFAULT_TOOL_PATH) :"
read tool_path

# set to default value if no input.
if [ -z $tool_path ]; then
	tool_path=$DEFAULT_TOOL_PATH
fi


# check if it exists.
if [ -f $tool_path ]; then
	echo "it exists"
else
	echo "does not exist. abort"
	exit 1
fi

## if the tool exists, in the directory there should also be make_ext4fs.
## check if it exists

TOOL_DIR=$(dirname $tool_path)

if [ ! -f $TOOL_DIR/make_ext4fs ]; then
	echo "make_ext4fs not found. abort"
	exit 1
fi

## update PATH  to included this
#echo $PATH
PATH=$TOOL_DIR:$PATH


echo ">> BoardConfig.mk location(default : $DEFAULT_BOARDCONFIGMK_PATH) :"
read boardconfigmk_path

if [ -z $boardconfigmk_path ]; then
	boardconfigmk_path=$DEFAULT_BOARDCONFIGMK_PATH
fi

if [ -f $boardconfigmk_path ]; then
	echo "it exists"
else
	echo "does not exist. abort"
	exit 1
fi



# we assume that in the 'boot' dir, all other images and files except 'root.img.gz' is already populated.

# remaking the initramfs based on 'root' directory.
$mkinitramfs_sh_path $target_dir/root $target_dir
mv $target_dir/root.img.gz $target_dir/boot



#make_ext4 ${BOARD_NAME} boot



board_name=musio
partition_name=boot

# change it to uppercase letters
partition_name_upper=$(echo ${partition_name} | tr '[[:lower:]]' '[[:upper:]]')

# fetch partition size
partition_size=$(grep "BOARD_${partition_name_upper}IMAGE_PARTITION_SIZE" ${boardconfigmk_path} | awk '{print $3}')


echo "partition name: ${partition_name}, partition name upper: ${partition_name_upper}, partition_size: ${partition_size}"


$tool_path -s $target_dir/boot $target_dir/${partition_name}.img ext4 ${partition_name} ${partition_size}

echo "===finish==="
