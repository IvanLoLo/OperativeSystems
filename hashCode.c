/**
    Code for https://journaldev.com article
    Purpose: A Hash Table Implementation
    @author: Vijay Ramachandran
    @date: 01-02-2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define CAPACITY 5923 // Size of the Hash Table

unsigned long hash_function(int x) {
	return (x*60)%CAPACITY;
}

typedef struct Travel {
    int sourceid;
    int dstid;
    int hod;
    float meantime;
} Travel;

typedef struct Info Info;

typedef struct Info {
    Travel travel;
    long next;
};

typedef struct Data Data;

typedef struct Data {
    int id;
    long pos;
};

typedef struct Ht_item Ht_item;

// Define the Hash Table Item here
typedef struct Ht_item {
    int sourceid;
    Travel value;
};


typedef struct LinkedList LinkedList;

// Define the Linkedlist here
typedef struct LinkedList {
    Ht_item* item; 
    LinkedList* next;
};


typedef struct HashTable HashTable;

// Define the Hash Table here
typedef struct HashTable {
    // Contains an array of pointers
    // to items
    Ht_item** items;
    LinkedList** overflow_buckets;
    int size;
    int count;
};


static LinkedList* allocate_list () {
    // Allocates memory for a Linkedlist pointer
    LinkedList* list = (LinkedList*) calloc (1, sizeof(LinkedList));
    return list;
}

static LinkedList* linkedlist_insert(LinkedList* list, Ht_item* item) {
    // Inserts the item onto the Linked List
    if (!list) {
        LinkedList* head = allocate_list();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    } 
    
    else if (list->next == NULL) {
        LinkedList* node = allocate_list();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }

    LinkedList* temp = list;
    while (temp->next) {
        temp = temp->next;
    }
    
    LinkedList* node = allocate_list();
    node->item = item;
    node->next = NULL;
    temp->next = node;
    
    return list;
}

static LinkedList** create_overflow_buckets(HashTable* table) {
    // Create the overflow buckets; an array of linkedlists
    LinkedList** buckets = (LinkedList**) calloc (table->size, sizeof(LinkedList*));
    for (int i=0; i<table->size; i++)
        buckets[i] = NULL;
    return buckets;
}

Ht_item* create_item(int sourceid, Travel value) {
    // Creates a pointer to a new hash table item
    Ht_item* item = (Ht_item*) malloc (sizeof(Ht_item));
    item->sourceid = sourceid;
    item->value = value;

    return item;
}

HashTable* create_table(int size) {
    // Creates a new HashTable
    HashTable* table = (HashTable*) malloc (sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->items = (Ht_item**) calloc (table->size, sizeof(Ht_item*));
    for (int i=0; i<table->size; i++)
        table->items[i] = NULL;
    table->overflow_buckets = create_overflow_buckets(table);

    return table;
}

void handle_collision(HashTable* table, unsigned long index, Ht_item* item) {
    //printf("Entrando aca con %d\n", item->value.dstid);
    LinkedList* head = table->overflow_buckets[index];

    if (head == NULL) {
        // We need to create the list
        head = allocate_list();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else {
        // Insert to the list
        table->overflow_buckets[index] = linkedlist_insert(head, item);
        return;
    }
 }

void ht_insert(HashTable* table, int sourceid, Travel value) {
    // Create the item
    Ht_item* item = create_item(sourceid, value);

    // Compute the index
    int index = hash_function(sourceid);

    Ht_item* current_item = table->items[index];
    
    if (current_item == NULL) {
        //printf("IF Entrando a insert con: %d\n ", value.dstid);
        // sourceid does not exist.
        if (table->count == table->size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Remove the create item
            return;
        }
        
        // Insert directly
        table->items[index] = item; 
        table->count++;
    }

    else {
        //printf("ELSE Entrando a insert con: %d\n ", value.dstid);
        handle_collision(table, index, item);
        return;
    }
}

LinkedList* ht_search(HashTable* table, int sourceid) {
    // Searches the sourceid in the hashtable
    // and returns NULL if it doesn't exist
    int index = hash_function(sourceid);
    Ht_item* item = table->items[index];
    LinkedList* head = table->overflow_buckets[index];

    // Ensure that we move to items which are not NULL
    while (item != NULL) {
        if (item->sourceid == sourceid){
            LinkedList aux = {.item = item, .next = head};
            LinkedList *aux2 = &aux;
            return aux2;
        }
        if (head == NULL)
            return NULL;
        item = head->item;
        head = head->next;
    }
    return NULL;
}

static void free_linkedlist(LinkedList* list) {
    LinkedList* temp = list;
    if (!list)
        return;
    while (list) {
        temp = list;
        list = list->next;
        free(temp->item);
        free(temp);
    }
}

static void free_overflow_buckets(HashTable* table) {
    // Free all the overflow bucket lists
    LinkedList** buckets = table->overflow_buckets;
    for (int i=0; i<table->size; i++)
        free_linkedlist(buckets[i]);
    free(buckets);
}

void free_item(Ht_item* item) {
    // Frees an item
    free(item);
}

void free_hashtable(HashTable* table) {
    // Frees the table
    for (int i=0; i<table->size; i++) {
        Ht_item* item = table->items[i];
        if (item != NULL)
            free_item(item);
    }

    free_overflow_buckets(table);
    free(table->items);
    free(table);
}

int main(){

    HashTable* ht = create_table(CAPACITY);
    
    int source, dst, hod;
    float time;
    char *rest = malloc(50*sizeof(char));
    
    unsigned t0 = clock();

    FILE *file;
    Travel tmp;
    file = fopen("bogota-cadastral-2020-1-All-HourlyAggregate.csv", "rb");
    int i = 0;
    while(!feof(file)){
        if(fgetc(file) == '\n') break;
        i++;
    }
    rewind(file);
    fseek(file, i, SEEK_SET);

    //lee el archivo y guarda los elementos en la hashtable
    while(!feof(file)){
        fscanf(file,"%d,%d,%d,%f,%s", &source, &dst, &hod, &time, rest);
        tmp = (Travel) {.sourceid = source, .dstid = dst, .hod = hod, .meantime = time};
        ht_insert(ht, source, tmp);
    }
    fclose(file);

    unsigned t1 = clock();

    double armar_tabla = ((double)(t1-t0)/CLOCKS_PER_SEC);

    t0 = clock();

    Info info;
    Data info2;

    FILE *viajes;
    FILE *posiciones;
    viajes = fopen("viajes.txt", "wb");
    posiciones = fopen("posiciones.txt", "wb");
    int angel = 1;
    int cont = 0;


    for(int i=0; i<ht->size; i++){
        info2 = (Data) {.id = -1, .pos = -1};
        if(ht->items[i]){
            info = (Info) {.travel = ht->items[i]->value, .next = angel};
            info2 = (Data) {.id = ht->items[i]->sourceid, .pos = cont};
            cont++;
            if(ht->overflow_buckets[i]){
                info.next = cont;
                LinkedList* head = ht->overflow_buckets[i];
                fwrite(&info, sizeof(Info), 1, viajes);
                angel++;
                while(head){
                    info = (Info) {.travel = head->item->value, .next = cont+1};
                    head = head->next;
                    if(head == NULL) info.next = -1;
                    fwrite(&info, sizeof(Info), 1, viajes);
                    angel++;
                    cont++;
                }
            }else{
                info.next = -1;
                fwrite(&info, sizeof(Info), 1, viajes);
            }
        }
        fwrite(&info2, sizeof(Data), 1, posiciones);
    }

    rewind(viajes);
    rewind(posiciones);

    fclose(viajes);
    fclose(posiciones);

    t1 = clock();

    double crear_archivos = ((double)(t1-t0)/CLOCKS_PER_SEC);

    printf("Tiempo creando HastTable: %lf\n", armar_tabla);
    printf("Tiempo creando Archivos: %lf\n", crear_archivos);
    printf("Cantidad de items en la hash (No Overflow) %d\n", ht->count);
    printf("Cantidad de items disponibles: %d\n", ht->size);

    free_hashtable(ht);

    return 0;
}
