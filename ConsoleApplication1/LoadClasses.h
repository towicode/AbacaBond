#pragma once

#include "jni.h"
#include "jvmti.h"
#include "Environment.h"

#include <string>



jclass Client;
jclass Character;
jclass Canvas;
jclass MouseEvent;
jclass MouseListener;
jclass MouseMotionListener;


char* ClientObf = "Lav;";
char* CharacterObf = "Lt;";
char* CanvasObf = "Ljava/awt/Canvas;";
char* MouseListenerObf = "Ljava/awt/event/MouseListener;";
char* MouseMotionListenerObf = "Ljava/awt/event/MouseMotionListener;";
char* MouseEventObf = "Ljava/awt/event/MouseEvent;";
char* ComponentObf = "Ljava/awt/Component;";



//	used to access localPlayer
char* localCharField = "hl";
char* localCanvasField = "qc";

//	Player fields
char * player_name = "f";



/// <summary>
/// Loads the classes.
///	Filles out a number of jclass objects
/// </summary>
/// <param name="agent">The agent.</param>
void loadClasses(Environment agent) {

	printf("Loading Classes");
	jint classCount;
	jclass *classes;
	jvmtiError err;
	agent.jvmti->GetLoadedClasses(&classCount, &classes);

	for (int i = 0; i < classCount; i++) {
		jclass cls = *(classes + i);
		char *signature;
		char *generic;
		err = agent.jvmti->GetClassSignature(cls, &signature, &generic);
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

			if (strstr(signature, MouseMotionListenerObf) != NULL) {
				printf("Found MouseMotionListener Class\n");
				MouseMotionListener = cls;
			}

		}
		else {
			fprintf(stderr, "Couldn't check class :(\n");
		}
	}
}
