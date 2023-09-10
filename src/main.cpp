#include <unordered_map>
#include <string>
#include <iostream>
#include <cstdio>
#include "object.h"
#include "env.h"
#include "procedure.h"
#include "gc.h"

using namespace MyScheme;

pair* the_empty_list;
symbol* quote_symbol;
symbol* define_symbol;
symbol* set_symbol;
symbol* if_symbol;
symbol* lambda_symbol;
symbol* begin_symbol;
symbol* cond_symbol;
symbol* else_symbol;
symbol* let_symbol;
symbol* and_symbol;
symbol* or_symbol;
eof* eof_object;
std::unordered_map<std::string, symbol*> symbol_table;
pair* the_global_environment;

symbol* make_symbol(std::string buffer);
Object* eval(Object* exp, pair* env);
void define_variable(symbol* var, Object* val, pair* env);
pair* setup_environment();
pair* make_environment();
Object* read(FILE*);
int peek(FILE *in);
void write(FILE *out, Object *obj);
Object *cons(Object *car, Object *cdr);

Object* primitive_proc_string_to_symbol::proc(Object* exp) {
    return make_symbol(((string*)(((pair*)exp)->car()))->value);
}
Object* primitive_proc_null_env::proc(Object* exp) {
    return setup_environment();
}
Object* primitive_proc_interaction_env::proc(Object* exp) {
    return the_global_environment;
}
Object* primitive_proc_envirnoment::proc(Object* exp) {
    return make_environment();
}

Object* primitive_proc_cons::proc(Object* args) {
    pair* arguments = (pair*)args;
    return cons(arguments->car(), ((pair*)(arguments->cdr()))->car());
} 

// IO
Object* primitive_proc_load::proc(Object* arguments) {
    const char *filename;
    FILE *in;
    Object *exp;
    Object *result;
    
    Object* first_argu = ((pair*)arguments)->car();
    if (first_argu->type != ObjectType::STRING){
        return nullptr;
    }
    filename = ((string*)first_argu)->value.c_str();
    
    in = fopen(filename, "r");
    if (in == NULL) {
        fprintf(stderr, "could not load file \"%s\"", filename);
        exit(1);
    }
    while ((exp = read(in)) != NULL) {
        result = eval(exp, the_global_environment);
    }
    fclose(in);
    return result;

}

Object* primitive_proc_open_input_port::proc(Object *arguments) {
    const char *filename;
    FILE *in;

    Object* first_argu = ((pair*)arguments)->car();
    if (first_argu->type != ObjectType::STRING){
        return eof_object;
    }
    filename = ((string*)first_argu)->value.c_str();
    in = fopen(filename, "r");
    if (in == NULL) {
        fprintf(stderr, "could not open file \"%s\"\n", filename);
        exit(1);
    }
    return new input_port(in);
}

Object * primitive_proc_close_input_port::proc(Object *arguments) {
    int result;
    Object* first_argu = ((pair*)arguments)->car();
    if (first_argu->type != ObjectType::INPUT_PORT){
        return eof_object;
    }
    result = fclose(((input_port*)first_argu)->stream);
    if (result == EOF) {
        fprintf(stderr, "could not close input port\n");
        exit(1);
    }
    return ok_symbol;
}

Object * primitive_proc_is_input_port::proc(Object *arguments) {
    Object* first_argu = ((pair*)arguments)->car();
    return (first_argu->type == ObjectType::INPUT_PORT) ? True: False;
}

Object *primitive_proc_read::proc(Object *arguments) {
    FILE *in;
    Object *result;
    if (arguments->type == ObjectType::THE_EMPTY_LIST){
        in = stdin;
    } else{
        Object* first_argu = ((pair*)arguments)->car();
        in = ((input_port*)first_argu)->stream;
    }
    result = read(in);
    return (result == nullptr) ? eof_object : result;
}

Object *primitive_proc_read_char::proc(Object *arguments) {
    FILE *in;
    int result;
    if (arguments->type == ObjectType::THE_EMPTY_LIST){
        in = stdin;
    } else{
        Object* first_argu = ((pair*)arguments)->car();
        in = ((input_port*)first_argu)->stream;
    }
    
    result = getc(in);
    return (result == EOF) ? (Object*)eof_object : (Object*)(new character(result));
}
Object *primitive_proc_peak_char::proc(Object *arguments) {
    FILE *in;
    int result;
    if (arguments->type == ObjectType::THE_EMPTY_LIST){
        in = stdin;
    } else{
        Object* first_argu = ((pair*)arguments)->car();
        in = ((input_port*)first_argu)->stream;
    }
    result = peek(in);
    return (result == EOF) ? (Object*)eof_object : (Object*)(new character(result));
}

Object* primitive_proc_is_eof::proc(Object *arguments) {
    Object* first_argu = ((pair*)arguments)->car();
    return (first_argu->type == ObjectType::EOF_OBJECT) ? True : False;
}
Object *primitive_proc_open_output_port::proc(Object *arguments) {
    const char *filename;
    FILE *out;
    
    Object* first_argu = ((pair*)arguments)->car();
    if (first_argu->type != ObjectType::STRING){
        return eof_object;
    }
    filename = ((string*)first_argu)->value.c_str();
    out = fopen(filename, "w");
    if (out == NULL) {
        fprintf(stderr, "could not open file \"%s\"\n", filename);
        exit(1);
    }
    return new output_port(out);
}

Object *primitive_proc_close_output_port::proc(Object *arguments) {
    int result;
    Object* first_argu = ((pair*)arguments)->car();
    if (first_argu->type != ObjectType::OUTPUT_PORT){
        return eof_object;
    }
    result = fclose(((output_port*)first_argu)->stream);
    if (result == EOF) {
        fprintf(stderr, "could not close output port\n");
        exit(1);
    }
    return ok_symbol;
}
Object * primitive_proc_is_output_port::proc(Object *arguments) {
    Object* first_argu = ((pair*)arguments)->car();
    return (first_argu->type == ObjectType::OUTPUT_PORT) ? True: False;
}

Object * primitive_proc_write_char::proc(Object *arguments) {
    Object *ch;
    FILE *out;
    
    ch = ((pair*)arguments)->car();
    arguments = ((pair*)arguments)->cdr();
    
    if (arguments->type == ObjectType::THE_EMPTY_LIST){
        out = stdout;
    } else{
        Object* first_argu = ((pair*)arguments)->car();
        out = ((output_port*)first_argu)->stream;
    }
    putc(((character*)ch)->value, out);    
    fflush(out);
    return ok_symbol;
}

Object *primitive_proc_write::proc(Object *arguments) {
    Object *exp;
    FILE *out;
    
    exp = ((pair*)arguments)->car();
    arguments = ((pair*)arguments)->cdr();
    
    if (arguments->type == ObjectType::THE_EMPTY_LIST){
        out = stdout;
    } else{
        Object* first_argu = ((pair*)arguments)->car();
        out = ((output_port*)first_argu)->stream;
    }
    write(out, exp);
    fflush(out);
    return ok_symbol;
}

Object *primitive_proc_error::proc(Object *arguments) {
    while (arguments->type !=  ObjectType::THE_EMPTY_LIST) {
        write(stderr, ((pair*)arguments)->car());
        fprintf(stderr, " ");
        arguments = ((pair*)arguments)->cdr();
    };
    printf("\nexiting\n");
    exit(1);
}

pair* setup_environment() {
    pair* env;
    env = extend_environment(new Frame(), the_empty_list);
    return env;
}

void populate_environment(pair* env){ 
    setup_environment();
#define add_procedure(scheme_name, obj)         \
    define_variable(make_symbol(scheme_name),   \
                    obj,                        \
                    env);

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
    
    add_procedure("apply"  , new primitive_proc_apply());
    add_procedure("interaction-environment"  , 
                    new primitive_proc_interaction_env());
    add_procedure("null-environment"  , new primitive_proc_null_env());
    add_procedure("environment"  , new primitive_proc_envirnoment());
    add_procedure("eval"  , new primitive_proc_eval());

    
    add_procedure("load"  , new primitive_proc_load());
    add_procedure("open_input_port"  , new primitive_proc_open_input_port());
    add_procedure("close_input_port"  , new primitive_proc_close_input_port());
    add_procedure("input-port?"  , new primitive_proc_is_input_port());
    add_procedure("read"  , new primitive_proc_read());
    add_procedure("read-char"  , new primitive_proc_read_char());
    add_procedure("peak-char"  , new primitive_proc_peak_char());
    add_procedure("eof-object?"  , new primitive_proc_is_eof());
    add_procedure("open-output-port"  , new primitive_proc_open_output_port());
    add_procedure("close-output-port"  , new primitive_proc_close_output_port());
    add_procedure("output-port?"  , new primitive_proc_is_output_port());
    add_procedure("write"  , new primitive_proc_write());
    add_procedure("write-char"  , new primitive_proc_write_char());
    
    add_procedure("error"  , new primitive_proc_error());

}

pair* make_environment() {
    pair* env;
    env  = setup_environment();
    populate_environment(env);    
    return env;
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
    begin_symbol = make_symbol("begin");
    cond_symbol = make_symbol("cond");
    else_symbol = make_symbol("else");
    if_symbol = make_symbol("if");
    let_symbol = make_symbol("let");
    and_symbol = make_symbol("and");
    or_symbol = make_symbol("or");
    
    eof_object = new eof();

    the_global_environment = make_environment();

    // gc
    gc_root_table.push_back(the_global_environment);
    gc_root_table.push_back(True);
    gc_root_table.push_back(False);
    gc_root_table.push_back(the_empty_list);
    gc_root_table.push_back(eof_object);
}


void destory() {
    delete True;
    delete False;
    delete the_empty_list;
}

Object *cons(Object *car, Object *cdr) {
    pair *obj = new pair(car, cdr);
    /*
    if (gc_root_table.back() == car || gc_root_table.back() == cdr) {
        gc_root_table.pop_back();
    }
    if (gc_root_table.back() == car || gc_root_table.back() == cdr) {
        gc_root_table.pop_back();
    }
    gc_root_table.push_back((Object*)obj);
    */
    return obj;
}

#define make_list_3(result,obj1,obj2,obj3) \
        result = cons(obj2, obj3);  \
        gc_root_table.push_back(result);    \
        result = cons(obj1, result);        \
        gc_root_table.pop_back();

#define make_list_4(result,obj1,obj2,obj3,obj4) \
        result = cons(obj3, obj4);              \
        gc_root_table.push_back(result);        \
        result = cons(obj2, result);            \
        gc_root_table.pop_back();               \
        gc_root_table.push_back(result);        \
        result = cons(obj1, result);            \
        gc_root_table.pop_back();               \


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
    
    // gc
    //gc_root_table.push_back((Object*)new_symbol);
    
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
    }else if (c == EOF) {
        return nullptr;
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

Object* make_begin(Object* exp) {
    return cons(begin_symbol, exp);
}

bool is_begin(Object* exp) {
    return is_tagged_list(exp, begin_symbol);
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

Object* make_if(Object* predicate, Object* consequent, Object* alternative) {
    return cons(if_symbol, 
                    cons(predicate, 
                            cons(consequent, 
                                    cons(alternative, the_empty_list))));
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

bool is_cond(Object* exp) {
    return is_tagged_list(exp, cond_symbol);
}
Object* cond_clauses(Object* exp) {
    return ((pair*)exp)->cdr();
}
Object* cond_predicate(Object* clause) {
    return ((pair*)clause)->car();
}
Object* cond_actions(Object* clause) {
    return ((pair*)clause)->cdr();
}
char is_cond_else_clause(Object* clause) {
    return cond_predicate(clause) == else_symbol;
}

Object *sequence_to_exp(Object *seq) {
    if (seq->type == ObjectType::THE_EMPTY_LIST) {
        return seq;
    }
    else if (is_last_exp(seq)) {
        return first_exp(seq);
    }
    else {
        return make_begin(seq);
    }
}

Object *expand_clauses(Object *clauses) {
    Object *first;
    Object *rest;
    
    if (clauses->type == ObjectType::THE_EMPTY_LIST) {
        return False;
    }
    else {
        first = ((pair*)clauses)->car();
        rest  = ((pair*)clauses)->cdr();
        if (is_cond_else_clause(first)) {
            if (rest->type == ObjectType::THE_EMPTY_LIST) {
                return sequence_to_exp(cond_actions(first));
            }
            else {
                fprintf(stderr, "else clause isn't last cond->if");
                exit(1);
            }
        }
        else {
            return make_if(cond_predicate(first),
                           sequence_to_exp(cond_actions(first)),
                           expand_clauses(rest));
        }
    }
}

Object *cond_to_if(Object *exp) {
    return expand_clauses(cond_clauses(exp));
}

Object* make_application(Object* op, Object* operands) {
    return cons(op, operands);
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

char is_let(Object* exp) {
    return is_tagged_list(exp, let_symbol);
}

Object* let_bindings(Object* exp) {
    return ((pair*)(((pair*)exp)->cdr()))->car();
}
Object* let_body(Object* exp) {
    return ((pair*)(((pair*)exp)->cdr()))->cdr();
}
Object* binding_parameter(Object* binding) {
    return ((pair*)binding)->car();
}
Object* binding_argument(Object* binding) {
    return ((pair*)(((pair*)binding)->cdr()))->car();
}

Object* bindings_arguments(Object* bindings) {
    return (bindings->type == ObjectType::THE_EMPTY_LIST) ?
                the_empty_list :
                    cons(binding_argument(((pair*)bindings)->car()),
                         bindings_arguments(((pair*)bindings)->cdr()));
}
Object* bindings_parameters(Object* bindings) {
    return (bindings->type == ObjectType::THE_EMPTY_LIST) ?
                the_empty_list :
                    cons(binding_parameter(((pair*)bindings)->car()),
                         bindings_parameters(((pair*)bindings)->cdr()));
}

Object* let_parameters(Object* exp) {
    return bindings_parameters(let_bindings(exp));
}
Object* let_arguments(Object* exp) {
    return bindings_arguments(let_bindings(exp));
}
Object* let_to_application(Object* exp) {
    return make_application(
                make_lambda(let_parameters(exp),
                            let_body(exp)),
                let_arguments(exp));
}

char is_and(Object* exp) {
    return is_tagged_list(exp, and_symbol);
}
char is_or(Object* exp) {
    return is_tagged_list(exp, or_symbol);
}
Object* and_tests(Object* exp) {
    return ((pair*)exp)->cdr();
}
Object* or_tests(Object* exp) {
    return ((pair*)exp)->cdr();
}

Object *apply_operator(Object *arguments) {
    return ((pair*)arguments)->car();
}

Object *prepare_apply_operands(Object *arguments) {
    if (((pair*)arguments)->cdr()->type == ObjectType::THE_EMPTY_LIST) {
        return ((pair*)arguments)->car();
    }
    else {
        return cons(((pair*)arguments)->car(),
                    prepare_apply_operands(((pair*)arguments)->cdr()));
    }
}

Object *apply_operands(Object *arguments) {
    return prepare_apply_operands(((pair*)arguments)->cdr());
}


Object *eval_expression(Object *exp) {
    return ((pair*)exp)->car();
}
Object* eval_environment(Object* exp) {
    return ((pair*)(((pair*)exp)->cdr()))->car();
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
    Object* result;
tailcall:
    if (is_self_evaluating(exp))
        return exp;
    else if (is_variable(exp)) {
        Object* obj = lookup_variable_value(static_cast<symbol*>(exp), env);
        if (obj == nullptr) {
            printf("Unbounded variable: ");
            exp->print();
            printf("\n");
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
    else if (is_begin(exp)) {
        exp = ((pair*)exp)->cdr();
        while(!is_last_exp(exp)) {
            eval(first_exp(exp), env);
            exp = rest_exps(exp);
        }
        exp = first_exp(exp);
        goto tailcall;
    }
    else if (is_cond(exp)) {
        exp = cond_to_if(exp);
        goto  tailcall;
    } else if (is_let(exp)) {
        exp = let_to_application(exp);
        goto tailcall;
    }
    else if (is_and(exp)) {
        exp = and_tests(exp);
        if (exp->type == ObjectType::THE_EMPTY_LIST) {
            return True;
        }
        while (!is_last_exp(exp)) {
            result = eval(first_exp(exp), env);
            if(is_false(result)) {
                return result;
            }
            exp = rest_exps(exp);
        }
        exp = first_exp(exp);
        goto tailcall;
    }
    else if (is_or(exp)) {
        exp = or_tests(exp);
        if (exp->type == ObjectType::THE_EMPTY_LIST) {
            return False;
        }
        while (!is_last_exp(exp)) {
            result = eval(first_exp(exp), env);
            if(is_true(result)) {
                return result;
            }
            exp = rest_exps(exp);
        }
        exp = first_exp(exp);
        goto tailcall;
    }
    else if (is_application(exp)) {
        procedure = eval(proc_operator(exp), env);
        arguments = list_of_values(proc_operands(exp), env);
        
        if (procedure->type == ObjectType::PRIMITIVE_PROC && 
                        dynamic_cast<primitive_proc_eval*>(procedure)){
            exp = eval_expression(arguments);
            env = (pair*)eval_environment(arguments);
            goto tailcall;
        }
        
        if (procedure->type == ObjectType::PRIMITIVE_PROC && 
                        dynamic_cast<primitive_proc_apply*>(procedure)){
            procedure = apply_operator(arguments);
            arguments = apply_operands(arguments);
        }
         
        if (procedure->type == ObjectType::PRIMITIVE_PROC){
            return ((Procedure*)procedure)->proc(arguments);
        }else if(procedure->type == ObjectType::COMPOUND_PROC) {
            compound_proc* cp = (compound_proc*)procedure;
            env = extend_environment(new Frame(cp->parameters, arguments), cp->env);
            exp = make_begin(cp->body);
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

void write(FILE* out, Object *obj) {
    if (obj == nullptr) return;
    obj->print(out); 
}


int main(int argc, char *argv[])
{
    Object* exp;
    std::cout << "Welcome to MyScheme. Use Ctrl-C to exit.\n";
    
    init();

    while  (true) {
        std::cout << "> ";
        exp = read(stdin);
        if (exp == nullptr){
            break;
        }
        write(stdout, eval(exp, the_global_environment));
        std::cout << "\n";
    }
    printf("Goodbye. Have a nice day!");
    destory();
       
    return 0;
}
