#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

void initCmdCount(CmdCount *cmdCount)
{
    cmdCount->_call = 0;
    cmdCount->_eq = 0;
    cmdCount->_function = 0;
    cmdCount->_gt = 0;
    cmdCount->_lt = 0;
    cmdCount->_main = 0;
    cmdCount->_return = 0;
    cmdCount->_sysinit = 0;
}

int empty(List L)
{
    return (L.head == NULL);
}

void init(List *L)
{
    if (L != NULL)
    {
        (*L).last = malloc(sizeof(listNode_t));
        (*L).head = NULL;
    }
}

void push(List *L, const char text[])
{
    if (L != NULL)
    {
        if (L->head == NULL)
        {
            L->head = L->last;
            // inserisco il primo elemento
            strcpy(L->last->text, text);
            L->last->next = NULL;
        }
        else
        {
            L->last->next = malloc(sizeof(listNode_t));
            strcpy(L->last->next->text, text);
            L->last = L->last->next;
        }
    }
}

void pushHead(List *L, const char text[])
{
    if (L != NULL)
    {
        if (L->head != NULL)
        {
            ListNode t = malloc(sizeof(listNode_t));
            strcpy(t->text, text);
            t->next = L->head;
            L->head = t;
        }
        else
        {
             L->head = L->last;
            strcpy(L->last->text, text);
            L->last->next = NULL;
        }
    }
}

void pop(List *L)
{
    if (L->head != NULL)
    {
        ListNode head = L->head;
        L->head = L->head->next;
        free(head);
    }
}

int checkInList(List L, const char text[])
{
    int found = 0;
    ListNode t = L.head;
    while (t != NULL && found == 0)
    {
        if (strcmp(t->text, text) == 0)
            found = 1;
        else
            t = t->next;
    }
    return found;
}

void delete (List *L)
{
    if (L != NULL)
    {
        while (L->head != NULL)
        {
            ListNode *t = &L->head;
            L->head = L->head->next;
        }

        L->head = NULL;
        L->last = NULL;
        free(L->head);
        free(L->last);
    }
}

int emptyStatic(ListStatic L)
{
    return (L.head == NULL);
}

void initStatic(ListStatic *L)
{
    if (L != NULL)
    {
        (*L).head = malloc(sizeof(liststatic_t));
        (*L).last = malloc(sizeof(liststatic_t));
        (*L).head->next = NULL;
        (*L).head->ramN = -1;
        (*L).head->n = -1;
    }
}

void pushStatic(ListStatic *L, int n, const char *nomeFile)
{
    int len = strlen(nomeFile);
    if (L != NULL)
    {
        ListNodeStatic t = malloc(sizeof(liststatic_t));
        t->n = n;
        t->ramN = (*L).last->ramN + 1;
        t->nomeFile = malloc(sizeof(char) * len);
        strcpy(t->nomeFile, nomeFile);

        (*L).last->next = t;
        (*L).last = t;

        if ((*L).head->n == -1)
        {
            // inserisco il primo elemento
            (*L).head->n = n;
            (*L).head->ramN = 0;
            (*L).head->nomeFile = malloc(sizeof(char) * len);
            strcpy((*L).head->nomeFile, nomeFile);

            (*L).last->ramN = 0;
        }
        else if ((*L).head->next == NULL)
        {
            (*L).head->next = (*L).last;
        }

        /*
        printf("\n\n------------------\n");
        ListNodeStatic t2 = (*L).head;
        while (t2 != NULL)
        {
            printf("STATIC NAME (%s), N (%d) , RAMN (%d)\n", t2->nomeFile, t2->n, t2->ramN);
            t2 = t2->next;
        }
        printf("------------------\n\n");
        */
    }
}

void deleteStatic(ListStatic *L)
{
    if (L != NULL)
    {
        while (L->head != NULL)
        {
            ListNodeStatic *t = &L->head;
            L->head = L->head->next;
        }

        L->head = NULL;
        L->last = NULL;
        free(L->head);
        free(L->last);
    }
}

void printStatic(ListStatic L)
{
    if (&L != NULL)
    {
        ListNodeStatic t = L.head;
        while (t != NULL)
        {
            printf("STATIC NAME (%s), N (%d) , RAMN (%d)\n", t->nomeFile, t->n, t->ramN);
            t = t->next;
        }
    }
}

int checkInStatic(ListStatic L, int n, const char *nomeFile)
{
    int ramM = -1;

    if (&L != NULL)
    {
        ListNodeStatic t = L.head;
        while (t != NULL && ramM == -1)
        {
            if (t->n == n && strcmp(t->nomeFile, nomeFile) == 0)
            {
                ramM = t->ramN;
            }
            else
            {
                t = t->next;
            }
        }
    }

    return ramM;
}