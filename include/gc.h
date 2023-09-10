#ifndef __GC_H__
#define __GC_H__

#include <list>
#include "object.h"

using namespace MyScheme;

static std::list<Object*> gc_root_table; // for gc

void* GC_malloc(size_t);

#endif
