#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "command.h"
#include "list.h"
#include "utility.h"

/*

    OTTIMIZZAZIONE DEL CODICE :
        - quando vengono utilizzati i numero 0 e 1 (es. push local 1)
    /     non si ricorre ad una variabile di appoggio

        - gli elementi del codice più prolissi (es. return, add, sub, etc)
          vengono definiti una sola volta e richiamati all'occorrenza

        - le variabili locali per una funzione se in numero superiore a 2 
          necessitano di un ciclo

        - verifiche le funzioni chiamate durante il codice
          ed eliminare quelle mai utilizzate

*/

void writeToFile(FILE *write, List data)
{
    ListNode head = data.head;
    while (head != NULL)
    {
        if (strlen(head->text) > 0)
        {
            strcat(head->text, "\n");
            fputs(head->text, write);
        }
        head = head->next;
    }
    free(head);
    delete (&data);

    fputs("\n", write);
}

void readFromFile(const char nomeFile[], FILE *read, FILE *write, int *error, ListStatic *sttc, List funCalled, CmdCount cmdCount, int *cmdN, int *funN)
{
    char line[256];

    char token[256];
    char currentFun[256];
    char filter = ' ';

    List out;

    int stopWrite = 0; // 1 quando trovo una funzione non utilizzata

    while (fgets(line, 256, read) && *error == 0)
    {
        removeChar(line, '\n');
        removeChar(line, '\t');
        removeChar(line, '\r');
        removeComment(line);
        removeInitialSpace(line);

        if (strlen(line) > 0)
        {
            int toWrite = 0;
            char comment[260];
            strcpy(comment, "// ");
            strcat(comment, line);
            strcat(comment, "\n");

            strtokMine(line, token, filter);

            if (token != NULL && strlen(token) > 0)
            {
                if (strcmp(token, "function") == 0)
                {
                    // definizione della funzione terminata
                    // posso tornare a scrivere
                    stopWrite = 0;
                }

                if (stopWrite == 0)
                {
                    // se il nome della funzione non è associato
                    // allora lo imposto al nome del file di default
                    if (strlen(currentFun) <= 1)
                    {
                        strcpy(currentFun, nomeFile);
                    }

                    if (strcmp(token, "push") == 0)
                    {
                        int arg = 0;
                        char segment[256];
                        char nSegment[256];
                        while (strlen(line) > 0)
                        {
                            strtokMine(line, token, filter);
                            if (arg == 0)
                            {
                                strcpy(segment, token);
                            }
                            else if (arg == 1)
                            {
                                strcpy(nSegment, token);
                            }
                            arg++;
                        }

                        out = pushCmd(nomeFile, sttc, segment, nSegment, *cmdN, error);
                        toWrite = 1;
                    }
                    else if (strcmp(token, "pop") == 0)
                    {
                        int arg = 0;
                        char segment[256];
                        char nSegment[256];
                        while (strlen(line) > 0)
                        {
                            strtokMine(line, token, filter);
                            if (arg == 0)
                            {
                                strcpy(segment, token);
                            }
                            else if (arg == 1)
                            {
                                strcpy(nSegment, token);
                            }
                            arg++;
                        }

                        out = popCmd(nomeFile, sttc, segment, nSegment, error);
                        toWrite = 1;
                    }
                    else if (strcmp(token, "label") == 0)
                    {
                        int arg = 0;
                        char labelText[256];
                        while (strlen(line) > 0)
                        {
                            strtokMine(line, token, filter);
                            if (arg == 0)
                            {
                                strcpy(labelText, token);
                            }
                            arg++;
                        }

                        out = labelCmd(currentFun, labelText);
                        toWrite = 1;
                    }
                    else if (strcmp(token, "goto") == 0)
                    {
                        int arg = 0;
                        char labelText[256];
                        while (strlen(line) > 0)
                        {
                            strtokMine(line, token, filter);
                            if (arg == 0)
                            {
                                strcpy(labelText, token);
                            }
                            arg++;
                        }

                        out = gotoCmd(currentFun, labelText);
                        toWrite = 1;
                    }
                    else if (strcmp(token, "if-goto") == 0)
                    {
                        int arg = 0;
                        char labelText[256];
                        while (strlen(line) > 0)
                        {
                            strtokMine(line, token, filter);
                            if (arg == 0)
                            {
                                strcpy(labelText, token);
                            }
                            arg++;
                        }

                        out = ifgotoCmd(currentFun, labelText);
                        toWrite = 1;
                    }
                    else if (strcmp(token, "add") == 0)
                    {
                        out = addCmd(*cmdN);
                        toWrite = 1;
                    }
                    else if (strcmp(token, "sub") == 0)
                    {
                        out = subCmd(*cmdN);
                        toWrite = 1;
                    }
                    else if (strcmp(token, "neg") == 0)
                    {
                        out = negCmd(*cmdN);
                        toWrite = 1;
                    }
                    else if (strcmp(token, "eq") == 0)
                    {
                        out = eqCmd(*cmdN, cmdCount._eq);
                        toWrite = 1;
                    }
                    else if (strcmp(token, "gt") == 0)
                    {
                        out = gtCmd(*cmdN, cmdCount._gt);
                        toWrite = 1;
                    }
                    else if (strcmp(token, "lt") == 0)
                    {
                        out = ltCmd(*cmdN, cmdCount._lt);
                        toWrite = 1;
                    }
                    else if (strcmp(token, "and") == 0)
                    {
                        out = andCmd();
                        toWrite = 1;
                    }
                    else if (strcmp(token, "or") == 0)
                    {
                        out = orCmd();
                        toWrite = 1;
                    }
                    else if (strcmp(token, "not") == 0)
                    {
                        out = notCmd();
                        toWrite = 1;
                    }
                    else if (strcmp(token, "call") == 0)
                    {
                        int arg = 0;
                        char nameF[256];
                        char nArgs[256];
                        while (strlen(line) > 0)
                        {
                            strtokMine(line, token, filter);
                            if (arg == 0)
                            {
                                strcpy(nameF, token);
                            }
                            else if (arg == 1)
                            {
                                strcpy(nArgs, token);
                            }
                            arg++;
                        }

                        out = callFunction(nameF, nArgs, *funN, error, cmdCount._call);
                        toWrite = 1;
                        *funN = *funN + 1;
                    }
                    else if (strcmp(token, "function") == 0)
                    {
                        int arg = 0;
                        char nameF[256];
                        char nVars[256];
                        while (strlen(line) > 0)
                        {
                            strtokMine(line, token, filter);
                            if (arg == 0)
                            {
                                strcpy(nameF, token);
                            }
                            else if (arg == 1)
                            {
                                strcpy(nVars, token);
                            }
                            arg++;
                        }

                        if (checkInList(funCalled, nameF) == 0)
                        {
                            //printf("FUN (%s) MAI CHIAMATA\n", nameF);
                            stopWrite = 1;
                        }
                        else
                        {
                            out = function(nameF, nVars, error);
                            toWrite = 1;
                            // la salvo per poterla usare ad esempio
                            // in label e goto, if-goto
                            strcpy(currentFun, nameF);
                        }
                    }
                    else if (strcmp(token, "return") == 0)
                    {
                        out = returnFunction(currentFun, error, cmdCount._return, *funN);
                        *funN = *funN + 1;
                        toWrite = 1;
                    }
                    else
                    {
                        *error = 1;
                    }
                }

                if (*error == 1)
                {
                    printf("COMANDO NON RICONOSCIUTO IN : %s\n", comment);
                }

                if (toWrite == 1)
                {
                    fputs(comment, write);
                    writeToFile(write, out);
                    *cmdN = *cmdN + 1;
                }
            }
        }
    }
}

void scanFunction(ListNode current, List *funCalled, const char dir[])
{
    if (current != NULL)
    {
        char filter = '.';
        char nomeFile[256];
        char nomeFun[256];
        char nomeCompleto[256];
        strcpy(nomeCompleto, dir);
        strcat(nomeCompleto, "/");
        strcpy(nomeFun, current->text);
        strtokMine(nomeFun, nomeFile, filter);
        strcat(nomeFile, ".vm");
        strcat(nomeCompleto, nomeFile);
        FILE *read = fopen(nomeCompleto, "r");
        char line[256];
        char token[256];
        int stop = 0;
        int examineCurrent = 0;
        filter = ' ';

        if (read != NULL)
        {
            //printf("FUNZIONE : %s\n", current->text);
            while (fgets(line, 256, read) && stop == 0)
            {
                removeChar(line, '\n');
                removeChar(line, '\t');
                removeChar(line, '\r');
                removeComment(line);
                removeInitialSpace(line);

                if (strlen(line) > 0)
                {
                    strtokMine(line, token, filter);
                    if (examineCurrent == 0 && strcmp(token, "function") == 0)
                    {
                        strtokMine(line, token, filter);
                        //printf("LINE : %s - %s\n", line, current->text);
                        if (strcmp(token, current->text) == 0)
                        {
                            //printf("LINE : %s\n", token);
                            examineCurrent = 1;
                        }
                    }
                    else if (examineCurrent == 1)
                    {
                        // la riga non è la dichiarazione di funzione
                        // verifico se è il return
                        // o se una call da aggiungere in lista
                        if (strcmp(token, "return") == 0)
                        {
                            stop = 1;
                        }
                        else if (strcmp(token, "call") == 0)
                        {
                            strtokMine(line, token, filter);
                            //printf("FUN : %s (%d)\n", token, checkInList(*funCalled, token));
                            if (checkInList(*funCalled, token) == 0)
                            {
                                //printf("AGGIUNGO : %s\n", token);
                                push(funCalled, token);
                            }
                        }
                    }
                }
            }

            //printf("\n\n");
            current = current->next;
            scanFunction(current, funCalled, dir);
            // if(read != NULL) fclose(read);
        }
        else
        {
            printf("IMPOSSIBILE LEGGERE IL FILE : %s\n", nomeFile);
        }
    }
    else
    {
        ListNode t = funCalled->head;
        while (t != NULL)
        {
            printf("FUNZIONE CHIAMATA : %s\n", t->text);
            t = t->next;
        }
        printf("SCANSIONE DELLE FUNZIONI TERMINATA\n\n");
    }
}

void preScanFile(const char nomeFile[], List *funCalled, CmdCount *cmdCount)
{
    FILE *read = fopen(nomeFile, "r");

    if (read != NULL)
    {
        char line[256];
        char token[256];
        char filter = ' ';

        while (fgets(line, 256, read))
        {
            removeChar(line, '\n');
            removeChar(line, '\t');
            removeChar(line, '\r');
            removeComment(line);
            removeInitialSpace(line);

            if (strlen(line) > 0)
            {
                strtokMine(line, token, filter);

                if (token != NULL && strlen(token) > 0)
                {
                    if (strcmp(token, "call") == 0)
                    {
                        // è presente almeno una call
                        cmdCount->_call++;
                        strtokMine(line, token, filter);
                        // inserisco alla liste delle funzioni chiamate
                        //push(funCalled, token);
                    }
                    else if (strcmp(token, "return") == 0)
                    {
                        // è presente almeno un return
                        cmdCount->_return++;
                    }
                    else if (strcmp(token, "function") == 0)
                    {
                        // è presente almeno una function
                        cmdCount->_function++;
                        strtokMine(line, token, filter);
                        if (strcmp(token, "Sys.init") == 0)
                        {
                            cmdCount->_sysinit = 1;
                        }
                        else if (strcmp(token, "Main.main") == 0)
                        {
                            cmdCount->_main = 1;
                        }
                    }
                    else if (strcmp(token, "eq") == 0)
                    {
                        // è presente almeno un eq (equal)
                        cmdCount->_eq++;
                    }
                    else if (strcmp(token, "lt") == 0)
                    {
                        // è presente almeno un lt (less than)
                        cmdCount->_lt++;
                    }
                    else if (strcmp(token, "gt") == 0)
                    {
                        // è presente almeno un gt (greater than)
                        cmdCount->_gt++;
                    }
                }
            }
        }
    }
}

void preScanDir(const char nomeDir[], List *funCalled, CmdCount *cmdCount)
{
    struct dirent *de; // Pointer for directory entry

    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir(nomeDir);

    if (dr == NULL) // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory");
    }
    else
    {

        while ((de = readdir(dr)) != NULL)
        {
            // file di estensione .vm == 3
            int new_len = (strlen(de->d_name) - 3);
            char nome[256];
            strcpy(nome, de->d_name);
            shift(nome, new_len);

            if (strcmp(nome, ".vm") == 0)
            {
                char nomeFile[256];
                strcpy(nomeFile, nomeDir);
                strcat(nomeFile, de->d_name);

                preScanFile(nomeFile, funCalled, cmdCount);
                //printf("FUN CALLED : %d\n", cmdCount->_call);
            }
        }

        free(dr);
    }
}

int main(int argc, char **argv)
{
    int error = 0;
    char errorMsg[256] = "";
    if (argc < 2)
    {
        error = 1;
        strcat(errorMsg, "INSERISCI PARAMETRI\n");
    }

    if (error == 0)
    {
        char input[256];
        strcpy(input, argv[1]);
        strcat(input, "/");
        //FILE *read = fopen(input, "r");

        /*
        if (read == NULL)
        {
            error = 1;
            strcat(errorMsg, "FILE NON TROVATO\n");
        }
        */

        if (error == 0)
        {
            char output[256];
            strcpy(output, input);
            strcat(output, argv[1]);
            strcat(output, ".asm");
            FILE *write = fopen(output, "wb");

            if (write == NULL)
            {
                error = 1;
                strcat(errorMsg, "ERRORE NELLA CREAZIONE DEL FILE .asm\n");
            }

            if (error == 0)
            {
                List funCalled;
                init(&funCalled);

                CmdCount cmdCount;
                initCmdCount(&cmdCount);

                preScanDir(input, &funCalled, &cmdCount);
                if (cmdCount._sysinit == 1)
                {
                    pushHead(&funCalled, "Sys.init");
                    scanFunction(funCalled.head, &funCalled, input);
                }
                if (cmdCount._main == 1)
                {
                    push(&funCalled, "Main.main");
                }
                //printf("FUN CALLED : %d\n", cmdCount._call);

                ListStatic sttc;
                initStatic(&sttc);

                List def = setDefaultParam(cmdCount);
                writeToFile(write, def);
                delete (&def);

                int cmdN = 0;
                int funN = 0;

                struct dirent *de; // Pointer for directory entry

                // opendir() returns a pointer of DIR type.
                DIR *dr = opendir(input);

                if (dr == NULL) // opendir returns NULL if couldn't open directory
                {
                    printf("Could not open current directory");
                    return 0;
                }

                while ((de = readdir(dr)) != NULL)
                {
                    // file di estensione .vm == 3
                    int new_len = (strlen(de->d_name) - 3);
                    char nome[256];
                    strcpy(nome, de->d_name);
                    shift(nome, new_len);

                    if (strcmp(nome, ".vm") == 0)
                    {
                        char nomeFile[256];
                        strcpy(nomeFile, input);
                        strcat(nomeFile, de->d_name);
                        //printf("%s\n", nomeFile);

                        FILE *read = fopen(nomeFile, "r");

                        if (read != NULL)
                        {
                            readFromFile(de->d_name, read, write, &error, &sttc, funCalled, cmdCount, &cmdN, &funN);
                            fclose(read);
                        }
                        else
                        {
                        }

                        if (error != 1)
                        {
                            printf("FILE (%s) CONVERTITO\n", de->d_name);
                        }
                    }
                }

                free(dr);

                deleteStatic(&sttc);

                if (error == 1)
                {
                    strcat(errorMsg, "ERRORE DURANTE LA CONVERSIONE\n");
                }

                fclose(write);
            }
        }
    }

    if (error == 1)
    {
        printf("%s\n", errorMsg);
    }
    else
    {
        printf("CONVERSIONE IN .asm AVVENUTA CON SUCCESSO");
    }

    return error;
}