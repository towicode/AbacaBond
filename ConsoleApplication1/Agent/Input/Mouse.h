#pragma once

#include <jni.h>
#include "Normal.h"
#include "BezierCurve.h"
#include "../Basic/Environment.h"
#include "../Basic/LoadClasses.h"

#include <string>
#include <Windows.h>

using boost::asio::ip::tcp;
namespace pt = boost::posix_time;

//	Mouse Stuff.
jfieldID canvas_field;	// Game Client Field
jobject local_canvas;	// Game Client Object
jmethodID constructor;	// Constructor for mouse event
jmethodID getMouseListeners;	// get Mouse Listeners
jmethodID getMouseMotionListeners;	// get Mouse motion Listeners
jobjectArray mouse_array;	//	Object of mouse listeners
jobjectArray mouseMotion_array;	//	Object of mouse listeners

jobject mouse_proper;	//	first mouse listener
jobject mouse_motion_proper; //first mouse motion listener
jmethodID create_event;	// Press method ID
jmethodID move_event; // move method ID


Normal targetDistribution;
Normal midpointDistribution;

int mousex = 0;
int mousey = 0;





/// <summary>
/// Clicks the mouse.
/// </summary>
/// <param name="agent">The agent.</param>
/// <returns></returns>
static std::string clickMouse(Environment agent) {

	jboolean fal = false;
	jint button = 1;
	jint zero = 0;

	jobject my_event;
	jint x = mousex;
	jint y = mousey;

	pt::ptime time_t_epoch(boost::gregorian::date(1970, 1, 1));
	pt::ptime now = pt::microsec_clock::local_time();
	pt::time_duration diff = now - time_t_epoch;
	long whenwhen = diff.total_milliseconds();
	jlong when = whenwhen;

	printf("CREATING MOUSE CLICK AT %d,%d\n", mousex, mousey);

	my_event = agent.env->NewObject(MouseEvent, constructor, local_canvas, button, when, zero, x, y, button, fal);
	if (!my_event) {
		std::string message = "ERROR couldn't create the Mouse Event.\n";
		return message;
	}
	agent.env->CallVoidMethod(mouse_proper, create_event, my_event);

}

/// <summary>
/// Sends the mouse movement.
///	should only ever be used to move 1 from current position
///	additonally updates the current location.
/// </summary>
/// <param name="xx">The xx.</param>
/// <param name="yy">The yy.</param>
/// <param name="agent">The agent.</param>
/// <returns></returns>
static std::string sendMouseMovement(int xx, int yy, Environment agent) {
	jboolean fal = false;
	jint button = 1;
	jint zero = 0;

	jobject my_event;
	jint x = xx;
	jint y = yy;
	mousex = xx;
	mousey = yy;

	pt::ptime time_t_epoch(boost::gregorian::date(1970, 1, 1));
	pt::ptime now = pt::microsec_clock::local_time();
	pt::time_duration diff = now - time_t_epoch;
	long whenwhen = diff.total_milliseconds();
	jlong when = whenwhen;

	printf("CREATING MOUSE EVENT AT %d,%d\n", xx, yy);

	my_event = agent.env->NewObject(MouseEvent, constructor, local_canvas, button, when, zero, x, y, button, fal);
	if (!my_event) {
		std::string message = "ERROR couldn't create the Mouse Event.\n";
		return message;
	}
	agent.env->CallVoidMethod(mouse_motion_proper, move_event, my_event);
}

/// <summary>
/// Moves the mouse.
/// </summary>
/// <param name="x">The x.</param>
/// <param name="y">The y.</param>
/// <param name="agent">The agent.</param>
void MoveMouse(int x, int y, Environment agent) {
	//Accuracy to desired pixel
	int Acc = 3;

	int targetX = x + (int)(Acc * targetDistribution.NextGaussian());
	int targetY = y + (int)(Acc * targetDistribution.NextGaussian());

	//Declare the original pointer position
	int originalX = mousex;
	int originalY = mousey;

	//Find a mid point between original and target position
	int midPointX = (x - targetX) / 2;
	int midPointY = (y - targetY) / 2;

	//Points normal to the straight line between start and end point
	int factor = 6;
	int bezierMidPointX = (int)((midPointX / factor) * (midpointDistribution.NextGaussian()));
	int bezierMidPointY = (int)((midPointY / factor) * (midpointDistribution.NextGaussian()));

	BezierCurve bc;
	double input[] = { originalX, originalY, bezierMidPointX, bezierMidPointY, targetX, targetY };

	//Change numberOfPoints for more or less control points
	const int numberOfPoints = 500;

	//Don't change numberOfDataPoints
	const int numberOfDataPoints = numberOfPoints * 2;
	double output[numberOfDataPoints];

	//control points are couplets of two so / 2
	bc.Bezier2D(input, numberOfDataPoints / 2, output);


	//std::vector<POINT> points;
	POINT A;

	for (int count = 1; count != numberOfDataPoints - 1; count += 2)
	{
		A.x = (int)output[count + 1];
		A.y = (int)output[count];

		//points.push_back(A);

		sendMouseMovement(A.x, A.y, agent);
		//clickMouse(jvmti, env);

		////Testing - sleep for a vaule between 0-20
		Sleep(20);
		////

	}

	//return points;
}





/// <summary>
/// Setups the mouse.
/// Gets the needed classes
/// </summary>
/// <param name="agent">The agent.</param>
/// <returns></returns>
static std::string setupMouse(Environment agent) {

	canvas_field = agent.env->GetStaticFieldID(Client, localCanvasField, CanvasObf);
	if (!canvas_field) {
		std::string messages = "ERROR no canvas field\n";
		return messages;
	}

	local_canvas = agent.env->GetStaticObjectField(Client, canvas_field);
	if (!local_canvas) {
		std::string message = "ERROR couldn't grab local canvas\n";
		return message;
	}

	constructor;
	constructor = agent.env->GetMethodID(MouseEvent, "<init>", "(Ljava/awt/Component;IJIIIIZ)V");
	if (!constructor) {
		std::string message = "ERROR couldn't grab constructor for mouse event\n";
		return message;
	}

	getMouseListeners = agent.env->GetMethodID(Canvas, "getMouseListeners", "()[Ljava/awt/event/MouseListener;");
	if (!getMouseListeners) {
		std::string message = "ERROR couldn't get Mouse Listener method\n";
		return message;
	}

	getMouseMotionListeners = agent.env->GetMethodID(Canvas, "getMouseMotionListeners", "()[Ljava/awt/event/MouseMotionListener;");
	if (!getMouseMotionListeners) {
		std::string message = "ERROR couldn't get Mouse Motion listner method\n";
		return message;
	}

	mouse_array = (jobjectArray)agent.env->CallObjectMethod(local_canvas, getMouseListeners);
	if (!mouse_array) {
		std::string message = "ERROR Get array of mouse listeners\n";
		return message;
	}

	mouseMotion_array = (jobjectArray)agent.env->CallObjectMethod(local_canvas, getMouseMotionListeners);
	if (!mouseMotion_array) {
		std::string message = "ERROR Get array of mouse motion listeners\n";
		return message;
	}

	mouse_proper = agent.env->GetObjectArrayElement(mouse_array, 0);
	if (!mouse_proper) {
		std::string message = "ERROR Get first object of mouse array.\n";
		return message;
	}

	mouse_motion_proper = agent.env->GetObjectArrayElement(mouseMotion_array, 0);
	if (!mouse_motion_proper) {
		std::string message = "ERROR Get first object of mouse motion array.\n";
		return message;
	}

	create_event = agent.env->GetMethodID(MouseListener, "mousePressed", "(Ljava/awt/event/MouseEvent;)V");
	if (!create_event) {
		std::string message = "ERROR Get first object of mouse array.\n";
		return message;
	}

	move_event = agent.env->GetMethodID(MouseMotionListener, "mouseMoved", "(Ljava/awt/event/MouseEvent;)V");
	if (!create_event) {
		std::string message = "ERROR Get first object of mouse array.\n";
		return message;
	}


	return "We setup mouse fine";
}