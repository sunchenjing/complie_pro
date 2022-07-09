#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#define int long long // to work with 64bit address

int write_file;    // write the executed instructions into result file
int show_assembly; // print out the assembly and source

int token; // current token

// assembly instructions
enum { LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH, OR  ,XOR ,AND ,
        EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,OPEN, READ, CLOS, PRTF, MALC, MSET, MCMP, EXIT };

// tokens and classes (operators last and in precedence order)
enum {Num = 128, Fun, Sys, Glo, Loc, Id, Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Add, Sub, Mul, Div, Mod, Inc, Dec };

// fields of identifier
enum {Token, Hash, Name, Type, Class, Value, BType, BClass, BValue, IdSize};

// types of variable/function
enum { CHAR, INT, PTR };

// type of declaration.
enum {Global, Local};

int *text, // text segment
    *stack;// stack
int * old_text; // for dump text segment
char *data; // data segment
int *idmain;

char *src, *old_src;  // pointer to source code string;

int poolsize; // default size of text/data/stack
int *pc, *bp, *sp, ax, cycle; // virtual machine registers

int *current_id, // current parsed ID
    *symbols,    // symbol table
    line,        // line number of source code
    token_val;   // value of current token (mainly for number)

int basetype;    // the type of a declaration, make it global for convenience
int expr_type;   // the type of an process_expr

int index_of_bp; // index of bp pointer on stack

void token_next() {
    char *last_pos;
    int hash;
    while (token = *src) {
        ++src;
        if (token == '\n') {
            if (show_assembly) {
                // print compile info
                printf("%d: %.*s", line, src-old_src, old_src);
                old_src = src;
                while (old_text < text) {
                    printf("%8.4s", & "LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,"
                                      "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
                                      "OPEN,READ,CLOS, PRTF, MALC, MSET, MCMP, EXIT"[*++old_text * 5]);
                    if (*old_text <= ADJ)
                        printf(" %d\n", *++old_text);
                    else
                        printf("\n");
                }
            }
            ++line;
        }
        else if (token == '#') {
            // skip macro, because we will not support it
            while (*src != 0 && *src != '\n') {
                src++;
            }
        }
        else if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || (token == '_')) {
            // parse identifier
            last_pos = src - 1;
            hash = token;
            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9') || (*src == '_')) {
                hash = hash * 147 + *src;
                src++;
            }
            // look for existing identifier, linear search
            current_id = symbols;
            while (current_id[Token]) {
                if (current_id[Hash] == hash && !memcmp((char *)current_id[Name], last_pos, src - last_pos)) {
                    //found one, return
                    token = current_id[Token];
                    return;
                }
                current_id = current_id + IdSize;
            }
            // store new ID
            current_id[Name] = (int)last_pos;
            current_id[Hash] = hash;
            token = current_id[Token] = Id;
            return;
        }
        else if (token >= '0' && token <= '9') {
            // parse number
            token_val = token - '0';
            if (token_val > 0) {
                // dec, starts with [1-9]
                while (*src >= '0' && *src <= '9') {
                    token_val = token_val*10 + *src++ - '0';
                }
            } 
            token = Num;
            return;
        }
        else if (token == '/') {
            if (*src == '/') {
                // skip comments
                while (*src != 0 && *src != '\n') {
                    ++src;
                }
            } else {
                // divide operator
                token = Div;
                return;
            }
        }
        else if (token == '"' || token == '\'') {
            // parse string literal, currently, the only supported escape
            // character is '\n', store the string literal into data.
            last_pos = data;
            while (*src != 0 && *src != token) {
                token_val = *src++;
                if (token_val == '\\') {
                    // escape character
                    token_val = *src++;
                    if (token_val == 'n') {
                        token_val = '\n';
                    }
                }
                if (token == '"') {
                    *data++ = token_val;
                }
            }
            src++;
            // if it is a single character, return Num token
            if (token == '"') {
                token_val = (int)last_pos;
            } else {
                token = Num;
            }
            return;
        }
        else if (token == '=') {
            // parse '==' and '='
            if (*src == '=') {
                src ++;
                token = Eq;
            } else {
                token = Assign;
            }
            return;
        }
        else if (token == '+') {
            // parse '+' and '++'
            if (*src == '+') {
                src ++;
                token = Inc;
            } else {
                token = Add;
            }
            return;
        }
        else if (token == '-') {
            // parse '-' and '--'
            if (*src == '-') {
                src ++;
                token = Dec;
            } else {
                token = Sub;
            }
            return;
        }
        else if (token == '!') {
            // parse '!='
            if (*src == '=') {
                src++;
                token = Ne;
            }
            return;
        }
        else if (token == '<') {
            // parse '<=', '<<' or '<'
            if (*src == '=') {
                src ++;
                token = Le;
            } else if (*src == '<') {
                src ++;
                token = Shl;
            } else {
                token = Lt;
            }
            return;
        }
        else if (token == '>') {
            // parse '>=', or '>'
            if (*src == '=') {
                src ++;
                token = Ge;
            } else {
                token = Gt;
            }
            return;
        }
        else if (token == '|') {
            // parse '|'
            token = Or;
            return;
        }
        else if (token == '&') {
            // parse '&' 
            token = And;
            return;
        }
        else if (token == '^') {
            token = Xor;
            return;
        }
        else if (token == '%') {
            token = Mod;
            return;
        }
        else if (token == '*') {
            token = Mul;
            return;
        }
        else if (token == '~' || token == ';' || token == '{' || token == '}' || token == '(' || token == ')' || token == ']' || token == ',' || token == ':') {
            // directly return the character as token;
            return;
        }
    }
}

void check_token(int tk) {
    if (token == tk) {
        token_next();
    } else {
        printf("%d: expected token: %d\n", line, tk);
        exit(-1);
    }
}

void process_expr(int level) {
    int *id;
    int tmp;
    int *addr;
    {
        if (!token) {
            printf("%d: unexpected token EOF of process_expr\n", line);
            exit(-1);
        }
        if (token == Num) {
            check_token(Num);
            // emit code
            *++text = IMM;
            *++text = token_val;
            expr_type = INT;
        }
        else if (token == '"') {
            // emit code
            *++text = IMM;
            *++text = token_val;
            check_token('"');
            // store the rest strings
            while (token == '"') {
                check_token('"');
            }
            // append the end of string character '\0', all the data are default
            // to 0, so just move data one position forward.
            data = (char *)(((int)data + sizeof(int)) & (-sizeof(int)));
            expr_type = PTR;
        }
        else if (token == Sizeof) {
            // sizeof is actually an unary operator
            // now only `sizeof(int)`, `sizeof(char)` are supported.
            check_token(Sizeof);
            check_token('(');
            expr_type = INT;
            if (token == Int) {
                check_token(Int);
            } else if (token == Char) {
                check_token(Char);
                expr_type = CHAR;
            }
            while (token == Mul) {
                check_token(Mul);
                expr_type = expr_type + PTR;
            }
            check_token(')');
            // emit code
            *++text = IMM;
            *++text = (expr_type == CHAR) ? sizeof(char) : sizeof(int);
            expr_type = INT;
        }
        else if (token == Id) {
            // there are several type when occurs to Id
            // but this is unit, so it can only be
            // 1. function call
            // 2. Enum variable
            // 3. global/local variable
            check_token(Id);
            id = current_id;
            if (token == '(') {
                // function call
                check_token('(');
                // pass in arguments
                tmp = 0; // number of arguments
                while (token != ')') {
                    process_expr(Assign);
                    *++text = PUSH;
                    tmp ++;
                    if (token == ',') {
                        check_token(',');
                    }
                }
                check_token(')');
                // emit code
                if (id[Class] == Sys) {
                    // system functions
                    *++text = id[Value];
                }
                else if (id[Class] == Fun) {
                    // function call
                    *++text = CALL;
                    *++text = id[Value];
                }
                else {
                    printf("%d: bad function call\n", line);
                    exit(-1);
                }
                // clean the stack for arguments
                if (tmp > 0) {
                    *++text = ADJ;
                    *++text = tmp;
                }
                expr_type = id[Type];
            }
            else if (id[Class] == Num) {
                // enum variable
                *++text = IMM;
                *++text = id[Value];
                expr_type = INT;
            }
            else {
                // variable
                if (id[Class] == Loc) {
                    *++text = LEA;
                    *++text = index_of_bp - id[Value];
                }
                else if (id[Class] == Glo) {
                    *++text = IMM;
                    *++text = id[Value];
                }
                else {
                    printf("%d: undefined variable\n", line);
                    exit(-1);
                }
                // emit code, default behaviour is to load the value of the
                // address which is stored in `ax`
                expr_type = id[Type];
                *++text = (expr_type == CHAR) ? LC : LI;
            }
        }
        else if (token == '(') {
            // cast or parenthesis
            check_token('(');
            if (token == Int || token == Char) {
                tmp = (token == Char) ? CHAR : INT; // cast type
                check_token(token);
                while (token == Mul) {
                    check_token(Mul);
                    tmp = tmp + PTR;
                }
                check_token(')');
                process_expr(Inc); // cast has precedence as Inc(++)
                expr_type  = tmp;
            } else {
                // normal parenthesis
                process_expr(Assign);
                check_token(')');
            }
        }
        else if (token == Mul) {
            // dereference *<addr>
            check_token(Mul);
            process_expr(Inc); // dereference has the same precedence as Inc(++)
            *++text = (expr_type == CHAR) ? LC : LI;
        }
        else if (token == And) {
            // get the address of
            check_token(And);
            process_expr(Inc); // get the address of
            if (*text == LC || *text == LI) {
                text --;
            } else {
                printf("%d: bad address of\n", line);
                exit(-1);
            }
            expr_type = expr_type + PTR;
        }
        else if (token == '!') {
            // not
            check_token('!');
            process_expr(Inc);
            // emit code, use <expr> == 0
            *++text = PUSH;
            *++text = IMM;
            *++text = 0;
            *++text = EQ;
            expr_type = INT;
        }
        else if (token == Add) {
            // +var, do nothing
            check_token(Add);
            process_expr(Inc);
            expr_type = INT;
        }
        else if (token == Sub) {
            // -var
            check_token(Sub);
            if (token == Num) {
                *++text = IMM;
                *++text = -token_val;
                check_token(Num);
            } else {
                *++text = IMM;
                *++text = -1;
                *++text = PUSH;
                process_expr(Inc);
                *++text = MUL;
            }
            expr_type = INT;
        }
        else if (token == Inc || token == Dec) {
            tmp = token;
            check_token(token);
            process_expr(Inc);
            if (*text == LC) {
                *text = PUSH;  // to duplicate the address
                *++text = LC;
            } else if (*text == LI) {
                *text = PUSH;
                *++text = LI;
            } else {
                printf("%d: bad lvalue of pre-increment\n", line);
                exit(-1);
            }
            *++text = PUSH;
            *++text = IMM;
            *++text = (expr_type > PTR) ? sizeof(int) : sizeof(char);
            *++text = (tmp == Inc) ? ADD : SUB;
            *++text = (expr_type == CHAR) ? SC : SI;
        }
        else {
            printf("%d: bad process_expr\n", line);
            exit(-1);
        }
    }

    // binary operator and postfix operators.
    {
        while (token >= level) {
            // handle according to current operator's precedence
            tmp = expr_type;
            if (token == Assign) {
                // var = expr;
                check_token(Assign);
                if (*text == LC || *text == LI) {
                    *text = PUSH; // save the lvalue's pointer
                } else {
                    printf("%d: bad lvalue in assignment\n", line);
                    exit(-1);
                }
                process_expr(Assign);

                expr_type = tmp;
                *++text = (expr_type == CHAR) ? SC : SI;
            }
            else if (token == Or) {
                // bitwise or
                check_token(Or);
                *++text = PUSH;
                process_expr(Xor);
                *++text = OR;
                expr_type = INT;
            }
            else if (token == Xor) {
                // bitwise xor
                check_token(Xor);
                *++text = PUSH;
                process_expr(And);
                *++text = XOR;
                expr_type = INT;
            }
            else if (token == And) {
                // bitwise and
                check_token(And);
                *++text = PUSH;
                process_expr(Eq);
                *++text = AND;
                expr_type = INT;
            }
            else if (token == Eq) {
                // equal ==
                check_token(Eq);
                *++text = PUSH;
                process_expr(Ne);
                *++text = EQ;
                expr_type = INT;
            }
            else if (token == Ne) {
                // not equal !=
                check_token(Ne);
                *++text = PUSH;
                process_expr(Lt);
                *++text = NE;
                expr_type = INT;
            }
            else if (token == Lt) {
                // less than
                check_token(Lt);
                *++text = PUSH;
                process_expr(Shl);
                *++text = LT;
                expr_type = INT;
            }
            else if (token == Gt) {
                // greater than
                check_token(Gt);
                *++text = PUSH;
                process_expr(Shl);
                *++text = GT;
                expr_type = INT;
            }
            else if (token == Le) {
                // less than or equal to
                check_token(Le);
                *++text = PUSH;
                process_expr(Shl);
                *++text = LE;
                expr_type = INT;
            }
            else if (token == Ge) {
                // greater than or equal to
                check_token(Ge);
                *++text = PUSH;
                process_expr(Shl);
                *++text = GE;
                expr_type = INT;
            }
            else if (token == Shl) {
                // shift left
                check_token(Shl);
                *++text = PUSH;
                process_expr(Add);
                *++text = SHL;
                expr_type = INT;
            }
            else if (token == Add) {
                // add
                check_token(Add);
                *++text = PUSH;
                process_expr(Mul);

                expr_type = tmp;
                if (expr_type > PTR) {
                    // pointer type, and not `char *`
                    *++text = PUSH;
                    *++text = IMM;
                    *++text = sizeof(int);
                    *++text = MUL;
                }
                *++text = ADD;
            }
            else if (token == Sub) {
                // sub
                check_token(Sub);
                *++text = PUSH;
                process_expr(Mul);
                if (tmp > PTR && tmp == expr_type) {
                    // pointer subtraction
                    *++text = SUB;
                    *++text = PUSH;
                    *++text = IMM;
                    *++text = sizeof(int);
                    *++text = DIV;
                    expr_type = INT;
                } else if (tmp > PTR) {
                    // pointer movement
                    *++text = PUSH;
                    *++text = IMM;
                    *++text = sizeof(int);
                    *++text = MUL;
                    *++text = SUB;
                    expr_type = tmp;
                } else {
                    // numeral subtraction
                    *++text = SUB;
                    expr_type = tmp;
                }
            }
            else if (token == Mul) {
                // multiply
                check_token(Mul);
                *++text = PUSH;
                process_expr(Inc);
                *++text = MUL;
                expr_type = tmp;
            }
            else if (token == Div) {
                // divide
                check_token(Div);
                *++text = PUSH;
                process_expr(Inc);
                *++text = DIV;
                expr_type = tmp;
            }
            else if (token == Mod) {
                // Modulo
                check_token(Mod);
                *++text = PUSH;
                process_expr(Inc);
                *++text = MOD;
                expr_type = tmp;
            }
            else if (token == Inc || token == Dec) {
                // postfix inc(++) and dec(--)
                // we will increase the value to the variable and decrease it on `ax` to get its original value.
                if (*text == LI) {
                    *text = PUSH;
                    *++text = LI;
                }
                else if (*text == LC) {
                    *text = PUSH;
                    *++text = LC;
                }
                else {
                    printf("%d: bad value in increment\n", line);
                    exit(-1);
                }
                *++text = PUSH;
                *++text = IMM;
                *++text = (expr_type > PTR) ? sizeof(int) : sizeof(char);
                *++text = (token == Inc) ? ADD : SUB;
                *++text = (expr_type == CHAR) ? SC : SI;
                *++text = PUSH;
                *++text = IMM;
                *++text = (expr_type > PTR) ? sizeof(int) : sizeof(char);
                *++text = (token == Inc) ? SUB : ADD;
                check_token(token);
            }
            else {
                printf("%d: compiler error, token = %d\n", line, token);
                exit(-1);
            }
        }
    }
}

void process_stmt() {
    // there are 6 kinds of statements here:
    // 1. if (...) <statement> [else <statement>]
    // 2. while (...) <statement>
    // 3. { <statement> }
    // 4. return xxx;
    // 5. <empty statement>;
    // 6. process_expr; (process_expr end with semicolon)
    int *a, *b; // bess for branch control
    if (token == If) {
        check_token(If);
        check_token('(');
        process_expr(Assign);  // parse condition
        check_token(')');
        *++text = JZ;
        b = ++text;
        process_stmt();         // parse statement
        if (token == Else) { // parse else
            check_token(Else);
            // emit code for JMP B
            *b = (int)(text + 3);
            *++text = JMP;
            b = ++text;
            process_stmt();
        }
        *b = (int)(text + 1);
    }
    else if (token == While) {
        check_token(While);
        a = text + 1;
        check_token('(');
        process_expr(Assign);
        check_token(')');
        *++text = JZ;
        b = ++text;
        process_stmt();
        *++text = JMP;
        *++text = (int)a;
        *b = (int)(text + 1);
    }
    else if (token == '{') {
        // { <statement> ... }
        check_token('{');
        while (token != '}') {
            process_stmt();
        }
        check_token('}');
    }
    else if (token == Return) {
        // return [process_expr];
        check_token(Return);
        if (token != ';') {
            process_expr(Assign);
        }
        check_token(';');
        // emit code for return
        *++text = LEV;
    }
    else if (token == ';') {
        // empty statement
        check_token(';');
    }
    else {
        // a = b; or function_call();
        process_expr(Assign);
        check_token(';');
    }
}

void enum_declaration() {
    // parse enum [id] { a = 1, b = 3, ...}
    int i = 0;
    while (token != '}') {
        if (token != Id) {
            printf("%d: bad enum identifier %d\n", line, token);
            exit(-1);
        }
        token_next();
        if (token == Assign) {
            // like {a=10}
            token_next();
            if (token != Num) {
                printf("%d: bad enum initializer\n", line);
                exit(-1);
            }
            i = token_val;
            token_next();
        }
        current_id[Class] = Num;
        current_id[Type] = INT;
        current_id[Value] = i++;
        if (token == ',') {
            token_next();
        }
    }
}

void function_parameter() {
    int type;
    int params;
    params = 0;
    while (token != ')') {
        // int name, ...
        type = INT;
        if (token == Int) {
            check_token(Int);
        } else if (token == Char) {
            type = CHAR;
            check_token(Char);
        }
        // pointer type
        while (token == Mul) {
            check_token(Mul);
            type = type + PTR;
        }
        // parameter name
        if (token != Id) {
            printf("%d: bad parameter declaration\n", line);
            exit(-1);
        }
        if (current_id[Class] == Loc) {
            printf("%d: duplicate parameter declaration\n", line);
            exit(-1);
        }
        check_token(Id);
        // store the local variable
        current_id[BClass] = current_id[Class]; current_id[Class]  = Loc;
        current_id[BType]  = current_id[Type];  current_id[Type]   = type;
        current_id[BValue] = current_id[Value]; current_id[Value]  = params++;   // index of current parameter
        if (token == ',') {
            check_token(',');
        }
    }
    index_of_bp = params+1;
}

void function_body() {
    int pos_local; // position of local variables on the stack.
    int type;
    pos_local = index_of_bp;
    while (token == Int || token == Char) {
        // local variable declaration, just like global ones.
        basetype = (token == Int) ? INT : CHAR;
        check_token(token);
        while (token != ';') {
            type = basetype;
            while (token == Mul) {
                check_token(Mul);
                type = type + PTR;
            }
            if (token != Id) {
                // invalid declaration
                printf("%d: bad local declaration\n", line);
                exit(-1);
            }
            if (current_id[Class] == Loc) {
                // identifier exists
                printf("%d: duplicate local declaration\n", line);
                exit(-1);
            }
            check_token(Id);
            // store the local variable
            current_id[BClass] = current_id[Class]; current_id[Class]  = Loc;
            current_id[BType]  = current_id[Type];  current_id[Type]   = type;
            current_id[BValue] = current_id[Value]; current_id[Value]  = ++pos_local;   // index of current parameter
            if (token == ',') {
                check_token(',');
            }
        }
        check_token(';');
    }
    // save the stack size for local variables
    *++text = ENT;
    *++text = pos_local - index_of_bp;
    // statements
    while (token != '}') {
        process_stmt();
    }
    // emit code for leaving the sub function
    *++text = LEV;
}

void function_declaration() {
    check_token('(');
    function_parameter();
    check_token(')');
    check_token('{');
    function_body();
    //check_token('}');
    // unwind local variable declarations for all local variables.
    current_id = symbols;
    while (current_id[Token]) {
        if (current_id[Class] == Loc) {
            current_id[Class] = current_id[BClass];
            current_id[Type]  = current_id[BType];
            current_id[Value] = current_id[BValue];
        }
        current_id = current_id + IdSize;
    }
}

void global_declaration() {
    // int [*]id [; | (...) {...}]
    int type; // tmp, actual type for variable
    int i; // tmp
    basetype = INT;
    // parse enum, this should be treated alone.
    if (token == Enum) {
        // enum [id] { a = 10, b = 20, ... }
        check_token(Enum);
        if (token != '{') {
            check_token(Id); // skip the [id] part
        }
        if (token == '{') {
            // parse the assign part
            check_token('{');
            enum_declaration();
            check_token('}');
        }
        check_token(';');
        return;
    }
    // parse type information
    if (token == Int) {
        check_token(Int);
    }
    else if (token == Char) {
        check_token(Char);
        basetype = CHAR;
    }
    // parse the comma seperated variable declaration.
    while (token != ';' && token != '}') {
        type = basetype;
        // parse pointer type, note that there may exist `int ****x;`
        while (token == Mul) {
            check_token(Mul);
            type = type + PTR;
        }
        if (token != Id) {
            // invalid declaration
            printf("%d: bad global declaration\n", line);
            exit(-1);
        }
        if (current_id[Class]) {
            // identifier exists
            printf("%d: duplicate global declaration\n", line);
            exit(-1);
        }
        check_token(Id);
        current_id[Type] = type;
        if (token == '(') {
            current_id[Class] = Fun;
            current_id[Value] = (int)(text + 1); // the memory address of function
            function_declaration();
        } else {
            // variable declaration
            current_id[Class] = Glo; // global variable
            current_id[Value] = (int)data; // assign memory address
            data = data + sizeof(int);
        }
        if (token == ',') {
            check_token(',');
        }
    }
    token_next();
}

void compile() {
    // get token_next token
    token_next();
    while (token > 0) {
        global_declaration();
    }
}

int instruction_set() {
    int op, *tmp;
    cycle = 0;
    FILE *fpwrite = fopen("result.txt", "w");
    while (1) {
        cycle ++;
        op = *pc++; // get token_next operation code
        if (write_file) {
            fprintf(fpwrite, "%d> %.4s", cycle,
                   & "LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,"
                   "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
                   "OPEN, READ,CLOS, PRTF, MALC, MSET, MCMP, EXIT"[op * 5]);
            if (op <= ADJ)
                fprintf(fpwrite, " %d\n", *pc);
            else
                fprintf(fpwrite, "\n");
        }
        if (op == IMM)       {ax = *pc++;}                                     // load immediate value to ax
        else if (op == LC)   {ax = *(char *)ax;}                               // load character to ax, address in ax
        else if (op == LI)   {ax = *(int *)ax;}                                // load integer to ax, address in ax
        else if (op == SC)   {ax = *(char *)*sp++ = ax;}                       // save character to address, value in ax, address on stack
        else if (op == SI)   {*(int *)*sp++ = ax;}                             // save integer to address, value in ax, address on stack
        else if (op == PUSH) {*--sp = ax;}                                     // push the value of ax onto the stack
        else if (op == JMP)  {pc = (int *)*pc;}                                // jump to the address
        else if (op == JZ)   {pc = ax ? pc + 1 : (int *)*pc;}                   // jump if ax is zero
        else if (op == JNZ)  {pc = ax ? (int *)*pc : pc + 1;}                   // jump if ax is not zero
        else if (op == CALL) {*--sp = (int)(pc+1); pc = (int *)*pc;}           // call subroutine
        //else if (op == RET)  {pc = (int *)*sp++;}                              // return from subroutine;
        else if (op == ENT)  {*--sp = (int)bp; bp = sp; sp = sp - *pc++;}      // make new stack frame
        else if (op == ADJ)  {sp = sp + *pc++;}                                // add esp, <size>
        else if (op == LEV)  {sp = bp; bp = (int *)*sp++; pc = (int *)*sp++;}  // restore call frame and PC
        else if (op == LEA)  {ax = (int)(bp + *pc++);}                         // load address for arguments.

        else if (op == OR)  ax = *sp++ | ax;
        else if (op == XOR) ax = *sp++ ^ ax;
        else if (op == AND) ax = *sp++ & ax;
        else if (op == EQ)  ax = *sp++ == ax;
        else if (op == NE)  ax = *sp++ != ax;
        else if (op == LT)  ax = *sp++ < ax;
        else if (op == LE)  ax = *sp++ <= ax;
        else if (op == GT)  ax = *sp++ >  ax;
        else if (op == GE)  ax = *sp++ >= ax;
        else if (op == SHL) ax = *sp++ << ax;
        else if (op == SHR) ax = *sp++ >> ax;
        else if (op == ADD) ax = *sp++ + ax;
        else if (op == SUB) ax = *sp++ - ax;
        else if (op == MUL) ax = *sp++ * ax;
        else if (op == DIV) ax = *sp++ / ax;
        else if (op == MOD) ax = *sp++ % ax;

        else if (op == EXIT) { printf("exit(%d)", *sp); return *sp;}
        else if (op == OPEN) { ax = open((char *)sp[1], sp[0]); }
        else if (op == CLOS) { ax = close(*sp);}
        else if (op == READ) { ax = read(sp[2], (char *)sp[1], *sp); }
        else if (op == PRTF) { tmp = sp + pc[1]; ax = printf((char *)tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5], tmp[-6]); }
        else if (op == MALC) { ax = (int)malloc(*sp);}
        else if (op == MSET) { ax = (int)memset((char *)sp[2], sp[1], *sp);}
        else if (op == MCMP) { ax = memcmp((char *)sp[2], (char *)sp[1], *sp);}
        else {
            printf("unknown instruction:%d\n", op);
            return -1;
        }
    }
}

#undef int // Mac/clang needs this to compile

int main(int argc, char **argv)
{
    #define int long long // to work with 64bit address
    int i, fd;
    int *tmp;
    argc--;
    argv++;
    // parse arguments
    if (argc > 0 && **argv == '-' && (*argv)[1] == 's') {
        show_assembly = 1;
        --argc;
        ++argv;
    }
    if (argc > 0 && **argv == '-' && (*argv)[1] == 'w') {
        write_file = 1;
        --argc;
        ++argv;
    }
    if (argc < 1) {
        printf("usage: xc [-s] [-d] file ...\n");
        return -1;
    }
    if ((fd = open(*argv, 0)) < 0) {
        printf("could not open(%s)\n", *argv);
        return -1;
    }
    poolsize = 256 * 1024; // arbitrary size
    line = 1;
    // allocate memory
    if (!(text = malloc(poolsize))) {
        printf("could not malloc(%d) for text area\n", poolsize);
        return -1;
    }
    if (!(data = malloc(poolsize))) {
        printf("could not malloc(%d) for data area\n", poolsize);
        return -1;
    }
    if (!(stack = malloc(poolsize))) {
        printf("could not malloc(%d) for stack area\n", poolsize);
        return -1;
    }
    if (!(symbols = malloc(poolsize))) {
        printf("could not malloc(%d) for symbol table\n", poolsize);
        return -1;
    }
    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);
    memset(symbols, 0, poolsize);
    old_text = text;
    src = "char else enum if int return sizeof while "
          "open read close printf malloc memset memcmp exit void main";
     // add keywords to symbol table
    i = Char;
    while (i <= While) {
        token_next();
        current_id[Token] = i++;
    }
    // add library to symbol table
    i = OPEN;
    while (i <= EXIT) {
        token_next();
        current_id[Class] = Sys;
        current_id[Type] = INT;
        current_id[Value] = i++;
    }
    token_next(); current_id[Token] = Char; // handle void type
    token_next(); idmain = current_id; // keep track of main
    if (!(src = old_src = malloc(poolsize))) {
        printf("could not malloc(%d) for source area\n", poolsize);
        return -1;
    }
    // read the source file
    if ((i = read(fd, src, poolsize-1)) <= 0) {
        printf("read() returned %d\n", i);
        return -1;
    }
    src[i] = 0; // add EOF character
    close(fd);
    compile();
    if (!(pc = (int *)idmain[Value])) {
        printf("main() not defined\n");
        return -1;
    }
    // dump_text();
    if (show_assembly) {
        // only for compile
        return 0;
    }
    // setup stack
    sp = (int *)((int)stack + poolsize);
    *--sp = EXIT; // call exit if main returns
    *--sp = PUSH; tmp = sp;
    *--sp = argc;
    *--sp = (int)argv;
    *--sp = (int)tmp;
    return instruction_set();
}
