%{
#define IT 1 //标识符
#define CT 2 //常数
//关键字
#define INT 3
#define MAIN 4
#define VOID 5
#define IF 6
#define ELSE 7
#define CHAR 8
#define FLOAT 9
#define DOUBLE 10
#define FOR 11
#define WHILE 12
#define CONTINUE 13
#define BREAK 14
#define SWITCH 15
#define CASE 16
#define CONST 17
#define RETURN 18
//运算符和界符
#define GREATER 19
#define GREATER_EQU 20
#define SMALLER 21
#define SAMLLER_EQU 22
#define EQUAL 23
#define ASSIGN 24
#define ADD 25
#define ADD_ASS 26
#define SUB 27
#define SUB_ASS 28
#define MULT 29
#define MULT_ASS 30
#define DIV 31
#define DIV_ASS 32
#define LEFT_PAR 33
#define RIGHT_PAR 34
#define LEFT_BRA 35
#define RIGHT_BRA 36
#define LEFT_BRACES 37
#define RIGHT_BRACES 38
#define COMMA 39
#define SEMI 40
%}

split       [ \n\t]
ws          {split}+
digit       [0-9]
char        [A-Za-z]
it          {char}({digit}|{char})*
num         {digit}+(\.{digit}+)?

%%

{ws}          { }
int         {return(INT);}
main        {return(MAIN);}
void        {return(VOID);}
if          {return(IF);}
else        {return(ELSE);}
char        {return(CHAR);}
float         {return(FLOAT);}
double        {return(DOUBLE);}
for         {return(FOR);}
while       {return(WHILE);}
continue         {return(CONTINUE);}
break         {return(BREAK);}
switch         {return(SWITCH);}
case        {return(CASE);}
const         {return(CONST);}
return      {return(RETURN);}

">"         {return(GREATER);}
">="         {return(GREATER_EQU);}
"<"         {return(SMALLER);}
"<="         {return(SAMLLER_EQU);}
"=="         {return(EQUAL);}
"="         {return(ASSIGN);}
"+"         {return(ADD);}
"+="         {return(ADD_ASS);}
"-"         {return(SUB);}
"-="         {return(SUB_ASS);}
"*"         {return(MULT);}
"*="         {return(MULT_ASS);}
"/"         {return(DIV);}
"/="         {return(DIV_ASS);}
"("         {return(LEFT_PAR);}
")"         {return(RIGHT_PAR);}
"["         {return(LEFT_BRA);}
"]"         {return(RIGHT_BRA);}
"{"         {return(LEFT_BRACES);}
"}"         {return(RIGHT_BRACES);}
","         {return(COMMA);}
";"         {return(SEMI);}

{it}        {return(IT);}
{num}       {return(CT);}

%%
void writeout(int c){
    switch(c){
        case INT: fprintf(yyout,"(%d,\"%s\")\n",INT,yytext);break;
        case MAIN: fprintf(yyout,"(%d,\"%s\")\n",MAIN,yytext);break;
        case VOID: fprintf(yyout,"(%d,\"%s\")\n",VOID,yytext);break;
        case IF: fprintf(yyout,"(%d,\"%s\")\n",IF,yytext);break;
        case ELSE: fprintf(yyout,"(%d,\"%s\")\n",ELSE,yytext);break;
        case CHAR: fprintf(yyout,"(%d,\"%s\")\n",CHAR,yytext);break;
        case FLOAT: fprintf(yyout,"(%d,\"%s\")\n",FLOAT,yytext);break;
        case DOUBLE: fprintf(yyout,"(%d,\"%s\")\n",DOUBLE,yytext);break;
        case FOR: fprintf(yyout,"(%d,\"%s\")\n",FOR,yytext);break;
        case WHILE: fprintf(yyout,"(%d,\"%s\")\n",WHILE,yytext);break;
        case CONTINUE: fprintf(yyout,"(%d,\"%s\")\n",CONTINUE,yytext);break;
        case BREAK: fprintf(yyout,"(%d,\"%s\")\n",BREAK,yytext);break;
        case SWITCH: fprintf(yyout,"(%d,\"%s\")\n",SWITCH,yytext);break;
        case CASE: fprintf(yyout,"(%d,\"%s\")\n",CASE,yytext);break;
        case CONST: fprintf(yyout,"(%d,\"%s\")\n",CONST,yytext);break;
        case RETURN: fprintf(yyout,"(%d,\"%s\")\n",RETURN,yytext);break;
        
        case GREATER: fprintf(yyout,"(%d,\"%s\")\n",GREATER,yytext);break;
        case GREATER_EQU: fprintf(yyout,"(%d,\"%s\")\n",GREATER_EQU,yytext);break;
        case SMALLER: fprintf(yyout,"(%d,\"%s\")\n",SMALLER,yytext);break;
        case SAMLLER_EQU: fprintf(yyout,"(%d,\"%s\")\n",SAMLLER_EQU,yytext);break;
        case EQUAL: fprintf(yyout,"(%d,\"%s\")\n",EQUAL,yytext);break;
        case ASSIGN: fprintf(yyout,"(%d,\"%s\")\n",ASSIGN,yytext);break;
        case ADD: fprintf(yyout,"(%d,\"%s\")\n",ADD,yytext);break;
        case ADD_ASS: fprintf(yyout,"(%d,\"%s\")\n",ADD_ASS,yytext);break;
        case SUB: fprintf(yyout,"(%d,\"%s\")\n",SUB,yytext);break;
        case SUB_ASS: fprintf(yyout,"(%d,\"%s\")\n",SUB_ASS,yytext);break;
        case MULT: fprintf(yyout,"(%d,\"%s\")\n",MULT,yytext);break;
        case MULT_ASS: fprintf(yyout,"(%d,\"%s\")\n",MULT_ASS,yytext);break;
        case DIV: fprintf(yyout,"(%d,\"%s\")\n",DIV,yytext);break;
        case DIV_ASS: fprintf(yyout,"(%d,\"%s\")\n",DIV_ASS,yytext);break;
        case LEFT_PAR: fprintf(yyout,"(%d,\"%s\")\n",LEFT_PAR,yytext);break;
        case RIGHT_PAR: fprintf(yyout,"(%d,\"%s\")\n",RIGHT_PAR,yytext);break;
        case LEFT_BRA: fprintf(yyout,"(%d,\"%s\")\n",LEFT_BRA,yytext);break;
        case RIGHT_BRA: fprintf(yyout,"(%d,\"%s\")\n",RIGHT_BRA,yytext);break;
        case LEFT_BRACES: fprintf(yyout,"(%d,\"%s\")\n",LEFT_BRACES,yytext);break;
        case RIGHT_BRACES: fprintf(yyout,"(%d,\"%s\")\n",RIGHT_BRACES,yytext);break;
        case COMMA: fprintf(yyout,"(%d,\"%s\")\n",COMMA,yytext);break;
        case SEMI: fprintf(yyout,"(%d,\"%s\")\n",SEMI,yytext);break;
        case IT: fprintf(yyout,"(%d,\"%s\")\n",IT,yytext);break;
        case CT: fprintf(yyout,"(%d,\"%s\")\n",CT,yytext);break;
        default:break;
    }
    return;
}

int main(int argc,char* argv[]){
    int c ;

    if(argc >= 2){
        if((yyin = fopen(argv[1],"r"))== NULL){
            printf("err");
            return 1;
        }
        if(argc >= 3){
            yyout = fopen(argv[2],"w");
        }
    }

    while((c = yylex()) != 0){
        writeout(c);
    }

    if(argc >= 2){
        fclose(yyin);
        if(argc >= 3){
            fclose(yyout);
        }
    }
    return 0;
}