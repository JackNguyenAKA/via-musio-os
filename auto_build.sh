FRAMEWORK_PATH="/home/susan/git-projects/VIA-musio-os"
SYSTEM_SOFTWARE_PATH="/home/susan/git-projects/musio-system-library"


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
			3)
				clean_framework
				clean_system_software
				;;
			4)	
				build_system_software
				apply_system_software
				;;
			5)
				apply_overlay	
				build_user
				build_dist
				;;
			a)
				pull_framework
				;;
			b)
				pull_system_software
				;;
			c)
				restore_system_software
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
				show_apk_updated
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
				build_user
				build_dist
				archive_ota_results
				;;		
			n)	
				archive_ota_results
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
	echo "		c) restore system software"
	echo "		e) Clean framework"
	echo "		f) Clean system software"
	echo "	4) Build System softwares"
	echo "		g) Build system softwares"
	echo "		h) Apply system softwares"
	echo "	5) Build frameworks"
	echo " 		i) Apply Overlay"
	echo "		j) Build develop mode and archive"
	echo "		k) Build userdebug mode and archive"
	echo "		l) Build user mode and archive"
	echo "		m) Build dist mode and archive"
	echo "		n) archive"
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
	cd ${FRAMEWORK_PATH}/android5.1.1-1.0.0
	./../docker/musio make update-api
}

function build_system_software()
{
	echo "========================================================"				
	echo "BUILD SYSTEM SOFTWARE"
	echo "========================================================"				
	cd ${SYSTEM_SOFTWARE_PATH}
	./gradlew assembleMusio	
}

function clean_framework()
{
	echo "========================================================"				
	echo "CLEAN FRAMEWORK"
	echo "========================================================"				
	cd ${FRAMEWORK_PATH}/android5.1.1-1.0.0
	./../docker/musio make installclean
	./../docker/musio make clobber
}

function apply_overlay()
{
	echo "========================================================"				
	echo "APPLY OVERLAY"
	echo "========================================================"			
	rm -rf ${FRAMEWORK_PATH}/android5.1.1-1.0.0/packages/apps/Apps*
	rm -rf ${FRAMEWORK_PATH}/android5.1.1-1.0.0/packages/apps/Musio*
	rm -rf ${FRAMEWORK_PATH}/android5.1.1-1.0.0/out/target/product/musio/system
	rm -rf ${FRAMEWORK_PATH}/android5.1.1-1.0.0/out/target/product/musio/obj/APPS/Musio*
	rm -rf ${FRAMEWORK_PATH}/android5.1.1-1.0.0/out/target/product/musio/obj/APPS/Apps*
	echo "Start copying source codes to overlay..."
	cp -Ra ${FRAMEWORK_PATH}/BSP/* ${FRAMEWORK_PATH}/android5.1.1-1.0.0/
	cp -Ra ${FRAMEWORK_PATH}/android_overlay/* ${FRAMEWORK_PATH}/android5.1.1-1.0.0/
	echo "Copied."
}

function clean_system_software()
{
	echo "========================================================"				
	echo "CLEAN SYSTEM SOFTWARE"
	echo "========================================================"				
	cd ${SYSTEM_SOFTWARE_PATH}
	./gradlew clean
}

function build_userdebug()
{
	echo "========================================================"				
	echo "BUILD USERDEBUG MODE"
	echo "========================================================"				
	cd ${FRAMEWORK_PATH}/android5.1.1-1.0.0
	cp ./device/fsl/imx6/aosp_musio_product.mk ./device/fsl/imx6/aosp_musio.mk
	${FRAMEWORK_PATH}/docker/musio ./build_userdebug.sh
}

function build_user()
{
	echo "========================================================"				
	echo "BUILD USER MODE"
	echo "========================================================"				
	cd ${FRAMEWORK_PATH}/android5.1.1-1.0.0
	cp ./device/fsl/imx6/aosp_musio_product.mk ./device/fsl/imx6/aosp_musio.mk
	${FRAMEWORK_PATH}/docker/musio ./build_user.sh
}
function build_develop()
{
	echo "========================================================"				
	echo "BUILD DEVELOP MODE"
	echo "========================================================"				
	cd ${FRAMEWORK_PATH}/android5.1.1-1.0.0
	cp ./device/fsl/imx6/aosp_musio_develop.mk ./device/fsl/imx6/aosp_musio.mk
	${FRAMEWORK_PATH}/docker/musio ./build_userdebug.sh
}

function build_dist()
{
	echo "========================================================"				
	echo "BUILD DISTRIBUTION"
	echo "========================================================"	
	rm -rf result
	cd ${FRAMEWORK_PATH}/android5.1.1-1.0.0
	cp ./device/fsl/imx6/aosp_musio_product.mk ./device/fsl/imx6/aosp_musio.mk
	${FRAMEWORK_PATH}/docker/musio ./build_dist.sh
}

function archive_ota_results(){
	echo "========================================================"				
	echo "ARCHIVE OTA RESULTS"
	echo "========================================================"		
	cd ${FRAMEWORK_PATH}/android5.1.1-1.0.0
	mkdir result
	mv ./out/target/product/musio/aosp_musio-ota-20151218-musio.zip ./result/ota.zip
	cd ./result 
	split ota.zip -b 250m -d "ota-files."
	md5sum ota-files.00 >> checksum
	md5sum ota-files.01 >> checksum
	md5sum ota-files.02 >> checksum
	md5sum ota-files.03 >> checksum
}


function restore_system_software(){
	rm ${FRAMEWORK_PATH}/android_overlay/packages/apps/Apps*/*.apk
	rm ${FRAMEWORK_PATH}/android_overlay/packages/apps/Musio*/*.apk
	rm ${FRAMEWORK_PATH}/android_overlay/packages/apps/*.so
	apply_system_software
}

function show_apk_updated(){
	ls -al ${FRAMEWORK_PATH}/android_overlay/packages/apps/Apps*/*.apk
	ls -al ${FRAMEWORK_PATH}/android_overlay/packages/apps/Musio*/*.apk
}

function apply_system_software()
{
	echo "========================================================"				
	echo "APPLY SYSTEM SOFTWARE"
	echo "========================================================"

	
	cp ${SYSTEM_SOFTWARE_PATH}/dummyhome/build/outputs/apk/musio/dummyhome-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioDummyHome/MusioDummyHome.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-english/build/outputs/apk/musio/apps-academy-english-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyEnglish/AppsAcademyEnglish.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-english-dialog/build/outputs/apk/musio/apps-academy-english-dialog-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyEnglishDialog/AppsAcademyEnglishDialog.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-english-pattern/build/outputs/apk/musio/apps-academy-english-pattern-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyEnglishPattern/AppsAcademyEnglishPattern.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-english-voca/build/outputs/apk/musio/apps-academy-english-voca-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyEnglishVoca/AppsAcademyEnglishVoca.apk

	cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-icebreaker/build/outputs/apk/musio/apps-academy-icebreaker-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyIcebreaker/AppsAcademyIcebreaker.apk

	cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-launcher/build/outputs/apk/musio/apps-academy-launcher-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyLauncher/AppsAcademyLauncher.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-academy-placementtest/build/outputs/apk/musio/apps-academy-placementtest-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAcademyPlacementTest/AppsAcademyPlacementTest.apk
	
	cp ${SYSTEM_SOFTWARE_PATH}/apps-alarm/build/outputs/apk/musio/apps-alarm-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsAlarm/AppsAlarm.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-B2C-launcher/build/outputs/apk/musio/apps-B2C-launcher-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsB2CLauncher/AppsB2CLauncher.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-B2C-english-pattern/build/outputs/apk/musio/apps-B2C-english-pattern-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsB2CEnglishPattern/AppsB2CEnglishPattern.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-B2C-english-dialog/build/outputs/apk/musio/apps-B2C-english-dialog-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsB2CEnglishDialog/AppsB2CEnglishDialog.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-B2C-english-voca/build/outputs/apk/musio/apps-B2C-english-voca-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsB2CEnglishVoca/AppsB2CEnglishVoca.apk

	cp ${SYSTEM_SOFTWARE_PATH}/apps-edge-client/build/outputs/apk/musio/apps-edge-client-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEdgeClient/AppsEdgeClient.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-client-manager/build/outputs/apk/musio/apps-client-manager-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsClientManager/AppsClientManager.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub/build/outputs/apk/musio/apps-eduhub-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhub/AppsEduhub.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-conversation/build/outputs/apk/musio/apps-eduhub-conversation-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubConversation/AppsEduhubConversation.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-definitionquiz/build/outputs/apk/musio/apps-eduhub-definitionquiz-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubDefinitionQuiz/AppsEduhubDefinitionQuiz.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-dictionary-eng2jpn/build/outputs/apk/musio/apps-eduhub-dictionary-eng2jpn-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsDictionaryEng2Jpn/AppsDictionaryEng2Jpn.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-dictionary-jpn2eng/build/outputs/apk/musio/apps-eduhub-dictionary-jpn2eng-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsDictionaryJpn2Eng/AppsDictionaryJpn2Eng.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-exam/build/outputs/apk/musio/apps-eduhub-exam-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubExam/AppsEduhubExam.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-hangman/build/outputs/apk/musio/apps-eduhub-hangman-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubHangman/AppsEduhubHangman.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-pattern/build/outputs/apk/musio/apps-eduhub-pattern-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubPattern/AppsEduhubPattern.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-spellingbee/build/outputs/apk/musio/apps-eduhub-spellingbee-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubSpellingBee/AppsEduhubSpellingBee.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-wordstudy/build/outputs/apk/musio/apps-eduhub-wordstudy-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubWordstudy/AppsEduhubWordstudy.apk

	cp ${SYSTEM_SOFTWARE_PATH}/apps-gamehub/build/outputs/apk/musio/apps-gamehub-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsGamehub/AppsGamehub.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-gamehub-moviequotes/build/outputs/apk/musio/apps-gamehub-moviequotes-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsGamehubMovieQuotes/AppsGamehubMovieQuotes.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-gamehub-musioadventure/build/outputs/apk/musio/apps-gamehub-musioadventure-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsGamehubMusioAdventure/AppsGamehubMusioAdventure.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-gamehub-rockscissors/build/outputs/apk/musio/apps-gamehub-rockscissors-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsGamehubRockScissors/AppsGamehubRockScissors.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-gamehub-tictactoe/build/outputs/apk/musio/apps-gamehub-tictactoe-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsGamehubTicTacToe/AppsGamehubTicTacToe.apk

	cp ${SYSTEM_SOFTWARE_PATH}/apps-inspect/build/outputs/apk/musio/apps-inspect-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsInspect/AppsInspect.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-jpnchat/build/outputs/apk/musio/apps-jpnchat-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsJpnChat/AppsJpnChat.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-launcher/build/outputs/apk/musio/apps-launcher-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioLauncher/MusioLauncher.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-member/build/outputs/apk/musio/apps-member-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioMemberProfile/MusioMemberProfile.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-notice/build/outputs/apk/musio/apps-notice-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioNotice/MusioNotice.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-push-sample/build/outputs/apk/musio/apps-push-sample-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsPushSample/AppsPushSample.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-radio/build/outputs/apk/musio/apps-radio-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsRadio/AppsRadio.apk

	cp ${SYSTEM_SOFTWARE_PATH}/apps-settings/build/outputs/apk/musio/apps-settings-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioSetting/MusioSetting.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy/build/outputs/apk/musio/apps-sophy-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophy/AppsSophy.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-card-tarot/build/outputs/apk/musio/apps-sophy-card-tarot-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyCardTarot/AppsSophyCardTarot.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-calculator/build/outputs/apk/musio/apps-sophy-calculator-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyCalculator/AppsSophyCalculator.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-card-music-video/build/outputs/apk/musio/apps-sophy-card-music-video-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyCardMusicVideo/AppsSophyCardMusicVideo.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-launchpad/build/outputs/apk/musio/apps-sophy-launchpad-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyCardMusicLaunchpad/AppsSophyCardMusicLaunchpad.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-attendance/build/outputs/apk/musio/apps-sophy-attendance-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyAttendance/AppsSophyAttendance.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-bubble/build/outputs/apk/musio/apps-sophy-bubble-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyBubble/AppsSophyBubble.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-collect-rocket/build/outputs/apk/musio/apps-sophy-collect-rocket-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyCollectRocket/AppsSophyCollectRocket.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-musioadventure/build/outputs/apk/musio/apps-sophy-musioadventure-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyMusioAdventure/AppsSophyMusioAdventure.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-tutor/build/outputs/apk/musio/apps-sophy-tutor-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyTutor/AppsSophyTutor.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-update/build/outputs/apk/musio/apps-update-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsUpdate/AppsUpdate.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-wardrobe/build/outputs/apk/musio/apps-wardrobe-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsWardrobe/AppsWardrobe.apk

	cp ${SYSTEM_SOFTWARE_PATH}/arduino/build/outputs/apk/musio/arduino-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioArduino/MusioArduino.apk
	cp ${SYSTEM_SOFTWARE_PATH}/controller/build/outputs/apk/musio/controller-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioController/MusioController.apk
	cp ${SYSTEM_SOFTWARE_PATH}/emotion/build/outputs/apk/musio/emotion-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioEmotion/MusioEmotion.apk
	cp ${SYSTEM_SOFTWARE_PATH}/liveface/build/outputs/apk/musio/liveface-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioLiveFace/MusioLiveFace.apk
	cp ${SYSTEM_SOFTWARE_PATH}/home/build/outputs/apk/musio/home-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioHome/MusioHome.apk
	cp ${SYSTEM_SOFTWARE_PATH}/member/build/outputs/apk/musio/member-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioMember/MusioMember.apk
	cp ${SYSTEM_SOFTWARE_PATH}/mode/build/outputs/apk/musio/mode-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioMode/MusioMode.apk
	cp ${SYSTEM_SOFTWARE_PATH}/muse/build/outputs/apk/musio/muse-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioMuse/MusioMuse.apk
	cp ${SYSTEM_SOFTWARE_PATH}/keyboard/build/outputs/apk/musio/keyboard-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioKeyboard/MusioKeyboard.apk
	cp ${SYSTEM_SOFTWARE_PATH}/push/build/outputs/apk/musio/push-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioPush/MusioPush.apk
	cp ${SYSTEM_SOFTWARE_PATH}/sophy/build/outputs/apk/musio/sophy-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioSophy/MusioSophy.apk
	cp ${SYSTEM_SOFTWARE_PATH}/sphinx/build/outputs/apk/musio/sphinx-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioSphinx/MusioSphinx.apk 
	cp "${SYSTEM_SOFTWARE_PATH}/status/build/outputs/apk/musio/status-musio.apk" "${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioStatus/MusioStatus.apk"
	cp ${SYSTEM_SOFTWARE_PATH}/system/build/outputs/apk/musio/system-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioSystem/MusioSystem.apk
	cp ${SYSTEM_SOFTWARE_PATH}/timeout/build/outputs/apk/musio/timeout-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioTimeout/MusioTimeout.apk
	cp ${SYSTEM_SOFTWARE_PATH}/touch/build/outputs/apk/musio/touch-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioTouch/MusioTouch.apk
	cp ${SYSTEM_SOFTWARE_PATH}/tts/build/outputs/apk/musio/tts-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioTTS/MusioTTS.apk
	cp ${SYSTEM_SOFTWARE_PATH}/ui/build/outputs/apk/musio/ui-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioUI/MusioUI.apk
	cp ${SYSTEM_SOFTWARE_PATH}/vision/build/outputs/apk/musio/vision-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioVision/MusioVision.apk

	cp "${SYSTEM_SOFTWARE_PATH}/arduino/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libserial_port.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/tts/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libengine.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/vision/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libdetection_based_tracker.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/vision/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libopencv_java3.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/touch/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libmultitouch2.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/keyboard/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/liblipitk.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/sphinx/build/intermediates/jniLibs/musio/armeabi-v7a/libpocketsphinx_jni.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	
	cp "${SYSTEM_SOFTWARE_PATH}/library-gdx4musio/libs/armeabi-v7a/libgdx.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/library-gdx4musio/libs/armeabi-v7a/libgdx-box2d.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/apps-sophy-tutor/build/intermediates/transforms/mergeJniLibs/musio/0/lib/armeabi-v7a/librealm-jni.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/

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


