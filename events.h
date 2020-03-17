typedef struct events{
    char *state;
    int p1;
    int p2;
    char *p3;
    char *p4;
    struct events* ptr;
}event;

typedef struct undo{
    event *e;
    int num;
}undo;
void inite(undo *);
void epush(undo *, event );
event epop(undo *);
int isEmptye(undo *);
