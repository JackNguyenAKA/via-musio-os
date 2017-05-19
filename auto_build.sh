FRAMEWORK_PATH="/home/susan/git_project/via-musio-os"
SYSTEM_SOFTWARE_PATH="/home/susan/git_project/musio-system"

	echo "========================================================"				
	echo "APPLY SYSTEM SOFTWARE"
	echo "========================================================"

	cp ${SYSTEM_SOFTWARE_PATH}/dummyhome/build/outputs/apk/dummyhome-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioDummyHome/MusioDummyHome.apk
	#cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-dictionary-eng2jpn/build/outputs/apk/apps-eduhub-dictionary-eng2jpn-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsDictionaryEng2Jpn/AppsDictionaryEng2Jpn.apk
	#cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-dictionary-jpn2eng/build/outputs/apk/apps-eduhub-dictionary-jpn2eng-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsDictionaryJpn2Eng/AppsDictionaryJpn2Eng.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-dictionary/build/outputs/apk/apps-dictionary-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsDictionary/AppsDictionary.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub/build/outputs/apk/apps-eduhub-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhub/AppsEduhub.apk
	#cp ${SYSTEM_SOFTWARE_PATH}/apps-eduhub-conversation/build/outputs/apk/apps-eduhub-conversation-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsEduhubConversation/AppsEduhubConversation.apk

	cp ${SYSTEM_SOFTWARE_PATH}/apps-push-sample/build/outputs/apk/apps-push-sample-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsPushSample/AppsPushSample.apk

	cp ${SYSTEM_SOFTWARE_PATH}/apps-launcher/build/outputs/apk/apps-launcher-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioLauncher/MusioLauncher.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-member/build/outputs/apk/apps-member-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioMemberProfile/MusioMemberProfile.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-settings/build/outputs/apk/apps-settings-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioSetting/MusioSetting.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy/build/outputs/apk/apps-sophy-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophy/AppsSophy.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-tutor/build/outputs/apk/apps-sophy-tutor-tutor.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyTutor/AppsSophyTutor.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-collect-rocket/build/outputs/apk/apps-sophy-collect-rocket-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyCollectRocket/AppsSophyCollectRocket.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-sophy-bubble/build/outputs/apk/apps-sophy-bubble-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsSophyBubble/AppsSophyBubble.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-update/build/outputs/apk/apps-update-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsUpdate/AppsUpdate.apk
	cp ${SYSTEM_SOFTWARE_PATH}/apps-inspect/build/outputs/apk/apps-inspect-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/AppsInspect/AppsInspect.apk
	cp ${SYSTEM_SOFTWARE_PATH}/arduino/build/outputs/apk/arduino-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioArduino/MusioArduino.apk
	cp ${SYSTEM_SOFTWARE_PATH}/controller/build/outputs/apk/controller-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioController/MusioController.apk
	cp ${SYSTEM_SOFTWARE_PATH}/emotion/build/outputs/apk/emotion-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioEmotion/MusioEmotion.apk
	cp ${SYSTEM_SOFTWARE_PATH}/liveface/build/outputs/apk/liveface-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioLiveFace/MusioLiveFace.apk
	cp ${SYSTEM_SOFTWARE_PATH}/home/build/outputs/apk/home-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioHome/MusioHome.apk
	cp ${SYSTEM_SOFTWARE_PATH}/member/build/outputs/apk/member-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioMember/MusioMember.apk
	cp ${SYSTEM_SOFTWARE_PATH}/mode/build/outputs/apk/mode-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioMode/MusioMode.apk
	cp ${SYSTEM_SOFTWARE_PATH}/muse/build/outputs/apk/muse-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioMuse/MusioMuse.apk
	cp ${SYSTEM_SOFTWARE_PATH}/keyboard/build/outputs/apk/keyboard-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioKeyboard/MusioKeyboard.apk
	cp ${SYSTEM_SOFTWARE_PATH}/push/build/outputs/apk/push-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioPush/MusioPush.apk
	cp ${SYSTEM_SOFTWARE_PATH}/sophy/build/outputs/apk/sophy-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioSophy/MusioSophy.apk
	cp ${SYSTEM_SOFTWARE_PATH}/sphinx/build/outputs/apk/sphinx-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioSphinx/MusioSphinx.apk 
	cp "${SYSTEM_SOFTWARE_PATH}/status/build/outputs/apk/status-musio.apk" "${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioStatus/MusioStatus.apk"
	cp ${SYSTEM_SOFTWARE_PATH}/system/build/outputs/apk/system-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioSystem/MusioSystem.apk
	cp ${SYSTEM_SOFTWARE_PATH}/touch/build/outputs/apk/touch-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioTouch/MusioTouch.apk
	cp ${SYSTEM_SOFTWARE_PATH}/tts/build/outputs/apk/tts-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioTTS/MusioTTS.apk
	cp ${SYSTEM_SOFTWARE_PATH}/ui/build/outputs/apk/ui-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioUI/MusioUI.apk
	cp ${SYSTEM_SOFTWARE_PATH}/vision/build/outputs/apk/vision-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/MusioVision/MusioVision.apk
	#cp ${SYSTEM_SOFTWARE_PATH}/tutorapp/build/outputs/apk/tutorapp-musio.apk ${FRAMEWORK_PATH}/android_overlay/packages/apps/Musio/MusioTutor/MusioTutor.apk

	cp "${SYSTEM_SOFTWARE_PATH}/arduino/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libserial_port.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/tts/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libengine.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/vision/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libdetection_based_tracker.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/vision/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libopencv_java3.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/touch/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/libmultitouch2.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/keyboard/build/intermediates/ndkBuild/musio/obj/local/armeabi-v7a/liblipitk.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/sphinx/build/intermediates/jniLibs/musio/armeabi-v7a/libpocketsphinx_jni.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/libgdx4musio/libs/armeabi-v7a/libgdx.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/libgdx4musio/libs/armeabi-v7a/libgdx-box2d.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/
	cp "${SYSTEM_SOFTWARE_PATH}/apps-sophy-tutor/build/intermediates/transforms/mergeJniLibs/tutor/folders/2000/1f/main/lib/armeabi-v7a/librealm-jni.so" ${FRAMEWORK_PATH}/android_overlay/packages/apps/




