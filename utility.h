#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

// reverse a string
void reverse(char str[]);

// convert int to char
char *itoaMine(int n);

// convert char to int
int atoiMine(const char str[]);

void shift(char str[], int n);

void strtokMine(char line[], char out[], char fl);

void removeChar(char s[], int c);

void removeComment(char s[]);

void removeInitialSpace(char s[]);

#endif