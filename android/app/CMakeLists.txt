cmake_minimum_required(VERSION 3.4.1)

include_directories(../.. ${ANDROID_NDK}/sources/android/native_app_glue)

aux_source_directory(../../common common-src)
aux_source_directory(../../common/Render common-render-src)

add_library(
	native-lib
	SHARED
	src/main/cpp/main.cpp
	src/main/cpp/Platform.cpp
	${common-src}
	${common-render-src})

add_library(
	native-app-glue
	STATIC
	${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

set_property(TARGET native-lib APPEND_STRING PROPERTY LINK_FLAGS " -u ANativeActivity_onCreate")

find_library(log-lib log)

target_link_libraries(native-lib native-app-glue ${log-lib} GLESv3 android EGL m)
