#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "command.h"
#include "utility.h"

List setDefaultParam(CmdCount cmdCount)
{
    /*
        if SP < 256 then SP = 256 
        Es. SP = 0   --> 256 - 0 > 0
        Es. SP = 261 --> 256 - 261 <= 0
        @256
        D=A
        @SP
        D=D-M
        @START_EXE  // inizia se D <= 0
        D;JLE
        @256
        D=A
        @SP
        M=D
        (START_EXE)
        @Sys.init // funzione di init
        0;JMP
    */

    List out;
    init(&out);

    push(&out, "@256");
    push(&out, "D=A");
    push(&out, "@SP");
    push(&out, "D=D-M");
    push(&out, "@START_EXE");
    push(&out, "D;JLE");
    push(&out, "@256");
    push(&out, "D=A");
    push(&out, "@SP");
    push(&out, "M=D");
    push(&out, "@START_EXE");
    push(&out, "0;JMP");

    /*
        usare una template function costa 7 righe
        quindi la funzione stessa è lunga uguale o meno
        non si utilizza
    */

    /*
        call function 
        template steps

        // implementazione - 43 (39) righe
        (call)
        @SP
        M=M+1
        @LCL
        D=M
        @SP
        A=M
        M=D
        @SP
        M=M+1
        @ARG
        D=M
        @SP
        A=M
        M=D
        @SP
        M=M+1
        @THIS
        D=M
        @SP
        A=M
        M=D
        @SP
        M=M+1
        @THAT
        D=M
        @SP
        A=M
        M=D
        @SP
        M=M+1
        @R13
        D=M
        @SP
        D=M-D
        @ARG
        M=D
        @SP
        D=M
        @LCL
        M=D
        @R15
        A=M
        0;JMP

        // utilizzo - 17
        // call test.fun 2
        @return$test.fun.0
        D=A
        @SP
        A=M
        M=D
        @nArgs
        D=A
        @R13
        M=D
            @return.call.1
            D=A
            @R15
            M=D
            @call
            0;JMP
            (return.call.1)
        @test.fun
        0;JMP
        (return$test.fun.0)
    */
    if (cmdCount._call > 1)
    {
        push(&out, "(call)");

        push(&out, "@SP");
        push(&out, "M=M+1");

        push(&out, "@LCL");
        push(&out, "D=M");
        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=D");

        push(&out, "@SP");
        push(&out, "M=M+1");

        push(&out, "@ARG");
        push(&out, "D=M");
        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=D");

        push(&out, "@SP");
        push(&out, "M=M+1");

        push(&out, "@THIS");
        push(&out, "D=M");
        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=D");

        push(&out, "@SP");
        push(&out, "M=M+1");

        push(&out, "@THAT");
        push(&out, "D=M");
        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=D");

        push(&out, "@SP");
        push(&out, "M=M+1");

        push(&out, "@R13");
        push(&out, "D=M");
        push(&out, "@SP");
        push(&out, "D=M-D");

        push(&out, "@ARG");
        push(&out, "M=D");

        push(&out, "@SP");
        push(&out, "D=M");

        push(&out, "@LCL");
        push(&out, "M=D");

        push(&out, "@R15");
        push(&out, "A=M");
        push(&out, "0;JMP");
    }

    /*
        return function
        template steps

        // implementazione - 63 (59) righe
        (return)
        @5          // RIPRISTINO @R14
        D=A
        @LCL
        A=M-D
        D=M
        @R14
        M=D
        @SP         // ESEGUO IL POP ARGUMENT 0
        M=M-1
        @0
        D=A
        @ARG
        D=M+D
        @R13
        M=D
        @SP
        A=M
        D=M
        @R13
        A=M
        M=D
        @LCL       // IMPOSTO @R13 (TEMP) = LCL
        D=M
        @R13
        M=D
        @ARG       // IMPOSTO SP = ARG+1
        D=M
        @SP
        M=D+1
        @1         // RIPRISTINO THAT
        D=A
        @R13
        A=M-D
        D=M
        @THAT
        M=D
        @2         // RIPRISTINO THIS
        D=A
        @R13
        A=M-D
        D=M
        @THIS
        M=D
        @3         // RIPRISTINO ARG
        D=A
        @R13
        A=M-D
        D=M
        @ARG
        M=D
        @4         // RIPRISTINO LCL
        D=A
        @R13
        A=M-D
        D=M
        @LCL
        M=D
        @R14
        A=M
        0;JMP
        @R15         // torna indietro
        A=M
        0;JMP

        // utilizzo - 7
        @return.return.1
        D=A
        @R15
        M=D
        @return
        0;JMP
        (return.return.1)
    */
    if (cmdCount._return > 1)
    {
        push(&out, "(return)");

        push(&out, "// RIPRISTINO @R14");
        push(&out, "@5");
        push(&out, "D=A");
        push(&out, "@LCL");
        push(&out, "A=M-D");
        push(&out, "D=M");
        push(&out, "@R14");
        push(&out, "M=D");

        /*
            pop argument 0
            riposiziono il risultato della funzione
        */

        int error = 0;
        push(&out, "// ESEGUO IL POP ARGUMENT 0");
        List popArg0 = popCmd("", NULL, "argument", "0", &error);
        while (popArg0.head != NULL)
        {
            push(&out, popArg0.head->text);
            popArg0.head = popArg0.head->next;
        }
        delete (&popArg0);

        /*
            @LCL
            D=M
            @R13
            M=D   --> RAM[13] = D (RAM[1])
        */

        push(&out, "// IMPOSTO @R13 (TEMP) = LCL");
        push(&out, "@LCL");
        push(&out, "D=M");
        push(&out, "@R13");
        push(&out, "M=D");

        /*
            @ARG
            D=M
            @SP
            M=D+1
        */

        push(&out, "// IMPOSTO SP = ARG+1");
        push(&out, "@ARG");
        push(&out, "D=M");
        push(&out, "@SP");
        push(&out, "M=D+1");

        /*
            setto THAT = RAM[RAM[13]-1]
            @1
            D=A
            @R13
            A=M-D  --> A = RAM[13](317)-D(1)
            D=M    --> D = RAM[RAM[13]-1]
            @THAT
            M=D 
        */

        push(&out, "// RIPRISTINO THAT");
        push(&out, "@R13");
        push(&out, "A=M-1");
        push(&out, "D=M");
        push(&out, "@THAT");
        push(&out, "M=D");

        /*
            setto THIS = RAM[RAM[13]-2]
            @2
            D=A
            @R13
            A=M-D  --> A = RAM[13](317)-D(2)
            D=M    --> D = RAM[RAM[13]-2]
            @THIS
            M=D 
        */

        push(&out, "// RIPRISTINO THIS");
        push(&out, "@2");
        push(&out, "D=A");
        push(&out, "@R13");
        push(&out, "A=M-D");
        push(&out, "D=M");
        push(&out, "@THIS");
        push(&out, "M=D");

        /*
            setto ARG = RAM[RAM[13]-3]
            @3
            D=A
            @R13
            A=M-D  --> A = RAM[13](317)-D(3)
            D=M    --> D = RAM[RAM[13]-3]
            @ARG
            M=D 
        */

        push(&out, "// RIPRISTINO ARG");
        push(&out, "@3");
        push(&out, "D=A");
        push(&out, "@R13");
        push(&out, "A=M-D");
        push(&out, "D=M");
        push(&out, "@ARG");
        push(&out, "M=D");

        /*
            setto LCL = RAM[RAM[13]-4]
            @4
            D=A
            @R13
            A=M-D  --> A = RAM[13](317)-D(4)
            D=M    --> D = RAM[RAM[13]-4]
            @LCL
            M=D 
        */

        push(&out, "// RIPRISTINO LCL");
        push(&out, "@4");
        push(&out, "D=A");
        push(&out, "@R13");
        push(&out, "A=M-D");
        push(&out, "D=M");
        push(&out, "@LCL");
        push(&out, "M=D");

        /*
            goto RAM[RAM[14]]
            @R14
            A=M   --> A = RAM[14]
            D=M   --> D = RAM[RAM[14]]
            @R14
            M=D   --> RAM[14] = D (RAM[RAM[14]])
            0;JMP

        */

        push(&out, "@R14");
        push(&out, "A=M");
        push(&out, "0;JMP");

        push(&out, "@R15");
        push(&out, "A=M");
        push(&out, "0;JMP");
    }

    /*
        eq function
        template steps

        // implementazione - 23 (19) righe
        (eq)
        @SP   
        AM=M-1  
        D=M  
        @SP  // leggo quello prima
        AM=M-1
        D=M-D
        @IF_EQ_TEMPLATE_STEP
        D;JEQ
        @SP
        A=M
        M=0
        @END_IF_EQ_TEMPLATE_STEP
        0;JMP
        (IF_EQ_TEMPLATE_STEP)
        @SP
        A=M
        M=-1
        (END_IF_EQ_TEMPLATE_STEP)
        @SP         // aumento SP
        M=M+1
        @R15 // torna indietro
        A=M
        0;JMP

        // utilizzo - 7 righe
        @return.eq.1
        D=A
        @R15
        M=D
        @eq
        0;JMP
        (return.eq.1) 
   */
    if (cmdCount._eq > 1)
    {
        push(&out, "(eq)");
        push(&out, "@SP");
        push(&out, "AM=M-1");
        push(&out, "D=M");

        push(&out, "@SP");
        push(&out, "AM=M-1");
        push(&out, "D=M-D");

        push(&out, "@IF_EQ_TEMPLATE_STEP");

        push(&out, "D;JEQ");

        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=0");

        push(&out, "@END_IF_EQ_TEMPLATE_STEP");

        push(&out, "0;JMP");

        push(&out, "(IF_EQ_TEMPLATE_STEP)");

        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=-1");

        push(&out, "(END_IF_EQ_TEMPLATE_STEP)");

        push(&out, "@SP");
        push(&out, "M=M+1");

        push(&out, "@R15");
        push(&out, "A=M");
        push(&out, "0;JMP");
    }

    /*
        lt function
        template steps

        // implementazione - 23 (19) righe
        (lt)    
        @SP
        AM=M-1
        D=M
        @SP
        AM=M-1
        D=M-D
        @IF_LT_TEMPLATE_STEP
        D;JLT
        @SP
        A=M
        M=0
        @END_IF_LT_TEMPLATE_STEP
        0;JMP
        (IF_LT_TEMPLATE_STEP)
        @SP
        A=M
        M=-1
        (END_IF_LT_TEMPLATE_STEP)
        @SP
        M=M+1
        @R15
        A=M
        0;JMP


        // utilizzo - 7 righe
        @return.lt.1
        D=A
        @R15
        M=D
        @lt
        0;JMP
        (return.lt.1) 
    */
    if (cmdCount._lt > 1)
    {
        push(&out, "(lt)");
        push(&out, "@SP");
        push(&out, "AM=M-1");
        push(&out, "D=M");

        push(&out, "@SP");
        push(&out, "AM=M-1");
        push(&out, "D=M-D");

        push(&out, "@IF_LT_TEMPLATE_STEP");

        push(&out, "D;JLT");

        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=0");

        push(&out, "@END_IF_LT_TEMPLATE_STEP");

        push(&out, "0;JMP");

        push(&out, "(IF_LT_TEMPLATE_STEP)");

        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=-1");

        push(&out, "(END_IF_LT_TEMPLATE_STEP)");

        push(&out, "@SP");
        push(&out, "M=M+1");

        push(&out, "@R15");
        push(&out, "A=M");
        push(&out, "0;JMP");
    }

    /*
        lt function
        template steps

        // implementazione - 23 (19) righe
        (gt)    
        @SP
        AM=M-1
        D=M
        @SP
        AM=M-1
        D=M-D
        @IF_GT_TEMPLATE_STEP
        D;JGT
        @SP
        A=M
        M=0
        @END_IF_GT_TEMPLATE_STEP
        0;JMP
        (IF_GT_TEMPLATE_STEP)
        @SP
        A=M
        M=-1
        (END_IF_GT_TEMPLATE_STEP)
        @SP
        M=M+1
        @R15
        A=M
        0;JMP


        // utilizzo - 7 righe
        @return.gt.1
        D=A
        @R15
        M=D
        @gt
        0;JMP
        (return.gt.1) 
    */
    if (cmdCount._gt > 1)
    {
        push(&out, "(gt)");
        push(&out, "@SP");
        push(&out, "AM=M-1");
        push(&out, "D=M");

        push(&out, "@SP");
        push(&out, "AM=M-1");
        push(&out, "D=M-D");

        push(&out, "@IF_GT_TEMPLATE_STEP");

        push(&out, "D;JGT");

        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=0");

        push(&out, "@END_IF_GT_TEMPLATE_STEP");

        push(&out, "0;JMP");

        push(&out, "(IF_GT_TEMPLATE_STEP)");

        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=-1");

        push(&out, "(END_IF_GT_TEMPLATE_STEP)");

        push(&out, "@SP");
        push(&out, "M=M+1");

        push(&out, "@R15");
        push(&out, "A=M");
        push(&out, "0;JMP");
    }

    /*
        Possibili
        push , pop
    */

    /*
        Non necessario per
        label, goto , if-goto, neg, not, add, sub, function
    */

    push(&out, "(START_EXE)");

    if (cmdCount._sysinit == 1)
    {
        push(&out, "@Sys.init");
        push(&out, "0;JMP");
    }else if (cmdCount._main == 1) {
        push(&out, "@Main.main");
        push(&out, "0;JMP");
    }

    return out;
}

void getMneOfSegment(const char segment[], char mne[])
{

    if (strcmp(segment, "static") == 0)
    {
        // static va da 16 a 255
        // numero fissato nessun mne
        strcpy(mne, segment);
    }
    else if (strcmp(segment, "pointer") == 0)
    {
        // pointer 0 = this
        // pointer 1 = that
        strcpy(mne, segment);
    }
    else if (strcmp(segment, "constant") == 0)
    {
        // pointer 0 = this
        // pointer 1 = that
        strcpy(mne, segment);
    }
    else if (strcmp(segment, "local") == 0)
    {
        // local viene creato al momento della call di una function
        strcpy(mne, "LCL");
    }
    else if (strcmp(segment, "argument") == 0)
    {
        // argument viene creato al momento della call di una function
        strcpy(mne, "ARG");
    }
    else if (strcmp(segment, "temp") == 0)
    {
        // temp sono 8 e vanno da RAM[5] a RAM[12];
        // numero fissato nessun mne
        strcpy(mne, segment);
    }
    else if (strcmp(segment, "this") == 0)
    {
        // this viene deciso al momento della call di una function
        strcpy(mne, "THIS");
    }
    else if (strcmp(segment, "that") == 0)
    {
        // that viene deciso al momento della call di una function
        strcpy(mne, "THAT");
    }
    else
        strcpy(mne, "NOTFOUND");
}

List addCmd(int cmdN)
{
    List out;
    init(&out);

    /*
        Es. add
        con RAM[0]   = 258
        con RAM[257] = 8
        con RAM[256] = 10
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 257
        D=M   --> D = RAM[257] (8)

        // leggo quello prima
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 256
        M=M+D --> RAM[256] = RAM[256]+D(8)

        // aumento SP
        @SP
        M=M+1
    */
    push(&out, "@SP");
    push(&out, "AM=M-1");
    push(&out, "D=M");
    push(&out, "@SP");
    push(&out, "AM=M-1");
    push(&out, "M=M+D");
    push(&out, "@SP");
    push(&out, "M=M+1");

    return out;
}

List subCmd(int cmdN)
{
    List out;
    init(&out);

    /*
        Es. add
        con RAM[0]   = 258
        con RAM[257] = 8
        con RAM[256] = 10
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 257
        D=M   --> D = RAM[257] (8)

        // leggo quello prima
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 256
        M=M-D --> RAM[256] = RAM[256]+D(8)

        // aumento SP
        @SP
        M=M+1
    */
    push(&out, "@SP");
    push(&out, "AM=M-1");
    push(&out, "D=M");
    push(&out, "@SP");
    push(&out, "AM=M-1");
    push(&out, "M=M-D");
    push(&out, "@SP");
    push(&out, "M=M+1");

    return out;
}

List negCmd(int cmdN)
{
    List out;
    init(&out);

    /*
        Es. add
        con RAM[0]   = 258
        con RAM[257] = 8
        con RAM[256] = 10

        // leggo quello prima
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 256
        M=-M --> RAM[256] = RAM[256]+D(8)
        M=M-1

        // aumento SP
        @SP
        M=M+1
    */
    push(&out, "@SP");
    push(&out, "AM=M-1");
    push(&out, "M=-M");

    push(&out, "@SP");
    push(&out, "M=M+1");

    return out;
}

List eqCmd(int cmdN, int eqCount)
{
    List out;
    init(&out);
    char text[256];
    char *num;
    num = itoaMine(cmdN);

    if (eqCount > 1)
    {
        /*
        @return.eq.1
        D=A
        @R15
        M=D
        @eq
        0;JMP
        (return.eq.1) 
        */
        strcpy(text, "@return.eq.");
        strcat(text, num);
        push(&out, text);
        push(&out, "D=A");
        push(&out, "@R15");
        push(&out, "M=D");
        push(&out, "@eq");
        push(&out, "0;JMP");
        strcpy(text, "(return.eq.");
        strcat(text, num);
        strcat(text, ")");
        push(&out, text);
    }
    else
    {

        /*
        Es. add
        con RAM[0]   = 258
        con RAM[257] = 8
        con RAM[256] = 10
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 257
        D=M   --> D = RAM[257] (8)

        // leggo quello prima
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 256
        D=M-D

        @IF_EQ
        D;JEQ
        @SP
        A=M
        M=0
        @END
        0;JMP
        (IF_EQ)
        @SP
        A=M
        M=-1
        (END)
        // aumento SP
        @SP
        M=M+1
    */
        push(&out, "@SP");
        push(&out, "AM=M-1");
        push(&out, "D=M");

        push(&out, "@SP");
        push(&out, "AM=M-1");
        push(&out, "D=M-D");

        strcpy(text, "@IF_EQ$");
        strcat(text, num);
        push(&out, text);

        push(&out, "D;JEQ");

        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=0");

        strcpy(text, "@END_EQ$");
        strcat(text, num);
        push(&out, text);

        push(&out, "0;JMP");

        strcpy(text, "(IF_EQ$");
        strcat(text, num);
        strcat(text, ")");
        push(&out, text);

        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=-1");

        strcpy(text, "(END_EQ$");
        strcat(text, num);
        strcat(text, ")");
        push(&out, text);

        push(&out, "@SP");
        push(&out, "M=M+1");
    }

    free(num);
    return out;
}

List gtCmd(int cmdN, int gtCount)
{
    List out;
    init(&out);
    char text[256];
    char *num;
    num = itoaMine(cmdN);

    if (gtCount > 1)
    {
        /*
        @return.gt.1
        D=A
        @R15
        M=D
        @gt
        0;JMP
        (return.gt.1) 
        */
        strcpy(text, "@return.gt.");
        strcat(text, num);
        push(&out, text);
        push(&out, "D=A");
        push(&out, "@R15");
        push(&out, "M=D");
        push(&out, "@gt");
        push(&out, "0;JMP");
        strcpy(text, "(return.gt.");
        strcat(text, num);
        strcat(text, ")");
        push(&out, text);
    }
    else
    {

        /*
        Es. add
        con RAM[0]   = 258
        con RAM[257] = 8
        con RAM[256] = 10
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 257
        D=M   --> D = RAM[257] (8)

        // leggo quello prima
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 256
        M=M-D --> RAM[256] = RAM[256]+D(8)
        D=M

        @IF_GT
        D;JGT
        @SP
        M=0
        @END
        0;JMP
        (IF_GT)
        @SP
        M=-1
        (END)
        // aumento SP
        @SP
        M=M+1
    */
        push(&out, "@SP");
        push(&out, "AM=M-1");
        push(&out, "D=M");

        push(&out, "@SP");
        push(&out, "AM=M-1");
        push(&out, "D=M-D");

        strcpy(text, "@IF_GT$");
        strcat(text, num);
        push(&out, text);

        push(&out, "D;JGT");

        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=0");

        strcpy(text, "@END_GT$");
        strcat(text, num);
        push(&out, text);

        push(&out, "0;JMP");

        strcpy(text, "(IF_GT$");
        strcat(text, num);
        strcat(text, ")");
        push(&out, text);

        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=-1");

        strcpy(text, "(END_GT$");
        strcat(text, num);
        strcat(text, ")");
        push(&out, text);

        push(&out, "@SP");
        push(&out, "M=M+1");
    }

    free(num);
    return out;
}

List ltCmd(int cmdN, int ltCount)
{
    List out;
    init(&out);
    char text[256];
    char *num;
    num = itoaMine(cmdN);

    if (ltCount > 1)
    {
        /*
        @return.lt.1
        D=A
        @R15
        M=D
        @lt
        0;JMP
        (return.lt.1) 
        */
        strcpy(text, "@return.lt.");
        strcat(text, num);
        push(&out, text);
        push(&out, "D=A");
        push(&out, "@R15");
        push(&out, "M=D");
        push(&out, "@lt");
        push(&out, "0;JMP");
        strcpy(text, "(return.lt.");
        strcat(text, num);
        strcat(text, ")");
        push(&out, text);
    }
    else
    {

        /*
        Es. add
        con RAM[0]   = 258
        con RAM[257] = 8
        con RAM[256] = 10
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 257
        D=M   --> D = RAM[257] (8)

        // leggo quello prima
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 256
        M=M-D --> RAM[256] = RAM[256]+D(8)
        D=M

        @IF_LT
        D;JLT
        @SP
        M=0
        @END
        0;JMP
        (IF_LT)
        @SP
        M=-1
        (END)
        // aumento SP
        @SP
        M=M+1
    */
        push(&out, "@SP");
        push(&out, "AM=M-1");
        push(&out, "D=M");

        push(&out, "@SP");
        push(&out, "AM=M-1");
        push(&out, "D=M-D");

        strcpy(text, "@IF_LT$");
        strcat(text, num);
        push(&out, text);

        push(&out, "D;JLT");

        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=0");

        strcpy(text, "@END_LT$");
        strcat(text, num);
        push(&out, text);

        push(&out, "0;JMP");

        strcpy(text, "(IF_LT$");
        strcat(text, num);
        strcat(text, ")");
        push(&out, text);

        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=-1");

        strcpy(text, "(END_LT$");
        strcat(text, num);
        strcat(text, ")");
        push(&out, text);

        push(&out, "@SP");
        push(&out, "M=M+1");
    }

    free(num);
    return out;
}

List andCmd()
{
    List out;
    init(&out);

    /*
        Es. add
        con RAM[0]   = 258
        con RAM[257] = 8
        con RAM[256] = 10
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 257
        D=M   --> D = RAM[257] (8)

        // leggo quello prima
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 256
        M=M&D --> RAM[256] = RAM[256]&D(8)

        // aumento SP
        @SP
        M=M+1
    */
    push(&out, "@SP");
    push(&out, "AM=M-1");
    push(&out, "D=M");
    push(&out, "@SP");
    push(&out, "AM=M-1");
    // operazione
    push(&out, "M=M&D");
    push(&out, "@SP");
    push(&out, "M=M+1");

    return out;
}

List orCmd()
{
    List out;
    init(&out);

    /*
        Es. add
        con RAM[0]   = 258
        con RAM[257] = 8
        con RAM[256] = 10
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 257
        D=M   --> D = RAM[257] (8)

        // leggo quello prima
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 256
        M=M|D --> RAM[256] = RAM[256]|D(8)

        // aumento SP
        @SP
        M=M+1
    */
    push(&out, "@SP");
    push(&out, "AM=M-1");
    push(&out, "D=M");
    push(&out, "@SP");
    push(&out, "AM=M-1");
    // operazione
    push(&out, "M=M|D");
    push(&out, "@SP");
    push(&out, "M=M+1");

    return out;
}

List notCmd()
{
    List out;
    init(&out);

    /*
        Es. add
        con RAM[0]   = 258
        con RAM[257] = 8
        con RAM[256] = 10

        // leggo quello prima
        @SP   --> @0
        AM=M-1 --> RAM[0]=RAM[0]-1 = 256
        M=!M --> RAM[256] = !RAM[256]
        M=M+1

        // aumento SP
        @SP
        M=M+1
    */
    push(&out, "@SP");
    push(&out, "AM=M-1");
    push(&out, "M=!M");

    push(&out, "@SP");
    push(&out, "M=M+1");

    return out;
}

List popCmd(const char nomeFile[], ListStatic *sttc, const char segment[], char n[], int *error)
{
    List out;
    init(&out);
    char text[256];
    char mne[256];
    // Es. ottengo LCL a partire da local
    getMneOfSegment(segment, mne);

    if (strcmp(mne, "NOTFOUND") == 0)
    {
        // deve essere un segment conosciuto
        *error = 1;
    }
    else if (atoiMine(n) < 0)
    {
        // n non puo essere un numero negativo
        *error = 1;
    }
    else
    {
        *error = 0;
        // Prendo il valore in SP-1
        /*
            @SP --> @0
            M=M-1 --> RAM[0] = RAM[0]-1 (257-1)
        */
        push(&out, "@SP");

        push(&out, "M=M-1");

        if (strcmp(segment, "static") == 0)
        {
            // assegno il valore appena preso a mne+n
            /*
            Es. pop static 6

            @16 --> @16
            A=M  --> A = RAM[16] -> A=300
            D=A  --> D = 300
            */

            int staticN = checkInStatic(*sttc, atoiMine(n), nomeFile);
            if (staticN == -1)
            {
                pushStatic(sttc, atoiMine(n), nomeFile);
                staticN = (*sttc).last->ramN;
            }
            n = itoaMine(staticN);

            char *num;
            int intN = atoiMine(n) + 16;
            num = itoaMine(intN);

            push(&out, "@SP");
            push(&out, "A=M");
            push(&out, "D=M");

            // 16 + n
            strcpy(text, "@");
            strcat(text, num);
            push(&out, text);

            push(&out, "M=D");
        }
        else if (strcmp(segment, "pointer") == 0)
        {
            /*  
                pointer 0 = this
                pointer 1 = that

                @SP
                A=M
                D=M
                @THIS
                M=D
            */

            push(&out, "@SP");
            push(&out, "A=M");
            push(&out, "D=M");

            if (strcmp(n, "0") == 0)
            {
                push(&out, "@THIS");
            }
            else if (strcmp(n, "1") == 0)
            {
                push(&out, "@THAT");
            }

            push(&out, "M=D");
        }
        else if (strcmp(segment, "temp") == 0)
        {
            /*
                temp 0 -> 5, ... , tmp 7 -> 12
                @5  --> temp 0
                D=M --> D = RAM[5]
            */

            char *num;
            int intN = atoiMine(n) + 5;
            num = itoaMine(intN);

            push(&out, "@SP");
            push(&out, "A=M");
            push(&out, "D=M");

            // 5 + n
            strcpy(text, "@");
            strcat(text, num);
            push(&out, text);

            push(&out, "M=D");
        }
        else
        {

            /*
                @n   --> 6
                D=D+A
                @LCL
                D=M+D  

                @R13
                M=D
                
                @SP  --> @0
                A=M  --> A = RAM[0] --> A=256
                D=M  --> D = RAM[256]

                @R13
                A=M  --> A = RAM[13] (306)
                M=D  --> RAM[306] = D (RAM[256])
            */

            int intN = atoiMine(n);
            if (intN == 0)
            {
                strcpy(text, "@");
                strcat(text, mne);
                push(&out, text);

                push(&out, "D=M");
            }
            else if (intN == 1)
            {
                strcpy(text, "@");
                strcat(text, mne);
                push(&out, text);

                push(&out, "D=M+1");
            }
            else
            {
                strcpy(text, "@");
                strcat(text, n);
                push(&out, text);

                push(&out, "D=A");

                strcpy(text, "@");
                strcat(text, mne);
                push(&out, text);

                push(&out, "D=M+D");
            }

            push(&out, "@R13");
            push(&out, "M=D");

            push(&out, "@SP");
            push(&out, "A=M");
            push(&out, "D=M");

            push(&out, "@R13");
            push(&out, "A=M");
            push(&out, "M=D");
        }
    }

    return out;
}

List pushCmd(const char nomeFile[], ListStatic *sttc, const char segment[], const char n[], int cmdN, int *error)
{
    List out;
    init(&out);
    char text[256];

    // cerco il mio mnemonico (LCL, ARG, etc...)
    char mne[256];
    getMneOfSegment(segment, mne);

    if (strcmp(mne, "NOTFOUND") == 0)
    {
        // deve essere un segment conosciuto
        *error = 1;
    }
    else if (atoiMine(n) < 0)
    {
        // n non puo essere un numero negativo
        *error = 1;
    }
    else
    {
        *error = 0;
        if (strcmp(segment, "constant") == 0)
        {
            int intN = atoiMine(n);
            if (intN == 0)
            {
                // assegnamento diretto
                strcpy(text, "D=0   ");
                push(&out, text);
            }
            else if (intN == 1)
            {
                // assegnamento diretto
                strcpy(text, "D=1");
                push(&out, text);
            }
            else
            {
                // si tratta di una costante
                // @n
                strcpy(text, "@");
                strcat(text, n);
                push(&out, text);

                // assegnamento diretto
                strcpy(text, "D=A");
                push(&out, text);
            }
        }
        else if (strcmp(segment, "static") == 0)
        {
            // static
            /*
            static 0 -> 16, ... , tmp 239 -> 255
            @16  --> static 0
            D=M --> D = RAM[16]
            */

            int staticN = checkInStatic(*sttc, atoiMine(n), nomeFile);
            if (staticN == -1)
            {
                pushStatic(sttc, atoiMine(n), nomeFile);
                staticN = (*sttc).last->ramN;
                //printStatic(*sttc);
            }

            char *num;
            //int intN = atoiMine(n) + 16;
            int intN = staticN + 16;
            num = itoaMine(intN);

            // @(n+16)
            strcpy(text, "@");
            strcat(text, num);
            push(&out, text);

            // assegnamento da RAM[n+16]
            strcpy(text, "D=M");
            push(&out, text);

            free(num);
        }
        else if (strcmp(segment, "pointer") == 0)
        {
            /*
            pointer 0 = this
            pointer 1 = that
        */

            if (strcmp(n, "0") == 0)
            {
                push(&out, "@THIS");
            }
            else if (strcmp(n, "1") == 0)
            {
                push(&out, "@THAT");
            }

            push(&out, "D=M");
        }
        else if (strcmp(segment, "temp") == 0)
        {
            /*
                temp 0 -> 5, ... , tmp 7 -> 12
                @5  --> temp 0
                D=M --> D = RAM[5]
            */
            char *num;
            int intN = atoiMine(n) + 5;
            num = itoaMine(intN);

            // @(n+5)
            strcpy(text, "@");
            strcat(text, num);
            push(&out, text);

            // assegnamento da RAM[n+5]
            push(&out, "D=M");

            free(num);
        }
        else
        {
            // Si tratta di copiare il valore del segment+n nel SP dello stack
            /*
            Es. push local 6
                @LCL --> @1
                D=M  --> D=RAM[1] -> D=300
                @n   --> 6
                D=D+A--> D=D+6 -> 306
                A=D  --> A=306 -> mi posiziono nella cella 
                D=M  --> D=RAM[306] -> copio il valore della cella
            */

            // assegno A al numero del segment (@LCL, @ARG, etc...)
            strcpy(text, "@");
            strcat(text, mne);
            push(&out, text);

            push(&out, "D=M");

            int intN = atoiMine(n);
            if (intN == 0)
            {
                push(&out, "A=D");
            }
            else if (intN == 1)
            {
                push(&out, "A=D+1");
            }
            else
            {
                // assegno A il valore di n
                strcpy(text, "@");
                strcat(text, n);
                push(&out, text);

                push(&out, "D=D+A");

                push(&out, "A=D");
            }

            push(&out, "D=M");
        }

        // assegno A al numero del SP (0)
        push(&out, "@SP");

        // A = RAM[0] (es. A = 256)
        push(&out, "A=M");

        // aggiungo a stack
        // M = D -> RAM[256] = D (mio numero)
        push(&out, "M=D");

        // assegno A al numero del SP (0)
        push(&out, "@SP");

        // SPOSTO DI 1 l' SP
        // RAM[0] = RAM[0] + 1
        push(&out, "M=M+1");
    }

    return out;
}

List labelCmd(const char nomeFun[], const char labelText[])
{
    /*
        Semplice label che il linguaggio .asm già implementa
    */
    List out;
    init(&out);
    char text[256];

    strcpy(text, "(");
    strcat(text, nomeFun);
    strcat(text, ".");
    strcat(text, labelText);
    strcat(text, ")");
    push(&out, text);

    return out;
}

List gotoCmd(const char nomeFun[], const char labelText[])
{
    List out;
    init(&out);
    char text[256];

    strcpy(text, "@");
    strcat(text, nomeFun);
    strcat(text, ".");
    strcat(text, labelText);
    push(&out, text);
    // salto incondizionato
    push(&out, "0;JMP");

    return out;
}

List ifgotoCmd(const char nomeFun[], const char labelText[])
{
    /*
        se il valore puntato da SP-1 è diverso da 0 eseguo il salto
        Es.
        con RAM[0] = 257
        @SP   --> @0
        AM=M-1 --> D=RAM[0]-1 = 256
        D=M
        @esempio
        D;JNE
    */
    List out;
    init(&out);
    char text[256];

    push(&out, "@SP");
    push(&out, "AM=M-1");
    push(&out, "D=M");

    strcpy(text, "@");
    strcat(text, nomeFun);
    strcat(text, ".");
    strcat(text, labelText);
    push(&out, text);

    push(&out, "D;JNE");

    return out;
}

List callFunction(const char nameF[], const char nArgs[], int funcN, int *error, int callCount)
{
    List out;
    init(&out);
    char text[256];

    if (atoiMine(nArgs) < 0)
    {
        *error = 1;
    }
    else if (callCount > 1)
    {
        *error = 0;
        /*
            call f nArgs
            n-parametri devono già essere stati caricati nello stack
            in sp+1 salvo l'indirizzo di ritorno
            in sp+2 salvo LCL
            in sp+3 salvo ARG
            in sp+4 salvo THIS
            in sp+5 salvo THAT
            sposto sp a sp+5+nVars
        
            Es. 
            SP = 256
            nArgs = 4
            nVars = 2

            @return$nameF.funcN
            D=A   --> D = funcN
            @SP   --> 0
            A=M   --> A = RAM[0] (261)
            M=D   --> RAM[261] = D (funcN)

            @SP
            M=M+1 --> RAM[0] = RAM[0] + 1
        */

        /*
        @return$test.fun.0
            D=A
            @SP
            A=M
            M=D
            @nArgs
            D=A
            @R13
            M=D
                @return.call.1
                D=A
                @R15
                M=D
                @call
                0;JMP
                (return.call.1)
            @test.fun
            0;JMP
            (return$test.fun.0)
        */

        strcpy(text, "@return$");
        strcat(text, nameF);
        strcat(text, ".");
        strcat(text, itoaMine(funcN));
        push(&out, text);

        push(&out, "D=A");
        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=D");

        /*
            @(nArgs+5) --> 4+5 = 9
            D=A
            @R13
            M=D

            @nameF
            0;JMP --> eseguo la funzione nameF

            (return$nameF.funcN)
        */
        int tmpArgs = atoiMine(nArgs) + 5;
        strcpy(text, "@");
        strcat(text, itoaMine(tmpArgs));
        push(&out, text);

        push(&out, "D=A");
        push(&out, "@R13");
        push(&out, "M=D");

        strcpy(text, "@return$");
        strcat(text, nameF);
        strcat(text, ".call.");
        strcat(text, itoaMine(funcN));
        push(&out, text);

        push(&out, "D=A");
        push(&out, "@R15");
        push(&out, "M=D");
        push(&out, "@call");
        push(&out, "0;JMP");

        strcpy(text, "(return$");
        strcat(text, nameF);
        strcat(text, ".call.");
        strcat(text, itoaMine(funcN));
        strcat(text, ")");
        push(&out, text);

        strcpy(text, "@");
        strcat(text, nameF);
        push(&out, text);
        push(&out, "0;JMP");

        strcpy(text, "(return$");
        strcat(text, nameF);
        strcat(text, ".");
        strcat(text, itoaMine(funcN));
        strcat(text, ")");
        push(&out, text);
    }
    else
    {
        *error = 0;
        /*
        call f nArgs
        n-parametri devono già essere stati caricati nello stack
        in sp+1 salvo l'indirizzo di ritorno
        in sp+2 salvo LCL
        in sp+3 salvo ARG
        in sp+4 salvo THIS
        in sp+5 salvo THAT
        sposto sp a sp+5+nVars
       
        Es. 
        SP = 256
        nArgs = 4
        nVars = 2

        @return$nameF.funcN
        D=A   --> D = funcN
        @SP   --> 0
        A=M   --> A = RAM[0] (261)
        M=D   --> RAM[261] = D (funcN)

        @SP
        M=M+1 --> RAM[0] = RAM[0] + 1
    */
        strcpy(text, "@return$");
        strcat(text, nameF);
        strcat(text, ".");
        strcat(text, itoaMine(funcN));
        push(&out, text);

        //printf("N FUN : %d\n", funcN);

        push(&out, "D=A");
        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=D");
        push(&out, "@SP");
        push(&out, "M=M+1");

        /*    

        @LCL
        D=M   --> D = RAM[1]
        @SP
        A=M   --> A = RAM[0]
        M=D   --> RAM[A] = D

        @SP
        M=M+1 --> RAM[0] = RAM[0] + 1
    */

        push(&out, "@LCL");
        push(&out, "D=M");
        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=D");

        push(&out, "@SP");
        push(&out, "M=M+1");

        /*
        @ARG
        D=M   --> D = RAM[2]
        @SP
        A=M   --> A = RAM[0]
        M=D   --> RAM[A] = D

        @SP
        M=M+1 --> RAM[0] = RAM[0] + 1
    */

        push(&out, "@ARG");
        push(&out, "D=M");
        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=D");

        push(&out, "@SP");
        push(&out, "M=M+1");

        /*
        @THIS
        D=M   --> D = RAM[3]
        @SP
        A=M   --> A = RAM[0]
        M=D   --> RAM[A] = D

        @SP
        M=M+1 --> RAM[0] = RAM[0] + 1
    */

        push(&out, "@THIS");
        push(&out, "D=M");
        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=D");

        push(&out, "@SP");
        push(&out, "M=M+1");

        /*
        @THAT
        D=M   --> D = RAM[4]
        @SP
        A=M   --> A = RAM[0]
        M=D   --> RAM[A] = D

        @SP
        M=M+1 --> RAM[0] = RAM[0] + 1
    */

        push(&out, "@THAT");
        push(&out, "D=M");
        push(&out, "@SP");
        push(&out, "A=M");
        push(&out, "M=D");

        push(&out, "@SP");
        push(&out, "M=M+1");

        /*
        @(nArgs+5) --> 4+5 = 9
        D=A
        @SP
        D=M-D --> RAM[0] = RAM[0] - D (9)

        @ARG  --> ARG = SP-(nArgs+5)
        M=D

        @SP
        D=M

        @LCL  --> LCL = SP
        M=D

        @nameF
        0;JMP --> eseguo la funzione nameF

        (return$nameF.funcN)
    */

        int tmpArgs = atoiMine(nArgs) + 5;
        strcpy(text, "@");
        strcat(text, itoaMine(tmpArgs));
        push(&out, text);

        push(&out, "D=A");
        push(&out, "@SP");
        push(&out, "D=M-D");

        push(&out, "@ARG");
        push(&out, "M=D");

        push(&out, "@SP");
        push(&out, "D=M");

        push(&out, "@LCL");
        push(&out, "M=D");

        strcpy(text, "@");
        strcat(text, nameF);
        push(&out, text);
        push(&out, "0;JMP");

        strcpy(text, "(return$");
        strcat(text, nameF);
        strcat(text, ".");
        strcat(text, itoaMine(funcN));
        strcat(text, ")");
        push(&out, text);
    }

    return out;
}

List function(const char nameF[], const char nVars[], int *error)
{
    /*
        la definizione di mi dice il suo nome (label)
        e il numero di variabili locali
        poi imposto SP a SP+nVars

        (nameF)

        ripeti nVars volte
        @SP
        A=M  --> A = RAM[0] (256)
        M=0  --> RAM[RAM[0]] (RAM[256]) = 0
        @SP
        M=M+1
    */
    List out;
    init(&out);
    char text[256];
    int intVars = atoi(nVars);

    if (intVars < 0)
    {
        *error = 1;
    }
    else
    {
        *error = 0;

        strcpy(text, "(");
        strcat(text, nameF);
        strcat(text, ")");
        push(&out, text);

        for (int i = 0; i < intVars; i++)
        {
            push(&out, "@SP");
            push(&out, "A=M");
            push(&out, "M=0");
            push(&out, "@SP");
            push(&out, "M=M+1");
        }
    }

    return out;
}

List returnFunction(const char nameF[], int *error, int returnCount, int funN)
{
    List out;
    init(&out);

    if (returnCount > 1)
    {
        char text[256];
        /*

            @return.return.1
            D=A
            @R15
            M=D
            @return
            0;JMP
            (return.return.1)

        */

        strcpy(text, "@");
        strcat(text, nameF);
        strcat(text, "$return.");
        strcat(text, itoaMine(funN));
        push(&out, text);
        push(&out, "D=A");
        push(&out, "@R15");
        push(&out, "M=D");
        push(&out, "@return");
        push(&out, "0;JMP");
        strcpy(text, "(");
        strcat(text, nameF);
        strcat(text, "$return.");
        strcat(text, itoaMine(funN));
        strcat(text, ")");
        push(&out, text);
    }
    else
    {

        /*
        setto @R14 = RAM[RAM[13]-5]
        @5
        D=A
        @LCL
        A=M-D
        D=M    --> D = RAM[RAM[0]-D]
        @R14
        M=D
    */

        push(&out, "// RIPRISTINO @R14");
        push(&out, "@5");
        push(&out, "D=A");
        push(&out, "@LCL");
        push(&out, "A=M-D");
        push(&out, "D=M");
        push(&out, "@R14");
        push(&out, "M=D");

        /*
            pop argument 0
            riposiziono il risultato della funzione
        */

        push(&out, "// ESEGUO IL POP ARGUMENT 0");
        List popArg0 = popCmd("", NULL, "argument", "0", error);
        while (popArg0.head != NULL)
        {
            push(&out, popArg0.head->text);
            popArg0.head = popArg0.head->next;
        }
        delete (&popArg0);

        /*
            @LCL
            D=M
            @R13
            M=D   --> RAM[13] = D (RAM[1])
        */

        push(&out, "// IMPOSTO @R13 (TEMP) = LCL");
        push(&out, "@LCL");
        push(&out, "D=M");
        push(&out, "@R13");
        push(&out, "M=D");

        /*
            @ARG
            D=M
            @SP
            M=D+1
        */

        push(&out, "// IMPOSTO SP = ARG+1");
        push(&out, "@ARG");
        push(&out, "D=M");
        push(&out, "@SP");
        push(&out, "M=D+1");

        /*
            setto THAT = RAM[RAM[13]-1]
            @1
            D=A
            @R13
            A=M-D  --> A = RAM[13](317)-D(1)
            D=M    --> D = RAM[RAM[13]-1]
            @THAT
            M=D 
        */

        push(&out, "// RIPRISTINO THAT");
        push(&out, "@R13");
        push(&out, "A=M-1");
        push(&out, "D=M");
        push(&out, "@THAT");
        push(&out, "M=D");

        /*
            setto THIS = RAM[RAM[13]-2]
            @2
            D=A
            @R13
            A=M-D  --> A = RAM[13](317)-D(2)
            D=M    --> D = RAM[RAM[13]-2]
            @THIS
            M=D 
        */

        push(&out, "// RIPRISTINO THIS");
        push(&out, "@2");
        push(&out, "D=A");
        push(&out, "@R13");
        push(&out, "A=M-D");
        push(&out, "D=M");
        push(&out, "@THIS");
        push(&out, "M=D");

        /*
            setto ARG = RAM[RAM[13]-3]
            @3
            D=A
            @R13
            A=M-D  --> A = RAM[13](317)-D(3)
            D=M    --> D = RAM[RAM[13]-3]
            @ARG
            M=D 
        */

        push(&out, "// RIPRISTINO ARG");
        push(&out, "@3");
        push(&out, "D=A");
        push(&out, "@R13");
        push(&out, "A=M-D");
        push(&out, "D=M");
        push(&out, "@ARG");
        push(&out, "M=D");

        /*
            setto LCL = RAM[RAM[13]-4]
            @4
            D=A
            @R13
            A=M-D  --> A = RAM[13](317)-D(4)
            D=M    --> D = RAM[RAM[13]-4]
            @LCL
            M=D 
        */

        push(&out, "// RIPRISTINO LCL");
        push(&out, "@4");
        push(&out, "D=A");
        push(&out, "@R13");
        push(&out, "A=M-D");
        push(&out, "D=M");
        push(&out, "@LCL");
        push(&out, "M=D");

        /*
            goto RAM[RAM[14]]
            @R14
            A=M   --> A = RAM[14]
            D=M   --> D = RAM[RAM[14]]
            @R14
            M=D   --> RAM[14] = D (RAM[RAM[14]])
            0;JMP

        */

        push(&out, "@R14");
        push(&out, "A=M");
        push(&out, "0;JMP");
    }

    return out;
}
