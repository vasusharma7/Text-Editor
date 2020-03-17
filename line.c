#include"line.h"
#include<malloc.h>
#include<ncurses.h>
#include<stdlib.h>
#include<stdio.h>
void initq(line *s){
    s->link = (point *)calloc(1,sizeof(point));
    //s->link->element = (istack *)calloc(1,sizeof(istack));
    inits(&(s->link->element));
    s->link->head = NULL;
    s->link->tail = NULL;
    //printw("%p\n",s->link->tail);
    //refresh();
    s->front = s->link;
    s->lines = 0;
}
void enqueue(line *s){
    point *temp =  (point *)calloc(1,sizeof(point));
    temp->head = s->link;
    temp->tail = NULL;
    //printw("1\n");
    //refresh();
    inits(&(temp->element));
    s->link->tail = temp;
    s->link = temp;
    s->lines++;


}
void enqueue_pos(line *l, int a){
    point *newnode =  (point *)calloc(1,sizeof(point));
    inits(&(newnode->element));
    point* temp = l->front;
    int i = 0;
    for(; i < a; i++)
        temp = temp->tail;
    temp->head->tail = newnode;
    newnode->tail = temp;
    newnode->head = temp->head;
    temp->head = newnode;
    l->lines++;
    return;

}
void dequeue(line *s){
    s->link->head->tail = NULL;
    point *temp = s->link;
    s->link = s->link->head;
    free(temp);
    return;
}
void dequeueloc(line *s,int x){
    point* temp = s->front;
    int num;
    int i;
    for(i = 0; i < x; i++)
        temp = temp->tail;
    
    if(temp->tail == NULL){
        dequeue(s);
        return;
    }
    temp->head->tail = temp->tail;
    temp->tail->head = temp->head;

    s->lines--;
    free(temp);
    return;

}
int isEmptyq(line *s){
    if(s->link->head == s->link->tail)
        return 1;
    else
        return 0;
}
/*int isFull(line *s->link){
    if(s->link->i ==  SIZE -1)
        return 1;
    else
        return 0;
}*/
int ipeepq(line *s){


}
