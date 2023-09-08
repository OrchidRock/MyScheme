#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <string>
#include <cstdio>

namespace MyScheme {

enum class ObjectType {
    THE_EMPTY_LIST, BOOLEAN, FIXNUM, 
    STRING, CHARACTER, PAIR, SYMBOL
};

class Object {
public:
    ObjectType type;
    virtual void print() = 0;
};

class boolean : public Object {
public:
    char value;
    void print() override {
        printf("#%c", (value == 0) ? 'f' : 't');
    }
};

class fixnum : public Object {
public:
    long value;
    fixnum(long val, ObjectType type = ObjectType::FIXNUM){
        this->type = type;
        this->value = val;
    }
    void print() override {
        printf("%ld", value);
    }
};

class character: public Object {
public:
    char value;
    character(char val, ObjectType type = ObjectType::CHARACTER){
        this->type = type;
        this->value = val;
    }
    void print() override {
        printf("#\\");
        switch (value) {
            case '\n':
                printf("newline");
                break;
            case ' ':
                printf("space");
                break;
            default:
                putchar(value);
        }
    }
};

class string: public Object {
public:
    std::string value;
    string(char* val, ObjectType type = ObjectType::STRING){
        this->type = type;
        this->value = val;
    }
    void print() override {
        putchar('"');
        for (int i = 0; i < value.size(); i++) {
            switch (value[i]) {
            case '\n':
                printf("\\n");
                break;
            case '\\':
                printf("\\\\");
                break;
            case '"':
                printf("\\\"");
                break;
            default:
                putchar(value[i]);
            }
        }
        putchar('"');
    }
};

class symbol: public Object {
public:
    std::string value;
    symbol(char* val, ObjectType type = ObjectType::SYMBOL){
        this->type = type;
        this->value = val;
    }
    void print() override {
        printf("%s", value.c_str());
    }
}; 

class pair: public Object {
public:
    Object* car;
    Object* cdr;
    pair(Object* car, Object *cdr, ObjectType type = ObjectType::PAIR){
        this->type = type;
        this->car = car;
        this->cdr = cdr;
    }
    void print_elements() {
        car->print();
        if (cdr->type == ObjectType::PAIR) {
            printf(" ");
            ((pair*)cdr)->print_elements();
            return;
        }else if (cdr->type == ObjectType::THE_EMPTY_LIST){
            return;
        }else{
            printf(" . ");
        }
        cdr->print();
    }
    void print() override{
        printf("(");
        if (type != ObjectType::THE_EMPTY_LIST) {
            this->print_elements();
        }
        printf(")");
    }
};


} // namespace MyScheme

#endif // __OBJECT_H__
