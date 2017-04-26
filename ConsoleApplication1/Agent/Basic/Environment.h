#pragma once

#include "jni.h"
#include "jvmti.h"

class Environment
{

	public:
		jvmtiEnv           *jvmti;
		JNIEnv			   *env;

		void setValues(jvmtiEnv *jvmtid, JNIEnv *envd) {
			jvmti = jvmtid;
			env = envd;
		}

		bool fromVM(JavaVM *vm) {

			jint	rc;
			/* Get JVMTI environment */
			rc = vm->GetEnv((void **)&jvmti, JVMTI_VERSION);
			if (rc != JNI_OK) {
				printf("jvmti error 1");
				return false;
			}

			/* Get JNI environment */

			int getEnvStat = vm->GetEnv((void **)&env, JNI_VERSION_1_6);
			if (getEnvStat == JNI_EDETACHED) {
				printf("JNI error 1");
				if (vm->AttachCurrentThread((void **)&env, NULL) != 0) {
					printf("JNI error 2");
					return false;
				}
			}
			else if (getEnvStat == JNI_OK) {
				//
			}
			else if (getEnvStat == JNI_EVERSION) {
				printf("JNI error 3");
				return false;
			}
		}
};

