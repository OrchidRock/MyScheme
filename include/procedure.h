#ifndef __PROCEDURE_H__
#define __PROCEDURE_H__
#include <cstdio>
#include <cstdlib>
#include "object.h"

namespace  MyScheme {
class Procedure : public Object {
public:
    Procedure(ObjectType type = ObjectType::PRIMITIVE_PROC) {
        this->type = type;
    }
    virtual Object* proc(Object* args) {
        return args;
    }
    void print() override {
        printf("#<procedure>"); 
    }
};

class compound_proc : public Procedure {
public:
    Object* parameters;
    Object* body;
    pair* env;
    
    compound_proc(Object* paras, Object* body, pair* env,
                    ObjectType type = ObjectType::COMPOUND_PROC) {
        this->type = type;
        this->parameters = paras;
        this->body = body;
        this->env = env;
    }
    Object* proc(Object* args) {
        return args;
    } 
};

class primitive_proc_add : public Procedure {
public:
    Object* proc(Object* args){
        pair* arguments = (pair*)args;
        long result =  0;
        while(arguments->type != ObjectType::THE_EMPTY_LIST) {
            if (arguments->type == ObjectType::FIXNUM){
                result += ((fixnum*)arguments)->value;
                break;
            }else if (arguments->type != ObjectType::PAIR){
                break;
            }
            if (arguments->car()->type != ObjectType::FIXNUM) {
                printf("invalid argument");
                arguments->car()->print();
                printf("\n");
                break;
            }
            result += ((fixnum*)(arguments->car()))->value;
            arguments = (pair*)(arguments->cdr());
        }
        return new fixnum(result);
    }
};
class primitive_proc_sub : public Procedure {
public:
    Object* proc(Object* args){
        pair* arguments = (pair*)args;
        long result = ((fixnum*)(arguments->car()))->value;
        arguments = (pair*)(arguments->cdr());
        while(arguments->type != ObjectType::THE_EMPTY_LIST) {
            if (arguments->type == ObjectType::FIXNUM){
                result -= ((fixnum*)arguments)->value;
                break;
            }else if (arguments->type != ObjectType::PAIR){
                break;
            }
            if (arguments->car()->type != ObjectType::FIXNUM) {
                printf("invalid argument");
                arguments->car()->print();
                printf("\n");
                break;
            }
            result -= ((fixnum*)(arguments->car()))->value;
            arguments = (pair*)(arguments->cdr());
        }
        return new fixnum(result);
    }
};
class primitive_proc_mul : public Procedure {
public:
    Object* proc(Object* args){
        pair* arguments = (pair*)args;
        long result =  1;
        while(arguments->type != ObjectType::THE_EMPTY_LIST) {
            if (arguments->type == ObjectType::FIXNUM){
                result *= ((fixnum*)arguments)->value;
                break;
            }else if (arguments->type != ObjectType::PAIR){
                break;
            }
            if (arguments->car()->type != ObjectType::FIXNUM) {
                printf("invalid argument");
                arguments->car()->print();
                printf("\n");
                break;
            }
            result *= ((fixnum*)(arguments->car()))->value;
            arguments = (pair*)(arguments->cdr());
        }
        return new fixnum(result);
    }
};
class primitive_proc_quotient : public Procedure {
public:
    Object* proc(Object* args){
        pair* arguments = (pair*)args;
        long result = ((fixnum*)(arguments->car()))->value;
        pair* p  = (pair*)(arguments->cdr());
        long tmp = ((fixnum*)(p->car()))->value;
        result = result / tmp;
        return new fixnum(result);
    }
};
class primitive_proc_remainder : public Procedure {
public:
    Object* proc(Object* args){
        pair* arguments = (pair*)args;
        long result = ((fixnum*)(arguments->car()))->value;
        pair* p  = (pair*)(arguments->cdr());
        long tmp = ((fixnum*)(p->car()))->value;
        result = result % tmp;
        return new fixnum(result);
    }
};
class primitive_proc_is_number_equ : public Procedure {
public:
    Object* proc(Object* args){
        pair* arguments = (pair*)args;
        long result = ((fixnum*)(arguments->car()))->value;
        arguments = (pair*)(arguments->cdr());
        while(arguments->type != ObjectType::THE_EMPTY_LIST) {
            if (arguments->car()->type != ObjectType::FIXNUM) {
                printf("invalid argument");
                arguments->car()->print();
                printf("\n");
                break;
            }
            if (result != ((fixnum*)(arguments->car()))->value) {
                return False;
            }
            arguments = (pair*)(arguments->cdr());
        }
        return True;
    }
};

class primitive_proc_is_less_than : public Procedure {
public:
    Object* proc(Object* args){
        pair* arguments = (pair*)args;
        long previous, next;
        previous = ((fixnum*)(arguments->car()))->value;
        arguments = (pair*)(arguments->cdr());
        while(arguments->type != ObjectType::THE_EMPTY_LIST) {
            if (arguments->car()->type != ObjectType::FIXNUM) {
                printf("invalid argument");
                arguments->car()->print();
                printf("\n");
                break;
            }
            next = ((fixnum*)(arguments->car()))->value;
            if (previous < next) {
                previous = next;
            }else{
                return False;
            }
            arguments = (pair*)(arguments->cdr());
        }
        return True;
    }
};
class primitive_proc_is_greater_than : public Procedure {
public:
    Object* proc(Object* args){
        pair* arguments = (pair*)args;
        long previous, next;
        previous = ((fixnum*)(arguments->car()))->value;
        arguments = (pair*)(arguments->cdr());
        while(arguments->type != ObjectType::THE_EMPTY_LIST) {
            if (arguments->car()->type != ObjectType::FIXNUM) {
                printf("invalid argument");
                arguments->car()->print();
                printf("\n");
                break;
            }
            next = ((fixnum*)(arguments->car()))->value;
            if (previous > next) {
                previous = next;
            }else{
                return False;
            }
            arguments = (pair*)(arguments->cdr());
        }
        return True;
    }
};
class primitive_proc_cons : public Procedure {
public:
    Object* proc(Object* args) {
        pair* arguments = (pair*)args;
        return new pair(arguments->car(), ((pair*)(arguments->cdr()))->car());
    } 
};
class primitive_proc_car : public Procedure {
public:
    Object* proc(Object* args) {
        pair* arguments = (pair*)args;
        return ((pair*)(arguments->car()))->car();
    } 
};
class primitive_proc_cdr : public Procedure {
public:
    Object* proc(Object* args) {
        pair* arguments = (pair*)args;
        return ((pair*)(arguments->car()))->cdr();
    } 
};
class primitive_proc_set_car : public Procedure {
public:
    Object* proc(Object* args) {
        pair* arguments = (pair*)args;
        Object* obj = arguments->car();
        if(obj->type != ObjectType::PAIR) {
            printf("error: the target object isn't a pair.");
            return False;
        }
        Object* new_car = ((pair*)(arguments->cdr()))->car();
        ((pair*)obj)->set_car(new_car);
        return ok_symbol;
    } 
};
class primitive_proc_set_cdr : public Procedure {
public:
    Object* proc(Object* args) {
        pair* arguments = (pair*)args;
        Object* obj = arguments->car();
        if(obj->type != ObjectType::PAIR) {
            printf("error: the target object isn't a pair.");
            return False;
        }
        Object* new_car = ((pair*)(arguments->cdr()))->car();
        ((pair*)obj)->set_cdr(new_car);
        return ok_symbol;
    } 
};
class primitive_proc_list : public Procedure {
public:
    Object* proc(Object* args) {
        return args;
    } 
};

class primitive_proc_is_equal : public Procedure {
public:
    Object* proc(Object *args) {
        Object *obj1;
        Object *obj2;
    
        obj1 = ((pair*)args)->car();
        obj2 = ((pair*)(((pair*)args)->cdr()))->car();
    
        if (obj1->type != obj2->type) {
            return False;
        }
        switch (obj1->type) {
        case ObjectType::FIXNUM:
            return (((fixnum*)obj1)->value == 
                    ((fixnum*)obj2)->value) ?
                        True : False;
            break;
        case ObjectType::CHARACTER:
            return (((character*)obj1)->value == 
                    ((character*)obj2)->value) ?
                        True : False;
            break;
        case ObjectType::STRING:
            return (((string*)obj1)->value == 
                    ((string*)obj2)->value) ?
                        True : False;
            break;
        default:
            return (obj1 == obj2) ? True : False;
        }
    }
};

class primitive_proc_apply : public Procedure {
public:
    Object* proc(Object *args) {
        fprintf(stderr, "illegal state: The body of the apply" 
                        "primitive procedure should not execute.\n");
        return nullptr;
    }
};
class primitive_proc_eval : public Procedure {
public:
    Object* proc(Object *args) {
        fprintf(stderr, "illegal state: The body of the eval" 
                        "primitive procedure should not execute.\n");
        return nullptr;
    }
};
            
class primitive_proc_is_null : public Procedure {
public:
    Object* proc(Object* args) {
        return (((pair*)args)->car()->type == ObjectType::THE_EMPTY_LIST) ? True : False;
    }
};
class primitive_proc_is_boolean : public Procedure {
public:
    Object* proc(Object* args) {
        return (((pair*)args)->car()->type == ObjectType::BOOLEAN) ? True : False;
    }
};

class primitive_proc_is_symbol : public Procedure {
public:
    Object* proc(Object* args) {
        return (((pair*)args)->car()->type == ObjectType::SYMBOL) ? True : False;
    }
};

class primitive_proc_is_integer : public Procedure {
public:
    Object* proc(Object* args) {
        return (((pair*)args)->car()->type == ObjectType::FIXNUM) ? True : False;
    }
};
class primitive_proc_is_char : public Procedure {
public:
    Object* proc(Object* args) {
        return (((pair*)args)->car()->type == ObjectType::CHARACTER) ? True : False;
    }
};
class primitive_proc_is_string : public Procedure {
public:
    Object* proc(Object* args) {
        return (((pair*)args)->car()->type == ObjectType::STRING) ? True : False;
    }
};
class primitive_proc_is_pair : public Procedure {
public:
    Object* proc(Object* args) {
        return (((pair*)args)->car()->type == ObjectType::PAIR) ? True : False;
    }
};
class primitive_proc_is_proc : public Procedure {
public:
    Object* proc(Object* args) {
        ObjectType type = ((pair*)args)->car()->type;
        return (type == ObjectType::PRIMITIVE_PROC || type == ObjectType::COMPOUND_PROC) 
                ? True : False;
    }
};
class primitive_proc_char_to_int : public Procedure {
public:
    Object* proc(Object* args) {
        Object* obj = ((pair*)args)->car();
        return new fixnum(((character*)obj)->value); 
    }
};
class primitive_proc_int_to_char : public Procedure {
public:
    Object* proc(Object* args) {
        Object* obj = ((pair*)args)->car();
        return new character(((fixnum*)obj)->value); 
    }
};
class primitive_proc_num_to_string : public Procedure {
public:
    Object* proc(Object* args) {
        Object* obj = ((pair*)args)->car();
        long value = ((fixnum*)obj)->value;
        return new string(std::to_string(value)); 
    }
};
class primitive_proc_string_to_num : public Procedure {
public:
    Object* proc(Object* args) {
        Object* obj = ((pair*)args)->car();
        return new fixnum(std::stoi(((string*)obj)->value));
    }
};
class primitive_proc_symbol_to_string : public Procedure {
public:
    Object* proc(Object* args) {
        Object* obj = ((pair*)args)->car();
        return new string(((symbol*)obj)->value);
    }
};
class primitive_proc_string_to_symbol : public Procedure {
public:
    Object* proc(Object* args);
};

class primitive_proc_interaction_env : public Procedure {
public:
    Object* proc(Object* args);
};

class primitive_proc_null_env : public Procedure {
public:
    Object* proc(Object* args);
};
class primitive_proc_envirnoment : public Procedure {
public:
    Object* proc(Object* args);
};

}

#endif // __PROCEDURE_H__
