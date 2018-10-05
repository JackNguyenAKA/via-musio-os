#!/bin/bash
# create: 18.08.07
# author: jerome

# this archive script is for via ota file only
# this will simply move the built file in ../aosptree/out/target/product/musio/aosp_musio-ota-20151218-musio.zip to ../result/ota.zip

OTA_OUT_FILE_PATH=../aosptree/out/target/product/musio/aosp_musio-ota-20151218-musio.zip
RESULT_PATH=../result/


echo "====================================================="
echo "ARCHIVE OTA RESULTS"
echo "====================================================="

echo "===== move ====="
echo "please type the ota info"
echo "rc number:"
read RC_NUMBER
echo "build_tag(ex: dummy/userdebug/user):"
read BUILD_TAG
echo "extra comments?"
read EXTRA_COMMENTS

ARCHIVE_DATETIME=$(date +"%y%m%d_%H%M")

OTA_NAME=OTA_${BUILD_TAG}_${RC_NUMBER}_${ARCHIVE_DATETIME}

if [ ! -z "$EXTRA_COMMENTS" ]; then
	OTA_NAME=${OTA_NAME}_${EXTRA_COMMENTS}
fi



echo "Start mv ../aosptree/out/target/product/musio/aosp_musio-ota-20151218-musio.zip ../result/${OTA_NAME}.zip"

sudo mv ${OTA_OUT_FILE_PATH} ${RESULT_PATH}/${OTA_NAME}.zip

echo "Done"
