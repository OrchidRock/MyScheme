#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include "../include/object.h"

namespace MyScheme {
class expression : public pair {
public:
    symbol* tag;
    virtual Object* eval() = 0;
};

class if_exp : public expression {
public:
    Object* eval() override {
        
    }
};

class define_exp: public expression {
public:
    Object* eval() override {
        
    }

};

class set_exp : public expression {
public:
    Object* eval() override {
        
    }

};
}


#endif // __EXPRESSION_H__
