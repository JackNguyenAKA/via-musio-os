FRAMEWORK_PATH="/home/aka/ext2/viaMOS"
SYSTEM_SOFTWARE_PATH="/home/aka/ext2/viaMOS/musio-system-library"
AOSP_TREE_NAME="aosptree"


function select_action() {
	for S in $MENU_SELECTION
	do
		case $S in
			0)
				pull_framework
				pull_system_software
				clean_framework
				clean_system_software
				build_system_software	
				apply_system_software
				apply_overlay	
				build_user
				build_dist
				;;
			1)
				pull_framework
				pull_system_software
				;;
			2)
				update_api
				;;
			a)
				pull_framework
				;;
			b)
				pull_system_software
				;;
			c)
				clean_aosptree_musio_packages
				;;
			d)
				;;
			e)
				clean_framework
				;;
			f)
				clean_system_software
				;;
			g)
				build_system_software
				;; 
			h)
				apply_system_software
				;;
			i)
				apply_overlay
				;;
			j)
				build_develop
				;;
			k)
				build_userdebug
				;;
			l)
				build_user
				;;		
			m)
				build_dist
				;;		

			q)
				exit 0
				;;
		esac
	done 
}

function menu() {
	echo "========================================================"				
	echo "AUTO BUILD"
	echo "========================================================"				

	echo "	0) Auto"
	echo "	1) Pull"
	echo "		a) Pull framework"
	echo "		b) Pull Musio System Library"
	echo "	2) Update API"
	echo "	3) Clean"
	echo "		c) Clean aosp tree musio packages"
	echo "		e) Clean aosp tree"
	echo "		f) Clean MSL"
	echo "	4) Build"
	echo "		g) Build MSL"
	echo "		j) Build develop mode and archive"
	echo "		k) Build userdebug mode and archive"
	echo "		l) Build user mode and archive"
	echo "		m) Build dist mode and archive"
	echo "	q) Quit"
	read MENU_SELECTION
}

function pull_framework()
{
	echo "========================================================"				
	echo "UPDATE FRAMEWORK"
	echo "========================================================"				
	cd ${FRAMEWORK_REPO_PATH}
	git pull origin master
}

function pull_system_software()
{
	echo "========================================================"				
	echo "UPDATE SYSTEM SOFTWARE"
	echo "========================================================"				
	cd ${SYSTEM_SOFTWARE_PATH}
	git pull upstream master
}

function update_api()
{
	echo "========================================================"				
	echo "UPDATE API"
	echo "========================================================"				
	cd ${FRAMEWORK_PATH}/${AOSP_TREE_NAME}
	./../docker/musio make update-api
}

function build_system_software()
{
	echo "========================================================"				
	echo "BUILD MSL"
	echo "========================================================"				
	cd ${SYSTEM_SOFTWARE_PATH}
	./gradlew assembleMusio	
	./gradlew assembleTutor	
}

function clean_framework()
{
	echo "========================================================"				
	echo "CLEAN AOSP TREE"
	echo "========================================================"				
	cd ${FRAMEWORK_PATH}/${AOSP_TREE_NAME}
	./../docker/musio make installclean
	./../docker/musio make clobber
}

# function apply_overlay()
# {
# 	echo "========================================================"				
# 	echo "APPLY OVERLAY"
# 	echo "========================================================"			
# 	rm -rf ${FRAMEWORK_PATH}/android5.1.1-1.0.0/out/target/product/musio/system
# 	rm -rf ${FRAMEWORK_PATH}/android5.1.1-1.0.0/out/target/product/musio/obj/APPS/Musio*
# 	rm -rf ${FRAMEWORK_PATH}/android5.1.1-1.0.0/out/target/product/musio/obj/APPS/Apps*
# 	echo "Start copying source codes to overlay..."
# 	cp -Ra ${FRAMEWORK_PATH}/BSP/* ${FRAMEWORK_PATH}/android5.1.1-1.0.0/
# 	cp -Ra ${FRAMEWORK_PATH}/android_overlay/* ${FRAMEWORK_PATH}/android5.1.1-1.0.0/
# 	echo "Copied."
# }

function clean_system_software()
{
	echo "========================================================"				
	echo "CLEAN MSL"
	echo "========================================================"				
	cd ${SYSTEM_SOFTWARE_PATH}
	./gradlew clean
}

function build_userdebug()
{
	echo "========================================================"				
	echo "BUILD USERDEBUG MODE"
	echo "========================================================"				
	cd ${FRAMEWORK_PATH}/${AOSP_TREE_NAME}
	cp ./device/fsl/imx6/aosp_musio_product.mk ./device/fsl/imx6/aosp_musio.mk
	echo "copied & using aosp_musio_product.mk"
	sleep 3
	sudo ${FRAMEWORK_PATH}/docker/musio ./build_userdebug.sh
}

function build_user()
{
	echo "========================================================"				
	echo "BUILD USER MODE"
	echo "========================================================"				
	cd ${FRAMEWORK_PATH}/${AOSP_TREE_NAME}
	cp ./device/fsl/imx6/aosp_musio_product.mk ./device/fsl/imx6/aosp_musio.mk
	${FRAMEWORK_PATH}/docker/musio ./build_user.sh
}
function build_develop()
{
	echo "========================================================"				
	echo "BUILD DEVELOP MODE"
	echo "========================================================"				
	cd ${FRAMEWORK_PATH}/${AOSP_TREE_NAME}
	cp ./device/fsl/imx6/aosp_musio_develop.mk ./device/fsl/imx6/aosp_musio.mk
	${FRAMEWORK_PATH}/docker/musio ./build_userdebug.sh
}

function build_dist()
{
	echo "========================================================"				
	echo "BUILD DISTRIBUTION"
	echo "========================================================"				
	cd ${FRAMEWORK_PATH}/${AOSP_TREE_NAME}
	cp ./device/fsl/imx6/aosp_musio_product.mk ./device/fsl/imx6/aosp_musio.mk
	${FRAMEWORK_PATH}/docker/musio ./build_dist.sh
}


function clean_aosptree_musio_packages()
{
	rm ${FRAMEWORK_PATH}/${AOSP_TREE_NAME}/packages/apps/Apps*/*.apk
	rm ${FRAMEWORK_PATH}/${AOSP_TREE_NAME}/packages/apps/Musio*/*.apk
	rm ${FRAMEWORK_PATH}/${AOSP_TREE_NAME}/packages/apps/*.so
}

function apply_system_software()
{

	echo "this is deprecated. please use copyapkfiles.py in scripts dir"

	# echo "========================================================"				
	# echo "APPLY SYSTEM SOFTWARE"
	# echo "========================================================"

	
	# #cp ${SYSTEM_SOFTWARE_PATH}/dummyhome/build/outputs/apk/dummyhome-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioDummyHome/MusioDummyHome.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-english/build/outputs/apk/apps-academy-english-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyEnglish/AppsAcademyEnglish.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-english-dialog/build/outputs/apk/apps-academy-english-dialog-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyEnglishDialog/AppsAcademyEnglishDialog.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-english-pattern/build/outputs/apk/apps-academy-english-pattern-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyEnglishPattern/AppsAcademyEnglishPattern.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-english-voca/build/outputs/apk/apps-academy-english-voca-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyEnglishVoca/AppsAcademyEnglishVoca.apk

	# cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-icebreaker/build/outputs/apk/apps-academy-icebreaker-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyIcebreaker/AppsAcademyIcebreaker.apk

	# cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-launcher/build/outputs/apk/apps-academy-launcher-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyLauncher/AppsAcademyLauncher.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-placementtest/build/outputs/apk/apps-academy-placementtest-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyPlacementTest/AppsAcademyPlacementTest.apk
	
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-alarm/build/outputs/apk/apps-alarm-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAlarm/AppsAlarm.apk

	# cp ${SYSTEM_SOFTWARE_PATH}/apps-edge-client/build/outputs/apk/apps-edge-client-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEdgeClient/AppsEdgeClient.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub/build/outputs/apk/apps-eduhub-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhub/AppsEduhub.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-conversation/build/outputs/apk/apps-eduhub-conversation-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubConversation/AppsEduhubConversation.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-definitionquiz/build/outputs/apk/apps-eduhub-definitionquiz-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubDefinitionQuiz/AppsEduhubDefinitionQuiz.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-dictionary-eng2jpn/build/outputs/apk/apps-eduhub-dictionary-eng2jpn-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsDictionaryEng2Jpn/AppsDictionaryEng2Jpn.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-dictionary-jpn2eng/build/outputs/apk/apps-eduhub-dictionary-jpn2eng-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsDictionaryJpn2Eng/AppsDictionaryJpn2Eng.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-exam/build/outputs/apk/apps-eduhub-exam-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubExam/AppsEduhubExam.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-hangman/build/outputs/apk/apps-eduhub-hangman-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubHangman/AppsEduhubHangman.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-pattern/build/outputs/apk/apps-eduhub-pattern-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubPattern/AppsEduhubPattern.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-spellingbee/build/outputs/apk/apps-eduhub-spellingbee-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubSpellingBee/AppsEduhubSpellingBee.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-wordstudy/build/outputs/apk/apps-eduhub-wordstudy-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubWordstudy/AppsEduhubWordstudy.apk

	# cp ${SYSTEM_SOFTWARE_PATH}/apps-gamehub/build/outputs/apk/apps-gamehub-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsGamehub/AppsGamehub.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-gamehub-moviequotes/build/outputs/apk/apps-gamehub-moviequotes-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsGamehubMovieQuotes/AppsGamehubMovieQuotes.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-gamehub-musioadventure/build/outputs/apk/apps-gamehub-musioadventure-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsGamehubMusioAdventure/AppsGamehubMusioAdventure.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-gamehub-rockscissors/build/outputs/apk/apps-gamehub-rockscissors-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsGamehubRockScissors/AppsGamehubRockScissors.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-gamehub-tictactoe/build/outputs/apk/apps-gamehub-tictactoe-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsGamehubTicTacToe/AppsGamehubTicTacToe.apk

	# cp ${SYSTEM_SOFTWARE_PATH}/apps-inspect/build/outputs/apk/apps-inspect-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsInspect/AppsInspect.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-jpnchat/build/outputs/apk/apps-jpnchat-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsJpnChat/AppsJpnChat.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-launcher/build/outputs/apk/apps-launcher-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioLauncher/MusioLauncher.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-member/build/outputs/apk/apps-member-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioMemberProfile/MusioMemberProfile.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-notice/build/outputs/apk/apps-notice-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioNotice/MusioNotice.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-push-sample/build/outputs/apk/apps-push-sample-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsPushSample/AppsPushSample.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-radio/build/outputs/apk/apps-radio-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsRadio/AppsRadio.apk

	# cp ${SYSTEM_SOFTWARE_PATH}/apps-settings/build/outputs/apk/apps-settings-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioSetting/MusioSetting.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy/build/outputs/apk/apps-sophy-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophy/AppsSophy.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-card-tarot/build/outputs/apk/apps-sophy-card-tarot-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyCardTarot/AppsSophyCardTarot.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-card-music-video/build/outputs/apk/apps-sophy-card-music-video-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyCardMusicVideo/AppsSophyCardMusicVideo.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-launchpad/build/outputs/apk/apps-sophy-launchpad-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyCardMusicLaunchpad/AppsSophyCardMusicLaunchpad.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-attendance/build/outputs/apk/apps-sophy-attendance-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyAttendance/AppsSophyAttendance.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-bubble/build/outputs/apk/apps-sophy-bubble-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyBubble/AppsSophyBubble.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-collect-rocket/build/outputs/apk/apps-sophy-collect-rocket-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyCollectRocket/AppsSophyCollectRocket.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-musioadventure/build/outputs/apk/apps-sophy-musioadventure-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyMusioAdventure/AppsSophyMusioAdventure.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-tutor/build/outputs/apk/apps-sophy-tutor-tutor.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyTutor/AppsSophyTutor.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-update/build/outputs/apk/apps-update-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsUpdate/AppsUpdate.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/apps-wardrobe/build/outputs/apk/apps-wardrobe-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsWardrobe/AppsWardrobe.apk

	# cp ${SYSTEM_SOFTWARE_PATH}/arduino/build/outputs/apk/arduino-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioArduino/MusioArduino.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/controller/build/outputs/apk/controller-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioController/MusioController.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/emotion/build/outputs/apk/emotion-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioEmotion/MusioEmotion.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/liveface/build/outputs/apk/liveface-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioLiveFace/MusioLiveFace.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/home/build/outputs/apk/home-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioHome/MusioHome.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/member/build/outputs/apk/member-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioMember/MusioMember.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/mode/build/outputs/apk/mode-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioMode/MusioMode.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/muse/build/outputs/apk/muse-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioMuse/MusioMuse.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/keyboard/build/outputs/apk/keyboard-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioKeyboard/MusioKeyboard.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/push/build/outputs/apk/push-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioPush/MusioPush.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/sophy/build/outputs/apk/sophy-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioSophy/MusioSophy.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/sphinx/build/outputs/apk/sphinx-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioSphinx/MusioSphinx.apk 
	# cp "${SYSTEM_SOFTWARE_PATH}/status/build/outputs/apk/status-musio.apk" "${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioStatus/MusioStatus.apk"
	# cp ${SYSTEM_SOFTWARE_PATH}/system/build/outputs/apk/system-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioSystem/MusioSystem.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/timeout/build/outputs/apk/timeout-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioTimeout/MusioTimeout.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/touch/build/outputs/apk/touch-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioTouch/MusioTouch.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/tts/build/outputs/apk/tts-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioTTS/MusioTTS.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/ui/build/outputs/apk/ui-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioUI/MusioUI.apk
	# cp ${SYSTEM_SOFTWARE_PATH}/vision/build/outputs/apk/vision-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioVision/MusioVision.apk

	# cp "${SYSTEM_SOFTWARE_PATH}/arduino/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libserial_port.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	# cp "${SYSTEM_SOFTWARE_PATH}/tts/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libengine.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	# cp "${SYSTEM_SOFTWARE_PATH}/vision/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libdetection_based_tracker.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	# cp "${SYSTEM_SOFTWARE_PATH}/vision/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libopencv_java3.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	# cp "${SYSTEM_SOFTWARE_PATH}/touch/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libmultitouch2.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	# cp "${SYSTEM_SOFTWARE_PATH}/keyboard/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/liblipitk.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	# cp "${SYSTEM_SOFTWARE_PATH}/sphinx/build/intermediates/jniLibs/musio/armeabi-v7a/libpocketsphinx_jni.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	
	# cp "${SYSTEM_SOFTWARE_PATH}/library-gdx4musio/libs/armeabi-v7a/libgdx.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	# cp "${SYSTEM_SOFTWARE_PATH}/library-gdx4musio/libs/armeabi-v7a/libgdx-box2d.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	# cp "${SYSTEM_SOFTWARE_PATH}/apps-sophy-tutor/build/intermediates/transforms/mergeJniLibs/tutor/folders/2000/1f/main/lib/armeabi-v7a/librealm-jni.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/

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


