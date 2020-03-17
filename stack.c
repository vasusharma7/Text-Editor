#include"stack.h"
#include<malloc.h>
void inits(istack *s){
    
    s->p = calloc(1,sizeof(node));
    s->p->next = NULL;
    s->p->prev = NULL;
    s->width = 0;
    s->store = s->p;
    return;
}
void ipushs(istack *s, int a){
	s->p->val = a;
    pntr temp;
    temp = s->p;
    s->p = calloc(1,sizeof(node));
    s->p->prev = temp;
    s->p->next = NULL;
    temp->next = s->p;
    s->width++;
    return;
}
void inserts(istack *s, int a,int loc){
	node* newnode = (node *)malloc(sizeof(node));
	newnode->val = a;
	node *temp = s->store;
	if( loc > s->width + 1)
		return;
	else if(loc == 0){

		newnode->next = s->store;
        s->store->prev = newnode;
        newnode->prev = NULL;
		s->store = newnode;
		s->width++;
		return;
	}
	else if(loc == s->width + 1){
        newnode->next = NULL;
        s->p->next = newnode;
        newnode->prev = s->p;
		s->p = newnode;
		s->width++;
		return;
	}
	else if(loc > 0){
		while(loc > 1){
			temp = temp->next;
			loc--;
		}
		newnode->next = temp->next;
        newnode->next->prev = newnode;
        newnode->prev = temp;
		temp->next = newnode;
		s->width++;
		return;
	}
   
}

int ipops(istack *s){
      //head pointer is ahead of the last element in the stack
    //printw("%d\t",s->width);
    pntr temp;
    temp = s->p;
    s->width--;
    s->p = s->p->prev;
    //printw("%d\t",s->width);
    s->p->next = NULL;
    int num = s->p->val;
    free(temp);
    return num;
}

int ipoploc(istack *s,int x){
      //head pointer is ahead of the last element in the stack
    //printw("%d\t",s->width);
    pntr temp = s->store;
    int num;
    int i;
    if(x == 0){
        s->store = s->store->next;
        s->store->prev = NULL;
        num = temp->val;
        free(temp);
        s->width--;
        return num;
    }
    for(i = 0; i < x; i++)
        temp = temp->next;
    
    temp->prev->next = temp->next;
    temp->next->prev = temp->prev;
    s->width--;
    num = temp->val;
    free(temp);
    return num;
}

int isEmptys(istack *s){
    if(s->p->prev == NULL)
        return 1;
    else
        return 0;

}
int isFull(istack *s){
    return 0;
}
int ipeeps(istack *s){
    if(!isEmptys(s))
        return s->p->prev->val;
}
