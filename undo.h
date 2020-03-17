//UNDO OPERATIONS
#include"events.h"
void backtrack(undo *s);
void retrack(undo *s);
void add_event(undo *s,char *state,int a,int b,char *c, char *d);
void undo_replace(event );
void undo_insert(event );
void undo_back(event );
void undo_next(event );
void redo_next(event );
