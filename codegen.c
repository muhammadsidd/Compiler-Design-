/*I Muhammad Siddiqui pleadge my honor that I have not and will not cheat on this assignment */

#include "codegen.h"
#include "ast.h"
#include "symtbl.h"
#define MAX_DISM_ADDR 65535

/* Global for the DISM output file */
FILE *fout;

/* Global to remember the nextunique label number to use*/
unsigned int labelNumber = 0;

void generateDISM(FILE *outputFile)
{
    fout = outputFile;

    genPrologue(-1, -1);

    codeGenExprs(mainExprs, -1, -1);

    genEpilogue(-1, -1);
}

void codeGenExpr(ASTree *t, int classNumber, int methodNumber)
{

    if (t->typ == NAT_LITERAL_EXPR)
    {
        fprintf(fout, "mov 1 %d\n", t->natVal);
        fprintf(fout, "str 6 0 1    ; M[SP] <- R[1] (A Nat Literal)\n");
        decSP();
    }

    else if (t->typ == FALSE_LITERAL_EXPR)
    {
        fprintf(fout, "mov 1 %d\n", t->natVal);
        fprintf(fout, "str 6 0 1    ; m[SP] <- R[1] (False Literal)\n");
        decSP();
    }

    else if (t->typ == TRUE_LITERAL_EXPR)
    {
        fprintf(fout, "mov 1 %d\n", t->natVal);
        fprintf(fout, "str 6 0 1    ; m[SP] <- R[1] (True Literal)\n");
        decSP();
    }

    else if(t->typ == ID_EXPR)
    {
        if(classNumber == -1)
        {
            fprintf(fout, "mov 1 %d\n", t->staticMemberNum); //get the static member number
            fprintf(fout, "sub 2 7 1 ; 65535 (main FP) - MemberNum to get the address of variable\n"); //calculate offset of the address
            fprintf(fout, "lod 3 2 0 ; load memory address of variable into R3\n");//load address
            fprintf(fout, "str 6 0 3 ; store the addr of local variable on the stack\n");//store address 
            decSP();
        }
    }

    else if (t->typ == NULL_EXPR)
    {
        fprintf(fout, "str 6 0 0    ; M[SP] < -0(null)\n");
        decSP();
    }

    else if (t->typ == NOT_EXPR)
    {
        int label1 = labelNumber++;
        int label2 = labelNumber++;

        codeGenExpr(t->children->data, classNumber, methodNumber);

        //Inverse Overation
        fprintf(fout, "lod 1 6 1    ; load M[SP + 1] for result of expr\n");
        fprintf(fout, "beq 1 0 #%d  ; If R1 == R0 jump to false case\n", label1);
        fprintf(fout, "mov 1 0      ; store 0 if value = 1\n");
        fprintf(fout, "str 6 1 1    ; m[SP+1] <- R[1]\n");
        fprintf(fout, "mov 1 #%d    ; jump and Exit \n", label2);
        fprintf(fout, "jmp 1 0\n");
        fprintf(fout, "#%d: mov 1 1\n", label1);
        fprintf(fout, "str 6 1 1    ; m[SP + 1] <- R[r1] invert of 0\n");
        fprintf(fout, "#%d: mov 0 0 ; exit label\n", label2);
        incSP();
    }

    else if (t->typ == PLUS_EXPR)
    {
        codeGenExpr(t->children->data, classNumber, methodNumber);
        codeGenExpr(t->children->next->data, classNumber, methodNumber);
        //PLUS OPERATION
        fprintf(fout, "lod 1 6 2 ; R[1]<- M[SP +2] \n");
        fprintf(fout, "lod 2 6 1 ; R[2]<- M[SP + 1] \n");
        fprintf(fout, "add 1 1 2 ; PLUS_EXPR opeartion \n");
        fprintf(fout, "str 6 2 1 ; M[SP] storing result \n");
        incSP();
    }

    else if (t->typ == MINUS_EXPR)
    {

        codeGenExpr(t->children->data, classNumber, methodNumber);
        codeGenExpr(t->children->next->data, classNumber, methodNumber);

        //MINUS OPERATION
        fprintf(fout, "lod 1 6 2 ; R[1]<- M[SP + 2] expr 1 \n");
        fprintf(fout, "lod 2 6 1 ; R[2]<- M[SP + 1] expr 2\n");
        fprintf(fout, "sub 1 1 2 ; subtract Operation\n");
        fprintf(fout, "str 6 2 1 ; m[SP] storing result\n");
        incSP();
    }

    else if (t->typ == TIMES_EXPR)
    {

        codeGenExpr(t->children->data, classNumber, methodNumber);
        codeGenExpr(t->children->next->data, classNumber, methodNumber);

        //TIMES OPERATION
        fprintf(fout, "lod 1 6 2 ; R[1]<- M[SP + 2] for first expression\n");
        fprintf(fout, "lod 2 6 1 ; R[2]<- M[SP + 1] for second expression\n");
        fprintf(fout, "mul 1 1 2 ; multiply operation on the stack\n");
        fprintf(fout, "str 6 2 1 ; m[SP] storing result\n");
        incSP();
    }

    else if (t->typ == GREATER_THAN_EXPR)
    {
        int label1 = labelNumber++;
        int label2 = labelNumber++;

        codeGenExpr(t->children->data, classNumber, methodNumber);
        codeGenExpr(t->children->next->data, classNumber, methodNumber);

        fprintf(fout, "lod 1 6 2    ; R[1]<--M[SP + 2] for expr1\n");
        fprintf(fout, "lod 2 6 1    ; R[2]<--M[SP + 1] for expr2\n");
        fprintf(fout, "blt 1 2 #%d  ; Jump to label 1 if expr1 < expr2 is true\n", label1); //jump on true
        //continue if false
        incSP();
        fprintf(fout, "str 6 1 0    ; m[SP] <- R[r0] store false on stack\n");
        fprintf(fout, "mov 1 #%d    ; label for finish\n", label2);
        fprintf(fout, "jmp 1 0      ; jump to label2\n");
        fprintf(fout, "#%d: mov 1 1 ; valid case start\n", label1);
        fprintf(fout, "str 6 1 1    ; store true statement on stack\n");
        fprintf(fout, "#%d: mov 0 0 ; exit\n", label2);
        incSP();
    }

    else if (t->typ == EQUALITY_EXPR)
    {
        int label1 = labelNumber++;
        int label2 = labelNumber++;

        codeGenExpr(t->children->data, classNumber, methodNumber);
        codeGenExpr(t->children->next->data, classNumber, methodNumber);

        fprintf(fout, "lod 1 6 2    ; R[1]<-- M[SP + 2] for expr1\n");
        fprintf(fout, "lod 2 6 1    ; R[2]<-- M[SP + 1] for expr2\n");
        fprintf(fout, "beq 1 2 #%d  ; Jump to label 1 if expr1 == expr2 is true\n", label1); //jump on true
        //continue if false
        fprintf(fout, "str 6 2 0    ; M[SP] <- R[r0] store false on stack\n");
        fprintf(fout, "mov 1 #%d    ; label for finish\n", label2);
        fprintf(fout, "jmp 1 0      ; jump to label2\n");
        fprintf(fout, "#%d: mov 1 1 ; valid case start\n", label1);
        fprintf(fout, "str 6 2 1    ; store true statement on stack\n");
        fprintf(fout, "#%d: mov 0 0 ; exit\n", label2);
        incSP();
    }

    else if (t->typ == PRINT_EXPR)
    {
        codeGenExpr(t->children->data, classNumber, methodNumber);
        fprintf(fout, "lod 1 6 1 ; load M[SP+1] for printing\n");
        fprintf(fout, "ptn 1\n");
    }

    else if (t->typ = READ_EXPR)
    {
        fprintf(fout, "rdn 1\n");
        fprintf(fout, "str 6 0 1 ; store M[SP] for reading\n");
        decSP();
    }

    else if (t->typ == AND_EXPR)
    {

        int label1 = labelNumber++;
        int label2 = labelNumber++;

        codeGenExpr(t->children->data, classNumber, methodNumber);
        codeGenExpr(t->children->next->data, classNumber, methodNumber);

        //check first expr
        fprintf(fout, "lod 1 6 2    ; R[1]<- M[SP+2] load 1st expr\n");
        fprintf(fout, "beq 1 0 #%d  ; if false jump and return false\n", label1);
        
        //check second expr
        fprintf(fout, "lod 1 6 1    ; R[1]<- M[SP+1] 2nd expr\n");
        fprintf(fout, "beq 1 0 #%d  ; if false jump and return false \n", label1);
        
        //Operation on true 
        fprintf(fout, "mov 1 1      ; continue if expr1 & expr 2 are true\n");
        fprintf(fout, "str 6 2 1    ;  M[SP+2] <-- M[R1] return true \n");
        fprintf(fout, "mov 1 #%d    ;  R[1]<-exit \n", label2);
        fprintf(fout, "jmp 1 0      ; jump to [R1 + 0]\n");
        //False and exit
        fprintf(fout, "#%d: str 6 2 0  ;  return 0 on stack \n", label1);
        fprintf(fout, "#%d: mov 0 0 ; Exit\n", label2);
        incSP();
        
    }

    else if (t->typ == ASSIGN_EXPR)
    {
        codeGenExpr(t->children->next->data, classNumber, methodNumber);
        //load 
        fprintf(fout, "lod 1 6 1 ; R[1]<- M[SP + 1] load expr \n");

        //Main block Execution
        if (classNumber == -1)
        {
            
            fprintf(fout, "mov 2 %d\n", t->children->data->staticMemberNum);
            fprintf(fout, "sub 3 7 2 ; Get addr of the local var\n");
            fprintf(fout, "str 3 0 1 ; store the RHS value into local variable address\n");
            incSP();
        }
    }

    else if (t->typ == IF_THEN_ELSE_EXPR)
    {
        
        int elseLabel  = labelNumber++;
        int endLabel = labelNumber++;

        //If
        codeGenExpr(t->children->data, classNumber, methodNumber);

        fprintf(fout, "lod 1 6 1    ; M[SP + 1] -> R[1] load expr \n");
        fprintf(fout, "beq 1 0 #%d  ; if false go to the else block\n", elseLabel);

        //Then EXPR LIST
        codeGenExprs(t->children->next->data, classNumber, methodNumber);
        /****END OF THEN*****/

        fprintf(fout, "mov 2 #%d\n  ; R[2]<- EndLabel", endLabel);
        fprintf(fout, "jmp 2 0      ; Jump and Exit\n");
        
        fprintf(fout, "#%d: mov 0 0 ; else block\n", elseLabel);
        //Else EXPR LIST
        codeGenExprs(t->children->next->next->data, classNumber, methodNumber);
        /*****END OF ELSE****/

        fprintf(fout, "#%d: mov 0 0 ; exit if_then_else\n", endLabel);

    }

    else if (t->typ == FOR_EXPR)
    {
        
        int label1 = labelNumber++;
        int label2 = labelNumber++;

        //loop intializer
        codeGenExpr(t->children->data, classNumber, methodNumber);
        fprintf(fout, "#%d: mov 0 0 ; Label for the loop\n", label1);
        //fprintf(fout, "lod 1 6 1; load the initial value of the loop\n");
        
        //Condition
        codeGenExpr(t->children->next->data, classNumber, methodNumber);
        fprintf(fout, "lod 1 6 1    ; R[1]<-- M[S+1]\n");
        fprintf(fout, "beq 1 0 #%d  ; Jump and Exit if False\n", label2); 
        
        //Loop Body
        codeGenExprs(t->children->next->next->next->data, classNumber, methodNumber);
        
        //loop Conclusion
        codeGenExpr(t->children->next->next->data, classNumber, methodNumber);
       /*fprintf(fout, "mov 3 1");
        fprintf(fout, "add 1 1 3 ");*/
        fprintf(fout, "mov 1 #%d    ; R[1] <- Loop Label \n", label1);
        fprintf(fout, "jmp 1 0      ; Jump To loop\n"); //Repeat until condition at M[S+1] is 0;
        fprintf(fout, "#%d: mov 0 0 ; exit label for loop\n", label2);

    }

    else if(t->typ == DOT_ASSIGN_EXPR)
    {
        codeGenExpr(t->children->data, classNumber, methodNumber);
        codeGenExpr(t->childrenTail->data, classNumber, methodNumber);

        //DOT Assign operation        
        fprintf(fout, "lod 1 6 2 ; R[1]<- M[S+2] Left hand side from exp1\n");
        fprintf(fout, "lod 2 6 1 ; R[2]<- M[S+1] right hand side from exp2\n");
        fprintf(fout, "str 1 %d 2 ; store value of RHS into member in LHS\n", t->staticMemberNum + 1);
           
        
    }
  
}

void codeGenExprs(ASTree *expList, int classNumber, int methodNumber)
{

    while (expList->children)
    {
        codeGenExpr(expList->children->data, classNumber, methodNumber);
        expList->children = expList->children->next;
    }
}

//Pop Stack
void incSP()
{   //check to see if it doesnt go out of bound
    fprintf(fout, "mov 1 1\n");
    fprintf(fout, "add 6 6 1    ; SP++\n");
    fprintf(fout, "mov 2 65536  ; MAX_ADDR\n");
    fprintf(fout, "blt 2 6 #%d  ; jump when MAX_ADDR < SP \n", labelNumber);
    fprintf(fout, "mov 1 88     ; error code 88 => out of stack memory \n");
    fprintf(fout, "hlt 1        ; out of stack memory! (MAX_ADDR>=SP)\n");
    fprintf(fout, "#%d: mov 0 0\n", labelNumber);
}

void decSP()
{
    fprintf(fout, "mov 1 1\n");
    fprintf(fout, "sub 6 6 1    ; SP--\n");
    fprintf(fout, "blt 5 6 #%d  ; branch if HP<SP \n", labelNumber);
    fprintf(fout, "mov 1 77     ; error code 77 => out of stack memory \n");
    fprintf(fout, "hlt 1        ; out of stack memory! (HP>=SP)\n");
    fprintf(fout, "#%d: mov 0 0\n", labelNumber);
}

void genPrologue(int classNumber, int methodNumber)
{
    //initialize the pointers 
    fprintf(fout, "mov 7 65535  ; initialize FP\n");
    fprintf(fout, "mov 6 65535  ; initialize SP\n");
    fprintf(fout, "mov 5 1      ; initialize HP\n");
    
    //Place holder for locals 
    int i = 0;
    for (i; i < numMainBlockLocals; i++)
    {
        fprintf(fout, "mov 1 0      ; store 0 as place holder for %s\n", mainBlockST[i].varName);
        fprintf(fout, "str 6 0 1    ; stack space created M[SP]\n");
        decSP();
    }
}

void genEpilogue(int classNumber, int methodNumber)
{
    //Halt and end
    fprintf(fout, "mov 1 0\n");
    fprintf(fout, "hlt 1 ; End here\n");
}

