//THIS IMPLEMENTS THE UNDO FUNCTION
#include"undo.h"
#include"stdedit.h"
#include"line.h"
#include<string.h>
#include<curses.h>
#include<ncurses.h>
#include<stdlib.h>
extern int by,bx;
extern int lines;
extern int stline;
extern line data;
extern undo redo;
extern undo states;
void add_event(undo *s,char *state,int a,int b,char *c, char *d){
    event e;
    e.state = state;
    e.p1 = a;
    e.p2 = b;
    e.p3 = (char *)calloc(sizeof(c)+1,1);
    strcpy(e.p3,c);
    e.p4 = (char *)calloc(sizeof(d)+1,1);
    strcpy(e.p4,d);
    epush(s,e);
}
void backtrack(undo *s){
    if(isEmptye(s) || (s->num == 0)){
       return; 
    }
    event e = epop(s);
    
    if(strstr(e.state,"replace")){
        undo_replace(e);        
        char *temp = e.p3;
        e.p3 = e.p4;
        e.p4 = temp;
        epush(&redo,e);
    }
    else if(strstr(e.state,"add")){
        undo_insert(e);
        
    }
    else if(strstr(e.state,"back")){
        undo_back(e);
    }
    else if(strstr(e.state,"next")){
        undo_next(e);
    }

}
void retrack(undo *s){
     if(isEmptye(s) || s->num == 0){
       return; 
    }
    event e = epop(s);
    if(strstr(e.state,"replace")){
        undo_replace(e);        
        char *temp = e.p3;
        e.p3 = e.p4;
        e.p4 = temp;
        epush(&states,e);       
    }
}
void undo_replace(event e){

    int rpt, j, k,i,y,x,t,r;
        point *temp = data.front;
        node *trace = temp->element.store;
        char *rplmt = e.p3;
        char *querry = e.p4;
        for(i = 0; i <= lines; i++){
            rpt = 0;
            j = 0;
            k = 0;
            trace = temp->element.store;
            char *str = (char *)calloc(2,1);
            while(rpt == 1 || (trace->next != NULL)){
                getyx(stdscr,y,x);
                if(trace->next->next == NULL)
                    rpt = 1;
                if(rpt == 1 || trace->val == ' ' || trace->val == '\n' ){
                    
                    k = 0;
                    getyx(stdscr,y,x);
                    if(!strcmp(str,querry)){
                        if(i >= (by - 5))
                                stline = i - (by - 5) + 1;
                            else
                                stline = 0;
                        
                            t = j;
                            r = strlen(querry);
                            while(r--){
                                if(!isEmptys(&(temp->element)) && t > 0){
                                    ipoploc(&(temp->element),t - 1);
                                    t--;
                                    j--;
                                }
                            }
                            for(r = 0; r < strlen(rplmt); r++) {
                                getyx(stdscr,y,x);
                                int mk = add_char(rplmt[r],i,t);
                                if( (t >= (bx - 3)) || (getwidth(i) >= (bx-3)) && ( mk != 0)){
                                    t = mk;
                                    j = mk;
                                }
                                else{
                                    t++;
                                    j++;
                                }
                            }
                        
                    }
                    trace = trace->next;
                    j++;
                    free(str);
                    str = (char *)calloc(1,2);
                    continue;            
            }
            str[k++] = trace->val;
            str = realloc(str,j+2);
            j++;
            trace = trace->next;
        }
        temp = temp->tail;
    }
    display(stline);
    refresh();
    x = y;
    y = x;
    return;

}
void undo_insert(event e){
    int x,y;
    getyx(stdscr,y,x);
    if(strstr(e.p3,"\n")){
        backspace(e.p2,1);
        epop(&states);
        display(stline);
        refresh();
        move(y,x);
    }
    else{
        backspace(e.p2,e.p1+1);
        epop(&states);
        display(stline);
        refresh();
    }
    if(x == e.p1)
        move(y,++x);

    return;
}

void undo_back(event e){
    int y,x;
    getyx(stdscr,y,x);
    if(strstr(e.p3,"\n")){
        add_char('\n',e.p2,e.p1);
        add_line(e.p2);
        display(stline);
        move(y,x+1);
    }
    else{
        add_char(e.p3[0],e.p2,e.p1-1);
        display(stline);
        move(y,x+1);
    }   
    display(stline);
    move(y,x+1);
    refresh();
    return;

}

void undo_next(event e){
    int x = 0,y = 0;
    getyx(stdscr,y,x);
    backspace(e.p2,0);
    epop(&states);
    clear();
    if((y == 0 ) && (stline != 0)){
        stline--;
    }
    display(stline);
    if(stline >  0){
        move(e.p2 - stline,getwidth(e.p2));
    }
    else{
        x += 2;
        move(e.p2,getwidth(e.p2 -stline));
    }
    refresh();
    return;
}
