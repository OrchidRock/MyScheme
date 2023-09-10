#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <string>
#include <cstdio>
#include <unordered_map>

namespace MyScheme {

enum class ObjectType {
    THE_EMPTY_LIST, BOOLEAN, FIXNUM, 
    STRING, CHARACTER, PAIR, SYMBOL, 
    PRIMITIVE_PROC, COMPOUND_PROC,
    INPUT_PORT, OUTPUT_PORT, EOF_OBJECT
};

class Object {
public:
    ObjectType type;
    virtual void print(FILE* out = stdout) = 0;
};

class boolean : public Object {
public:
    char value;
    boolean(long val, ObjectType type = ObjectType::BOOLEAN) {
        this->value = val;
        this->type = type;
    }
    void print(FILE* out = stdout) override {
        fprintf(out, "#%c", (value == 0) ? 'f' : 't');
    }
};

class fixnum : public Object {
public:
    long value;
    fixnum(long val, ObjectType type = ObjectType::FIXNUM){
        this->type = type;
        this->value = val;
    }
    void print(FILE* out = stdout) override {
        fprintf(out, "%ld", value);
    }
};

class character: public Object {
public:
    char value;
    character(char val, ObjectType type = ObjectType::CHARACTER){
        this->type = type;
        this->value = val;
    }
    void print(FILE* out = stdout) override {
        fprintf(out, "#\\");
        switch (value) {
            case '\n':
                fprintf(out, "newline");
                break;
            case ' ':
                fprintf(out, "space");
                break;
            default:
                putc(value, out);
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
    void print(FILE* out = stdout) override {
        putc('"', out);
        for (int i = 0; i < value.size(); i++) {
            switch (value[i]) {
            case '\n':
                fprintf(out, "\\n");
                break;
            case '\\':
                fprintf(out, "\\\\");
                break;
            case '"':
                fprintf(out, "\\\"");
                break;
            default:
                putc(value[i], out);
            }
        }
        putc('"', out);
    }
};

class symbol: public Object {
public:
    std::string value;
    symbol(std::string val, ObjectType type = ObjectType::SYMBOL){
        this->type = type;
        this->value = val;
    }
    void print(FILE* out = stdout) override {
        fprintf(out, "%s", value.c_str());
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
    void print_elements(FILE* out = stdout) {
        _car->print(out);
        if (_cdr->type == ObjectType::PAIR) {
            fprintf(out, " ");
            ((pair*)_cdr)->print_elements();
            return;
        }else if (_cdr->type == ObjectType::THE_EMPTY_LIST){
            return;
        }else{
            fprintf(out, " . ");
        }
        _cdr->print(out);
    }
    void print(FILE* out = stdout) override {
        fprintf(out, "(");
        if (type != ObjectType::THE_EMPTY_LIST) {
            this->print_elements(out);
        }
        fprintf(out, ")");
    }
};

class input_port : public Object {
public:
    FILE* stream;
    input_port(FILE* stream, ObjectType type = ObjectType::INPUT_PORT){
        this->stream = stream;
        this->type = type;
    }
    void print(FILE* out = stdout) override {
        fprintf(out, "#<input-port>");
    }
};

class output_port : public Object {
public:
    FILE* stream;
    output_port(FILE* stream, ObjectType type = ObjectType::OUTPUT_PORT){
        this->stream = stream;
        this->type = type;
    }
    void print(FILE* out = stdout) override {
        fprintf(out, "#<output-port>");
    }
};

class eof : public Object {
public:
    eof(ObjectType type = ObjectType::EOF_OBJECT){
        this->type = type;
    }
    void print(FILE* out = stdout) override {
        fprintf(out, "#<eof>");
    }
};



// signtone
static boolean* True;// = new boolean(1);
static boolean* False;// = new boolean(0);
static symbol* ok_symbol;
} // namespace MyScheme


#endif // __OBJECT_H__
