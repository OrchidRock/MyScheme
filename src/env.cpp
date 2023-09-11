#include <cstdio>
#include "env.h"
#include "object.h"

using namespace MyScheme;

pair* enclosing_environment(pair* env) {
    return (pair*)(env->cdr());
}

Frame* first_frame(pair* env) {
    return (Frame*)(env->car());
}


Object* lookup_variable_value(symbol* var, pair* env) {
    Object* obj = nullptr;
    while (env->type != ObjectType::THE_EMPTY_LIST) {
        Frame* frame = first_frame(env);
        obj = frame->lookup_variable(var);
        if (obj != nullptr) return obj;

        env = enclosing_environment(env);
    }
    //fprintf(stderr, "unbounded variable\n");
    //exit(1);
    return obj;
}

bool set_variable_value(symbol* var, Object* val, pair* env) {
    Object* obj = nullptr;
    while (env->type != ObjectType::THE_EMPTY_LIST) {
        Frame* frame = first_frame(env);
        obj = frame->lookup_variable(var);
        if (obj != nullptr) {
            frame->add_binding(var, val);
            return true;
        }

        env = enclosing_environment(env);
    }
    return false;
}

