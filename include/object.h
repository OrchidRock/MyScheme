#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <string>
#include <cstdio>
#include <unordered_map>

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

class Frame : public Object {
private:
    std::unordered_map<symbol*, Object*> _symbol_table;
public:
    Frame(ObjectType type = ObjectType::PAIR) {
        this->type = type;
    }
    void add_binding(symbol* var, Object* val){
        _symbol_table[var] = val;
    }
    Object* lookup_variable(symbol* val) {
        std::unordered_map<symbol*, Object*>::iterator iter;
        iter = _symbol_table.find(val);
        if (iter != _symbol_table.end())
            return iter->second;
        return nullptr;
    }
    void print() override {}
};


} // namespace MyScheme

#endif // __OBJECT_H__
