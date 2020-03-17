//ALL THE BASIC FUNCTIONALITIES OF THE EDITOR
#include"line.h"
#include"undo.h"
#include"stdedit.h" 
#include<string.h>  
#include<strings.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<ncurses.h>
#include<curses.h>
#include<malloc.h>
#include<stdio.h>
#include<stdlib.h>

//using the variables in editor.c file here
extern int rtype;
extern undo redo;
extern undo states;
extern int thresh;
extern char* clip;
extern line data;
extern int Mmode;
extern int lines;
extern int bx,by;
extern int stline; 
extern int fdr,fdw;
extern int ftrack;

//All the functions of a text editor

WINDOW *create_newwin(int height, int width, int starty, int startx){       
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);
    wrefresh(local_win);
    keypad(local_win, TRUE);
    return local_win; 
}
void destroy_win(WINDOW *local_win) {
     wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
     wrefresh(local_win);        
     delwin(local_win); 
}
 
void line_num(){
    int i,j;
    for(i = stline,j = 0; j < (by - 4) ;i++,j++){
        move(j,bx-1);
        printw("%d",(i+1)%10);
    }
    refresh();
}

void display(int stline){
    //clear();
    move(0,0);
    int a,y,x;
    point *temp = data.front;
    for(a = 0; (a < stline) && (stline >=0); a++){
        temp = temp->tail;
    }
    int count = 1;
    while(temp){
        if(count >= (by - 3))
            break;
        clrtoeol();
        node *row = temp->element.store;
        while(row->next){
                printw("%c",row->val);
            row = row->next;    
        }
        count++;
        temp = temp->tail;
    }
    getyx(stdscr,y,x);
    refresh();
    line_num();
    move(y,x);
    refresh();
    return;
    
}

void initialise(int line){
        initq(&data);
        inite(&states);
        inite(&redo);
        getmaxyx(stdscr,by,bx);
        return;
}

int add_line(int line){
    int i = 0;
    point *temp = data.front;
    if(line < lines){
        enqueue_pos(&data,line);
    }
    else if( lines == line){
        enqueue(&data);
    }
    for(i = 0; i < line - 1; i++){
        temp = temp->tail;
    }
    char *str = (char *)calloc(2,1);
    str[0] = '\0';
    int x,y;
    char ch[2] = {'\0'};
    i = 0;
    ch[0] = ipops(&(temp->element));

    getyx(stdscr,y,x);
    
    while(ch[0] != '\n'){
       
        i++;
        strcat(str,ch);
         str = realloc(str,strlen(str) +2);
        ch[0] = ipops(&(temp->element));
    }
    ipushs(&(temp->element),'\n');
    if(strlen(str)){
        while(i--){
            ipushs(&(temp->tail->element),str[i]);
        }
    }
    if(line < lines)
        ipushs(&(temp->tail->element),'\n');
    return temp->tail->element.width;
}

int search_char(char ch,int param,istack t){
    if(param == 0){
        node *temp = t.store;
        while(temp->next){
            if(ch == temp->val)
                return 1;
            temp = temp->next;
        }
        return 0;
    }
    else{
        point *temp = data.front;
        int i = 0;
        for(; i < param; i++)
            temp = temp->tail;
        node *trav = temp->element.store;
        while(trav->next){
            if(ch == trav->val)
                return 1;
            trav = trav->next;
        }
    }

    return 0;

}
int add_char(char ch,int line,int loc){
        int i = 0,y,x,j = 0;
        point *temp = data.front;
        for(i = 0; i < line; i++){
          temp = temp->tail;
        }
        //printw("%p\n",temp);
        //refresh();

        if( ch == '\n'){
            if(ipeeps(&(temp->element)) == '\n'){
                ipops(&(temp->element));
                inserts(&(temp->element),ch,loc);
            }
            else{
                inserts(&(temp->element),ch,loc);
            }
        }
        else if( (temp->element.width == (bx-3)) && (ipeeps(&(temp->element)) == '\n')){
            inserts(&(temp->element),ch,loc);
            return 0;
        }
        else if(temp->element.width >= (bx - 3)){
            if(line == lines){
                
                inserts(&(temp->element),ch,loc);
                lines++;
                //add_line(lines);
                enqueue(&data);
                while((ipeeps(&(temp->element)) == ' ') )
                    ipops(&(temp->element));
                
                if(!search_char(' ',0,temp->element)){
                    inserts(&(temp->tail->element),ipops(&(temp->element)),0);
                    ipushs(&(temp->element),'\n');
                    j = 1;
                }
                else{
                    while(  (!isEmptys(&(temp->element))) && ((i = ipops(&(temp->element))) != ' ') && ++j){
                        inserts(&(temp->tail->element),i,0);
                    }
                    
                    //inserts(&(temp->tail->element),' ',j+1);
                    ipushs(&(temp->element),'\n');
                }
                add_event(&states, "next",bx - 3,line,"","");
                char snd[2];
                snd[0] = '\n';
                snd[1] = ch;
                add_event(&states, "add",0,line+1,snd,"");
                return j;
            }
            else{
                if(loc >= (bx - 3)){
                    inserts(&(temp->tail->element),' ',0);
                    inserts(&(temp->tail->element),ch,0);
                        j++;
                    ipops(&(temp->element));
                    
                    while((ipeeps(&(temp->element)) == ' ') ||  (ipeeps(&(temp->element)) == '\n')){
                        ipops(&(temp->element));
                    }
                    //inserts(&(temp->tail->element),' ',0);
                    // ++j;
                    if(!search_char(' ',0,temp->element)){
                        inserts(&(temp->tail->element),ipops(&(temp->element)),0);
                        ipushs(&(temp->element),'\n');
                        j = 1;
                    }
                    else{
                        while( !isEmptys(&(temp->element)) && ((i = ipops(&(temp->element))) != ' ') && ++j)
                            inserts(&(temp->tail->element),i,0);
                        
                        ipushs(&(temp->element),'\n');
                    }
                }
                else{
                    inserts(&(temp->element),ch,loc);
                    i = ipops(&(temp->element));

                    while((ipeeps(&(temp->element)) == ' ') || (ipeeps(&(temp->element)) == '\n'))
                        ipops(&(temp->element));

                    //++j;            //check this well
                    //inserts(&(temp->tail->element),' ',0);
                    if(!search_char(' ',0,temp->element)){
                        inserts(&(temp->tail->element),ipops(&(temp->element)),0);
                        ipushs(&(temp->element),'\n');
                        j = 1;

                    }else{
                         inserts(&(temp->tail->element),' ',0);
                        while( !isEmptys(&(temp->element)) && ((i = ipops(&(temp->element))) != ' ') && ++j){
                            inserts(&(temp->tail->element),i,0);    
                        }
                        ipushs(&(temp->element),'\n');
                    }
                }
                
            }
            temp = temp->tail;
            int k;
            while(temp->tail && (temp->element.width >= (bx-2))){
                if(ipeeps(&(temp->element)) == '\n')
                    k = ipops(&(temp->element));

                while((ipeeps(&(temp->element)) == ' ') || (ipeeps(&(temp->element)) == '\n'))
                    k = ipops(&(temp->element));

                inserts(&(temp->tail->element),' ',0);
                if(!isEmptys(&(temp->element)))
                    k = ipops(&( temp->element));

                while( (!isEmptys(&(temp->element))) &&  (k != ' ')){
                    inserts(&(temp->tail->element),k,0);
                    k = ipops(&( temp->element));
                    if( (k == ' ') && temp->element.width > (bx - 2)){
                        k = ipops(&( temp->element));
                        inserts(&(temp->tail->element),' ',0);
                    }
                }

                ipushs(&(temp->element),'\n');
                temp = temp->tail;
            }
            if((temp->tail == NULL) && (temp->element.width >= (bx - 2))){
                lines++;
                enqueue(&data);
                while((ipeeps(&(temp->element)) == ' '))
                    k = ipops(&(temp->element));

                //inserts(&(temp->tail->element),' ',0);
                k = ipops(&( temp->element));
                while( (!isEmptys(&(temp->element))) &&  (k != ' ') && (k != '\t')){
                    inserts(&(temp->tail->element),k,0);
                    k = ipops(&( temp->element));
                    if( (k == ' ') && temp->element.width >= (bx - 2)){
                        k = ipops(&( temp->element));
                        //inserts(&(temp->tail->element),' ',0);
                    }
                }
                ipushs(&(temp->element),'\n');
            }
            
        }

        else{
            inserts(&(temp->element),ch,loc);
        }

        return j;
}
        
int getwidth(int y){
    point *temp = data.front;
    int i = 0;
    for(; i < y; i++){
        if(temp->tail == NULL)
            break;
        else
            temp = temp->tail;
    }
    if(ipeeps(&temp->element) == '\n')
        return (temp->element.width - 1);    
    else
        return (temp->element.width);
}

int backspace(int y, int x){
    int i;
    point *temp = data.front;
    for(i = 0; i < y; i++){
        temp = temp->tail;  
    }
    char str[2] = {'\0'};
    //printw("%d",x);
    //refresh();  
    
    if(x == 0){
        if(temp->element.width >= (bx - 2))
            return (bx - 3);
        istack *temp2 = &(temp->tail->element);
        char *str = (char *)calloc(2,1);
        str[0] = '\0';
        char ch[2] = {'\0'};
        i = 0;
        while(!isEmptys(temp2)){

            ch[0] = ipops(temp2);
            i++;
            strcat(str,ch);
            str = realloc(str,strlen(str) +2);

        }
        int t = temp->element.width; 
        if(!isEmptys(&(temp->element))){
                while(ipops(&(temp->element)) != '\n');
            }   
        if(strlen(str)){
            
            while(i--){
                if(temp->element.width >= (bx - 3)){
                        ipushs(&(temp->element),'\n');
                     break;
                 }
                ipushs(&(temp->element),str[i]);
                
            }
            i++;
            if(i >=1 ){
                while(i--)
                    ipushs(&(temp->tail->element),str[i]);
            }
        }
        if(temp->tail->element.width == 0){
            dequeueloc(&data,y+1);
            lines--;
        }
        add_event(&states,"back",temp->element.width - strlen(str),y,"\n","");

        return (temp->element.width - strlen(str));   //check this well
        //return (temp->element.width - 1);
    }
    else if(!isEmptys(&(temp->element))){
        if(x == temp->element.width){
            str[0] = ipops(&(temp->element));
            add_event(&states,"back",x,y,str,"");
        }
        else if(!isEmptys(&(temp->element))){
            str[0] = ipoploc(&(temp->element),x-1);
            add_event(&states,"back",x,y,str,"");

        }
        
    }
    
    int n_x = temp->element.width;
    return n_x;

}
void save(char *fname){
    int fd = open(fname,O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    //int i = 0, fd = open("random.txt", O_WRONLY | O_CREAT ,S_IRUSR | S_IWUSR);
    point *temp = data.front;
    while(temp){
        char *str = (char *)calloc(1,1);
        node *row = temp->element.store;
        char ch[2] = {'\0'};
        str = realloc(str,strlen(str)+1);
        while(row->next){
            write(fd,&row->val,1);
            row = row->next;
        }
        temp = temp->tail;
    }

    close(fd);
}


int readfile(int fdw){
    char buffer[2];
    int x,y,mk;
    point *temp = data.front;
    clear();
    move(0,0);
    stline = 0;
    int count = 0;
    while(read(fdw,buffer,1)){
        getyx(stdscr,y,x);
        if(buffer[0] == '\t' || buffer[0] == '\r')
            continue;
        // if(stline > 0){
        //     display(stline);
        //     break;
        // }
        if(buffer[0] == '\n'){
            add_char(buffer[0],y+stline,x);
                y++;
                lines++;
                if(y >= (by - 4)){
                    stline++;
                    add_line(y+stline - 1);
                }
                else if(lines >= (by - 4)){
                    
                    add_line(y+stline);
                }
                else{
                    add_line(y + stline);
                }
                if(y >= (by - 4))
                    move(by - 5,0);
                else
                    move(y,0);
        }
        else{
            mk = add_char(buffer[0],y+stline,x);
                if( ((x >= (bx - 3)) || (getwidth(y+stline) >= bx-3)) && (mk != 0)){
                    y++;
                    if(y >= (by - 4)){
                        stline++;
                        move(by - 5,mk);
                    }
                    else{
                        move(y,mk);
                    }
                }
                else{
                    move(y,++x);
                }
        }
    }
    stline = 0;
    clear();
    display(0);
    refresh();
    move(0,0);
    refresh();
    return 1;
}

int read_chunck(){
    char buffer[2] = {'\0'};
    int x,y,mk;
    point *temp = data.front;
    int count = 0;
    int t = ftrack;
    int k = 0;
    while(k = read(fdr,buffer,1)){
        if(k == 0)  
            return 0;
        if(buffer[0] == '\t' || buffer[0] == '\r')
            continue;
        if(buffer[0] == '\n'){
            x = getwidth(t);
            add_char(buffer[0],t,x);
            t++;
            lines++;
            add_line(t);
            count++;
            move(t,0);
        }
        else{
            x = getwidth(t);
            mk = add_char(buffer[0],t,x);
                if( (x >= (bx - 3))){
                    count++;
                    t++;
                    move(t,0);
                }
                else{
                    move(t,++x);
                }
        }
        if(count >= thresh){
                break;
        }
    }
    clear();
    display(stline);
    refresh();
    ftrack += thresh;
    return 1;
}


char* get_word(FILE *fp){
    char ch='\0';
    int i = 0;
    char *temp  = (char *)calloc(50,sizeof(char));
    ch = '\0';
    while(ch != EOF){
        ch= fgetc(fp);
        if(ch == 10 || ch == 13 || ch == ' ' || ch == EOF){
            temp[i] = '\0';
            if(ch == EOF)
                return NULL;
            i=0;
            return temp;
        }
        else{
        temp[i] = ch;
        i++;
        }
    }
}
void printopt(WINDOW *win, char** dict,int mark,int count){
    int x,y,i;
    x = 1; 
    y = 1;
    box(win,0,0);
    for(i = 0; i < 4 && i < count; i++ ){                        //show 4 or less most relevant words;
        if(mark == i+1){
            wattron(win,A_REVERSE);
            mvwprintw(win,y,x,"%s",dict[i]);
            wattroff(win,A_REVERSE);
        }
        else{
            mvwprintw(win, y, x, "%s", dict[i]);
        }
        y++;
    }
    wrefresh(win);
    return; 
}
char* dict(int t){
    point *temp = data.front;
    int y,x;
    getyx(stdscr,y,x);
    int i,j,k;
    for(i = 0; i < (stline + y); i++){
        temp = temp->tail;
    }
    char *str = (char *)calloc(100,1);
    char ch;
    i = 0;
    k = t;
    while( !isEmptys(&(temp->element)) && (t > 0) && ((ch = ipoploc(&(temp->element),t-1)) != ' ')){
        str[i++] = ch;
        move(y,--x);
        if(i > 5){
            for(j = 0, k = strlen(str) - 1; j < k; j++, k--){
                ch = str[j];
                str[j] = str[k];
                str[k] = ch;
            }
            return str;
        }
        t--;
    }
    
    if(isEmptys(&(temp->element)))
        move(y,0);
    else{
        k = k - strlen(str) - 1; 
        move(y,k);
        add_char(' ', y+stline, k++);
        move(y,k);
    }
    for(j = 0, k = strlen(str) - 1; j < k; j++, k--){
        ch = str[j];
        str[j] = str[k];
        str[k] = ch;
    }
    
    if(strlen(str) <= 1)
        return str;
    k = 1;
    char *word;
    char **dict = (char **)calloc(4,sizeof(char *));
    i = 0;
    char *filename = (char *)calloc(12,1);

    if (str[0] >= 65 && str[0] <= 90){
        sprintf(filename,"dict/%c.txt",str[0] + 32);
    }
    else{
        sprintf(filename,"dict/%c.txt",str[0]);
    }
    FILE *fp = fopen(filename,"r+");
    if(!fp)
        return str;
    int q;
    while(word = get_word(fp)){ 
        if(strstr(word,"\n"))
            continue;
        for(q = 0; q < strlen(str); q++){
            if (str[q] >= 65 && str[q] <= 90){
                word[q] = word[q] - 32;         
            }
        }
        if(strlen(str) == 2){
            if(strstr(word,str) && word[1] == str[1]){
                dict[i++] = word;
            }
        }
        else if(strlen(str) == 3){
            if(strstr(word,str) && word[1] == str[1] && word[2] == str[2]){
                dict[i++] = word;
            }
        }
        else if(strlen(str) == 4){
            if(strstr(word,str) && word[1] == str[1] && word[2] == str[2] && word[3] == str[3]){
                dict[i++] = word;
            }
        }
        else if(strlen(str) == 5){
            if(strstr(word,str) && word[1] == str[1] && word[2] == str[2] && word[3] == str[3] && word[4] == str[4]){
                dict[i++] = word;
            }
        }
        if(i == 4)
            break;
    }
    int count = i;
    if (count == 0){
        fclose(fp);
        return str;
    }
    WINDOW  *dict_win = create_newwin(count*2, 20,y,x+strlen(str));
    int g,mark = 1,choice = 0;
    printopt(dict_win,dict,mark,count);
    while(1){
        g = wgetch(dict_win);
        switch(g){
            case KEY_UP:
                if(mark == 1)
                    mark = count;  
                else
                    mark--;
                break;
            case KEY_DOWN:
                if(mark == count)
                    mark = 1;  
                else
                    mark++;
                break;
            case 10:
                choice = mark;
                break;
            
            default:
                choice = -1;
                break;
        }
        printopt(dict_win,dict,mark,count);
        if(choice != 0)
            break;
    }
    if(choice == -1){
        destroy_win(dict_win);
        fclose(fp);
        return str;
    }
    else{
        destroy_win(dict_win);
        fclose(fp);
        return dict[mark-1];

    }
    fclose(fp);
    destroy_win(dict_win);
    return str;
}

void master_menu(){
    int y,x;
    getyx(stdscr,y,x);
    WINDOW *menu = create_newwin(3,bx,by-3,0);
    //wborder(menu, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wattron(menu,A_REVERSE);
    mvwprintw(menu,1,1," OPTIONS:F1 | SAVE:F5 | EXIT:F2 | ^X:CUT | ^C:COPY | SUGGESTIONS:TAB | VASU'S TEXT EDITOR | LINE NO.: %d | COORDINATES:(%d,%d)| #LINE:%d",y+stline+1,x,y,(y+stline)/10);
    wattroff(menu,A_REVERSE);
    wrefresh(menu);
    move(y,x);
    return;
}

char* line_copy(int y){
    point *temp = data.front;
    while(y--){
        temp = temp->tail;
    }
    int i;
    node * traverse = temp->element.store;
    char *str = (char *)calloc(temp->element.width  + 1,1);
    for(i = 0; i < temp->element.width; i++){
        if(traverse->val == '\n')
            break;
        str[i] = traverse->val;
        traverse = traverse->next;
    }
    return str;
}
char* line_cut(int y){
    point *temp = data.front;
    while(y--){
        temp = temp->tail;
    }
    int i = 0;
    char ch;
    char *str = (char *)calloc(temp->element.width  + 2,1);
    while(1){
        if(!isEmptys(&(temp->element))){
            ch = ipoploc(&(temp->element),0);
        }
        else
            break;
        if(ch == '\n'){
            inserts(&(temp->element),ch,0);
            break;
        }
        str[i++] = ch;
        str = realloc(str,strlen(str) + 2);
    }
    return str;
}
char* word_cut(int p,int q){
    point *temp = data.front;
    while(q--){
        temp = temp->tail;
    }
    int i = 0;
    char ch;
    char *str = (char *)calloc(2,1); 
    while(1){
        if(!isEmptys(&(temp->element)))
            ch = ipoploc(&(temp->element),p);
        else
            break;
        
        if(ch == '\n' || ch == ' '){
            inserts(&(temp->element),ch,p);
            break;
        }
        str[i++] = ch;    
        str = realloc(str,strlen(str) + 2);
    }
    return str;
}

char* word_copy(int p,int q){
    point *temp = data.front;
    while(q--){
        temp = temp->tail;
    }
    int i = 0;
    node * traverse = temp->element.store;
    while(p--){
        traverse = traverse->next;
    }
    char *str = (char *)calloc(2,1);
    while(1){
        if(!traverse->next || traverse->val == '\n' || traverse->val == ' ')
            break;
        str[i++] = traverse->val;
        str = realloc(str,strlen(str) + 2);
        traverse = traverse->next;
    }
    return str;
}
void options(){
    int y,x;
    getyx(stdscr,y,x);
    WINDOW *options = create_newwin(36,70 ,by/15,bx/4);
    mvwprintw(options,2,2,"COPY - F6 or CTRL + C");
    mvwprintw(options,4,2,"COPY - F3 or CTRL + X");
    mvwprintw(options,6,2,"PASTE  - F12 or CTRL + V");
    mvwprintw(options,8,2,"CLIPBOARD - %s",clip);
    if(Mmode == 0)
        mvwprintw(options,10,2,"ENABLE MOUSE MODE - F4 + M");
    else
        mvwprintw(options,10,2,"DISABLE MOUSE MODE - F4 + N");

    mvwprintw(options,12,2,"GOTO LINE NUMBER - G");
    mvwprintw(options,14,2,"REPLACE - F4 + R or CTRL + R");
    mvwprintw(options,16,2,"SEARCH - F4 + F or CTRL + F");
    mvwprintw(options,18,2,"EXIT - F2 or CTRL + W");
    mvwprintw(options,20,2,"SAVE - F5");
    mvwprintw(options,22,2,"SAVE AND EXIT- F5 + Q");
    mvwprintw(options,24,2,"SUGGESTIONS - TAB");
    mvwprintw(options,26,2,"NO. OF LINES - %d",lines+1);
    mvwprintw(options,28,2,"ELSE PRESS ANY OTHER KEY TO RETURN TO EDITOR");

    wrefresh(options);
    char ch = getch();
    destroy_win(options);
    if(ch == 'g' || ch == 'G'){
        goto_line();
        return;
    }
    clear();
    display(stline);
    move(y,x);
    return;
}

void display_high(int stline, int hx, int hy,int len){
    clear();
    move(0,0);
    int a,y,x;
    point *temp = data.front;
    for(a = 0; (a < stline) && (stline >=0); a++){
        temp = temp->tail;
    }
    int count = 1;
    while(temp){
        if(count >= (by - 3))
            break;
        node *row = temp->element.store;
        int check = 0;
        while(row->next){
            getyx(stdscr,y,x);
            if(((y+stline) == hy) && (x == hx)){
                wattron(stdscr,A_REVERSE);
                check = 1;
            }
            if(check == 1)
                len--;
            if(len  <= 0)
                wattroff(stdscr,A_REVERSE);    
            printw("%c",row->val);
            row = row->next;
        }
        count++;
        temp = temp->tail;
    }
    attroff(A_REVERSE);
    getyx(stdscr,y,x);
    refresh();
    line_num();
    move(y,x);
    return;
    
}


void find_replace(int ctrl){
    WINDOW *getwrd = create_newwin(16,40,by/3,bx/3);
    char *querry = (char *)calloc(2,1);
    char *rplmt = (char *)calloc(2,1);
    char ch[2] = {'\0'};
    int i,j,x,y,k,u,v,r,t,rpt = 0;
    char *str = (char *)calloc(1,2);
    point *temp = data.front;
    node *trace = NULL;
    if(ctrl == 0){   
        mvwprintw(getwrd,2  ,1,  "HEY ! YOU ARE IN SEARCH MODE : -  \n");
        mvwprintw(getwrd,4,1,  "ENTER WORD YOU WANT TO SEARCH : -  \n");
        wmove(getwrd,6,1);
        wrefresh(getwrd);
        int mg2 = 0,mg1;
        while(mg2 != -1){
            getyx(getwrd,u,v);
            mg1 = wgetch(getwrd);
            switch(mg1){
                case KEY_BACKSPACE:
                    break;
                case '\n':case ' ':
                    mg2 = -1;
                    break;
                case '\t':
                    break;
                default:
                    mvwprintw(getwrd,u,v++,"%c",mg1);
                    wrefresh(getwrd);
                    querry[mg2++] = mg1;
                    querry = realloc(querry,strlen(querry) + 2);
                    break;
            }
        }
        wclear(getwrd);
        box(getwrd, 0 , 0);
        mvwprintw(getwrd,6,1, "DURING SEARCH : \n");
        mvwprintw(getwrd,8,1, "PRESS TAB TO SEARCH NEXT");
        mvwprintw(getwrd,10,1, "PRESS ANY KEY TO EXIT");
        mvwprintw(getwrd,12,1, "PRESS ANY KEY NOW TO SEE RESULTS");
        wrefresh(getwrd);
        getchar();
    }
    else{
        mvwprintw(getwrd,2  ,1,  "HEY ! YOU ARE IN REPLACE MODE : -  \n");
        mvwprintw(getwrd,4,1,  "ENTER WORD YOU WANT TO REPLACE  : -  \n");
        wmove(getwrd,6,1);
        wrefresh(getwrd);
        int mg2 = 0,mg1;
        while(mg2 != -1){
            mg1 = wgetch(getwrd);
            switch(mg1){
                case KEY_BACKSPACE:
                    break;
                case '\n':case ' ':
                    mg2 = -1;
                    break;
                case '\t':
                    break;
                default:
                    getyx(getwrd,u,v);
                    mvwprintw(getwrd,u,v++,"%c",mg1);
                    wrefresh(getwrd);
                    querry[mg2++] = mg1;
                    querry = realloc(querry,strlen(querry) + 2);
                    break;
                }
        }
        wclear(getwrd);
        box(getwrd, 0 , 0);
        mvwprintw(getwrd,2,1, "ENTER THE WORD YOU WANT TO REPLACE WITH :- \n");
        wrefresh(getwrd);
        wmove(getwrd,4,1);
        mg2 = 0,mg1;
        while(mg2 != -1){
            mg1 = wgetch(getwrd);
            switch(mg1){
                case KEY_BACKSPACE:
                    break;
                case '\n':case ' ':
                    mg2 = -1;
                    break;
                case '\t':
                    break;
                default:
                    getyx(getwrd,u,v);
                    mvwprintw(getwrd,u,v++,"%c",mg1);
                    wrefresh(getwrd);
                    rplmt[mg2++] = mg1;
                    rplmt = realloc(rplmt,strlen(rplmt) + 2);
                    break;
                }
        }
        //strcat(rplmt," ");

    }
    wrefresh(getwrd);
    destroy_win(getwrd);
    clear();
    getyx(stdscr,y,x);
    display(stline);
    move(y,x);
    for(i = 0; i <= lines; i++){
        rpt = 0;
        j = 0;
        k = 0;
        if((fdr != -1) && (i >= (ftrack  - 2)))
                read_chunck();
        trace = temp->element.store;
        while(rpt == 1 || (trace->next != NULL)){
            getyx(stdscr,y,x);
            if((rpt == 1) || (trace->val == ' ') || (trace->val == '\n') ){
                k = 0;
                getyx(stdscr,y,x);
                if(!strcmp(str,querry)){
                    if(i >= (by - 5))
                            stline = i - (by - 5) + 1;
                        else
                            stline = 0;
                    if(ctrl == 1){
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
                                if( (t >= (bx - 3)) || (getwidth(i) >= (bx-3)) && mk != 0){
                                    t = mk;
                                    j = mk;
                                }
                                else{
                                    t++;
                                    j++;
                                }
                            }
                    }

                    else{
                        display_high(stline, j - strlen(str), i, strlen(querry) + 1);
                        ch[0] = getch();
                        if(ch[0] == '\t'){
                            trace = trace->next;
                            j++;
                            free(str);
                            str = (char *)calloc(2,1);
                            continue;            
                        }
                        else 
                            return;
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
    getwrd = create_newwin(10,40,by/3,bx/3);
    if(ctrl == 0){
        mvwprintw(getwrd, 3,2,"SEARCH FINISHED :\n No More Entries for the word '%s'",querry);
        mvwprintw(getwrd, 6,2,"press any key to exit search mode  :)");
    }
    else{
        mvwprintw(getwrd, 3,2,"REPLACE FINISHED :\n All %s replaced with '%s'",querry,rplmt);
        mvwprintw(getwrd, 6,2,"press any key to see the effect and exit  :)");
        add_event(&states,"replace",0,0,querry,rplmt);
        wrefresh(getwrd);
        return;
    }
    wrefresh(getwrd);
    getchar();
    getyx(stdscr,y,x);
    destroy_win(getwrd);
    clear();
    display(stline);
    refresh();
   // add_event(&states,"replace",0,0,querry,rplmt);
    return;
}

void goto_line(){
    int q,v,u;
    WINDOW *lgoto = create_newwin(10,40,by/3,bx/3);
    mvwprintw(lgoto,2  ,1,  "HEY ! ENTER LINE NUMBER : -  \n");
    char ch[2] = {'\0'};
    char *querry = (char *)calloc(2,1);
    wmove(lgoto,4,1);
    ch[0] = wgetch(lgoto);
    while((ch[0] != '\n')  &&  (ch[0] != ' ')){
            if(ch[0] == KEY_BACKSPACE){
                ch[0] = wgetch(lgoto);
                continue;
            }
            getyx(lgoto,u,v);
            mvwprintw(lgoto,u,v++,ch);
            wrefresh(lgoto);
            strcat(querry,ch);
            querry = realloc(querry,strlen(querry) + 2);
            ch[0] = wgetch(lgoto);
    }
    q = atoi(querry);
    wclear(lgoto);
    box(lgoto,0,0);
    mvwprintw(lgoto,4,1,"LINE NUMBER  - %d",q);
    mvwprintw(lgoto,6,1,  "PRESS ANY KEY TO GO....GOING->\n");
    wrefresh(lgoto);
    getchar();
    clear();
    wclear(lgoto);
    int st = 1;
    if(fdr != -1){
        while((q > lines)){
            st = read_chunck();
            if(st == 0)
                    break;
        }
        //missing
    }
    if((st == 0) || q > lines){
            wclear(lgoto);
            box(lgoto,0,0);
            getyx(stdscr,u,v); 
            box(lgoto,0,0);
            mvwprintw(lgoto,2  ,1, "NO SUCH LINE EXITING \n");
            mvwprintw(lgoto,4,1,"Press any key to exit");
            wrefresh(lgoto);
            getchar();
            destroy_win(lgoto);
            clear();
            display(stline);
            move(u,v);
            return;
        }
        else{
            destroy_win(lgoto);
        }
    stline = q - (by - 5);
    if( stline <= 0){
        stline = 0;
        display_high(stline,0,q-1,getwidth(q-1) );
        move(q - 1,0);
    }
    else{
        display_high(stline,0,q-1,getwidth(q-1) );
        move(q - stline - 1,0);
    }
    return;
}
void self_destruct(){
    while(!isEmptyq(&data)){
        while(!isEmptys(&(data.front->element)))
                ipops(&(data.front->element));
        dequeue(&data);
    }
    printf("\nThe Program terminated Successfully :)\nThanks For Using Vasu's Text Editor\n\n");
    return;
}