/*I PLEDGE MY HONOR THAT I HAVE NOT CHEATED AND WILL NOT CHEAT ON THIS ASSIGNMENT*/
#include "typecheck.h"
#include "ast.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXNUM 15

void graph();
void addEdge(ClassDecl cl1, ClassDecl cl2);
int edgeexists(ClassDecl cl1, ClassDecl cl2);

/*struct defines the edges used for subtyping */
typedef struct _Edge
{

    ClassDecl A;
    ClassDecl B;

} Edge;

//the array itself which contains all the edges
//with the variable count which is use to iterate thru this array
Edge *edges;
int count = 0;

/*Helper function to add the edge between the super and sub type*/
void addEdge(ClassDecl cl1, ClassDecl cl2)
{
    edges[count].A = cl1;
    edges[count].B = cl2;

    //incremeant count eerytime this method is called
    count++;
}

//return non zero iff the one class extends another class
int edgeexists(ClassDecl cl1, ClassDecl cl2)
{
    int i;
    int found = 0;

    for (i = 0; i < MAXNUM; i++)
    {
        //if the edge lists contains the class name and the other end of the edge it returns true
        if (strcmp(edges[i].A.className, cl1.className) == 0 && strcmp(edges[i].B.className, cl2.className) == 0)
        {
            found = 1;
            return 1; //true
        }
        else
        {
            continue;
        }
    }
    if (found == 0)
    {
        return 0; //false
    }
}

//helper function to add edges between two vertices
void graph()
{
    int i;
    int j;
    //iterate thru the class symbol table
    for (i = 0; i < numClasses; i++)
    { //iterate agian and check if the super class number in the given class matches any of the class numbers
        for (j = 0; j < numClasses; j++)
        {
            if (classesST[i].superclass == j)
            { //if matching number exits add edge between them
                addEdge(classesST[j], classesST[i]);
            }
        }
    }
}

//type checker function verifies if the type is correct in a given program
void typecheckProgram()
{
    //declare variable to be used in the for loops
    int i;
    int j;
    int k;
    int l;
    int m;
    int n;
    int message;

    //initialize the size of the edges array
    edges = malloc(sizeof(struct _Edge) * MAXNUM);

    //graph classes of the whole program into vertices and add edges between them iff they exist
    graph(wholeProgram);

    //type checker program begins by checking all the classes declared
    //iterates thru the class and checks if the duplicated value for the static variable exists
    for (i = 0; i < numClasses; i++)
    {

        for (j = 0; j < classesST[i].numStaticVars - 1; j++)
        {
            for (k = j + 1; k < classesST[i].numStaticVars; k++)
            {
                //use string compare to compare the name of the static vars in the classess
                if (strcmp(classesST[i].staticVarList[j].varName, classesST[i].staticVarList[k].varName) == 0)
                {
                    printf("duplicate static variable name at line %d", classesST[i].varList[k].varNameLineNumber);
                    exit(0);
                }
                else
                {
                    continue;
                }
            }
        }
        //iterates thru all the non static ariables in the class and checks for duplicates
        for (m = 0; m < classesST[i].numVars - 1; m++)
        {
            for (n = m + 1; n < classesST[i].numVars; n++)
            {
                if (strcmp(classesST[i].varList[m].varName, classesST[i].varList[n].varName) == 0)
                { //line printed on which the duplicate exists
                    printf("duplicate variable name at line %d", classesST[i].varList[n].varNameLineNumber);
                    exit(0);
                }
                else
                {
                    continue;
                }
            }
        }
        //method duplicates checker in the class
        for (l = 0; l < classesST[i].numMethods - 1; l++)
        {
            for (int r = l + 1; r < classesST[i].numMethods; r++)
            {

                if (strcmp(classesST[i].methodList[l].methodName, classesST[i].methodList[r].methodName) == 0)
                {
                    printf("duplicate method name at line %d", classesST[i].methodList[r].methodNameLineNumber);
                    exit(0);
                }
                else
                {
                    continue;
                }
            }
            //each method is passed into typeExprs for its expressions to be type checked
            message = typeExprs(classesST[i].methodList[l].bodyExprs, i, l);
        }
        //tyoe checker for the last method done seprately because for loop ends on numMethod -2
        message = typeExprs(classesST[i].methodList[classesST[i].numMethods - 1].bodyExprs, i, classesST[i].numMethods - 1);
    }

    //if typeExprs returns -5 the program exits with an error code, line on which the error occurs is also printed.
    //along with the error specifics
    if (message == -5)
    {
        exit(0);
    }

    //main block type checking is done seprately
    for (i = 0; i < numMainBlockLocals - 1; i++)
    {
        for (j = i + 1; j < numMainBlockLocals; j++)
        { //check for duplicate variable names
            if (strcmp(mainBlockST[i].varName, mainBlockST[j].varName) == 0)
            {
                printf("duplicate variable name at line %d", mainBlockST[j].varNameLineNumber);
            }
            else
            {
                continue;
            }
        }
    }
    //main blcok is passed into typeEprs with -1 and 0 as parameter to differenciate between classes and methods respectively
    message = typeExprs(mainExprs, -1, 0);
    //if main method returns -5 exit with an error
    if (message == -5)
    {
        exit(0);
    }
}

//bolen function to check if one class is a subtype of the other
int isSubtype(int sub, int super)
{
    int i;
    int found = 0;
    for (i = 0; i < MAXNUM; i++)
    { //edge exists iff the super is a super class of sub.
        if (edgeexists(classesST[super], classesST[sub]) == 1)
        {
            found = 1; //return true
            return found;
        }
        else
        {
            continue;
        }
    }

    if (found == 0)
    {
        return found; //return false
    }
}

//Returns the type of the expression AST in the given context.
int typeExpr(ASTree *t, int classContainingExpr, int methodContainingExpr)
{
    int child1; //first child of the node
    int child2; //second child of the node
    int child3; //3rd child of the node
    int child4; // 4th ,, ,, , ,
    int type;   //the type to be saved for further calculations

    if (t == NULL)
    {
        printf("undefined Error on %d\n", t->lineNumber);
        return -5;
    }
    if (t->typ == NAT_LITERAL_EXPR)
    {
        return -1; //-1 for nat
    }
    else if (t->typ = FALSE_LITERAL_EXPR)
    {
        return -2; //-2 for bool type
    }
    else if (t->typ = TRUE_LITERAL_EXPR)
    {
        return -2; //-2 for bool type
    }
    else if (t->typ == NULL_EXPR)
    {
        return -3; //-3 for null
    }
    else if (t->typ == AST_ID)
    {
        //variables for the loop
        int i;
        int j;
        int k;
        //bool variable
        int found = 0;

        /* CHECK IF AST IS OF TYPE CLASS*/
        for (i = 0; i < numClasses; i++)
        {
            if (strcmp(t->idVal, classesST[i].className) == 0)
            {
                found = 1;
                return i; //returns the index as its type
            }
        }
        /*CHECK IF THE AST IS OF TYPE STATIC VARIABLE IN A GIVEN CLASS */
        for (j = 0; j < classesST[classContainingExpr].numStaticVars; j++)
        { //compare the name of the id with the variable name
            if (strcmp(t->idVal, classesST[classContainingExpr].staticVarList[j].varName) == 0)
            {
                found = 1;
                //return the type of the variable from symbo table
                return classesST[classContainingExpr].staticVarList[j].type;
            }
        }
        /*CHECK IF THE AST IS OF TYPE NON STATIC VARIABLE IN A GIVEN CLASS*/
        for (k = 0; k < classesST[classContainingExpr].numVars; k++)
        {
            if (strcmp(t->idVal, classesST[classContainingExpr].varList[k].varName) == 0)
            {
                found = 1;
                //return the type of the ast variable from the symbol table
                return classesST[classContainingExpr].varList[k].type;
            }
        }

        if (found == 0)
        {
            printf("variable on line %d not found", t->lineNumber);
            return -5;
        }
    }
    //type of the id exprs returned
    else if (t->typ == ID_EXPR)
    {
        return typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
    }

    //not expression return as bool type
    else if (t->typ == NOT_EXPR)
    {
        //recursively find its children
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
        if (child1 == -2)
        {
            return child1; //returns bool
        }
        else
        {
            printf("Bool error on line %d\n", t->lineNumber);
            return -5; //error
        }
    }
    //new should be of type new (e) and should return the type of its children
    else if (t->typ == NEW_EXPR)
    {
        return typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
    }
    //plus exprs should have 2 children each of type nat
    else if (t->typ == PLUS_EXPR)
    { //recursively find the children
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
        child2 = typeExpr(t->children->next->data, classContainingExpr, methodContainingExpr);
        //check the type of the children and return -1 iff both the children are of type nat
        if (child1 == -1 && child2 == -1)
        {
            return -1; //returns nat type
        }
        else
        {
            printf("Expression should be nat type %d\n", t->lineNumber);
            return -5; //error
        }
    }
    //minus exprs should have 2 children each of type nat
    else if (t->typ == MINUS_EXPR)
    { //recursively find the children
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
        child2 = typeExpr(t->children->next->data, classContainingExpr, methodContainingExpr);
        //check the type of the children and return -1 iff both the children are of type nat
        if (child1 == -1 && child2 == -1)
        {
            return -1; //returns nat type
        }
        else
        {
            printf("Expression should be nat type %d\n", t->lineNumber);
            return -5; //error
        }
    }
    //time expr should have 2 children each of type nat
    else if (t->typ == TIMES_EXPR)
    { //recursively find the children
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
        child2 = typeExpr(t->children->next->data, classContainingExpr, methodContainingExpr);
        //check the type of the children and return -1 iff both the children are of type nat
        if (child1 == -1 && child2 == -1)
        {
            return -1; //returns nat type
        }
        else
        {
            printf("Expression should be nat type %d\n", t->lineNumber);
            return -5; //error
        }
    }
    //returns a boolean expression should have two children for comaparison
    else if (t->typ == GREATER_THAN_EXPR)
    {
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
        child2 = typeExpr(t->children->next->data, classContainingExpr, methodContainingExpr);

        if (child1 == -1 && child2 == -1)
        {
            return -2; //bool returned
        }
        else
        {
            printf("Expected a Bool on line %d\n", t->lineNumber);
            return -5; //error
        }
    }
    //returns a boolean expression should have two children for comaparison
    else if (t->typ == EQUALITY_EXPR)
    {
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
        child2 = typeExpr(t->children->next->data, classContainingExpr, methodContainingExpr);

        if (child1 == -1 && child2 == -1)
        {
            return -2; //bool returned
        }
        else
        {
            printf("Expected a Bool return type on line %d\n", t->lineNumber);
            return -5; //error
        }
    }
    /*The assignment expression ID=E (where IDis anidentifier and E is an expression) is well
     typed (with whatever type  IDhas) exactly when (1) IDis a well-typed variableand (2) E’s  
     type  is  a  subtype  of  ID’s  type.*/
    else if (t->typ == ASSIGN_EXPR)
    {
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
        child2 = typeExpr(t->children->next->data, classContainingExpr, methodContainingExpr);
        //to check the condition
        if (child1 == child2 >= 0 && isSubtype(child2, child1) == 1)
        {
            return child1;
        }
        else
        {
            printf("Expression Should be a of a matching type on line  %d\n", t->lineNumber);
            return -5;
        }
    }
    //return boolean
    else if (t->typ == AND_EXPR)
    { //recursively get the children
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
        child2 = typeExpr(t->children->next->data, classContainingExpr, methodContainingExpr);
        if (child1 == -2 && child2 == -2)
        {
            return -2; //bool returned
        }
        else
        {
            printf("Expected a bool return type on line %d\n", t->lineNumber);
            return -5; //error
        }
    }
    //returns a nat type
    else if (t->typ == PRINT_EXPR)
    {
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
        if (child1 == -1)
        {
            return child1;
        }
        else
        {
            printf("print exp should return a nat type %d\n", t->lineNumber);
            return -5; //error
        }
    }
    //should return a nat type
    else if (t->typ = READ_EXPR)
    {
        return -1;
    }
    //if statement follows below listed rules
    else if (t->typ == IF_THEN_ELSE_EXPR)
    {

        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);              //if condition
        child2 = typeExprs(t->children->next->data, classContainingExpr, methodContainingExpr);       //then codition
        child3 = typeExprs(t->children->next->next->data, classContainingExpr, methodContainingExpr); //else condition
        //child 1 bool type
        //or child 2 and child 3 have nat type
        //or child 2 and 3 both have bool type
        //or child 2 or child 3 both have object type
        if (child1 == -2 && ((child2 == -1 && child3 == -1) || (child2 == -2 && child3 == -2) ||
                             (child2 >= 0 && child3 >= 0)))
        {
            //return bool type if child 2 and child 3 are nat types or child 2 and child 3 are bool types
            if ((child2 == child3 == -1) || (child2 == child3 == -2))
            {
                return -2;
            }
            //return join of child 2 and child 3 if they are of object types
            else if (child2 == child3 >= 0 && (isSubtype(child3, child2) || isSubtype(child2, child3)))
            {
                return child3;
            }
            else
            {
                printf("Expression should be of a matching type %d\n", t->lineNumber);
                return -5;
            }
        }
        else
        {
            printf("IF expression expects a bool type %d\n", t->lineNumber);
            return -5;
        }
    }
    /*FOR LOOP*/
    else if (t->typ == FOR_EXPR)
    {
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);                    //loop-initializationsubexpression,
        child2 = typeExpr(t->children->next->data, classContainingExpr, methodContainingExpr);              // loop-test subexpression
        child3 = typeExpr(t->children->next->next->data, classContainingExpr, methodContainingExpr);        //loop-conclusion subexpression
        child4 = typeExprs(t->children->next->next->next->data, classContainingExpr, methodContainingExpr); //and loop-body expression-list
        //returns bool type iff when its child1, child3, child 4 are all well typed, and child2 has booltype.
        if (child1 != -5 && child3 != -5 && child4 != -5 && child2 == -2)
        {
            //return nat type
            return -1;
        }
        else
        {
            printf("For loop Condition error on  %d\n", t->lineNumber);
            return -5;
        }
    }
    /*INSTANCE OF*/
    else if (t->typ == INSTANCEOF_EXPR)
    {
        int i;
        int ispresent = 0;  //flag variable 

        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);        //anyobject type 
        child2 = typeExpr(t->children->next->data, classContainingExpr, methodContainingExpr);  //valid classname


        for (i = 0; i < numClasses; i++)    //check if child 2 returns a valid AST classname
        {
            if (strcmp(classesST[child2].className, classesST[i].className) == 0)
            {
               ispresent = 1; //set flag to 1 
            }
        }
        if (child1 >= 1 && ispresent == 1)  //if condition true return bool type for instance of 
        {
            return -2;
        }
        else
        {
            printf("Instance of Error on %d\n", t->lineNumber);
            return -5;
        }
    }
    else if (t->typ == THIS_EXPR)
    {
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
        if (child1 >= 1)
        {
            return child1;
        }
        else
        {
            printf("undefined Error on %d\n", t->lineNumber);
            return -5;
        }
    }
    else if (t->typ == DOT_ID_EXPR)
    {
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
        child2 = typeExpr(t->children->next->data, classContainingExpr, methodContainingExpr);

        if (child1 == type && child2 == type)
        {
            return type;
        }
        else
        {
            printf("undefined Error on %d\n", t->lineNumber);
            return -5;
        }
    }
    else if (t->typ == DOT_METHOD_CALL_EXPR)
    {
        child1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
        child2 = typeExpr(t->children->next->data, classContainingExpr, methodContainingExpr);

        if (child1 == type && child2 == type)
        {
            return type;
        }
        else
        {
            printf("undefined Error on %d\n", t->lineNumber);
            return -5;
        }
    }
}
//Helper fuction for every expression list, this is mutally recursive in case of for loops and if statements. 
int typeExprs(ASTree *t, int classContainingExprs, int methodContainingExprs)
{

    while (t->children)
    {
        typeExpr(t->children->data, classContainingExprs, methodContainingExprs);
        t->children = t->children->next;
    }
}
