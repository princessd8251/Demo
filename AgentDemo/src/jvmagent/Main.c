/*
 * catch the single step action in each thread
 */

#include <jvmti.h>
#include <string.h>
#include <stdlib.h>

static const int CALL_CHAIN_LENGTH = 1;

static void 
check_jvmti_error(jvmtiEnv *jvmti, jvmtiError error, const char *msg) {
    if (error != JVMTI_ERROR_NONE) {
        char *name = NULL;
        (*jvmti)->GetErrorName(jvmti, error, &name);
        fprintf(
                stderr, \
                "ERROR: JVMTI: %d(%s): %s\n", \
                error, \
                (name == NULL ? "Unknown" : name), \
                (msg == NULL ? "" : msg));
    }
}


void JNICALL
callbackSingleStep(
    jvmtiEnv *jvmti, 
    JNIEnv* jni, 
    jthread thread,
    jmethodID method,
    jlocation location) {

    jvmtiThreadInfo thread_info;
    int depth;
    jclass declaring_class;
    jvmtiError error;

    error = (*jvmti)->GetThreadInfo(jvmti, thread, &thread_info);
    check_jvmti_error(jvmti, error, "Couldn't get thread info\n");

    for (depth = 0; depth < CALL_CHAIN_LENGTH; depth++) {
        char *method_name, *method_signature, *class_signature;
        char **const SKIP_GENERIC = NULL;
        
        jmethodID method;
        jlocation location, s_location, e_location;
        error = (*jvmti)->GetFrameLocation( \
                jvmti, thread, depth, &method, &location);
        if (error == JVMTI_ERROR_NO_MORE_FRAMES) {
            /* printf("ROOT\n"); */
            break;
        }
        check_jvmti_error(jvmti, error, "ERROR\tCouldn't get frame location\n");

        error = (*jvmti)->GetMethodLocation( \
                jvmti, method, &s_location, &e_location);
        
        error = (*jvmti)->GetMethodName( \
                jvmti, method, &method_name, &method_signature, SKIP_GENERIC);
        check_jvmti_error(jvmti, error, "ERROR\tCouldn't get method name\n");
        
        error = (*jvmti)->GetMethodDeclaringClass( \
                jvmti, method, &declaring_class);
        check_jvmti_error( \
                jvmti, error, "ERROR\tCouldn't get declaring class\n");
        
        error = (*jvmti)->GetClassSignature( \ 
                jvmti, declaring_class, &class_signature, SKIP_GENERIC);
        check_jvmti_error( \
                jvmti, error, "ERROR\tCouldn't get class signature\n");

        if (strncmp(method_name, "run", 4) == 0   && \
            strstr(class_signature, "PossibleReordering") != NULL) {
            printf("%s\t", thread_info.name);
            printf("%s\t", class_signature);
            printf("%lld\t", location);
            printf("%s %lld:%lld\t", method_name, s_location, e_location);

            printf("\n");
        }

        (*jni)->DeleteLocalRef(jni, declaring_class);
        error = (*jvmti)->Deallocate(jvmti, (unsigned char*) method_name);
        check_jvmti_error(jvmti, error, "ERROR\tCouldn't deallocate memory\n");

        error = (*jvmti)->Deallocate(jvmti, (unsigned char*) method_signature);
        check_jvmti_error(jvmti, error, "ERROR\tCouldn't deallocate memory\n");

        error = (*jvmti)->Deallocate(jvmti, (unsigned char*) class_signature);
        check_jvmti_error(jvmti, error, "ERROR\tCouldn't deallocate memory\n");
    }
    error = (*jvmti)->Deallocate(jvmti, (unsigned char*)thread_info.name);
    (*jni)->DeleteLocalRef(jni, thread_info.thread_group);
    (*jni)->DeleteLocalRef(jni, thread_info.context_class_loader);
}


JNIEXPORT jint JNICALL 
Agent_OnLoad(JavaVM *jvm, char *options, void *reserved) {
    jvmtiEnv *jvmti = NULL;
    jvmtiCapabilities capa;
    jvmtiError error;
    jvmtiEventCallbacks callbacks;

    /* get env */
    error = (*jvm)->GetEnv(jvm, (void **)&jvmti, JVMTI_VERSION);
    if (error != JNI_OK) {
        fprintf(stderr, "Couldn't get JVMTI environment");
        return JNI_ERR;
    }

    /* add capabilities */
    memset(&capa, 0, sizeof(jvmtiCapabilities));
    capa.can_generate_single_step_events = 1;
    error = (*jvmti)->AddCapabilities(jvmti, &capa);
    check_jvmti_error(jvmti, error, \
            "Unable to get necessary JVMTI capabilities.");

    /* set events */
    error = (*jvmti)->SetEventNotificationMode \
            (jvmti, JVMTI_ENABLE, JVMTI_EVENT_SINGLE_STEP, NULL);
    check_jvmti_error(jvmti, error, "Cannot set event notification");



    /* add callbacks */
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.SingleStep = &callbackSingleStep;
    error = (*jvmti)->SetEventCallbacks \
            (jvmti, &callbacks, (jint)sizeof(callbacks)); 
    check_jvmti_error(jvmti, error, "Canot set jvmti callbacks");
    return JNI_OK;
}


JNIEXPORT void JNICALL
Agent_OnUnload(JavaVM *vm)
{
}
