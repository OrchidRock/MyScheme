#include <unordered_map>
#include <string>
#include <iostream>
#include <cstdio>
#include "object.h"
#include "env.h"
#include "procedure.h"

using namespace MyScheme;

pair* the_empty_list;
symbol* quote_symbol;
symbol* define_symbol;
symbol* set_symbol;
symbol* if_symbol;
symbol* lambda_symbol;
std::unordered_map<std::string, symbol*> symbol_table;
pair* the_global_environment;

symbol* make_symbol(std::string buffer);
Object* eval(Object* exp, pair* env);

void setup_environment() {
    the_global_environment = the_empty_list;
    the_global_environment = extend_environment(new Frame(), the_global_environment);
}


void destory() {
    delete True;
    delete False;
    delete the_empty_list;
}

Object *cons(Object *car, Object *cdr) {
    pair *obj = new pair(car, cdr);
    return obj;
}

char is_pair(Object *obj) {
    return obj->type == ObjectType::PAIR;
}
char is_false(Object* obj) {
    return obj == False;
}
char is_true(Object* obj) {
    return !is_false(obj);
}

/*
#define caar(obj)   car(car(obj))
#define cadr(obj)   car(cdr(obj))
#define cdar(obj)   cdr(car(obj))
#define cddr(obj)   cdr(cdr(obj))
#define caaar(obj)  car(car(car(obj)))
#define caadr(obj)  car(car(cdr(obj)))
#define cadar(obj)  car(cdr(car(obj)))
#define caddr(obj)  car(cdr(cdr(obj)))
#define cdaar(obj)  cdr(car(car(obj)))
#define cdadr(obj)  cdr(car(cdr(obj)))
#define cddar(obj)  cdr(cdr(car(obj)))
#define cdddr(obj)  cdr(cdr(cdr(obj)))
#define caaaar(obj) car(car(car(car(obj))))
#define caaadr(obj) car(car(car(cdr(obj))))
#define caadar(obj) car(car(cdr(car(obj))))
#define caaddr(obj) car(car(cdr(cdr(obj))))
#define cadaar(obj) car(cdr(car(car(obj))))
#define cadadr(obj) car(cdr(car(cdr(obj))))
#define caddar(obj) car(cdr(cdr(car(obj))))
#define cadddr(obj) car(cdr(cdr(cdr(obj))))
#define cdaaar(obj) cdr(car(car(car(obj))))
#define cdaadr(obj) cdr(car(car(cdr(obj))))
#define cdadar(obj) cdr(car(cdr(car(obj))))
#define cdaddr(obj) cdr(car(cdr(cdr(obj))))
#define cddaar(obj) cdr(cdr(car(car(obj))))
#define cddadr(obj) cdr(cdr(car(cdr(obj))))
#define cdddar(obj) cdr(cdr(cdr(car(obj))))
#define cddddr(obj) cdr(cdr(cdr(cdr(obj))))
*/
symbol* make_symbol(std::string buffer) {
    std::unordered_map<std::string, symbol*>::iterator iter;
    iter = symbol_table.find(buffer);
    if (iter != symbol_table.end()) {
        return iter->second;        
    }
    symbol* new_symbol = new symbol(buffer);
    symbol_table[buffer] = new_symbol;
    return new_symbol; 
}


char is_delimiter(int c) {
    return isspace(c) || c == EOF ||
           c == '('   || c == ')' ||
           c == '"'   || c == ';';
}

char is_initial(int c) {
    return isalpha(c) || c == '*' || c == '/' || c == '>' ||
             c == '<' || c == '=' || c == '?' || c == '!';
}

int peek(FILE *in) {
    int c;

    c = getc(in);
    ungetc(c, in);
    return c;
}

void eat_whitespace(FILE *in) {
    int c;
    
    while ((c = getc(in)) != EOF) {
        if (isspace(c)) {
            continue;
        }
        else if (c == ';') { /* comments are whitespace also */
            while (((c = getc(in)) != EOF) && (c != '\n'));
            continue;
        }
        ungetc(c, in);
        break;
    }
}

void eat_expected_string(FILE *in, std::string str) {
    int c;

    for(int i = 0; i < str.size(); i++) {
        c = getc(in);
        if (c != str[i]) {
            fprintf(stderr, "unexpected character '%c'\n", c);
            exit(1);
        }
    }
}

void peek_expected_delimiter(FILE *in) {
    if (!is_delimiter(peek(in))) {
        fprintf(stderr, "character not followed by delimiter\n");
        exit(1);
    }
}

Object *read_character(FILE *in) {
    int c;

    c = getc(in);
    switch (c) {
        case EOF:
            fprintf(stderr, "incomplete character literal\n");
            exit(1);
        case 's':
            if (peek(in) == 'p') {
                eat_expected_string(in, "pace");
                peek_expected_delimiter(in);
                return new character(' ');
            }
            break;
        case 'n':
            if (peek(in) == 'e') {
                eat_expected_string(in, "ewline");
                peek_expected_delimiter(in);
                return new character('\n');
            }
            break;
    }
    peek_expected_delimiter(in);
    return new character(c);
}
Object* read(FILE* in);

Object *read_pair(FILE *in) {
    int c;
    Object *car_obj;
    Object *cdr_obj;
    
    eat_whitespace(in);
    
    c = getc(in);
    if (c == ')') { /* read the empty list */
        return the_empty_list;
    }
    ungetc(c, in);

    car_obj = read(in);

    eat_whitespace(in);
    
    c = getc(in);    
    if (c == '.') { /* read improper list */
        c = peek(in);
        if (!is_delimiter(c)) {
            fprintf(stderr, "dot not followed by delimiter\n");
            exit(1);
        }
        cdr_obj = read(in);
        eat_whitespace(in);
        c = getc(in);
        if (c != ')') {
            fprintf(stderr,
                    "where was the trailing right paren?\n");
            exit(1);
        }
        return cons(car_obj, cdr_obj);
    }
    else { /* read list */
        ungetc(c, in);
        cdr_obj = read_pair(in);        
        return cons(car_obj, cdr_obj);
    }
}

Object *read(FILE *in) {
    int c;
    short sign = 1;
    int i;
    long num = 0;
#define BUFFER_MAX 1000
    char buffer[BUFFER_MAX];

    eat_whitespace(in);

    c = getc(in);    

    if (c == '#') { /* read a boolean or character */
        c = getc(in);
        switch (c) {
            case 't':
                return True;
            case 'f':
                return False;
            case '\\':
                return read_character(in);
            default:
                fprintf(stderr,
                        "unknown boolean or character literal\n");
                exit(1);
        }
    }
    else if (isdigit(c) || (c == '-' && (isdigit(peek(in))))) {
        /* read a fixnum */
        if (c == '-') {
            sign = -1;
        }
        else {
            ungetc(c, in);
        }
        while (isdigit(c = getc(in))) {
            num = (num * 10) + (c - '0');
        }
        num *= sign;
        if (is_delimiter(c)) {
            ungetc(c, in);
            return new fixnum(num);
        }
        else {
            fprintf(stderr, "number not followed by delimiter\n");
            exit(1);
        }
    }
    else if (is_initial(c) ||
             ((c == '+' || c == '-') &&
              is_delimiter(peek(in)))) { /* read a symbol */
        i = 0;
        while (is_initial(c) || isdigit(c) ||
               c == '+' || c == '-') {
            /* subtract 1 to save space for '\0' terminator */
            if (i < BUFFER_MAX - 1) {
                buffer[i++] = c;
            }
            else {
                fprintf(stderr, "symbol too long. "
                        "Maximum length is %d\n", BUFFER_MAX);
                exit(1);
            }
            c = getc(in);
        }
        if (is_delimiter(c)) {
            buffer[i] = '\0';
            ungetc(c, in);
            return make_symbol(buffer);
        }
        else {
            fprintf(stderr, "symbol not followed by delimiter. "
                            "Found '%c'\n", c);
            exit(1);
        }
    }
    else if (c == '"') { /* read a string */
        i = 0;
        while ((c = getc(in)) != '"') {
            if (c == '\\') {
                c = getc(in);
                if (c == 'n') {
                    c = '\n';
                }
            }
            if (c == EOF) {
                fprintf(stderr, "non-terminated string literal\n");
                exit(1);
            }
            /* subtract 1 to save space for '\0' terminator */
            if (i < BUFFER_MAX - 1) {
                buffer[i++] = c;
            }
            else {
                fprintf(stderr, 
                        "string too long. Maximum length is %d\n",
                        BUFFER_MAX);
                exit(1);
            }
        }
        buffer[i] = '\0';
        return new string(buffer);
    }
    else if (c == '(') { /* read the empty list or pair. */
        return read_pair(in);
    }
    else if (c == '\'') { /* read quoted expression. */
        return cons(quote_symbol, cons(read(in), the_empty_list));
    }
    else {
        fprintf(stderr, "bad input. Unexpected '%c'\n", c);
        exit(1);
    }
    fprintf(stderr, "read illegal state\n");
    exit(1);
}

/* ############################# EVAL ###################### */
bool is_self_evaluating(Object* exp) {
    return exp->type == ObjectType::BOOLEAN ||
            exp->type == ObjectType::CHARACTER || 
            exp->type == ObjectType::FIXNUM ||
            exp->type == ObjectType::STRING;
}
bool is_variable(Object* exp) {
    return exp->type == ObjectType::SYMBOL;
}
bool is_tagged_list(Object* exp, symbol* tag) {
    Object* the_car;
    if (exp->type == ObjectType::PAIR) {
        the_car = ((pair*)exp)->car();
        return the_car->type == ObjectType::SYMBOL && 
                ((symbol*)the_car) == tag;
    }
    return false;
}

bool is_quoted(Object* exp) {
    return is_tagged_list(exp, quote_symbol);
}

Object* text_of_quotation(Object* exp) {
    return ((pair*)(((pair*)exp)->cdr()))->car();
}
bool is_assignment(Object* exp) {
    return is_tagged_list(exp, set_symbol);
}
Object* assignment_variable(Object* exp) {
    return ((pair*)(((pair*)exp)->cdr()))->car();
}
Object* assignment_value(Object* exp) {
    return ((pair*)(((pair*)(((pair*)exp)->cdr()))->cdr()))->car();
}
bool is_definition(Object* exp) {
    return is_tagged_list(exp, define_symbol);
}
Object* definition_variable(Object* exp) {
    Object* cadr = ((pair*)(((pair*)exp)->cdr()))->car();
    if (cadr->type == ObjectType::SYMBOL) {
        return cadr;
    }else{ // pair
        return ((pair*)cadr)->car();
    }
}

Object* make_lambda(Object*, Object*);

Object* definition_value(Object* exp) {
    Object* cddr = ((pair*)(((pair*)exp)->cdr()))->cdr();
    Object* cadr = ((pair*)(((pair*)exp)->cdr()))->car();
    if (cadr->type == ObjectType::SYMBOL) {
        return ((pair*)cddr)->car();
    }else {
        return make_lambda(((pair*)cadr)->cdr(), cddr);
    }
}

Object* make_lambda(Object* parameters, Object* body) {
    return cons(lambda_symbol, cons(parameters, body));
}

bool is_lambda(Object* exp) {
    return is_tagged_list(exp, lambda_symbol);
}

Object* lambda_parameters(Object* exp) {
    return ((pair*)(((pair*)exp)->cdr()))->car();
}
Object* lambda_body(Object* exp) {
    return ((pair*)(((pair*)exp)->cdr()))->cdr();
}

bool is_last_exp(Object* seq) {
    return ((pair*)seq)->cdr()->type == ObjectType::THE_EMPTY_LIST;
}
Object* first_exp(Object* seq) {
    return ((pair*)seq)->car();
}
Object* rest_exps(Object* seq) {
    return ((pair*)seq)->cdr();
}

bool is_if(Object* exp) {
    return is_tagged_list(exp, if_symbol);
}
Object* if_predicate(Object* exp) {
    return ((pair*)(((pair*)exp)->cdr()))->car();
}
Object* if_consequent(Object* exp) {
    return ((pair*)(((pair*)(((pair*)exp)->cdr()))->cdr()))->car();
}
Object* if_alternative(Object* exp) {
    Object* obj = ((pair*)(((pair*)(((pair*)exp)->cdr()))->cdr()))->cdr();
    if (obj->type == ObjectType::THE_EMPTY_LIST) return False;
    return ((pair*)obj)->car();
}

char is_application(Object* exp) {
    return is_pair(exp);
}
Object* proc_operator(Object* exp) {
    return ((pair*)exp)->car();
}
Object* proc_operands(Object* exp) {
    return ((pair*)exp)->cdr();
}
Object* first_operands(Object* ops) {
    return ((pair*)ops)->car();
}
Object* rest_operands(Object* ops) {
    return ((pair*)ops)->cdr();
}

Object* list_of_values(Object* exp, pair* env) {
    if (exp->type == ObjectType::THE_EMPTY_LIST) {
        return the_empty_list;
    }
    else {
        return cons(eval(first_operands(exp), env),
                        list_of_values(rest_operands(exp), env));
    }
}

Object* eval_assignment(Object* exp, pair* env) {
    if(!set_variable_value((symbol*)assignment_variable(exp),
                        eval(assignment_value(exp), env), env)) {
        fprintf(stderr, "unbound variable\n");
        exit(1);
    }
    return ok_symbol;
}

void define_variable(symbol* var, Object* val, pair* env) {
    Frame* f = first_frame(env);
    f->add_binding(var, val);
}

Object* eval_definition(Object* exp, pair* env) {
    define_variable((symbol*)definition_variable(exp),
                    eval(definition_value(exp), env), env);
    return ok_symbol; 
}

Object* eval(Object* exp, pair* env) {
    Object* procedure;
    Object* arguments;
tailcall:
    if (is_self_evaluating(exp))
        return exp;
    else if (is_variable(exp)) {
        Object* obj = lookup_variable_value(static_cast<symbol*>(exp), env);
        if (obj == nullptr) {
            fprintf(stderr, "Unbounded variable\n");
            exit(1);
        }else
            return obj;
    }
    else if (is_quoted(exp))
        return text_of_quotation(exp);
    else if (is_assignment(exp)) {
        return eval_assignment(exp, env);        
    }
    else if (is_definition(exp)) {
        return eval_definition(exp, env);        
    }
    else if (is_if(exp)) {
        exp = is_true(eval(if_predicate(exp), env)) ?
                if_consequent(exp) :
                if_alternative(exp);
        goto tailcall;
    }
    else if (is_lambda(exp)) {
        return new compound_proc(lambda_parameters(exp),
                                 lambda_body(exp),
                                 env);
    }
    else if (is_application(exp)) {
        procedure = eval(proc_operator(exp), env);
        arguments = list_of_values(proc_operands(exp), env);
        if (procedure->type == ObjectType::PRIMITIVE_PROC){
            return ((Procedure*)procedure)->proc(arguments);
        }else if(procedure->type == ObjectType::COMPOUND_PROC) {
            compound_proc* cp = (compound_proc*)procedure;
            env = extend_environment(new Frame(cp->parameters, arguments), cp->env);
            exp = cp->body;
            while(!is_last_exp(exp)) {
                eval(first_exp(exp), env);
                exp = rest_exps(exp);
            }
            exp = first_exp(exp);
            goto tailcall;
        }else {
            return exp;
        }
    }
    else {
        fprintf(stderr, "cannot eval unknown expression type\n");
        exit(1);
    }
    fprintf(stderr, "eval illegal state\n");
    exit(1);
}

void write(Object *obj) {
    if (obj == nullptr) return;
    obj->print();    
}

void init() {
    True = new boolean(1);
    False = new boolean(0);
    
    the_empty_list = new pair(nullptr, nullptr);
    the_empty_list->type = ObjectType::THE_EMPTY_LIST;
    
    quote_symbol = make_symbol("quote");
    define_symbol = make_symbol("define");
    set_symbol = make_symbol("set!");
    ok_symbol = make_symbol("ok");
    if_symbol = make_symbol("if");    
    lambda_symbol = make_symbol("lambda");

    setup_environment();
#define add_procedure(scheme_name, obj)         \
    define_variable(make_symbol(scheme_name),   \
                    obj,                        \
                    the_global_environment);
    add_procedure("null?", new primitive_proc_is_null());
    add_procedure("boolean?", new primitive_proc_is_boolean());
    add_procedure("symbol?", new primitive_proc_is_symbol());
    add_procedure("integer?", new primitive_proc_is_integer());
    add_procedure("char?", new primitive_proc_is_char());
    add_procedure("string?", new primitive_proc_is_string());
    add_procedure("pair?", new primitive_proc_is_pair());
    add_procedure("procedure?", new primitive_proc_is_proc());
    
    add_procedure("char->integer" , new primitive_proc_char_to_int());
    add_procedure("integer->char" , new primitive_proc_int_to_char());
    add_procedure("number->string", new primitive_proc_num_to_string());
    add_procedure("string->number", new primitive_proc_string_to_num());
    add_procedure("symbol->string", new primitive_proc_symbol_to_string());
    add_procedure("string->symbol", new primitive_proc_string_to_symbol());
      
    add_procedure("+", new primitive_proc_add());
    add_procedure("-"        , new primitive_proc_sub());
    add_procedure("*"        , new primitive_proc_mul());
    add_procedure("quotient" , new primitive_proc_quotient());
    add_procedure("remainder", new primitive_proc_remainder());
    add_procedure("="        , new primitive_proc_is_number_equ());
    add_procedure("<"        , new primitive_proc_is_less_than());
    add_procedure(">"        , new primitive_proc_is_greater_than());

    add_procedure("cons"    , new primitive_proc_cons());
    add_procedure("car"     , new primitive_proc_car());
    add_procedure("cdr"     , new primitive_proc_cdr());
    add_procedure("set-car!", new primitive_proc_set_car());
    add_procedure("set-cdr!", new primitive_proc_set_cdr());
    add_procedure("list"    , new primitive_proc_list());

    add_procedure("eq?"    , new primitive_proc_is_equal());

}

int main(int argc, char *argv[])
{
    std::cout << "Welcome to MyScheme. Use Ctrl-C to exit.\n";
    
    init();

    while  (true) {
        std::cout << "> ";
        write(eval(read(stdin), the_global_environment));
        std::cout << "\n";
    }

    destory();
       
    return 0;
}
