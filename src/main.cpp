#include <unordered_map>
#include <string>
#include <iostream>
#include <cstdio>
#include "../include/object.h"
#include "../include/env.h"

using namespace MyScheme;

boolean* True;
boolean* False;
pair* the_empty_list;
symbol* quote_symbol;
symbol* define_symbol;
symbol* set_symbol;
symbol* ok_symbol;
std::unordered_map<std::string, symbol*> symbol_table;
pair* the_global_environment;

symbol* make_symbol(char* buffer);

void setup_environment() {
    the_global_environment = the_empty_list;
    the_global_environment = extend_environment(new Frame(), the_global_environment);
}

void init() {
    True = new boolean();
    False = new boolean();
    True->type = ObjectType::BOOLEAN;
    False->type = ObjectType::BOOLEAN;
    True->value = 1;
    False->value = 0;
    
    the_empty_list = new pair(nullptr, nullptr);
    the_empty_list->type = ObjectType::THE_EMPTY_LIST;
    
    quote_symbol = make_symbol("quote");
    define_symbol = make_symbol("define");
    set_symbol = make_symbol("set!");
    ok_symbol = make_symbol("ok");
    
    setup_environment();
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
symbol* make_symbol(char* buffer) {
    std::string key = buffer;
    std::unordered_map<std::string, symbol*>::iterator iter;
    iter = symbol_table.find(key);
    if (iter != symbol_table.end()) {
        return iter->second;        
    }
    symbol* new_symbol = new symbol(buffer);
    symbol_table[key] = new_symbol;
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

void eat_expected_string(FILE *in, char *str) {
    int c;

    while (*str != '\0') {
        c = getc(in);
        if (c != *str) {
            fprintf(stderr, "unexpected character '%c'\n", c);
            exit(1);
        }
        str++;
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
    return ((pair*)(((pair*)exp)->cdr()))->car();
}
Object* definition_value(Object* exp) {
    return ((pair*)(((pair*)(((pair*)exp)->cdr()))->cdr()))->car();
}


Object* eval(Object* exp, pair* env);

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
