#ifndef __ENV_H__
#define __ENV_H__
#include "../include/object.h"

using  namespace MyScheme;


void setup_environment(pair*);
pair* extend_environment(Frame*, pair*);
pair* enclosing_environment(pair*);
Frame* first_frame(pair*);
bool set_variable_value(symbol*, Object*, pair*);
Object* lookup_variable_value(symbol*, pair*);


#endif // __ENV_H__
