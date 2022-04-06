#ifndef STRUCT_H
#define STRUCT_H

typedef struct Item {
        int pos;  
        int size;
} Item;

typedef struct KeySpace1 {
        Item key;
        Item par;
        Item busy;
        Item info;
} KeySpace1;

typedef struct KeySpace2 {
        Item key;
        Item realise;
        Item busy;
        Item info;
} KeySpace2;

typedef struct Table {
        KeySpace1 ks1;
        KeySpace2 ks2;
        Item msize;
        Item csize;
} Table;

#endif
