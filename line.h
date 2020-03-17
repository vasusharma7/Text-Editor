#include"stack.h"
typedef struct point{
    struct point* tail;
    istack element;
    struct point* head;
}point;
typedef struct line{
  point* front;
  int lines;
  point* link;
}line;
void enqueue(line *);
void dequeue(line *);
void enqueue_pos(line *, int);
int isEmptyq(line *);
//int isFull(line *);
int ipeepq(line *);
void initq(line *);
int tokenise(char *);
void dequeueloc(line *,int);
