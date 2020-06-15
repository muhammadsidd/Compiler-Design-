/* I, Muhammad Siddiqui pledge my Honor that I have not cheated, and will not cheat, on this assignment. */
%option yylineno
%option noyywrap

%{
  #define DEBUG 1
  typedef YYTOKENTYPE Token;
  Token scanned(Token t);
%}
digit	[0-9]
ID	[a-zA-Z]+[a-zA-Z0-9]*
wspace	[ \t\r\n]+

%%

"class"		{return scanned(CLASS);}
"extends"	{return scanned(EXTENDS);} 
"main"		{return scanned(MAIN);}
"nat"		{return scanned(NATTYPE);}
"bool"		{return scanned(BOOLTYPE);}
"true"		{return scanned(TRUELITERAL);}
"false"		{return scanned(FALSELITERAL);}
"&&"		{return scanned(AND);}
"if"		{return scanned(IF);}
"else"		{return scanned(ELSE);}
"for"		{return scanned(FOR);}
"=="		{return scanned(EQUALITY);}
"="		{return scanned(ASSIGN);}
"static"	{return scanned(STATIC);}
"+"		{return scanned(PLUS);}
"-"		{return scanned(MINUS);}
"*"		{return scanned(TIMES);}
">"		{return scanned(GREATER);}
"this" 		{return scanned(THIS);}
"new"		{return scanned(NEW);}
"printNat"	{return scanned(PRINTNAT);}
"readNat"	{return scanned(READNAT);}
"instanceof"	{return scanned(INSTANCEOF);}
"null" 		{return scanned(NUL);}
{digit}+	{return scanned(NATLITERAL);}
{ID}    	{return scanned(ID);}
";"		{return scanned(SEMICOLON);}
"{"		{return scanned(LBRACE);}
"}"		{return scanned(RBRACE);}
"("		{return scanned(LPAREN);}			
")"		{return scanned(RPAREN);}
"."		{return scanned(DOT);}
{wspace}   	{ /*skip whitespace*/ }
<<EOF>>    	{return scanned(ENDOFFILE);}
.          	{ if(DEBUG) printf("\n");
             printf("Lex error on line %d: Illegal character %s\n",
                    yylineno,yytext);
             exit(-1);
			}
%%

/* process and return scanned token t */
Token scanned(Token t) {
  if(DEBUG==1) return t;
  switch(t) {
    case CLASS: printf("CLASS "); return t;
    case EXTENDS: printf("EXTENDS "); return t;
    case NATTYPE: printf("NATTYPE "); return t;
    case MAIN: printf("MAIN "); return t;
    case BOOLTYPE: printf("BOOLTYPE "); return t;
    case TRUELITERAL: printf("TRUELITERAL "); return t;
    case FALSELITERAL: printf("FALSELITERAL "); return t;
    case AND: printf("AND "); return t;
    case IF: printf("IF "); return t;
    case ELSE: printf("ELSe "); return t;
    case FOR: printf("FOR "); return t;
    case EQUALITY: printf("EQUALITY "); return t;
    case ASSIGN: printf("ASSIGN "); return t;
	case STATIC: printf("STATIC ");return t;
	case PLUS: printf("PLUS ");return t;
	case MINUS: printf("MINUS ");return t;
	case TIMES: printf("TIMES ");return t;
	case GREATER: printf("GREATER ");return t;
	case NATLITERAL: printf("NATLITERAL(%s) ",yytext);return t;
    case ID: printf("ID(%s) ", yytext); return t;
    case SEMICOLON: printf("SEMICOLON "); return t;
	case LBRACE: printf("LBRACE ");return t;
	case RBRACE: printf("RBRACE ");return t;
	case LPAREN: printf("LPAREN ");return t;
	case RPAREN: printf("RPAREN ");return t;
	case DOT: printf("DOT ");return t;
	case THIS: printf("THIS ");return t;
	case NEW: printf("NEW ");return t;
	case PRINTNAT: printf("PRINTNAT ");return t;
	case READNAT: printf("READNAT ");return t;
	case INSTANCEOF: printf("INSTANCEOF ");return t;
	case NUL: printf("NUL ");return t;
    case ENDOFFILE: printf("ENDOFFILE\n"); return t;
    default: printf("ERROR: invalid token in scanned().\n"); exit(-1);
  }
}
