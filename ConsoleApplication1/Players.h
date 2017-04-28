#pragma once
#include <string>
#include <jni.h>
#include "obfinfo.h"
#include "Agent\Basic\LoadClasses.h"
#include "Agent\Basic\Environment.h"


jfieldID PlayersArrayField;
jfieldID PlayerNameField;

static void load(Environment agent);



bool loaded = false;

static jobjectArray getPlayersArray(Environment agent) {

	jobjectArray PlayersArray = (jobjectArray)agent.env->GetStaticObjectField(Client, PlayersArrayField);
	if (!PlayersArray) {
		printf("ERROR couldn't grab local PlayersArray\n");
		return nullptr;
	}

	return PlayersArray;
}

static std::string serializePlayer(jobject player, Environment agent) {
	jstring jstr;
	jstr = (jstring)agent.env->GetObjectField(player, PlayerNameField);

	if (!jstr) {
		printf("Failed to get jstring of name\n");
		return "Failed to get jstring of name\n";
	}
	const char *str;

	str = agent.env->GetStringUTFChars(jstr, 0);

	std::string message = str;
	return message;

}

static std::string getPlayers(Environment agent) {

	if (!loaded)
		load(agent);

	jobjectArray myArray = getPlayersArray(agent);
	if (!myArray) {
		printf("Error getting the array of players\n");
		return nullptr;
	}
	jsize len = agent.env->GetArrayLength(myArray);
	if (!len) {
		printf("Error getting the length of players\n");
		return nullptr;
	}

	printf("Length of players %d\n", len);

	std::string playerSerialization = "";

	for (int i = 0; i < len; i++) {
		jobject player = agent.env->GetObjectArrayElement(myArray, i);
		if (!player) {
			continue;
		}
		playerSerialization += serializePlayer(player, agent) + ";;";
	}

	return playerSerialization;


}





static void load(Environment agent) {

	PlayersArrayField = agent.env->GetStaticFieldID(Client, playersAccessorObf, "[Lt;");
	if (!PlayersArrayField) {
		printf("Could not find players array\n");
		return;
	}

	PlayerNameField = agent.env->GetFieldID(Character, player_name, "Ljava/lang/String;");
	if (PlayerNameField == 0) {
		printf("Error getting player name field\n");
		return;
	}


	loaded = true;

}


