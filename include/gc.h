#ifndef __GC_H__
#define __GC_H__

#include <list>
#include "object.h"

using namespace MyScheme;


void* GC_malloc(size_t);
void GC_push_root(Object* root);
void GC_pop_root();

#endif
