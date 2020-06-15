/* File ast.c
   I, Muhammad Siddiqui, pleadge my honor that i have not and will not cheat on this assignment. 
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

void printError(char *reason) {
  printf("AST Error: %s\n", reason);
  exit(-1);
}

/* Create a new AST node of type t */
ASTree *newAST(ASTNodeType t, ASTree *child, unsigned int natAttribute, char *idAttribute, unsigned int lineNum){
  ASTree *toReturn = malloc(sizeof(ASTree));
  if(toReturn==NULL) printError("malloc in newAST()");
  toReturn->typ = t;
  // create a linked list of children 
  //use malloc to dynamically allocate memory
  ASTList *childList = malloc(sizeof(ASTList));
  if(childList==NULL) printError("malloc in newAST()");
  childList->data = child;
  childList->next = NULL;
  toReturn->children = childList;
  toReturn->childrenTail = childList;
  //append to AST if the node is not empty using the predefined funtion AppendAST
  if(child->childrenTail!=NULL) appendToChildrenList(toReturn, child);
  // node's attribute
  if(idAttribute==NULL) {
      //set to null if no attribute set and return
      toReturn->idVal=NULL;
  }
  else {
      //if id not empty toReturn is set to the designated type and line number is stored
    char *copyStr = malloc(strlen(idAttribute)+1);
    if(copyStr==NULL) printError("malloc in newAST()");
    strcpy(copyStr, idAttribute);
    toReturn->idVal = copyStr;
    toReturn->lineNumber = lineNum;
  }
  if(natAttribute==NULL) {
      toReturn->natVal = NULL;
  }
  else {
      //toReturn set to type and line number saved
    char *copyStr = malloc(strlen(natAttribute)+1);
    if(copyStr==NULL) printError("malloc in newAST()");
    strcpy(copyStr, natAttribute);
    toReturn->natVal = copyStr;
    toReturn->lineNumber = lineNum;
  }
  return toReturn;
}

/* Append a new child AST node onto a parent's list of children */
ASTree *appendToChildrenList(ASTree *parent, ASTree *newChild) {
  if(parent==NULL) printError("append called with null parent");
  if(parent->children==NULL || parent->childrenTail==NULL)
    printError("append called with bad parent");
  if(newChild==NULL) printError("append called with null newChild");
  if(parent->childrenTail->data==NULL) //replace empty tail with new child
    parent->childrenTail->data = newChild;
  else { //tail of children is currently nonempty; append new child to list
    ASTList *newList = malloc(sizeof(ASTList));
    if(newList==NULL) printError("malloc in appendAST()");
    newList->data = newChild;
    newList->next = NULL;
    parent->childrenTail->next = newList;
    parent->childrenTail = newList;
  }
  return parent;
}

/* Print the type of this node and any node attributes */
void printNodeTypeAndAttribute(ASTree *t) {
  if(t==NULL) return;
  switch(t->typ) {
    case CLASS_DECL_LIST: printf("CLASS_DECL_LIST"); break;
    case CLASS_DECL: printf("CLASS_DECL"); break;
    case STATIC_VAR_DECL_LIST: printf("STATIC_VAR_DECL_LIST"); break;
    case STATIC_VAR_DECL: printf("STATIC_VAR_DECL"); break;
    case VAR_DECL_LIST: printf("VAR_DECL_LIST"); break;
    case VAR_DECL: printf("MOV_AST"); break;
    case METHOD_DECL_LIST: printf("METHOD_DECL_LIST"); break;
    case METHOD_DECL: printf("METHOD_DECL"); break;
    case NAT_TYPE: printf("NAT_TYPE"); break;
    case BOOL_TYPE: printf("BOOL_TYPE"); break;
    //id used by user mentioned in paranthesis 
    case AST_ID: printf("AST_ID(%s)", t->idVal); break;
    case EXPR_LIST: printf("EXPR_LIST"); break;
    case DOT_METHOD_CALL_EXPR: printf("DOT_METHOD_CALL_EXPR"); break;
    case METHOD_CALL_EXPR: printf("METHOD_CALL_EXPR"); break;
    case DOT_ID_EXPR: printf("DOT_ID_EXPR"); break;
    case ID_EXPR: printf("ID_EXPR"); break;
    case DOT_ASSIGN_EXPR: printf("DOT_ASSIGN_EXPR"); break;
    case ASSIGN_EXPR: printf("ASSIGN_EXPR"); break;
    case PLUS_EXPR: printf("PLUS_EXPR"); break;
    case MINUS_EXPR: printf("MINUS_EXPR"); break;
    case TIMES_EXPR: printf("TIMES_EXPR"); break;
    case EQUALITY_EXPR: printf("EQUALITY_EXPR"); break;
    case GREATER_THAN_EXPR: printf("GREATER_THAN_EXPR"); break;
    case NOT_EXPR: printf("NOT_EXPR"); break;
    case AND_EXPR: printf("AND_EXPR"); break;
    case INSTANCEOF_EXPR: printf("INSTANCEOF_EXPR"); break;
    case IF_THEN_ELSE_EXPR: printf("IF_THEN_ELSE_EXPR"); break;
    case FOR_EXPR: printf("FOR_EXPR"); break;
    case PRINT_EXPR: printf("PRINT_EXPR"); break;
    case READ_EXPR: printf("READ_EXPR"); break;
    case THIS_EXPR: printf("THIS_EXPR"); break;
    case NEW_EXPR: printf("NEW_EXPR"); break;
    case NULL_EXPR: printf("NULL_EXPR"); break;
    //Literal used by the user mentioned in paranthesis
    case NAT_LITERAL_EXPR: printf("NAT_LITERAL_EXPR (%s)", t->natVal); break;
    case TRUE_LITERAL_EXPR: printf("TRUE_LITERAL_EXPR"); break;
    case FALSE_LITERAL_EXPR: printf("FALSE_LITERAL_EXPR"); break;
    default: printError("unknown node type in printNodeTypeAndAttribute()");
  }  
}

/* print in Preorder Treversalwith appropriate spacing between the children */
void printASTree(ASTree *t, int depth) {
  if(t==NULL) return;
  printf("%d:",depth);
  int i=0;
  //spacing for the children to differentiate
  for(; i<depth; i++) printf("  ");
  printNodeTypeAndAttribute(t);
  //print the line number next to the type
  printf("(ends on line %d)", t->lineNumber);
  printf("\n");
  //recursively print all children
  ASTList *childListIterator = t->children;
  while(childListIterator!=NULL) {
    printASTree(childListIterator->data, depth+1);
    childListIterator = childListIterator->next;
  }
}

/* Print the AST to stdout with indentations marking tree depth. */
void printAST(ASTree *t) { printASTree(t, 0); }

