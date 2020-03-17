#include"events.h"
#include<malloc.h>
void inite(undo *s){
    s->e = NULL;
    s->num = 0;
}
void epush(undo *s, event a){
    event *new = (event *)calloc(1,sizeof(event));
    *new  = a;
    if(!(s->e)){
        s->e = new;
        s->num++;
        return;
    }
    else
    {
        event *temp = s->e;
        s->e = new;
        s->e->ptr = temp;
        s->num++;
        return;
    }
}
event epop(undo *s){
    event e = {NULL,0,0,NULL,NULL,NULL};

    if(!s->e){
        return e;
    }
    else
    {
        event *temp = s->e;
        s->e = s->e->ptr;
        e = *temp;
        s->num--;
        free(temp);
        return e;
    }

}

int isEmptye(undo *s){
    return s->e == NULL;
}