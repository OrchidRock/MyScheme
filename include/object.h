#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <string>
#include <cstdio>
#include <unordered_map>

namespace MyScheme {

enum class ObjectType {
    THE_EMPTY_LIST, BOOLEAN, FIXNUM, 
    STRING, CHARACTER, PAIR, SYMBOL, 
    PRIMITIVE_PROC, COMPOUND_PROC
};

class Object {
public:
    ObjectType type;
    virtual void print() = 0;
};

class boolean : public Object {
public:
    char value;
    boolean(long val, ObjectType type = ObjectType::BOOLEAN) {
        this->value = val;
        this->type = type;
    }
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
    string(std::string val, ObjectType type = ObjectType::STRING){
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
    symbol(std::string val, ObjectType type = ObjectType::SYMBOL){
        this->type = type;
        this->value = val;
    }
    void print() override {
        printf("%s", value.c_str());
    }
}; 

class pair: public Object {
private:
    Object* _car;
    Object* _cdr;
public:
    pair(Object* car, Object *cdr, ObjectType type = ObjectType::PAIR){
        this->type = type;
        this->_car = car;
        this->_cdr = cdr;
    }
    Object* car() {
        return _car;
    }
    Object* cdr() {
        return _cdr;
    }
    void set_car(Object* val) {
        _car =  val;
    }
    void set_cdr(Object* val) {
        _cdr = val;
    }
    void print_elements() {
        _car->print();
        if (_cdr->type == ObjectType::PAIR) {
            printf(" ");
            ((pair*)_cdr)->print_elements();
            return;
        }else if (_cdr->type == ObjectType::THE_EMPTY_LIST){
            return;
        }else{
            printf(" . ");
        }
        _cdr->print();
    }
    void print() override{
        printf("(");
        if (type != ObjectType::THE_EMPTY_LIST) {
            this->print_elements();
        }
        printf(")");
    }
};

// signtone
static boolean* True;// = new boolean(1);
static boolean* False;// = new boolean(0);
static symbol* ok_symbol;
} // namespace MyScheme


#endif // __OBJECT_H__
