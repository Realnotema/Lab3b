#include "struct.h"
#include "input.h"
#include "process.h"
#include <stdlib.h>
#include <stdio.h>

int Dialog(const char *options[], int CountOptions) {
        int choose = 0;
        for (int i = 0; i < CountOptions; i++)
                puts(options[i]);
        char *errmess = "";
        do {
                puts(errmess);
                do {
                        choose = GetInt();
                } while (choose > CountOptions || choose < 0);
                if (choose < 0 || choose > CountOptions)
                        errmess = "Incorrect input. Try again";
        } while (choose < 0 || choose > CountOptions);
        return choose;
}

int AddMenu (FILE *fb, Table *table) {
        int key[2];
        int par;
        int rel;
        char *mess[] = {"Enter first key:", "Enter second key ", "Enter parent of first element (0 if no parent)", "Enter string:", "Enter realise:"};
        for (int i = 0; i < 2; i++) {
                puts(mess[i]);
                key[i] = GetInt();
                if (i == 0) {
                        puts(mess[2]);
                        do {
                                par = GetInt();
                        } while (FindKey1(table, par) == 1 && par != 0);
                }
                if (i == 1) {
                        puts(mess[4]);
                        rel = GetInt();
                }
        }
        puts(mess[3]);
        char *string = GetStr();
        AddElement(table, fb, key[0], par, key[1], rel, string);
        free(string);
        return 0;
}

int EditMenu (FILE *fb, int choose, Table *table) {
        int flag = 0, key = 0, rel = 0;
        char *name = NULL;
        char *mess[] = {"Enter key of parent:", "Reorganized succesfully!", "Enter realise:", "Enter new filename:"};
        switch (choose) {
                case 1:
                        puts(mess[0]);
                        key = GetInt();
                        flag = DellWithChildren(fb, table, key);
                        break;
                case 2:
                        puts(mess[3]);
                        name = GetStr();
                        flag = Reorganize (fb, name, table);
                        if (flag == 0)
                                puts(mess[1]);
                        free(name);
                        break;
                case 3:
                        puts(mess[2]);
                        rel = GetInt();
                        SearchVersion(fb, table, rel);
                        break;
                case 4:
                        return 1;
        }
        return 0;
}

int Menu(int choose, FILE *fb, Table* table) {
        const char *mess = "Enter key from first key space to delete:";
        const char *options[] = {"1. Delete relatives of element", "2. Reorganize the table", "3. Search by version of element", "4. Quit"};
        int flag, n;
        switch(choose) {
                case 1:
                        AddMenu(fb, table);
                        break;
                case 2:
                        PrintTable (table, fb);
                        break;
                case 3:
                        puts(mess);
                        n = GetInt();
                        DeleteElement(fb, table, n);
                        break;
                case 4:
                        do {
                                flag = EditMenu(fb, Dialog(options, 4), table);
                        } while (flag == 0);
                        break;
                case 5:
                        return 1;
        }
        return 0;
}

int main() {
        Table *table = NULL;
        int flag = 0;
        int count = 0;
        char *filename = NULL;
        char *mess[] = {"Enter count of elements", "Enter name of file:"};
        const char *options[] = {"1. Add element", "2. Print table", "3. Delete element", "4. Edit table", "5. Quit"};
        puts(mess[1]);
        do {
                filename = GetStr();
        } while (filename == NULL);
        FILE *fb = fopen(filename, "r+b");
        if (fb == NULL) {
                fb = fopen(filename, "w+b");
                puts(mess[0]);
                do {
                        count = GetInt();
                } while (count <= 0);
                fwrite(&count, sizeof(int), 1, fb);
                table = InitTable(count);
        } else {
                fread(&count, sizeof(int), 1, fb);
                table = InitTable(count);
                fread(&table->csize, sizeof(int), 1, fb);
                printf("lox %d\n", table->csize);
                ReadElements(fb, table);
        }
        do {
                flag = Menu(Dialog (options, 5), fb, table);
        } while (flag == 0);
        free(filename);
        fclose(fb);
        DelTable(table);
        return 0;
}
