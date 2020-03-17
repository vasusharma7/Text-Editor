//libraries and header files
#include<ncurses.h>
#include<curses.h>
#include<malloc.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include"line.h"
#include"stdedit.h"
#include"undo.h"

//macros for 'CONTROL' key
#define CTRL_KEYPRESS(k) ((k)  & 0x1f)

//variable declrations
undo redo;
undo states;
int ftrack;
int thresh;
int mode;
line data;
int Mmode;
int lines;
int bx,by;
int fdr,fdw;
int stline ; //change to stl later
char *clip;

//main function

int main(int argc, char *argv[]){
    initscr();
	clear();
	noecho();
	cbreak();
    MEVENT event;
    char *fname = (char *)calloc(2,1);
    int mg1,mg2 = 0;
    keypad(stdscr, TRUE);
    int mk,get,u,v;
    char *copy;
    lines = 0;
    initialise(lines);
    thresh = by;
    if(argc < 2){
        // errno = EINVAL;
        getmaxyx(stdscr,by,bx);
        move(by/3,bx/2.5);
        wattron(stdscr,A_UNDERLINE);
        printw("VASU'S TEXT EDITOR");
        wattroff(stdscr,A_UNDERLINE);
        move(by/2,bx/3);
        printw("No File Name provided :(");
        mvprintw(by/2 + 2,bx/3,"PRESS ENTER TO CREATE A NEW FILE or ANY OTHER KEY TO EXIT :->");
        mg1 = getch();
        if(mg1 != '\n'){
            endwin();
            exit(0);
        }
        mvprintw(by/2 + 4,bx/2-10,"Enter FileName : ");
        move(by/2 + 4,bx/2+10);
        while(mg2 != -1){
            mg1 = getch();
            switch(mg1){
                case KEY_BACKSPACE:
                    mg2--;
                    continue;
                case '\n':
                    mg2 = -1;
                    break;
                default:
                    fname[mg2++] = mg1;
                    mvprintw(by/2 +4, bx/2 + 10 + mg2,"%c",mg1);
                    refresh();
                    fname = realloc(fname,strlen(fname)+2);
                    break;
                }
        }
        fdr = open(fname,O_RDONLY);
        if(fdr != -1 ){
            fdr = open(fname,O_RDONLY);
            read_chunck();
            stline = 0;
            display(stline);
            move(0,0);
            mode = 1;
        }
        clear();
        move(0,0);
    }
    else{
        fname = argv[1];
        fdr = open(fname,O_RDONLY);
        if(fdr != -1){
            sprintf(fname,"%s",argv[1]);
            fdr = open(argv[1],O_RDONLY);
            //readfile(fdr);
            read_chunck();
            stline = 0;
            display(stline);
            move(0,0);
            mode = 1;   
        }
        clear();
        move(0,0);
    }
    stline = 0;
    display(stline);
    refresh();
    int ch;
    int x = 0, y = 0;
    line_num();
    move(0,0);
    char *insert;
    int i = 0;
    while(1){
        if((mode == 1) && ((y + stline ) >= (ftrack - 5))){
            getyx(stdscr,y,x);
            read_chunck();
            move(y,x);
        }
        if(Mmode == 1)
            mousemask(BUTTON1_CLICKED, NULL);
        else
            mousemask(0, NULL);
        getyx(stdscr,y,x);
        master_menu();
        //cbreak();
        raw();
        if(x >= (bx -2))
            move(y,0);
        ch = getch();
        switch(ch){
            case KEY_MOUSE: 
                if(getmouse(&event) == OK && Mmode == 1){
                    if(event.bstate & BUTTON1_PRESSED){
                        if((event.y+ stline) > lines)
                            break;
                        if(event.x +1 > getwidth(event.y+stline)){
                            move(event.y, getwidth(event.y+stline));
                        }
                        else{
                            move(event.y,event.x);
                        }
                    }
                }
                break;
            case CTRL_KEYPRESS('g'):
                goto_line();    
                break;
            case KEY_F(1):
                options();
                break;
            case KEY_F(4):case CTRL_KEYPRESS('r'):case CTRL_KEYPRESS('f'):
                if (ch !=  CTRL_KEYPRESS('r') && ch != CTRL_KEYPRESS('f')){
                    cbreak();
                get = getch();
                }
                getyx(stdscr,v,u);
                if(get == 'r' || get == 'R' || ch == CTRL_KEYPRESS('r')){
                    getyx(stdscr,y,x);
                    get = stline;
                    find_replace(1);
                    stline = get;
                    clear();
                    display(stline);
                    move(y,x);
                    refresh();
                }
                else if(get == 's' || get == 'S' ||( ch == CTRL_KEYPRESS('f'))){
                    clear();
                    get = stline;
                    find_replace(0);
                    stline = get;
                    display(stline);
                    move(y,x);
                    refresh();
                }
                else if(get == 'm' || get == 'M'){
                    Mmode = 1;
                    break;
                }
                else if(get == 'n' || get == 'N'){
                    Mmode = 0;
                    break;
                }
                else
                    break;
                break;
            case KEY_F(6): case CTRL_KEYPRESS('c'):
                if(x >= getwidth(y))
                    clip = line_copy(y+stline);
                else{
                    clip = word_copy(x,y+stline);
                }
                break; 
            case KEY_F(12): case CTRL_KEYPRESS('v'):
                cbreak();
                if(!clip)
                    break;
                getyx(stdscr,y,x);
                //strcat(clip," ");
                for(i = 0; i < strlen(clip); i++){ 
                    get = getwidth(y+stline);
                    getyx(stdscr,y,x);
                    mk = add_char(clip[i],y+stline,x);
                    if( (x >= (bx - 3))){
                        //clear();
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
                getyx(stdscr,y,x);
                display(stline);
                move(y,x);
                break;
            case KEY_F(2): case CTRL_KEYPRESS('w'):
                endwin();
                self_destruct();
                exit(0);
                break;
            case KEY_F(3): case CTRL_KEYPRESS('x'):
                if(x >= getwidth(y)){
                   clip = line_cut(y);
                   display(stline);
                   move(y,0);
                }
                else{
                    clip = word_cut(x,y);
                    
                    display(stline);
                    move(y,x);
                }
                refresh();
                break;   
            case KEY_F(5): case CTRL_KEYPRESS('s'):
                save(fname);
                get = getch();
                if(get == 'q' || get == 'Q'){
                    endwin();
                    self_destruct();
                    exit(0);
                }
                break;
            case '\n':
                add_event(&states,"next",x,y+stline,"\n","");
                cbreak();
                add_char(ch,y+stline,x);
                y++;
                lines++;
                if(y >= (by - 4)){
                    stline++;
                    add_line(y+stline - 1);
                }
                else if(lines >= (by - 4)){
                    //stline++;
                    add_line(y+stline);
                }
                else{
                    add_line(y + stline);
                }
                clear();
                display(stline);
                if(y >= (by - 4))
                    move(by - 5,0);
                else
                    move(y,0);
                refresh();
                break;
            
            case KEY_BACKSPACE:
                if(x == 0){
                    if(y == 0){
                        if(stline == 0){
                            break;
                        }
                        else{
                            stline--;
                            x = backspace(stline,x);
                            clear();
                            display(stline);
                            refresh();
                            move(y,x);
                            break;
                        }
                    }    
                    else{   
                        --y;
                        x = backspace(y+stline,x);
                        clear();
                        refresh();
                        display(stline);
                        refresh();
                        move(y,x);
                        break;
                    }
                }
                backspace(y+stline,x);
                clear();
                display(stline);
                move(y,--x);
                refresh();
                break;
            case KEY_DC:
                if( (x == 0) && (getwidth(y+stline) == 0))
                    break;
                if( (x == getwidth(y+stline) ) || (x == (bx -3))){
                    backspace(y+stline, 0);
                }
                else{
                    backspace(y+stline, x+1);
                }
                display(stline);
                move(y,x);
                break;
            case KEY_UP:
                if(y == 0){
                    if(stline <= 0)
                        break;
                    x = getwidth(stline-1);
                    clear();
                    display(--stline);
                    refresh();
                    move(0,x);
                    break;
                }
                else{
                    x = getwidth(--y+stline);
                    move(y,x);
                    break;
                }
            case KEY_DOWN:
                y++;
                if( (mode == 1) && ((y + stline ) >= (ftrack - 10))){
                    getyx(stdscr,y,x);
                    read_chunck();
                    display(stline);
                    refresh();
                    move(y-1,x);
                }
                if(lines < (y + stline))
                    break;
                else if(y == (by - 4)){
                        x = getwidth(y+stline);        
                        display(++stline);
                        move((by - 5),x);
                        break;
                    
                }
                else{
                    x = getwidth(y+stline);
                    move(y,x);
                    break;
                }
            case KEY_LEFT:
                if(x == 0){
                    move(y-1,getwidth(y+stline-1));
                    break;
                }
                else{
                    move(y,--x);
                    break;
                }
                    //printw("%d %d",y,x);
            case KEY_RIGHT:
                if(getwidth(y+stline) > x){
                    move(y,++x);
                    break;
                }
                else if(getwidth(y+stline) == x && (y+stline) < lines){
                    move(y+1,0);
                }
                    break;
            case '\t':
                cbreak();
                insert = dict(x);
                i = strlen(insert);
                getyx(stdscr,y,x);
                while(i--){
                    get = getwidth(y+stline);
                    getyx(stdscr,y,x);
                    mk = add_char(insert[strlen(insert) - i - 1],y+stline,x);
                    if( (x >= (bx - 3))){
                        y++;
                        if(y >= by - 4){
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
                getyx(stdscr,y,x);
                clear();
                display(stline);
                move(y,x);
                break;
            case CTRL_KEYPRESS('y'):
                retrack(&redo);
                break;    
            case CTRL_KEYPRESS('z'):
                backtrack(&states);
                break;
            default:
                add_event(&states,"add",x,y+stline,"","");
                cbreak();
                get  = getwidth(y+stline);
                mk = add_char(ch,y+stline,x);
                if( (x >= (bx - 3))){
                    clear();
                    y++;
                    if(y >= (by - 4)){
                        stline++;
                        display(stline);
                        move(by - 5,mk);
                    }
                    else{
                        display(stline);
                            move(y,mk);

                    }
                    
                }
                else{
                    display(stline);
                    move(y,++x);
                }
                break;


        }

    }

}
