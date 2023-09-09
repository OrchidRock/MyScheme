#ifndef __ENV_H__
#define __ENV_H__
#include "object.h"

namespace MyScheme {
class Frame : public Object {
private:
    std::unordered_map<symbol*, Object*> _symbol_table;
public:
    Frame(ObjectType type = ObjectType::PAIR) {
        this->type = type;
    }
    Frame(Object* vars, Object* vals, ObjectType type = ObjectType::PAIR) {
        this->type = type;
        while (vars->type != ObjectType::THE_EMPTY_LIST &&
                vals->type != ObjectType::THE_EMPTY_LIST){
            symbol* var = (symbol*)((pair*)vars)->car();
            Object* val = ((pair*)vals)->car();
            
            _symbol_table[var] = val;
            
            vars = ((pair*)vars)->cdr();
            vals = ((pair*)vals)->cdr();
        } 
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
    void print() override {
        printf("#frame<unordered_map>");
    }
};
}

using  namespace MyScheme;
void setup_environment(pair*);
pair* extend_environment(Frame*, pair*);
pair* enclosing_environment(pair*);
Frame* first_frame(pair*);
bool set_variable_value(symbol*, Object*, pair*);
Object* lookup_variable_value(symbol*, pair*);


#endif // __ENV_H__
