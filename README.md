Compiler
========

A programming project to create a compiler for a simple language. A project to learn the basics involded in writing a compiler from scratch.

Following is the grammar of the language:
	program -> var_section body
	var_section -> VAR int_var_decl array_var_decl
	int_var_decl -> id_list SEMICOLON
	array_var_decl -> id_list COLON ARRAY LBRAC NUM RBRAC SEMICOLON
	id_list -> ID COMMA id_list | ID
	body -> LBRACE stmt_list RBRACE
	stmt_list -> stmt stmt_list | stmt
	stmt -> assign_stmt | print_stmt | while_stmt | if_stmt | repeat_stmt
	assign_stmt -> ID EQUAL expr SEMICOLON
	assign_stmt -> ID LBRAC expr RBRAC EQUAL expr SEMICOLON
	expr -> term PLUS expr
	expr -> term
	term -> factor MULT term
	term -> factor
	factor -> LPAREN expr RPAREN
	factor -> NUM
	factor -> ID
	factor -> ID LBRAC expr RBRAC
	print_stmt -> print ID SEMICOLON
	print_stmt -> print ID LBRAC expr RBRAC SEMICOLON
	while_stmt -> WHILE condition body
	if_stmt -> IF condition body
	repeat_stmt -> REPEAT body UNTIL condition
	condition -> expr relop expr
	relop -> GREATER | LESS | NOTEQUAL

Note
----

Students enrolled in CSE340 at ASU should not copy the code. Please refer it to understand the concept but refrain from copying the code as is. It will attributed as plagiarism and you alone  will be responsible for the consequences.

Ravs, Fall 2013