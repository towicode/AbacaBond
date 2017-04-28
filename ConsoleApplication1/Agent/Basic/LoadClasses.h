#pragma once

#include "jni.h"
#include "jvmti.h"
#include "Environment.h"
#include "obfinfo.h"

#include <string>



jclass PlayerHolder;
jclass CanvasHolder;
jclass Character;
jclass CharacterArray;
jclass Canvas;
jclass MouseEvent;
jclass MouseListener;
jclass MouseMotionListener;
jclass Client;









/// <summary>
/// Loads the classes.
///	Filles out a number of jclass objects
/// </summary>
/// <param name="agent">The agent.</param>
void loadClasses(Environment agent) {

	printf("Loading Classes\n");
	jint classCount;
	jclass *classes;
	jvmtiError err;
	agent.jvmti->GetLoadedClasses(&classCount, &classes);

	int expected_classes = 8;

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
				expected_classes--;
				Client = cls;
			}

			if (strstr(signature, PlayerHolderObf) != NULL) {
				printf("Found PlayerHolderObf Class\n");
				expected_classes--;
				PlayerHolder = cls;
			}

			if (strstr(signature, CanvasHolderObf) != NULL) {
				printf("Found CanvasHolder Class\n");
				expected_classes--;
				CanvasHolder = cls;
			}

			if (strstr(signature, CharacterObf) != NULL) {

				if (strstr(signature, "[") != NULL) {
					continue;
				}
				printf("Found Character Class\n");
				expected_classes--;
				Character = cls;
			}

			if (strstr(signature, CanvasObf) != NULL) {
				printf("Found Canvas Class\n");
				expected_classes--;
				Canvas = cls;
			}

			if (strstr(signature, MouseEventObf) != NULL) {
				printf("Found MouseEvent Class\n");
				expected_classes--;
				MouseEvent = cls;
			}

			if (strstr(signature, MouseListenerObf) != NULL) {
				printf("Found MouseListener Class\n");
				expected_classes--;
				MouseListener = cls;
			}

			if (strstr(signature, MouseMotionListenerObf) != NULL) {
				printf("Found MouseMotionListener Class\n");
				expected_classes--;
				MouseMotionListener = cls;
			}

		}
		else {
			fprintf(stderr, "Couldn't check class :(\n");
		}
	}

	if (expected_classes != 0) {
		printf("One more more classes not found");
	}
}
