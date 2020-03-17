typedef struct node{
        struct node *prev;
    	char val;
    	struct node *next;
    } node;
typedef node* pntr;
typedef struct istack{  
    pntr store;
    int width;
    pntr p;
} istack;
void ipushs(istack *, int );
void inserts(istack *, int ,int );
int ipoploc(istack *,int);
int ipops(istack *);
int isEmptys(istack *);

//int isFull(istack *);
int ipeeps(istack *);
void inits(istack *);
int tokenise(char *);
