#include <stdlib.h>
#include <list>
#include "gc.h"
#include "object.h"
#include "env.h"

using namespace MyScheme;

std::list<Object*> active_list;

static void _mark(Object* root) {
    if (root == nullptr) return;
    root->mark = 1;
    if (root->type == ObjectType::PAIR) {
        if (dynamic_cast<Frame*>(root)){ // Frame
            ((Frame*)root)->frame_map([](Object* obj){
                        obj->mark = 1;
                    });
        }else{
            _mark(((pair*)root)->car());
            _mark(((pair*)root)->cdr());
        }
    }
}


static void _garbage_collection() {
    fprintf(stderr, "gcing\n");
    for(Object* root : gc_root_table) {
        _mark(root); 
    }

    // sweep
    std::list<Object*>::iterator iter = active_list.begin();
    while(iter != active_list.end()) {
        if ((*iter)->mark == 0) {
            delete (*iter);
            iter = active_list.erase(iter);
        }else if ((*iter)->mark == 1) {
            (*iter)->mark = 0;
        }
    }
}

void* GC_malloc(size_t len){
    if (active_list.size() > 500) {
        _garbage_collection();
    }
    void* obj = malloc(len);
    active_list.push_back((Object*)obj);
    return obj;
}

