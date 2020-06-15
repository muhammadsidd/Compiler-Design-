/* DJ PARSER by Muhammad Siddiqui, I pledge on my honor that i have not cheated and will not cheat on this assignment*/

%code provides {
  #include <stdio.h>
  #include "lex.yy.c"
  #include "ast.h"

  /* Symbols in this grammar are represented as ASTs */ 
  #define YYSTYPE ASTree *

  /* Declare global AST for entire program */
  ASTree *pgmAST;
  
  /* Function for printing generic syntax-error messages */
  void yyerror(const char *str) {
    printf("Syntax error on line %d at token %s\n",yylineno,yytext);
    printf("Exiting simulator.\n");
    exit(-1);
  }
}

%token CLASS ID EXTENDS MAIN NATTYPE BOOLTYPE
%token TRUELITERAL FALSELITERAL AND NOT IF ELSE FOR
%token NATLITERAL PRINTNAT READNAT PLUS MINUS TIMES EQUALITY GREATER
%token STATIC ASSIGN NUL NEW THIS DOT INSTANCEOF
%token SEMICOLON LBRACE RBRACE LPAREN RPAREN
%token ENDOFFILE

%start pgm

%right ASSIGN
%left OR
%left AND
%nonassoc EQUALITY
%nonassoc GREATER
%nonassoc INSTANCEOF
%left PLUS MINUS
%left TIMES
%right NOT
%left DOT

%%

pgm 	: begin ENDOFFILE 
		{ pgmAST = $1; return 0; }
    	;

classList : classList class main
		{$$ =appendToChildrenList($1, newAST(CLASS_DECL,$2,0,NULL,yylineno));
		$$ = appendToChildrenList($$,$3);}
		;
main	: MAIN variable
		{$$= newAST(VAR_DECL_LIST,$2,0,NULL,yylineno));}
		;
class	: CLASS ID EXTENDS ID LBRACE staticList regular methodlist RBRACE
		{$$ = newAST(STATIC_VAR_DECL_LIST,$6,0,NULL,yylineno));
		 $$ = appendToChildrenList($$,$7);
		 $$ = appendToChildrenList($$,$8);}
		| 
		{$$= newAST(CLASS_DECL_LIST,NULL,NULL,NULL,yylineno);}
		;

staticList : staticList static SEMICOLON
		{$$ = appendToChildrenList($1, newAST(STATIC_VAR_DECL,$2,0,NULL,yylineno));}
		|
		{$$ = newAST(STATIC_VAR_DECL_LIST,NULL,NULL,NULL,yylineno);}
		;
static 	: STATIC type ID
		{$$ = appendToChildrenList($$,$2);}
		;

regular	: regular type ID SEMICOLON
		{$$ = newAST(VAR_DECL,$1,0,NULL,yylineno);
		 $$ = appendToChildrenList($$,$2);}
		|
		{$$= newAST(VAR_DECL_LIST,NULL,NULL,NULL,yylineno);}
		;

methodlist : methodlist method variable
			{$$ = appendToChildrenList($1, newAST(METHOD_DECL,$2,0,NULL,yylineno));
			 $$ = appendToChildrenList($$,$3);}
			| 
			{$$ = newAST(METHOD_DECL_LIST,NULL,NULL,NULL,yylineno);}
		;

method	: type ID LPAREN type ID RPAREN 
		{$$ = appendToChildrenList($1,$4);}
		;

variable: LBRACE regular explist RBRACE
		{$$ = newAST(EXPR_LIST,$3,0,NULL,yylineno);
		 $$ = appendToChildrenList($2,$$);}
		;

type	: NATTYPE
		{$$ =  newAST(NAT_TYPE,NULL,NULL,NULL,yylineno);}
		| BOOLTYPE
		{$$ =  newAST(BOOL_TYPE,NULL,NULL,NULL,yylineno);}
		| ID
		{$$ =  newAST(AST_ID,NULL,NULL,yytext,yylineno);}
		;

explist	: explist exp SEMICOLON
		{$$ = newAST(EXPR_LIST,$1,0,NULL,yylineno);
		 $$ = appendToChildrenList($$,$2);}
		;

exp	  	: exp PLUS exp 
		{$$ =newAST (PLUS_EXPR, $1,0,NULL,yylineno);
		$$= appendToChildrenList($$,$3);}
		| exp MINUS exp
		{$$ =newAST (MINUS_EXPR, $1,0,NULL,yylineno);
		$$= appendToChildrenList($$,$3);} 
		| exp TIMES exp
		{$$ =newAST (TIMES_EXPR, $1,0,NULL,yylineno);
		$$= appendToChildrenList($$,$3);}
		| exp AND exp
		{$$ =newAST (AND_EXPR, $1,0,NULL,yylineno);
		$$= appendToChildrenList($$,$3);}
		| ID ASSIGN exp 
		{$$ =newAST (ASSIGN_EXPR, $1,0,yytext,yylineno);
		$$= appendToChildrenList($$,$3);} 
		| exp EQUALITY exp
		{$$ =newAST (EQUALITY_EXPR, $1,0,NULL,yylineno);
		$$= appendToChildrenList($$,$3);}
		| exp GREATER exp
		{$$ =newAST (GREATER_EXPR, $1,0,NULL,yylineno);
		$$= appendToChildrenList($$,$3);}
		| NOT exp
		{$$ =newAST (NOT_EXPR, $1,0,NULL,yylineno);
		$$ = appendToChildrenList($$,$2)}
		| exp INSTANCEOF ID
		{$$ =newAST (INSTANCEOF_EXPR, $3,0,yytext,yylineno);
		$$= appendToChildrenList($$,$1);}
		| TRUELITERAL 
		{$$ =newAST (TRUE_LITERAL_EXPR, $1,0,NULL,yylineno);} 
		| FALSELITERAL 
		{$$ =newAST (FALSE_LITERAL_EXPR, $1,0,NULL,yylineno);}
		| NUL	
		{$$ =newAST (NULL_EXPR, $1,0,NULL,yylineno);}
		| NATLITERAL  
		{$$ =newAST (NAT_LITERAL_EXPR, $1,atoi(yytext),NULL,yylineno);}
		| if
		{$$ = appendToChildrenList($$,$1);}
		| for 
		{$$ = appendToChildrenList($$,$1);}
		| NEW ID
		{$$ = newAST(NEW_EXPR, NULL, NULL,NUL,yylineno);
		 $$ = appendTOChildrenList($$, newAST(AST_ID,NULL,NULL,yytext,yylineno));} 
		| exp DOT ID LPAREN exp RPAREN  
		{$$ = newAST(DOT_METHOD_CALL_EXPR,NULL,NULL,NULL,yylineno);}
		| ID LPAREN exp RPAREN
		{$$ = newAST(ID_EXPR,NULL,NULL,NULL,yylineno);}
		| PRINTNAT LPAREN exp RPAREN
		{$$ = newAST(PRINT_EXPR,NULL,NULL,NULL,yylineno);
		 $$ = appendTOChildrenList($$,$3);}
		| READNAT LPAREN RPAREN
		{$$ = newAST(READ_EXPR,NULL,NULL,NULL,yylineno);}
		;

if		: IF LPAREN exp RPAREN LBRACE explist RBRACE else
		{$$ = appendTOChildrenList($$,newAST(IF_THEN_ELSE_EXPR,NULL,NULL,NULL,yylineno));
		 $$ = appendToChildrenList($$,$6);
		 $$ = appendToChildrenList($$,$8);}
		;
else 	: ELSE LBRACE explist RBRACE
		{$$ = appendToChildrenList($$, newAST(EXPR_LIST,$3,NULL,NULL,yylineno));}
		;
for		: FOR LPAREN exp SEMICOLON exp SEMICOLON exp RPAREN LBRACE explist RBRACE
		{$$ = appendToChildrenList($$,newAST(FOR_EXPR,NULL,NULL,NULL,yylineno));
		 $$ = appendTOChildrenList($$,newAST(EXPR_LIST,$10,0,NULL,yylineno));}
		;
		

%%

int main(int argc, char **argv) {
  if(argc!=2) {
    printf("Usage: dj-parse filename\n");
    exit(-1);
  }
  yyin = fopen(argv[1],"r");
  if(yyin==NULL) {
    printf("ERROR: could not open file %s\n",argv[1]);
    exit(-1);
  }
  /* parse the input program */
  return yyparse();
}
