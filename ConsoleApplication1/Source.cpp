#pragma warning(disable:4996)

#include <boost/asio.hpp>
#include <boost/array.hpp>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <jvmti.h>

#include "agent/Basic/Environment.h"
#include "agent/Basic/LoadClasses.h"
#include "agent/Input/mouse.h"


#include "agent/Input/BezierCurve.h"
#include "agent/Input/Normal.h"
#include "agent/Basic/Environment.h"

#include "Players.h";

#include <iostream>
#include <string>


#include <Windows.h>



using boost::asio::ip::tcp;
namespace pt = boost::posix_time;

unsigned short const PORT = 19876;

Environment agent;

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


static std::string lookup(int command, Environment agent) {


	switch (command) {
	case 1:
		//return "abc";
		return getPlayers(agent);

	case 2:
		//return moveMouse(jvmti, env);
		return nullptr;
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
	printf("Attempting To Load\n");

	
	agent.fromVM(vm);
	loadClasses(agent);
	//setupMouse(agent);

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
			std::string message = lookup(1, agent);
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
	//Weird error where we need to not buffer prints for debugging.
	setbuf(stdout, NULL);
	return Agent_OnLoad(vm, options, reserved);
}



/* Agent_OnUnload() is called last */
JNIEXPORT void JNICALL
Agent_OnUnload(JavaVM *vm)
{
}