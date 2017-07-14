cmake_minimum_required(VERSION 3.4.1)

include_directories(../.. ${ANDROID_NDK}/sources/android/native_app_glue)

add_library(
	native-lib
	SHARED
	src/main/cpp/main.cpp
	../../common/Application.cpp
	../../common/Batcher.cpp
	../../common/BitmapFont.cpp
	../../common/DDSLoader.cpp
	../../common/File.cpp
	../../common/GameState.cpp
	../../common/Math.cpp
	../../common/MenuState.cpp
	../../common/OpenGL.cpp
	../../common/State.cpp
	../../common/StateMachine.cpp
	../../common/TestState.cpp
	../../common/World.cpp)

add_library(
	native-app-glue
	STATIC
	${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

set_property(TARGET native-lib APPEND_STRING PROPERTY LINK_FLAGS " -u ANativeActivity_onCreate")

find_library(log-lib log)

target_link_libraries(native-lib native-app-glue ${log-lib} GLESv3 android EGL m)
