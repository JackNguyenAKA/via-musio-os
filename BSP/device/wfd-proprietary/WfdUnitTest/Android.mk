LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= wfd

LOCAL_MODULE:= wfd

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := EXECUTABLES

LOCAL_MODULE_PATH := $(TARGET_OUT)/bin

include $(BUILD_PREBUILT)