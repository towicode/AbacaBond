#pragma once

#include "jni.h"
#include "jvmti.h"
#include "Environment.h"
#include "obfinfo.h"
#include <unordered_map>

#include <string>



jclass PlayerHolder;
jclass CanvasHolder;
jclass Character;
jclass Canvas;
jclass MouseEvent;
jclass MouseListener;
jclass MouseMotionListener;
jclass Client;


jclass localPlayerHolder;
jclass canvasHolder;


void attemptLoad(int *cur, jclass *fill, std::string name, std::unordered_map<std::string, jclass> *mymap) {
	try {
		*fill = mymap -> at(MouseEventObf);
	}
	catch (...) {
		
	}
	*cur--;
}


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


	std::unordered_map<std::string, jclass> mymap;
	std::pair<std::string, jclass> myshopping("baking powder", Client);
	mymap.insert(myshopping);


	for (int i = 0; i < classCount; i++) {
		jclass cls = *(classes + i);
		char *signature;
		char *generic;
		err = agent.jvmti->GetClassSignature(cls, &signature, &generic);
		if (err == JVMTI_ERROR_NONE) {
			//printf("THE dog: %s \n", signature);
			// Just be simple and look for our classes...

			std::pair<std::string, jclass> classmatch(signature, cls);
			mymap.insert(classmatch);
			
		}
		else {
			fprintf(stderr, "Couldn't check class :(\n");
		}
	}
	int expected_classes = 8;
	attemptLoad(&expected_classes, &MouseEvent, MouseEventObf, &mymap);
	attemptLoad(&expected_classes, &MouseMotionListener, MouseListenerObf, &mymap);
	attemptLoad(&expected_classes, &Canvas, CanvasObf, &mymap);
	attemptLoad(&expected_classes, &MouseListener, MouseListenerObf, &mymap);
	attemptLoad(&expected_classes, &localPlayerHolder, localPlayerHolderObf, &mymap);
	attemptLoad(&expected_classes, &canvasHolder, canvasHolderObf, &mymap);
	attemptLoad(&expected_classes, &MouseEvent, MouseEventObf, &mymap);
	attemptLoad(&expected_classes, &MouseEvent, MouseEventObf, &mymap);
	attemptLoad(&expected_classes, &MouseEvent, MouseEventObf, &mymap);




}
