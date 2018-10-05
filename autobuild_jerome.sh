#!/bin/bash

# author: chadrick(creator), jerome
# update: 2018/8/3

# this auto builder is for VIA

# recommended workflow:
# 1. fetch VIA / origin
# 2. clean build result
# 3. build with engineer mode without apps
# 4. archive.
# 4. build musio system
# 5. copy assembled results and .so files
# 6. build with user mode
# 7. archive.
# 8. build with distribution mode
# 9. archive.
# 10. make a new tag.
# 11. make a release note.


DEVICE="null"
FRAMEWORK_PATH="$(pwd)"
AOSP_PATH="$(pwd)/aosptree"
SYSTEM_SOFTWARE_PATH="$(pwd)/musio-system-library"


function select_action() {
	for S in $MENU_SELECTION
	do
		case $S in
			
			a)
				clean_framework
				;;
			b)
				clean_msl
				;;
			c)
				apply_overlay
				;;
	
			d)
				build_msl
				;;
			e)
				copy_msl_to_androidtree
				;;

			m)
				build_dev
				;;
			n)
				build_userdebug
				;;
			o)
				build_user
				;;
			t)
				show_current_product_info;;
			s)
				build_ota_user
				;;
			q)
				exit 0
				;;
			i) check_product_code;;
		esac
	done 
}

function menu() {
	echo "========================================================"				
	echo "AUTO BUILD"
	echo "========================================================"				

	echo "	> clean"
	echo "		a) Clean AOSP tree"
	echo "		b) Clean MSL"
	echo "  > manage AOSP"
	echo " 		c) Copy AOSP overlay to AOSP tree"
	echo "	> manage MSL"
	echo "		d) Build MSL"
	echo "		e) Copy MSL apk to AOSP tree"
	echo "	> Build AOSP"
	echo "		m) Build dev mode (dummyhome only)"
	echo "		n) Build userdebug mode (all apps+userdebug mode)"
	echo "		o) Build user mode (all apps+user mode)"
	echo "		s) build OTA file for user mode"
	echo "	> product info control"
	echo "		t) show current product info"
	echo "	q) Quit"
	read MENU_SELECTION
}



function clean_framework()
{
	echo "========================================================"				
	echo "CLEAN AOSP OUTPUTS"
	echo "========================================================"				
	cd ${AOSP_PATH}
	../docker/musio make installclean
	../docker/musio make clobber
	cd ${FRAMEWORK_PATH}

	# wanted to add a script that would delete the /android/result dir too
	# but discovered that for ota build, it requires the /android/result files in order to make one
	# therefore this feature is not added. it will be
}

function clean_msl()
{
	echo "========================================================"				
	echo "CLEAN MSL"
	echo "========================================================"				
	cd ${SYSTEM_SOFTWARE_PATH}
	./gradlew clean
	cd ${FRAMEWORK_PATH}
}

function apply_overlay()
{
	echo "========================================================"

	echo "COPY AOSP OVERLAY TO AOSP TREE"
	echo "========================================================"

	echo "Start removing aosp build files"
	rm -rf ${AOSP_PATH}/out/target/product/musio/system
	rm -rf ${AOSP_PATH}/out/target/product/musio/obj/APPS/Musio*
	rm -rf ${AOSP_PATH}/out/target/product/musio/obj/APPS/Apps*
	echo "Done"

	echo "Start copying source codes from overlay"
	cp -Ra ${FRAMEWORK_PATH}/BSP/* ${AOSP_PATH}/
	cp -Ra ${FRAMEWORK_PATH}/android_overlay/* ${AOSP_PATH}/
	echo "Copied"
}


function build_msl()
{
	echo "========================================================"				
	echo "BUILD MSL"
	echo "========================================================"				
	cd ${SYSTEM_SOFTWARE_PATH}
	assembleMusio_flag=true

	./gradlew assembleMusio	
	if [ $? -ne 0 ]; then
		echo "assembleMusio failed. abort"
		assembleMusio_flag=false
		return
	fi


	echo "** report **"
	if [ $assembleMusio_flag == 'true' ]; then
		echo "assembleMusio build OK"
	else
		echo "assembleMusio build FAIL"
	fi

	cd ${FRAMEWORK_PATH}
}



function copy_msl_to_androidtree()
{
	echo "========================================================"				
	echo "COPY MUSIO APPS TO ANDROID/PACKAGE/APPS"
	echo "========================================================"

	cd scripts
	python copyapkfromconfig_via.py
	cd ${FRAMEWORK_PATH}
}



function build_userdebug()
{
	echo "========================================================"	
	echo "BUILD USERDEBUG MODE"
	echo "========================================================"	

	# save start timestamp
	start_timestamp=$(date +%s)

	#need to apply appropriate copy override
	cd ${AOSP_PATH}/
	cp ./device/fsl/imx6/aosp_musio_product.mk ./device/fsl/imx6/aosp_musio.mk

	sleep 5
	${FRAMEWORK_PATH}/docker/musio ./build_userdebug.sh
	
	cd ${FRAMEWORK_PATH}
	finished_timestamp=$(date +%s)
	printpassedtime $start_timestamp $finished_timestamp
	echo "finished building userdebug image"
}

function printpassedtime(){
	timediff=$(($1-$2))

	takhr=$((timediff/3600))
	takmin=$((timediff/60))
	taksec=$((timediff%60))

	echo "elapsed time: ${takhr}hr ${takmin}min ${taksec}sec"
}

function build_dev()
{
	echo "========================================================"				
	echo "BUILD DEV IMAGE WITH USERDEBUG MODE"
	echo "========================================================"				
	cd ${FRAMEWORK_PATH}
	echo "Start to build dev image: " $(date -u) >> buildLog
	
	#need to apply appropriate copy override
	cd ${AOSP_PATH}/
	cp ./device/fsl/imx6/aosp_musio_develop.mk ./device/fsl/imx6/aosp_musio.mk	

	${FRAMEWORK_PATH}/docker/musio ./build_userdebug.sh

	cd ${FRAMEWORK_PATH}
	echo "Finished to build dev image: " $(date -u) >> buildLog
}


function build_user()
{
	echo "========================================================"				
	echo "BUILD USER MODE"
	echo "========================================================"		

	# save start timestamp
	start_timestamp=$(date +%s)


	#need to apply appropriate copy override
	cd ${AOSP_PATH}
	cp ./device/fsl/imx6/aosp_musio_product.mk ./device/fsl/imx6/aosp_musio.mk

	sleep 5
	${FRAMEWORK_PATH}/docker/musio ./build_user.sh

	cd ${FRAMEWORK_PATH}
	
	# get finished timestamp and print buildtime
	finished_timestamp=$(date +%s)
	printpassedtime $start_timestamp $finished_timestamp

	echo "Finished to build user image"
}



function build_ota_user(){
	echo "========================================================"				
	echo "BUILD OTA FOR USER MODE"
	echo "========================================================"				
	
	echo "Start to build ota for user mode: " $(date -u) >> buildLog

	# save start timestamp
	start_timestamp=$(date +%s)

	#need to apply appropriate copy override
	cd ${AOSP_PATH}/
	cp ./device/fsl/imx6/aosp_musio_product.mk ./device/fsl/imx6/aosp_musio.mk

	sleep 5
	${FRAMEWORK_PATH}/docker/musio ./build_dist.sh

	cd ${FRAMEWORK_PATH}

	# get finished timestamp and print buildtime
	finished_timestamp=$(date +%s)
	printpassedtime $start_timestamp $finished_timestamp

	
	echo "== BUILD OTA FOR USER MODE : FINISHED =="
}


function extract_product_info_from_file(){
	targetfilepath=$1

	if [ -z $targetfilepath ]; then
		echo "no argument given"
		return 1
	fi

	if [ ! -f $targetfilepath ]; then
		echo "no file found"
		return
	fi

	# sed -n 's/^PRODUCT_BRAND :=\(.+\)$/\1/p' $targetfilepath
	extracted_product_brand=$(cat $targetfilepath | sed -r -n 's/PRODUCT_BRAND.*:=(.+)/\1/p')
	extracted_product_model=$(cat $targetfilepath | sed -r -n 's/PRODUCT_MODEL.*:=(.+)/\1/p')
	extracted_product_code=$(cat $targetfilepath | sed -r -n 's/ro\.product\.code=(.+)/\1/p')


	echo "extracted product brand = $extracted_product_brand"
	echo "extracted product model = $extracted_product_model"
	echo "extracted product code = $extracted_product_code"


}

function show_current_product_info(){
	echo "========================================================"				
	echo "SHOW CURRENT PRODUCT INFO"
	echo "========================================================"	


	TARGET_DIR=${AOSP_PATH}/device/fsl/imx6/

	CHANGE_FILES=("aosp_musio_develop.mk" "aosp_musio_product.mk")

	for item in "${CHANGE_FILES[@]}"; do
		echo ""
		echo ">>> $item"
		filepath=$TARGET_DIR/$item
		extract_product_info_from_file $filepath
	done
}

function check_product_code(){
	TARGET_DIR=${AOSP_PATH}/device/fsl/imx6/

	echo "select which mk file to check"
	echo "1) aosp_musio_develop.mk"
	echo "2) aosp_musio_product.mk"

	read select_num
	file_to_search=
	if [ $select_num -eq 1 ];then
		file_to_search="aosp_musio_develop.mk"
	elif [ $select_num -eq 2 ];then
		file_to_search="aosp_musio_product.mk"
	else
		echo "invalid input. abort"
		return
	fi

	file_prefix=${AOSP_PATH}/device/fsl/imx6/
	file_fullpath=$file_prefix$file_to_search

	echo "product info in $file_to_search"
	echo "filefullpath $file_fullpath"
	extract_product_info_from_file $file_fullpath

	
}




if [ $# -eq 0 ]; then
	while [ 1 ]
	do
		menu
		select_action	
	done
else
	$@
fi


