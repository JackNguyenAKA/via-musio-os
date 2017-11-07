#!/bin/bash
# create: 17.11.07
# author: chadrick
# 
# this archive script is for chadrick's use
# for VIA build

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
	echo "$abspath/$filename"

}


#get current dir
curdir=$(pwd)
getonlydir=$(dirname $0)
actualdir=$curdir/$getonlydir
cd $actualdir
LOCAL_DIR=$(pwd)
echo $LOCAL_DIR


ARCHIVE_DATETIME=$(date +"%y%m%d_%H%M")

AOSP_OUT_DIR=$(getabspath $LOCAL_DIR/../aosptree/out/target/product/musio)


# get build info


echo "====== archive for VIA AOSP BUILD ====="
echo "please type in info"
echo "rc number:"
read RC_NUMBER
echo "build_tag(ex: dummy/userdebug/user/ota):"
read BUILD_TAG
echo "extra comments?"
read EXTRA_COMMENTS



isotabuild=false
# check if it is OTA build
## we check if it is OTA build if there is a file named OTA_DESC
if [ -f $LOCAL_DIR/../android/result/OTA_DESC ]; then
	isotabuild=true
fi

# discriminate the prefix_name based on whether it is ota build or not.
if [ "$isotabuild" = true ]; then
	COMPRESS_NAME=OTA_${BUILD_TAG}_${RC_NUMBER}_${ARCHIVE_DATETIME}
else
	COMPRESS_NAME=${BUILD_TAG}_${RC_NUMBER}_${ARCHIVE_DATETIME}
fi

if [ ! -z "$EXTRA_COMMENTS"]; then
	COMPRESS_NAME=${COMPRESS_NAME}_${EXTRA_COMMENTS}
fi

RESULT_PATH=$LOCAL_DIR/../result/$COMPRESS_NAME

# check if result directory already exists.
if [ -d $RESULT_PATH ]; then
	echo "result directory with same name already exists. abort."
	exit 1
fi

# create new result directory
echo "creating new result directory"
mkdir -p $RESULT_PATH

if [ -e $RESULT_PATH ]; then
	echo "result path exists"
fi


# copy files
echo "copying files..."
# cp -Ra $LOCAL_DIR/../android/result $RESULT_PATH



cp $AOSP_OUT_DIR/u-boot-musio.imx $RESULT_PATH
cp $AOSP_OUT_DIR/recovery-musio.img $RESULT_PATH
cp $AOSP_OUT_DIR/boot-musio.img $RESULT_PATH
cp $AOSP_OUT_DIR/system.img $RESULT_PATH
cp -Ra $AOSP_OUT_DIR/system $RESULT_PATH
cp -Ra $AOSP_OUT_DIR/root $RESULT_PATH


# # copy these files if not ota build
# if [ "$isotabuild" = false ]; then
	
# 	cp ../android/device/nexell/tools/partmap/partmap_musio.txt $RESULT_PATH/result
# 	cp ../android/device/nexell/musio/boot/2ndboot.bin $RESULT_PATH/result
# fi


## add buildinfo.txt
echo "generating buildinfo.txt"

BUILDINFOTXT_PATH=$RESULT_PATH/buildinfo.txt

touch $BUILDINFOTXT_PATH
echo "archive date: $ARCHIVE_DATETIME" >> $BUILDINFOTXT_PATH
echo "product code: $PRODUCT_CODE" >> $BUILDINFOTXT_PATH
echo "build tag: $BUILD_TAG" >> $BUILDINFOTXT_PATH
echo "MSL version: $MSL_VERSION" >> $BUILDINFOTXT_PATH


if [ "$isotabuild" = true ]; then
	echo "OTA BUILD" >> $BUILDINFOTXT_PATH
fi



# if non ota build, generate flash.sh in result

# if [ "$isotabuild" = false ]; then
	

# 	FLASHSCRIPT_PATH=$RESULT_PATH/result/flash.sh
# 	SUBRESULT_PATH=$RESULT_PATH/result

# 	# the default fastboot command. "fastboot" is default since we assume that the user has fastboot command already installed and added to the path

# 	FASTBOOTEXEC="fastboot"

# 	touch $FLASHSCRIPT_PATH

# 	# check partmap_musio.txt
# 	if [ -f $SUBRESULT_PATH/partmap_musio.txt ]; then
# 		echo "partmap_musio.txt --- o"
# 		echo "$FASTBOOTEXEC -S 256M flash partmap partmap_musio.txt" >> $FLASHSCRIPT_PATH
# 	else
# 		echo "partmap_musio.txt --- x"
# 	fi

# 	# check 2ndboot.bin
# 	if [ -f $SUBRESULT_PATH/2ndboot.bin ]; then
# 		echo "2ndboot.bin --- o"
# 		echo "$FASTBOOTEXEC -S 256M flash 2ndboot 2ndboot.bin" >> $FLASHSCRIPT_PATH
# 	else
# 		echo "2ndboot.bin --- x"
# 	fi

# 	# check u-boot.bin
# 	if [ -f $SUBRESULT_PATH/u-boot.bin ]; then
# 		echo "u-boot.bin --- o"
# 		echo "$FASTBOOTEXEC -S 256M flash bootloader u-boot.bin" >> $FLASHSCRIPT_PATH
# 	else
# 		echo "u-boot.bin --- x"
# 	fi

# 	# check boot.img
# 	if [ -f $SUBRESULT_PATH/boot.img ]; then
# 		echo "boot.img --- o"
# 		echo "$FASTBOOTEXEC -S 256M flash boot boot.img" >> $FLASHSCRIPT_PATH
# 	else
# 		echo "boot.img --- x"
# 	fi

# 	# check system.img
# 	if [ -f $SUBRESULT_PATH/system.img ]; then
# 		echo "system.img --- o"
# 		echo "$FASTBOOTEXEC -S 256M flash system system.img" >> $FLASHSCRIPT_PATH
# 	else
# 		echo "system.img --- x"
# 	fi

# 	# check userdata.img
# 	if [ -f $SUBRESULT_PATH/userdata.img ]; then
# 		echo "userdata.img --- o"
# 		echo "$FASTBOOTEXEC -S 256M flash userdata userdata.img" >> $FLASHSCRIPT_PATH
# 	else
# 		echo "userdata.img --- x"
# 	fi

# 	# check cache.img
# 	if [ -f $SUBRESULT_PATH/cache.img ]; then
# 		echo "cache.img --- o"
# 		echo "$FASTBOOTEXEC -S 256M flash cache cache.img" >> $FLASHSCRIPT_PATH
# 	else
# 		echo "cache.img --- x"
# 	fi

# 	# final fastboot reboot add
# 	echo "$FASTBOOTEXEC reboot" >> $FLASHSCRIPT_PATH
# fi

# # chmod flash.sh
# chmod u+x $FLASHSCRIPT_PATH


# compress.. but silently
echo "compressing ..."
tar -czvf $RESULT_PATH/$COMPRESS_NAME.tar.gz $RESULT_PATH >> /dev/null

echo "== archive finish : $RESULT_PATH =="