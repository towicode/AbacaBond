
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

using boost::asio::ip::tcp;

unsigned short const PORT = 19876;

jclass Client;
jclass Character;

char* ClientObf = "Lav;";
char* CharacterObf = "Lt;";

//	used to access localPlayer
char* localCharField = "hl";

//	Player fields
char * player_name = "f";


static std::string getMyRSPlayer(jvmtiEnv *jvmti, JNIEnv *env) {

	jfieldID character_field = env->GetStaticFieldID(Client, localCharField, ClientObf);
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
			printf("THE dog: %s \n", signature);
			// Just be simple and look for our classes...
			if (strstr(signature, ClientObf) != NULL) {
				printf("Found Client Class\n");
				Client = cls;
			}

			if (strstr(signature, CharacterObf) != NULL) {
				printf("Found Character Class\n");
				Character = cls;
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
			std::string message = lookup(1, jvmti, jni);
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