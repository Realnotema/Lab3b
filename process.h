#ifndef PROCESS_H
#define PROCESS_H
#include <stdio.h>

Table *InitTable(int n);
Table *SearchVersion (FILE *fb, Table *table, int rel);
int DelTable (Table *table);
int ReadElements (FILE *fb, Table *table);
int AddElement (Table *table, FILE *fb, int key1, int par, int key2, int rel, char *info);
int FindKey1 (Table *table, int key);
int PrintTable (Table *table, FILE *fb);
int DeleteElement (FILE *fb, Table *table, int key);
int DellWithChildren (FILE *fb, Table *table, int key);
int Reorganize (FILE *fb, char *filename, Table *table);

#endif
