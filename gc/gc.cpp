#include <stdlib.h>
#include <list>
#include "gc.h"
#include "object.h"
#include "env.h"

using namespace MyScheme;

std::list<Object*> active_list;
std::list<Object*> gc_root_table; // for gc
int limit = 640;

static void _mark(Object* root) {
    if (root == nullptr) return;
    root->mark = 1;
    printf("GC mark object: ");
    root->print();
    printf(" 0x%llx \n", (long long)root);

    if (root->type == ObjectType::PAIR) {
        if (dynamic_cast<Frame*>(root)){ // Frame
            ((Frame*)root)->frame_map([](Object* obj){
                        obj->mark = 1;
                        printf("GC mark object: ");
                        printf("#<pair> ");
                        printf(" 0x%llx \n", (long long)obj);
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
        Object* obj  = *iter;

        if (obj->mark == 0) {
            printf("delete garbage: ");
            obj->print();
            printf(" 0x%llx \n", (long long)obj);
            delete obj;
            iter = active_list.erase(iter);
        }else if (obj->mark == 1) {
            obj->mark = 0;
        }
        iter++;
    }
}

void GC_push_root(Object* root) {
    printf("GC push root: ");
    root->print();
    printf(" %llx \n", root);
    gc_root_table.push_back(root);
}

void  GC_pop_root() {
    printf("GC pop root: ");
    Object* root = gc_root_table.back();
    root->print();
    printf(" %llx \n", root);
    gc_root_table.pop_back();
}

void* GC_malloc(size_t len){
    if (active_list.size() > limit) {
        _garbage_collection();
        if (active_list.size() > limit * 0.8) {
            limit *= 2;
        } else if (active_list.size() < limit * 0.4 && limit >= 1280) {
            limit /= 2;
        }
    }
    

    void* obj = malloc(len);
    active_list.push_back((Object*)obj);
    
    return obj;
}

