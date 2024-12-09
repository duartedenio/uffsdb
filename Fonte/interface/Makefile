y.tab.c y.tab.h: lex.l yacc.y
	lex lex.l
	yacc -d yacc.y

lex.yy.c: lex.l y.tab.h
	lex lex.l
	yacc -d yacc.y

clean:
	rm y.tab.c lex.yy.c y.tab.h
