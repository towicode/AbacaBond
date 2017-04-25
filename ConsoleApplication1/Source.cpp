
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <windows.h>

#pragma warning(disable:4996)

#include "jni.h"
#include "jvmti.h"

#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <Windows.h>

using boost::asio::ip::tcp;
namespace pt = boost::posix_time;

unsigned short const PORT = 19876;

jclass Client;
jclass Character;
jclass Canvas;
jclass MouseEvent;
jclass MouseListener;


char* ClientObf = "Lav;";
char* CharacterObf = "Lt;";
char* CanvasObf = "Ljava/awt/Canvas;";
char* MouseListenerObf = "Ljava/awt/event/MouseListener;";
char* MouseEventObf = "Ljava/awt/event/MouseEvent;";
char* ComponentObf = "Ljava/awt/Component;";




//	used to access localPlayer
char* localCharField = "hl";
char* localCanvasField = "qc";

//	Player fields
char * player_name = "f";

static std::string moveMouse(jvmtiEnv *jvmti, JNIEnv *env) {

	jfieldID canvas_field = env->GetStaticFieldID(Client, localCanvasField, CanvasObf);
	if (!canvas_field) {
		std::string messages = "ERROR no canvas field\n";
		return messages;
	}

	jobject local_canvas = env->GetStaticObjectField(Client, canvas_field);
	if (!local_canvas) {
		std::string message = "ERROR couldn't grab local canvas\n";
		return message;
	}

	jmethodID constructor;
	constructor = env->GetMethodID(MouseEvent, "<init>", "(Ljava/awt/Component;IJIIIIZ)V");
	if (!constructor) {
		std::string message = "ERROR couldn't grab constructor for mouse event\n";
		return message;
	}

	jboolean fal = false;
	jint button = 1;
	jint zero = 0;
	pt::ptime time_t_epoch(boost::gregorian::date(1970, 1, 1));
	pt::ptime now = pt::microsec_clock::local_time();
	pt::time_duration diff = now - time_t_epoch;
	long x = diff.total_milliseconds();
	jlong when = x;

	jmethodID getMouseListeners = env->GetMethodID(Canvas, "getMouseListeners", "()[Ljava/awt/event/MouseListener;");
	if (!getMouseListeners) {
		std::string message = "ERROR couldn't get Mouse Listener method\n";
		return message;
	}

	jobjectArray mouse_array = (jobjectArray) env->CallObjectMethod(local_canvas, getMouseListeners);
	if (!mouse_array) {
		std::string message = "ERROR Get array of mouse listeners\n";
		return message;
	}

	jobject mouse_proper = env->GetObjectArrayElement(mouse_array, 0);
	if (!mouse_proper) {
		std::string message = "ERROR Get first object of mouse array.\n";
		return message;
	}

	jmethodID create_event = env->GetMethodID(MouseListener, "mousePressed", "(Ljava/awt/event/MouseEvent;)V");
	if (!create_event) {
		std::string message = "ERROR Get first object of mouse array.\n";
		return message;
	}
	for (int i = 0; i < 200; i++) {
		jobject my_event;
		jint x = i;
		jint y = i;

		printf("CREATING MOUSE EVENT AT %d,%d\n", i, i);

		my_event = env->NewObject(MouseEvent, constructor, local_canvas, button, when, zero, x, y, button, fal);
		if (!my_event) {
			std::string message = "ERROR couldn't create the Mouse Event.\n";
			return message;
		}

		Sleep(200);

		env->CallVoidMethod(mouse_proper, create_event, my_event);

	}

	return "True";
}


static std::string getMyRSPlayer(jvmtiEnv *jvmti, JNIEnv *env) {

	jfieldID character_field = env->GetStaticFieldID(Client, localCharField, CharacterObf);
	if (!character_field) {
		std::string messages = "ERROR no localPlayer field\n";
		return messages;
	}

	jobject local_player = env->GetStaticObjectField(Client, character_field);
	if (!local_player) {
		std::string message = "ERROR couldn't grab local player\n";
		return message;
	}

	jfieldID fid = env->GetFieldID(Character, player_name, "Ljava/lang/String;");
	if (fid == 0) {
		std::string message = "ERROR couldn't grab local player\n";
		return message;
	}

	jstring jstr;
	jstr = (jstring) env->GetObjectField(local_player, fid);
	const char *str;

	str = env->GetStringUTFChars(jstr, 0);

	std::string message = str;
	return message;

}


static std::string lookup(int command, jvmtiEnv *jvmti, JNIEnv *env) {


	switch (command) {
	case 1:
		return getMyRSPlayer(jvmti, env);

	case 2:
		return moveMouse(jvmti, env);
	}

}



/* Callback for JVMTI_EVENT_VM_INIT */
static void JNICALL
vm_init(jvmtiEnv *jvmti, JNIEnv *env, jthread thread)
{

}

/* Agent_OnLoad() is called first, we prepare for a VM_INIT event here. */
JNIEXPORT jint JNICALL
Agent_OnLoad(JavaVM *vm, char *options, void *reserved)
{

	
	jint                rc;
	jvmtiEnv           *jvmti;
	JNIEnv			   *jni;
	printf("Attempting To Load\n");


	/* Get JVMTI environment */
	rc = vm->GetEnv((void **)&jvmti, JVMTI_VERSION);
	if (rc != JNI_OK) {
		fprintf(stderr, "ERROR: Unable to create jvmtiEnv, GetEnv failed, error=%d\n", rc);
		return -1;
	}

	/* Get JNI environment */

	int getEnvStat = vm->GetEnv((void **)&jni, JNI_VERSION_1_6);
	if (getEnvStat == JNI_EDETACHED) {
		std::cout << "GetEnv: not attached" << std::endl;
		if (vm->AttachCurrentThread((void **)&jni, NULL) != 0) {
			std::cout << "Failed to attach" << std::endl;
		}
	}
	else if (getEnvStat == JNI_OK) {
		//
	}
	else if (getEnvStat == JNI_EVERSION) {
		std::cout << "GetEnv: version not supported" << std::endl;
	}


	printf("Loading Classes");
	jint classCount;
	jclass *classes;
	jvmtiError err;
	jvmti->GetLoadedClasses(&classCount, &classes);

	for (int i = 0; i < classCount; i++) {
		jclass cls = *(classes + i);
		char *signature;
		char *generic;
		err = jvmti->GetClassSignature(cls, &signature, &generic);
		if (err == JVMTI_ERROR_NONE) {
			//printf("THE dog: %s \n", signature);
			// Just be simple and look for our classes...
			if (strstr(signature, ClientObf) != NULL) {
				printf("Found Client Class\n");
				Client = cls;
			}

			if (strstr(signature, CharacterObf) != NULL) {
				printf("Found Character Class\n");
				Character = cls;
			}

			if (strstr(signature, CanvasObf) != NULL) {
				printf("Found Canvas Class\n");
				Canvas = cls;
			}

			if (strstr(signature, MouseEventObf) != NULL) {
				printf("Found MouseEvent Class\n");
				MouseEvent = cls;
			}

			if (strstr(signature, MouseListenerObf) != NULL) {
				printf("Found MouseListener Class\n");
				MouseListener = cls;
			}
			
		}
		else {
			fprintf(stderr, "Couldn't check class :(\n");
		}
	}

	try
	{
		boost::asio::io_service ioService;
		tcp::acceptor acceptor(ioService, tcp::endpoint(tcp::v4(), PORT));

		while (true)
		{
			// Listen for clients
			std::cout << "Listening for client..." << std::endl;
			tcp::socket socket(ioService);
			acceptor.accept(socket);
			std::cout << "Client heard..." << std::endl;

			size_t len;

			// Receive string from client and print it out
			boost::array<char, 128> cBuf;
			len = socket.read_some(boost::asio::buffer(cBuf, sizeof(cBuf)));
			std::cout.write(cBuf.data(), len);

			// Send string to client
			//std::string message = "Server string to send to client\n";
			//boost::asio::write(socket, boost::asio::buffer(message));
			//lookup(1, jvmti, jni);
			std::string message = lookup(2, jvmti, jni);
			boost::asio::write(socket, boost::asio::buffer(message));

		}
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

JNIEXPORT jint JNICALL
Agent_OnAttach(JavaVM *vm, char *options, void *reserved)
{	
	setbuf(stdout, NULL);


	return Agent_OnLoad(vm, options, reserved);
}



/* Agent_OnUnload() is called last */
JNIEXPORT void JNICALL
Agent_OnUnload(JavaVM *vm)
{
}


/*

#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

int main()
{
	unsigned short const PORT = 19876;

	try
	{
		boost::asio::io_service ioService;
		tcp::acceptor acceptor(ioService, tcp::endpoint(tcp::v4(), PORT));

		while (true)
		{
			// Listen for clients
			std::cout << "Listening for client..." << std::endl;
			tcp::socket socket(ioService);
			acceptor.accept(socket);
			std::cout << "Client heard..." << std::endl;

			size_t len;

			// Receive string from client and print it out
			boost::array<char, 128> cBuf;
			len = socket.read_some(boost::asio::buffer(cBuf, sizeof(cBuf)));
			std::cout.write(cBuf.data(), len);

			// Send string to client
			std::string message = "Server string to send to client\n";
			boost::asio::write(socket, boost::asio::buffer(message));

		}
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

*/