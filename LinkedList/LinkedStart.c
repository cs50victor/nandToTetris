/*
Linked List Starter
Victor Atasie
CS 325
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h> //for malloc

struct Node{
    char* symbol;
    char* memLocation;
    struct Node* next;
};

struct LinkedList{
    struct Node* head;
};

void printList(struct LinkedList* list);
void insertAtEnd(struct LinkedList* list,char* position, char* word);
void deleteList(struct LinkedList* list);
char* searchNode(struct LinkedList* list, char* word);


int main()
{
    struct LinkedList myList;
    myList.head = NULL;
    insertAtEnd(&myList, "1","A");
    insertAtEnd(&myList, "1","A");
    insertAtEnd(&myList, "1","A");
    insertAtEnd(&myList, "1","A");
    insertAtEnd(&myList, "2","B");
    insertAtEnd(&myList, "2","B");
    insertAtEnd(&myList, "3","C");
    insertAtEnd(&myList, "3","C");

    printf("Label memlocation %s\n", searchNode(&myList, "A"));
    printList(&myList);
    deleteList(&myList);
    insertAtEnd(&myList, "Reset","A");
    puts("-------------");
    printList(&myList);

    return 0;
}


void insertAtEnd(struct LinkedList* list, char* currentPosition, char* word)
{

    if(list->head == NULL)
    {
        list->head = malloc(sizeof(struct Node*));
        list->head->memLocation = currentPosition;
        list->head->symbol = word;
        list->head->next = NULL;

    }
    else
    {
        if( strcmp(searchNode(list,word), "none") == 0){

            struct Node* position = list->head;

            while(position->next != NULL){
                position = position->next;
            }

            struct Node* new_node = malloc(sizeof(struct Node));
            new_node->memLocation = currentPosition;
            new_node->symbol = word;
            new_node->next = NULL;
            position->next = new_node;
        }
    }

}

char* searchNode(struct LinkedList* list,char* word){
    char* found = "none";
    struct Node* temp = list->head;

    while(temp != NULL){
         //symbol found return location
         if(strcmp(temp->symbol, word) == 0){
             found = temp->memLocation;
         }
         temp = temp->next;
    }
    //assert(temp == NULL);
    //symbol not found="0"
    return found;
}

void deleteList(struct LinkedList* list)
{
    struct Node* temp = list->head;
    while(temp != NULL){
        list->head = temp->next;
        free(temp);
        temp = list->head;
    }
}

void printList(struct LinkedList* list)
{
    struct Node* temp = list->head;

    while( temp != NULL )
    {
        printf("(%s,%s)\n", temp->memLocation, temp->symbol);
        temp = temp -> next;
    }
}
