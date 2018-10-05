#!/bin/bash

# update: 2018/08/01
# contributor: jerome

RESULT_DIR="../result"
TARGET_DIR_NAME=$1

if [ -z $TARGET_DIR_NAME ]; then
    echo "no input. exit"
    exit 1
fi

# check if dir exist
if [ -d $RESULT_DIR/$TARGET_DIR_NAME ]; then
    echo "target dir exist"
else
    echo "target dir not exist. abort"
    exit 1
fi


NEW_FILE_NAME=$TARGET_DIR_NAME.tar.gz


if [ -f $RESULT_DIR/$NEW_FILE_NAME ]; then
    rm $RESULT_DIR/$NEW_FILE_NAME
fi

cd $RESULT_DIR/$TARGET_DIR_NAME/result

# cd $RESULT_DIR/$TARGET_DIR_NAME

if [ ! -f flash.sh ]; then
    echo "flash.sh not exist"
    exit 1
fi

if [ ! -f u-boot-musio.imx ]; then
    echo "u-boot-musio.imx not exist"
    exit
fi

#if [ ! -f 2ndboot.bin ]; then
#    echo "2ndboot.bin not exist"
#    exit 1
#fi

if [ ! -f boot-musio.img ]; then
    echo "boot-musio.img not exist"
    exit 1
fi

#if [ ! -f cache.img ]; then
#    echo "cache.img not exist"
#    exit 1
#fi

#if [ ! -f partmap_musio.txt ]; then
#    echo "partmap_musio.txt not exist"
#    exit 1
#fi

if [ ! -f system.img ]; then
    echo "system.img not exist"
    exit 1
fi

if [ ! -f recovery-musio.img ]; then
    echo "recovery-musio.img not exist"
    exit 1
fi

#if [ ! -f u-boot.bin ]; then
#    echo "u-boot.bin not exist"
#    exit 1
#fi


#if [ ! -f userdata.img ]; then
#    echo "userdata.img not exist"
#    exit 1
#fi

echo "zipping..."
tar -czf $NEW_FILE_NAME flash.sh u-boot-musio.imx boot-musio.img system.img recovery-musio.img

mv $NEW_FILE_NAME ../..
