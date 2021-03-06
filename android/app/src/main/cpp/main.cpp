#include <jni.h>
#include <string>

#include <android/log.h>

#include <android_native_app_glue.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <common/Application.h>
#include <common/File.h>

namespace
{
	JNIEnv* jniEnv = nullptr;

	EGLDisplay display = EGL_NO_DISPLAY;
	EGLSurface surface = EGL_NO_SURFACE;
	EGLContext context = EGL_NO_CONTEXT;

	Application* application = nullptr;

	int64_t getMicroseconds()
	{
		timeval tv;
		gettimeofday(&tv, NULL);
		return tv.tv_sec * 1000000L + tv.tv_usec;
	}

	void deinit()
	{
		if(application)
		{
			delete application;
			application = nullptr;
		}

		if(display != EGL_NO_DISPLAY)
		{
			eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			if(context != EGL_NO_CONTEXT)
			{
				eglDestroyContext(display, context);
				context = EGL_NO_CONTEXT;
			}
			if(surface != EGL_NO_SURFACE)
			{
				eglDestroySurface(display, surface);
				surface = EGL_NO_SURFACE;
			}
			eglTerminate(display);
			display = EGL_NO_DISPLAY;
		}
	}

	void init(ANativeWindow* window)
	{
		deinit();

		display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		eglInitialize(display, NULL, NULL);

		const EGLint chooseAttributes[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_DEPTH_SIZE, 24,
			EGL_NONE
		};

		EGLConfig config;
		EGLint numConfigs;
		eglChooseConfig(display, chooseAttributes, &config, 1, &numConfigs);

		EGLint format;
		eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

		ANativeWindow_setBuffersGeometry(window, 0, 0, format);

		surface = eglCreateWindowSurface(display, config, window, NULL);

		const EGLint createAttributes[] = {
			EGL_CONTEXT_CLIENT_VERSION, 3,
			EGL_NONE
		};

		context = eglCreateContext(display, config, EGL_NO_CONTEXT, createAttributes);

		eglMakeCurrent(display, surface, surface, context);

		int width, height;
		eglQuerySurface(display, surface, EGL_WIDTH, &width);
		eglQuerySurface(display, surface, EGL_HEIGHT, &height);
		__android_log_print(ANDROID_LOG_WARN, "native-activity", "surface %d x %d", width, height);

		application = createApplication();
		application->init();
	}

	void handleAppCmd(struct android_app* app, int32_t cmd)
	{
		switch(cmd)
		{
		case APP_CMD_INIT_WINDOW:
			__android_log_print(ANDROID_LOG_WARN, "init window", __PRETTY_FUNCTION__);
			init(app->window);
			break;

		case APP_CMD_TERM_WINDOW:
			__android_log_print(ANDROID_LOG_WARN, "term window", __PRETTY_FUNCTION__);
			deinit();
			break;
		}
	}

	int32_t handleInputEvent(struct android_app* app, AInputEvent* event)
	{
		if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION && AInputEvent_getSource(event) == AINPUT_SOURCE_TOUCHSCREEN)
		{
			float x = AMotionEvent_getX(event, 0);
			float y = AMotionEvent_getY(event, 0);
			switch(AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK)
			{
			case AMOTION_EVENT_ACTION_DOWN:
				application->mouseDown(x, y);
				break;

			case AMOTION_EVENT_ACTION_UP:
				application->mouseUp(x, y);
				break;

			case AMOTION_EVENT_ACTION_MOVE:
				application->mouseMove(x, y);
				break;
			}
			return 1;
		}
		return 0;
	}
}

namespace Platform
{
	void init(JNIEnv* jniEnv, ANativeActivity* activity);
}

void android_main(struct android_app* app)
{
	app->activity->vm->AttachCurrentThread(&jniEnv, 0);

	Platform::init(jniEnv, app->activity);
	File::init(app->activity->assetManager);

	app->onAppCmd = handleAppCmd;
	app->onInputEvent = handleInputEvent;

	int64_t startTime = getMicroseconds();
	int64_t lastUpdateTime = startTime;

	while(true)
	{
		int events;
		android_poll_source* source;
		while(ALooper_pollAll(0, nullptr, &events, (void**)&source) >= 0)
		{
			if(source)
			{
				source->process(app, source);
			}
		}

		if(app->destroyRequested)
		{
			__android_log_print(ANDROID_LOG_WARN, "destroy requested", __PRETTY_FUNCTION__);
			break;
		}

		if(display && surface && context && application)
		{
			int64_t time = getMicroseconds();

			int width = ANativeWindow_getWidth(app->window);
			int height = ANativeWindow_getHeight(app->window);

			application->update((float)width, (float)height, (time - startTime) * (1.0 / 1000000.0), (time - lastUpdateTime) * (1.0 / 1000000.0));
			application->render(width, height);

			lastUpdateTime = time;

			eglSwapBuffers(display, surface);
		}
	}

	app->activity->vm->DetachCurrentThread();

	deinit();
}
