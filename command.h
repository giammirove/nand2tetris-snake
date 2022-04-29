#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include "list.h"

typedef struct {
    char name[256];
    char mne[256];
} segment_t;

typedef segment_t Segment;

List setDefaultParam(CmdCount cmdCount);
void getMneOfSegment(const char segment[], char mne[]);

List addCmd(int cmdN);
List subCmd(int cmdN);
List negCmd(int cmdN);
List eqCmd(int cmdN, int eqCount);
List gtCmd(int cmdN, int gtCount);
List ltCmd(int cmdN, int ltCount);
List andCmd();
List orCmd();  
List notCmd();

List pushCmd(const char nomeFile[], ListStatic *sttc, const char segment[], const char n[], int cmdN, int *error);
List popCmd(const char nomeFile[], ListStatic *sttc, const char segment[], char n[], int *error);
List labelCmd(const char nomeFun[], const char labelText[]);
List gotoCmd(const char nomeFun[], const char labelText[]);
List ifgotoCmd(const char nomeFun[], const char labelText[]);

List callFunction(const char nameF[], const char nArgs[], int funcN, int *error, int callCount);
List function(const char nameF[], const char nVars[], int *error);
List returnFunction(const char nameF[], int *error, int returnCount, int funN);

#endif