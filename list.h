#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

typedef struct listNode
{
    char text[256];
    struct listNode *next;
} listNode_t;

typedef listNode_t *ListNode;

typedef struct staticNode {
    int n;
    int ramN;
    char *nomeFile;
    struct staticNode *next;
} staticNode_t;

typedef staticNode_t *ListNodeStatic;

typedef struct 
{
    ListNode head;
    ListNode last;
} list_t;

typedef list_t List;

typedef struct {
    ListNodeStatic head;
    ListNodeStatic last;
} liststatic_t;

typedef liststatic_t ListStatic;

typedef struct {
    int _call;
    int _return;
    int _function;
    int _eq;
    int _lt;
    int _gt; 
    int _sysinit;
    int _main;
} CmdCount;

void initCmdCount (CmdCount *cmdCount);

int empty(List L);

void init(List *L);

void push(List *L, const char text[]);

void pop(List *L);

void pushHead(List *L, const char text[]);

int checkInList(List L, const char text[]);

void delete(List *L);

int emptyStatic(ListStatic L);

void initStatic(ListStatic *L);

void pushStatic(ListStatic *L, int n, const char *nomeFile);

void deleteStatic(ListStatic *L);

int checkInStatic(ListStatic L, int n, const char *nomeFile);

void printStatic(ListStatic L);

#endif