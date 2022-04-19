#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "struct.h"

Table *InitTable(int n) {
        Table *table = malloc(sizeof(Table));
        table->msize = n;
        table->ks1 = malloc(n*sizeof(KeySpace1));
        for (int i = 0; i < n; i++)
                table->ks1[i].busy = 0;
        table->ks2 = malloc(n*sizeof(KeySpace2));
        for (int i = 0; i < n; i++)
                table->ks2[i].busy = 0;
        return table;
}

int ReadElements (FILE *fb, Table *table) {
        int pos1, pos2, size;
        int n = 0;
        while (n != table->csize) {
                fread(&pos1, sizeof(int), 1, fb);
                fread(&table->ks1[pos1].key, sizeof(int), 1, fb);
                fread(&table->ks1[pos1].par, sizeof(int), 1, fb);
                printf("tell%d = %d", n, ftell(fb));
                fread(&table->ks1[pos1].busy, sizeof(int), 1, fb);
                printf(" busy = %d\n", table->ks1[pos1].busy);
                fread(&pos2, sizeof(int), 1, fb);
                table->ks2[pos2].busy = table->ks1[pos1].busy;
                fread(&table->ks2[pos2].key, sizeof(int), 1, fb);
                fread(&table->ks2[pos2].realise, sizeof(int), 1, fb);
                table->ks1[pos1].info = malloc(sizeof(Item));
                fread(&table->ks1[pos1].info->size, sizeof(int), 1, fb);
                table->ks2[pos2].info = table->ks1[pos1].info;
                table->ks1[pos1].info->pos = ftell(fb);
                fseek(fb, table->ks1[pos1].info->pos + table->ks1[pos1].info->size * sizeof(char), SEEK_SET);
                if (table->ks1[pos1].busy == 0) free(table->ks1[pos1].info);
                n++;
        }
        return 1;
}

int DelTable (Table *table) {
        for (int i = 0; i < table->msize; i++) {
                if (table->ks1[i].busy != 0) {
                        free(table->ks1[i].info);
                }
        }
        free(table->ks1);
        free(table->ks2);
        free(table);
        return 0;
}

int hash (int *hash_position) {
        if (*hash_position == *hash_position % 10) return 1;
        *hash_position %= 10;
        return 0;
}

int AddElement (Table *table, FILE *fb, int key1, int par, int key2, int realise, char *info) {
        int pos1 = key1 - 1, pos2 = key2 - 1, flag = 0;
        int busy = 1;
        if (pos1 < 0) pos1 = 0;
        if (pos2 < 0) pos2 = 0;
        if (pos1 >= table->msize) {
                pos1 = 0;
                while (table->ks1[pos1].busy == 1)
                        pos1++;
        }
        if (pos1 < table->msize && table->ks1[pos1].busy == 1) {
                while (pos1 != table->msize - 1 && table->ks1[pos1].busy == 1) {
                        pos1++;
                }
                if (table->ks1[pos1].busy == 1)
                        flag++;
                if (flag == 1) {
                        for (int i = 0; i <  table->msize; i++) {
                                if (table->ks1[i].busy == 0) {
                                        pos1 = i;
                                }
                        }
                }
        }
        if (flag == 1) {
                char *errmess = "Memory error!";
                puts(errmess);
                return 1;
        }
        flag = 0;
        if (pos2 >= table->msize) {
                pos2 = 0;
                while (table->ks2[pos2].busy == 1)
                        pos2++;
        }
        while (pos2 < table->msize && table->ks2[pos2].busy == 1) {
                flag = hash(&pos2);
                for (int i = 0; i < table->msize && table->ks2[pos2].busy == 1; i++)
                        pos2 = (pos2 + i) % table->msize;
                if (flag == 1)
                        break;
        }
        table->csize++;
        fseek(fb, sizeof(int), SEEK_SET);
        fwrite(&table->csize, sizeof(int), 1, fb);
        fseek(fb, 0, SEEK_END);
        // Добавляем в таблицу элементы
        table->ks1[pos1].key = key1;
        table->ks2[pos2].key = key2;
        table->ks1[pos1].par = par;
        table->ks2[pos2].realise = realise;
        table->ks1[pos1].busy = 1;
        table->ks2[pos2].busy = 1;
        // Записываем все в файл
        fwrite(&pos1, sizeof(int), 1, fb);
        fwrite(&key1, sizeof(int), 1, fb);
        fwrite(&par, sizeof(int), 1, fb);
        fwrite(&busy, sizeof(int), 1, fb);
        fwrite(&pos2, sizeof(int), 1, fb);
        fwrite(&key2, sizeof(int), 1, fb);
        fwrite(&realise, sizeof(int), 1, fb);
        // Аналогично работаем с информацией
        table->ks1[pos1].info = (Item *) malloc(sizeof(Item));
        table->ks1[pos1].info->size = strlen(info);
        fwrite(&table->ks1[pos1].info->size, sizeof(int), 1, fb);
        table->ks1[pos1].info->pos = ftell(fb);
        table->ks2[pos2].info = table->ks1[pos1].info;
        fwrite(info, sizeof(char), strlen(info), fb);
        return 0;
}

int FindKey1 (Table *table, int key) {   
        char *errmess = "No match key!\nTry Again";
        if (key == 0) return 0;
        for (int i = 0; i < table->msize; i++) {
                if (table->ks1[i].busy == 1 && table->ks1[i].key == key) {
                        return 0;
                }
        }
        puts(errmess);
        return 1;
}

int FindPos2 (Table *table, int pos1) {   
        int pos2 = 0;
        for (int i = 0; i < table->msize; i++) {
                if (table->ks2[i].busy == 1 && table->ks2[i].info == table->ks1[pos1].info) {
                        pos2 = i;
                        break;
                }
        }
        return pos2;
}

int PrintTable (Table *table, FILE *fb) {
        for (int i = 0; i < table->msize; i++) {
                if (table->ks1[i].busy == 1) {
                        int pos2 = FindPos2(table, i);
                        char *info = calloc(table->ks1[i].info->size + 1, sizeof(char));
                        fseek(fb, table->ks1[i].info->pos, SEEK_SET);
                        fread(info, sizeof(char), table->ks1[i].info->size*sizeof(char), fb);
                        printf("(%d,%d)\t%s\tbusy1 = %d busy2 = %d rel = %d\n", table->ks1[i].key, table->ks2[pos2].key, info, table->ks1[i].busy, table->ks2[pos2].busy, table->ks2[pos2].realise);
                        free(info);
                }
        }
        return 0;
}

int FindPos1 (Table *table, int key) {
        int pos1 = 0;
        for (int i = 0; i < table->msize; i++) {
                if (table->ks1[i].busy == 1 && table->ks1[i].key == key) {
                        pos1 = i;
                        return pos1;
                }
        }
        return pos1;
}

int DeleteElement (FILE *fb, Table *table, int key) {
        int temp = -1, n = 0;
        int pos1 = FindPos1(table, key);
        table->ks1[pos1].busy = 0;
        int pos2 = FindPos2(table, pos1);
        table->ks2[pos2].busy = 0;
        table->csize--;
        fseek(fb, sizeof(int), SEEK_SET);
        fwrite(&table->csize, sizeof(int), 1, fb);
        fseek(fb, 0*sizeof(int), SEEK_END);
        fseek(fb, 3 * sizeof(int), SEEK_SET);
        fread(&temp, sizeof(int), 1, fb);
        while (temp != key && n < table->msize) {
                fseek(fb, table->ks1[pos1].info->pos + (table->ks1[pos1].info->size + 1) * sizeof(char), SEEK_SET);
                fread(&temp, sizeof(int), 1, fb);
                n++;
        }
        fseek(fb, sizeof(int), SEEK_SET);
        fwrite(&table->ks1[pos1].busy, sizeof(int), 1, fb);
        free(table->ks1[pos1].info);
        fseek(fb, 0, SEEK_END);
        return 0;
}

int FindParents (Table *table, int key) {
        for (int i = 0; i < table->msize; i++) {
                if (table->ks1[i].busy == 1 && table->ks1[i].par == key)
                        return table->ks1[i].key;
        }
        return -1;
}

int DellWithChildren (FILE *fb, Table *table, int key) {
        if (key == -1) return 0;
        DeleteElement(fb, table, key);
        DellWithChildren(fb, table, FindParents(table, key));
        return 1;
}

Table *SearchVersion (FILE *fb, Table *table, int rel) {
        int j = 0;
        Table *temp = InitTable(table->csize);
        for (int i = 0; i < table->msize; i++) {
                if (table->ks2[i].busy == 1 && table->ks2[i].realise == rel) {
                        temp->ks2[j] = table->ks2[i];
                        temp->ks1[j] = table->ks1[FindPos2(table, j)];
                        j++;
                }
        }
        PrintTable(temp, fb);
        free(temp->ks1);
        free(temp->ks2);
        free(temp);
        return temp;
}

int Reorganize (FILE *fb, char *filename, Table *table) {
        FILE *nf = fopen(filename, "w+b");
        fwrite(&table->msize, sizeof(int), 1, nf);
        fwrite(&table->csize, sizeof(int), 1, nf);
        int busy = 1;
        for (int i = 0; i < table->msize; i++) {
                if (table->ks1[i].busy == 1) {
                        int pos2 = FindPos2(table, i);
                        char *info = calloc(table->ks1[i].info->size + 1, sizeof(char));
                        fseek(fb, table->ks1[i].info->pos, SEEK_SET);   
                        fread(info, sizeof(char), table->ks1[i].info->size*sizeof(char), fb);
                        fwrite(&i, sizeof(int), 1, nf);
                        fwrite(&table->ks1[i].key, sizeof(int), 1, nf);
                        fwrite(&table->ks1[i].par, sizeof(int), 1, nf);
                        fwrite(&busy, sizeof(int), 1, nf);
                        fwrite(&pos2, sizeof(int), 1, nf);
                        fwrite(&table->ks2[pos2].key, sizeof(int), 1, nf);
                        fwrite(&table->ks2[pos2].realise, sizeof(int), 1, nf);
                        fwrite(&table->ks1[i].info->size, sizeof(int), 1, nf);
                        fwrite(info, sizeof(char), strlen(info), nf);
                        free(info);
                }
        }
        fclose(nf);
        return 0;
}
