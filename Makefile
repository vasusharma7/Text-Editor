all:
	cc -c stack.c -o stack
	cc -c line.c -o line
	cc -c stdedit.c -o stdedit
	cc -c editor.c -o editor
	cc -c events.c -o events
	cc -c undo.c  -o undo
	cc stack line stdedit undo events editor -o text_editor -lncurses 
