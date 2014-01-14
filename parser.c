/*
CSE340 Project4
Ravs, FALL2013

Note for ASU students: Please do not copy the code. You are free to
learn the concept and implement it in your way but copying as it is
is plagiarism.

*/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#include "parser.h"

/*---------------------------------------------------------
	Auxiliary Data Structure for variables
----------------------------------------------------------*/
struct varNode* varNodeList[20];

//Searches name in varNode List
//Returns index if found else 0
int foundPrimaryVar(char* name){
	int i = 1;
	while(varNodeList[i] != NULL){
		if(varNodeList[i]->var_type == 0 && 
			strcmp(varNodeList[i]->primary_node->name, name) == 0)
			return i;//return the index where id found
		i++;
	}
	return FALSE;
}

void appendPrimaryVar(char* name){
	int i = 1;
	while(varNodeList[i] != NULL)
		i++;
	varNodeList[i] = make_varNode();
	varNodeList[i]->var_type = 0;
	varNodeList[i]->primary_node = make_primaryVarNode();
	varNodeList[i]->primary_node->name = (char*)malloc(sizeof(strlen(name)+1));
	strcpy(varNodeList[i]->primary_node->name, name);
	varNodeList[i]->primary_node->value = 0; //initialize all var with 0
}

void printVarList(){
	int i = 1, idx = 0;
	while(varNodeList[i] != NULL){
		if(varNodeList[i]->var_type == 0){
			printf("Var Node List entry at : %d (type PRIMARY)\n", i);
			printf("Name: %s, Value: %d \n", varNodeList[i]->primary_node->name, varNodeList[i]->primary_node->value);
		}
		else if(varNodeList[i]->var_type == 1){
			printf("Var Node List entry at : %d (type ARRAY)\n", i);
			printf("Name: %s \n", varNodeList[i]->array_node->name);
			if(varNodeList[i]->array_node->primary_node != NULL){
				for(idx=0;idx<varNodeList[i]->array_node->size;idx++){
					printf("Var Array list %s[%d] = %d\n",varNodeList[i]->array_node->name,idx,varNodeList[i]->array_node->primary_node[idx].value);
				}
			}
		}
		i++;
	}
}

int foundArrayVar(char* name){
	int i = 1;
	while(varNodeList[i] != NULL){
		if(varNodeList[i]->var_type == 1 && 
			strcmp(varNodeList[i]->array_node->name, name) == 0)
			return i;//return the index where id found
		i++;
	}
	return FALSE;
}

void appendArrayVar(char* name){
	int i = 1;
	while(varNodeList[i] != NULL)
		i++;
	varNodeList[i] = make_varNode();
	varNodeList[i]->var_type = 1;
	varNodeList[i]->array_node = make_arrayVarNode();
	varNodeList[i]->array_node->name = (char*)malloc(sizeof(strlen(name)+1));
	strcpy(varNodeList[i]->array_node->name, name);
}

void initializeVarNodeArray(int size){
	int i = 1;// idx = 0;
	while(varNodeList[i] != NULL){
		if(varNodeList[i]->var_type == 1){
			varNodeList[i]->array_node->size = size;
			varNodeList[i]->array_node->primary_node = (struct primaryVarNode*)calloc(size, sizeof(struct primaryVarNode));
		}
		i++;
	}
}


/*----------------------------------------------------------
	Create var nodes and statement nodes
-----------------------------------------------------------*/
struct varNode* make_varNode(){
	struct varNode* temp = (struct varNode*)malloc(sizeof(struct varNode));
	temp->var_type = -1;
	temp->primary_node = NULL;
	temp->array_node = NULL;
	temp->index = NULL;
	return temp;
}

struct primaryVarNode* make_primaryVarNode(){
	return (struct primaryVarNode*)malloc(sizeof(struct primaryVarNode));
}

struct arrayVarNode* make_arrayVarNode(){
	return (struct arrayVarNode*)malloc(sizeof(struct arrayVarNode));
}

struct exprNode* make_exprNode(){
	return (struct exprNode*)malloc(sizeof(struct exprNode));
}

struct statementNode* make_statementNode(){
	return (struct statementNode*)malloc(sizeof(struct statementNode));
}

struct assignmentStatement* make_assignmentStatement(){
	return (struct assignmentStatement*)malloc(sizeof(struct assignmentStatement));
}

struct printStatement* make_printStatement(){
	return (struct printStatement*)malloc(sizeof(struct printStatement));
}

struct ifStatement* make_ifStatement(){
	return (struct ifStatement*)malloc(sizeof(struct ifStatement));
}

struct gotoStatement* make_gotoStatement(){
	return (struct gotoStatement*)malloc(sizeof(struct gotoStatement));
}

/*----------------------------------------------------------
	Parse functions
-----------------------------------------------------------*/
void parse_id_list(){
	ttype = getToken();
	if(ttype == ID){
		if(!foundPrimaryVar(token)){
			appendPrimaryVar(token);
		}
		ttype = getToken();
		if(ttype == COMMA){
			parse_id_list();
			return;
		}
		else if(ttype == SEMICOLON){
			ungetToken();
			return;
		}
		else{
			print_debug("Error: Syntax error while parsing id-list\n");
			exit(1);
		}
	}
}

void parse_var(){
	ttype = getToken();
	if(ttype == ID){
		ungetToken();
		parse_id_list();
		ttype = getToken();
		if(ttype == SEMICOLON){
			return;
		}
		else{
			print_debug("Error: Syntax error while parsing var\n");
			exit(1);
		}
	}
	else if(ttype == LBRACE){
		ungetToken();
		return;
	}
	else{
		ungetToken();
		return;
	}
}

void parse_array_id_list(){
	ttype = getToken();
	if(ttype == ID){
		if(!foundArrayVar(token)){
			appendArrayVar(token);
		}
		ttype = getToken();
		if(ttype == COMMA){
			parse_array_id_list();
			return;
		}
		else if(ttype == COLON){
			ungetToken();
			return;
		}
		else{
			print_debug("Error: Syntax error while parsing id-list\n");
			exit(1);
		}
	}
}

void parse_var_array(){
	ttype = getToken();
	if(ttype == ID){
		ungetToken();
		parse_array_id_list();
		ttype = getToken();
		if(ttype == COLON){
			ttype = getToken();
			if(ttype == ARRAY){
				ttype = getToken();
				if(ttype == LBRAC){
					ttype = getToken();
					if(ttype == NUM){
						initializeVarNodeArray(atoi(token));
						ttype = getToken();
						if(ttype == RBRAC){
							ttype = getToken();
							if(ttype == SEMICOLON){
								return;
							}
							else{
								print_debug("Error: Syntax error while parsing parse_var_array, SEMICOLON expected\n");
								exit(1);
							}
						}
						else{
							print_debug("Error: Syntax error while parsing parse_var_array, RBRAC expected\n");
							exit(1);
						}
					}
					else{
						print_debug("Error: Syntax error while parsing parse_var_array, NUM expected\n");
						exit(1);
					}
				}
				else{
					print_debug("Error: Syntax error while parsing parse_var_array, LBRAC expected\n");
					exit(1);
				}
			}
			else{
				print_debug("Error: Syntax error while parsing parse_var_array, ARRAY expected\n");
				exit(1);
			}
		}
		else{
			print_debug("Error: Syntax error while parsing parse_var_array, COLON expected\n");
			exit(1);
		}
	}
	else if(ttype == LBRACE){
		ungetToken();
		return;
	}
	else{
		ungetToken();
		return;
	}
}

struct primaryVarNode* parse_primary_node(){
	int idx = 0;
	idx = foundPrimaryVar(token);
	if(idx)
		return varNodeList[idx]->primary_node;
	else{
		print_debug("Error: ");
		print_debug(token);
		print_debug(" Variable not found in list\n");
		exit(1);
	}
}

struct arrayVarNode* parse_array_node(){
	int idx = 0;
	idx = foundArrayVar(token);
	if(idx)
		return varNodeList[idx]->array_node;
	else{
		print_debug("Error: ");
		print_debug(token);
		print_debug(" Variable not found in list\n");
		exit(1);
	}
}

struct exprNode* factor()
{
	struct exprNode* facto;
	
	ttype = getToken();
	
	if(ttype == LPAREN)
	{	facto = expr();
		ttype = getToken();
		if (ttype == RPAREN)
			return facto;
		else{	
			print_debug("Error: Syntax error while parsing factor. RPAREN expected\n");
			exit(1);
		}
	} 
	else if(ttype == NUM){	
		facto = make_exprNode();
		facto->var_node = make_varNode();
		facto->tag = 0;
		facto->operator = 0;
		facto->leftOperand = NULL;
		facto->rightOperand = NULL;
		facto->var_node->var_type = 0;
		facto->var_node->primary_node = make_primaryVarNode();
		facto->var_node->primary_node->value = atoi(token);
		return facto;
	} 
	else if(ttype == ID){
		ttype = getToken();
		if(ttype == LBRAC){
			ungetToken();//unget the curr token
			facto = make_exprNode();
			facto->var_node = make_varNode();
			facto->tag = 0;
			facto->operator = 0;
			facto->leftOperand = NULL;
			facto->rightOperand = NULL;
			facto->var_node->var_type = 1;//array type
			facto->var_node->array_node = parse_array_node();
			ttype = getToken();
			ttype = getToken();//get token twice since last one we unget
			if((ttype == ID)|(ttype == LPAREN)|(ttype == NUM)){
				ungetToken();
				facto->var_node->index = expr();
				ttype = getToken();
				if(ttype == RBRAC){
					return facto;
				}
				else{
					print_debug("Error: Syntax error while parsing factor, RBRAC expected\n");
					exit(1);
				}
			}
			else{
				print_debug("Error: Syntax error while parsing factor, ID LPAREN or NUM expected\n");
				exit(1);
			}
		}
		else{
			ungetToken();//unget the curr token
			facto = make_exprNode();
			facto->var_node = make_varNode();
			facto->tag = 0;
			facto->operator = 0;
			facto->leftOperand = NULL;
			facto->rightOperand = NULL;
			facto->var_node->var_type = 0;
			facto->var_node->primary_node = parse_primary_node();
			return facto;
		}
	} 
	else{	
		print_debug("Error: Syntax error while parsing factor. NUM or ID expected\n");
		exit(1);
	}
}

struct exprNode* term()
{
	struct exprNode* ter;
	struct exprNode* f;

	
	ttype = getToken();
	if((ttype == ID)|(ttype == LPAREN)|(ttype == NUM))
	{	ungetToken();
		f = factor();
		ttype = getToken();
		if(ttype == MULT){
			ter = make_exprNode();
			ter->operator = ttype; 
			ter->leftOperand = f;
			ter->rightOperand = term();
			ter->tag = 1;
			return ter;
		} 
		else if((ttype == SEMICOLON)|(ttype == PLUS)|(ttype == RPAREN)|(ttype == RBRAC)|
			(ttype == GREATER)|(ttype == LESS)|(ttype == NOTEQUAL)|(ttype == LBRACE)|
			(ttype == ID)|(ttype == IF)|(ttype == PRINT)|(ttype == WHILE)|(ttype == REPEAT)){
			ungetToken();
			return f;
		}
		else{
			print_debug("Error: Syntax error while parsing term, MULT SEMICOLON PLUS RPAREN or RBRAC expected\n");
			exit(1);
		}
	} 
	else{
		print_debug("Error: Syntax error while parsing term, ID LPAREN or NUM expected\n");
		exit(1);
	}
}

struct exprNode* expr()
{
	struct exprNode* exp;
	struct exprNode* t;
	
	ttype = getToken();
	if((ttype == ID)|(ttype == LPAREN)|(ttype == NUM))
	{	ungetToken();
		t = term();
		ttype = getToken();
		if(ttype == PLUS){	
			exp = make_exprNode();
			exp->operator = ttype; 
			exp->leftOperand = t;
			exp->rightOperand = expr();			
			exp->tag = 1;
			return exp;
		}
		else if((ttype == SEMICOLON)|(ttype == MULT)|(ttype == RPAREN)|(ttype == RBRAC)|
			(ttype == GREATER)|(ttype == LESS)|(ttype == NOTEQUAL)|(ttype == LBRACE)|
			(ttype == ID)|(ttype == IF)|(ttype == PRINT)|(ttype == WHILE)|(ttype == REPEAT)){	
			ungetToken();
			return t;
		} 
		else{	
			print_debug("Error: Syntax error while parsing expr, PLUS MULT RPAREN RBRAC or SEMICOLON expected\n");
			exit(1);
		}
	} 
	else{	
		print_debug("Error: Syntax error while parsing expr, ID LPAREN or NUM expected\n");
		exit(1);
	}
} 

struct assignmentStatement* parse_assign_stmt(){
	//print_debug("parse_assign_stmt: Entered\n");
	struct assignmentStatement* astmt;
	ttype = getToken();
	if(ttype == ID){
		ttype = getToken();
		if(ttype == EQUAL){
			ungetToken();
			astmt = make_assignmentStatement();
			astmt->lhs = make_varNode();
			astmt->lhs->var_type = 0;
			astmt->lhs->primary_node = parse_primary_node();
			ttype = getToken();
			ttype = getToken();
			if((ttype == ID)|(ttype == LPAREN)|(ttype == NUM)){
				ungetToken();
				astmt->expr_node = expr();
				ttype = getToken();
				if(ttype == SEMICOLON){
					ungetToken();
					return astmt;
				}
				else{
					print_debug("Error: Syntax error while parsing parse_assign_stmt, SEMICOLON expected\n");
					exit(1);
				}
			}
			else{
				print_debug("Error: Syntax error while parsing parse_assign_stmt, ID NU or LPAREN expected\n");
				exit(1);
			}
		}
		else if(ttype == LBRAC){
			ungetToken();
			astmt = make_assignmentStatement();
			astmt->lhs = make_varNode();
			astmt->lhs->var_type = 1;
			astmt->lhs->array_node = parse_array_node();
			ttype = getToken();
			ttype = getToken();
			if((ttype == ID)|(ttype == LPAREN)|(ttype == NUM)){
				ungetToken();
				astmt->lhs->index = expr();
				ttype = getToken();
				if(ttype == RBRAC){
					ttype = getToken();
					if(ttype == EQUAL){
						ttype = getToken();
						if((ttype == ID)|(ttype == LPAREN)|(ttype == NUM)){
							ungetToken();
							astmt->expr_node = expr();
							ttype = getToken();
							if(ttype == SEMICOLON){
								ungetToken();
								return astmt;
							}
							else{
								print_debug("Error: Syntax error while parsing parse_assign_stmt, SEMICOLON expected\n");
								exit(1);
							}
						}
						else{
							print_debug("Error: Syntax error while parsing parse_assign_stmt, ID NUM or LPAREN expected\n");
							exit(1);
						}
					}
					else{
						print_debug("Error: Syntax error while parsing parse_assign_stmt, EQUAL expected\n");
						exit(1);
					}
				}
				else{
					print_debug("Error: Syntax error while parsing parse_assign_stmt, RBRAC expected\n");
					exit(1);
				}
			}
			else{
				print_debug("Error: Syntax error while parsing parse_assign_stmt, ID NUM or LPAREN expected\n");
				exit(1);
			}
		}
		else{
			print_debug("Error: Syntax error while parsing parse_assign_stmt, EQUAL or LBRAC expected\n");
			exit(1);
		}
	}
	else{
		print_debug("Error: Syntax error while parsing parse_assign_stmt, ID expected\n");
		exit(1);
	}
}

struct printStatement* parse_print_stmt(){
	struct printStatement* pstmt;
	ttype = getToken();
	if(ttype == PRINT){
		ttype = getToken();
		if(ttype == ID){
			ttype = getToken();
			if(ttype == SEMICOLON){
				ungetToken();
				pstmt = make_printStatement();
				pstmt->id = make_varNode();
				pstmt->id->var_type = 0;
				pstmt->id->primary_node = parse_primary_node();
				return pstmt;
			}
			else if(ttype == LBRAC){
				ungetToken();
				pstmt = make_printStatement();
				pstmt->id = make_varNode();
				pstmt->id->var_type = 1;
				pstmt->id->array_node = parse_array_node();
				ttype = getToken();
				ttype = getToken();
				//printf("token = %s, ttype = %d\n",token, ttype);
				if((ttype == ID)|(ttype == LPAREN)|(ttype == NUM)){
					ungetToken();
					pstmt->id->index = expr();
					ttype = getToken();
					if(ttype == RBRAC){
						ttype = getToken();
						if(ttype == SEMICOLON){
							ungetToken();
							return pstmt;
						}
						else{
							print_debug("Error: Syntax error while parsing parse_print_stmt, SEMICOLON expected\n");
							exit(1);
						}
					}
					else{
						print_debug("Error: Syntax error while parsing parse_print_stmt, RBRAC expected\n");
						exit(1);
					}
				}
				else{
					print_debug("Error: Syntax error while parsing parse_print_stmt, ID or NUM expected\n");
					exit(1);
				}
			}
			else{
				print_debug("Error: Syntax error while parsing parse_print_stmt, SEMICOLON or LBRAC expected\n");
				exit(1);
			}
		}
		else{
			print_debug("Error: Syntax error while parsing parse_print_stmt, ID expected\n");
			exit(1);
		}
	}
	else{
		print_debug("Error: Syntax error while parsing parse_print_stmt, PRINT expected\n");
		exit(1);
	}
}

void parse_condition(struct ifStatement* ifStmt){
	ttype = getToken();
	if((ttype == ID)|(ttype == LPAREN)|(ttype == NUM)){
		ungetToken();
		//Left operand
		ifStmt->leftOperand = expr();
		ttype = getToken();
		if(ttype == GREATER | ttype == LESS | ttype == NOTEQUAL){
			switch(ttype){
				case GREATER:
					ifStmt->relop = GREATER;
					break;
				case LESS:
					ifStmt->relop = LESS;
					break;
				case NOTEQUAL:
					ifStmt->relop = NOTEQUAL;
					break;
				default:
					print_debug("Error: Invalid real-operation while parsing parse_condition\n");
					exit(1);
			}
			ttype = getToken();
			if((ttype == ID)|(ttype == LPAREN)|(ttype == NUM)){
				ungetToken();
				//Right operand
				ifStmt->rightOperand = expr();
				return;
			}
			else{
				print_debug("Error: Syntax  error while parsing parse_condition, ID LPAREN or NUM expected\n");
				exit(1);
			}
		}
		else{
			print_debug("Error: Syntax error while parsing parse_condition, GREATER LESS or NOTEQUAL expected\n");
			exit(1);
		}
	}
	else{
		print_debug("Error: Syntax error while parsing parse_condition, ID LPAREN or NUM expected\n");
		exit(1);
	}
}

struct ifStatement* parse_if_stmt(){
	struct ifStatement* ifStmt;
	ttype = getToken();
	if(ttype == IF){
		ifStmt = make_ifStatement();
		ttype = getToken();
		if((ttype == ID)|(ttype == LPAREN)|(ttype == NUM)){
			ungetToken();
			parse_condition(ifStmt);
			ttype = getToken();
			if(ttype == LBRACE){
				ungetToken();
				ifStmt->true_branch = parse_body();
				struct statementNode* noopStmt = make_statementNode();
				noopStmt->stmt_type = NOOPSTMT;
				struct statementNode* temp = ifStmt->true_branch;
				while(temp->next != NULL)
					temp = temp->next;
				temp->next = noopStmt;
				ifStmt->false_branch = noopStmt;
				return ifStmt;
			}
			else{
				print_debug("Error: Syntax error while parsing parse_if_stmt, LBRACE expected\n");
				exit(1);
			}
		}
		else{
			print_debug("Error: Syntax error while parsing parse_if_stmt, ID LPAREN or NUM expected\n");
			exit(1);
		}
	}
	else{
		print_debug("Error: Syntax error while parsing parse_if_stmt, IF expected\n");
		exit(1);
	}
}

struct ifStatement* parse_while_stmt(){
	struct ifStatement* ifStmt;
	ttype = getToken();
	if(ttype == WHILE){
		ifStmt = make_ifStatement();
		ttype = getToken();
		if((ttype == ID)|(ttype == LPAREN)|(ttype == NUM)){
			ungetToken();
			parse_condition(ifStmt);
			ttype = getToken();
			if(ttype == LBRACE){
				ungetToken();
				ifStmt->true_branch = parse_body();
				struct statementNode* noopStmt = make_statementNode();
				noopStmt->stmt_type = NOOPSTMT;
				ifStmt->false_branch = noopStmt;
				return ifStmt;
			}
			else{
				print_debug("Error: Syntax error while parsing parse_while_stmt, LBRACE expected\n");
				exit(1);
			}
		}
		else{
			print_debug("Error: Syntax error while parsing parse_while_stmt, ID LPAREN or NUM expected\n");
			exit(1);
		}
	}
	else{
		print_debug("Error: Syntax error while parsing parse_while_stmt, IF expected\n");
		exit(1);
	}
}

struct ifStatement* parse_repeat_stmt(){
	struct ifStatement* ifStmt;
	ttype = getToken();
	if(ttype == REPEAT){
		ttype = getToken();
		if(ttype == LBRACE){
			ungetToken();
			struct statementNode* repeat_body = parse_body();
			ttype = getToken();
			if(ttype == UNTIL){
				ifStmt = make_ifStatement();
				ttype = getToken();
				if((ttype == ID)|(ttype == LPAREN)|(ttype == NUM)){
					ungetToken();
					parse_condition(ifStmt);
					ifStmt->false_branch = repeat_body;
					struct statementNode* noopStmt = make_statementNode();
					noopStmt->stmt_type = NOOPSTMT;
					ifStmt->true_branch = noopStmt;
					return ifStmt;
				}
				else{
					print_debug("Error: Syntax error while parsing parse_repeat_stmt, ID or NUM expected\n");
					exit(1);
				}
			}
			else{
				print_debug("Error: Syntax error while parsing parse_repeat_stmt, UNTIL expected\n");
				exit(1);
			}
		}
		else{
			print_debug("Error: Syntax error while parsing parse_repeat_stmt, LBRACE expected\n");
			exit(1);
		}
	}
	else{
		print_debug("Error: Syntax error while parsing parse_repeat_stmt, REPEAT expected\n");
		exit(1);
	}
}

struct statementNode* parse_stmt(){
	//print_debug("parse_stmt: Entered\n");
	struct statementNode* st;
	ttype = getToken();
	if(ttype == ID){
		ungetToken();
		st = make_statementNode();
		st->stmt_type = ASSIGNSTMT;
		st->assign_stmt = parse_assign_stmt();
		ttype = getToken();
		if(ttype == SEMICOLON){
			return st;
		}
		else{
			print_debug("Error: Syntax error while parsing parse_stmt, RBRACE expected\n");
			exit(1);
		}
	}
	else if(ttype == PRINT){
		ungetToken();
		st = make_statementNode();
		st->stmt_type = PRINTSTMT;
		st->print_stmt = parse_print_stmt();
		ttype = getToken();
		if(ttype == SEMICOLON){
			return st;
		}
		else{
			print_debug("Error: Syntax error while parsing parse_stmt, SEMICOLON expected\n");
			exit(1);
		}
	}
	else if(ttype == WHILE){
		ungetToken();
		st = make_statementNode();
		st->stmt_type = IFSTMT;
		st->if_stmt = parse_while_stmt();
		//Create gt statementNode
		struct statementNode* gt = make_statementNode();
		gt->stmt_type = GOTOSTMT;
		//Create GOTO-NODE
		struct gotoStatement* gotoStmt = make_gotoStatement();
		gt->goto_stmt = gotoStmt;
		gotoStmt->target = st;
		//append gt to body of the while
		struct statementNode* temp = st->if_stmt->true_branch;
		while(temp->next != NULL)
			temp = temp->next;
		temp->next = gt;
		st->next = st->if_stmt->false_branch;
		ttype = getToken();
		if(ttype == ID | ttype == PRINT | ttype == WHILE | 
			ttype == IF | ttype == REPEAT | ttype == RBRACE){
			ungetToken();
			return st;
		}
		else{
			print_debug("Error: Syntax error while parsing parse_stmt, ID PRINT WHILE IF REPEAT RBRACE expected\n");
			exit(1);
		}
	}
	else if(ttype == IF){
		ungetToken();
		st = make_statementNode();
		st->stmt_type = IFSTMT;
		st->if_stmt = parse_if_stmt();
		st->next = st->if_stmt->false_branch;
		ttype = getToken();
		if(ttype == ID | ttype == PRINT | ttype == WHILE | 
			ttype == IF | ttype == REPEAT | ttype == RBRACE){
			ungetToken();
			return st;
		}
		else{
			print_debug("Error: Syntax error while parsing parse_stmt, ID PRINT WHILE IF REPEAT RBRACE expected\n");
			exit(1);
		}
	}
	else if(ttype == REPEAT){
		ungetToken();
		st = make_statementNode();
		st->stmt_type = IFSTMT;
		st->if_stmt = parse_repeat_stmt();
		//Create gt statementNode
		struct statementNode* gt = make_statementNode();
		gt->stmt_type = GOTOSTMT;
		//Create GOTO-NODE
		struct gotoStatement* gotoStmt = make_gotoStatement();
		gt->goto_stmt = gotoStmt;
		gotoStmt->target = st;
		//append gt to body of the while
		struct statementNode* temp = st->if_stmt->false_branch;
		while(temp->next != NULL)
			temp = temp->next;
		temp->next = gt;
		st->next = st->if_stmt->true_branch;
		ttype = getToken();
		if(ttype == ID | ttype == PRINT | ttype == WHILE | 
			ttype == IF | ttype == REPEAT | ttype == RBRACE){
			ungetToken();
			return st;
		}
		else{
			print_debug("Error: Syntax error while parsing parse_stmt, ID PRINT WHILE IF REPEAT RBRACE expected\n");
			exit(1);
		}
	}
	else{
		print_debug("Error: Syntax error while parsing parse_stmt, statement not recognized\n");
		exit(1);
	}
}

struct statementNode* parse_stmt_list(){
	//print_debug("parse_stmt_list: Entered\n");
	struct statementNode* st;
	struct  statementNode* stl;
	st = parse_stmt();
	ttype = getToken();
	if(ttype == ID | ttype == PRINT | ttype == WHILE | 
		ttype == IF | ttype == REPEAT){
		ungetToken();
		stl = parse_stmt_list();
		if(st->stmt_type == IFSTMT){
			//This is the CATCH!!
			//DO NOT look for false_branch, rather look for
			//NOOPSTMT and link it to the list.
			//st followed by noop followed by stl
			if(st->if_stmt->false_branch->stmt_type == NOOPSTMT){
				st->if_stmt->false_branch->next = stl;
				st->next = st->if_stmt->false_branch;
			}
			else if(st->if_stmt->true_branch->stmt_type == NOOPSTMT){
				st->if_stmt->true_branch->next = stl;
				st->next = st->if_stmt->true_branch;
			}
		}
		else{
			st->next = stl;
		}
		return st;
	}
	else if(ttype == RBRACE){
		ungetToken();
		return st;
	}
	else{
		print_debug("Error: Syntax error while parsing parse_stmt_list, ID PRINT WHILE IF REPEAT expected\n");
		exit(1);
	}
}

struct statementNode* parse_body(){
	//print_debug("parse_body: Entered\n");
	struct statementNode* stl;
	ttype = getToken();
	if(ttype == LBRACE){
		stl = parse_stmt_list();
		ttype = getToken();
		if(ttype == RBRACE){
			return stl;
		}
		else{
			print_debug("Error: Syntax error while parsing body, RBRACE expected");
			exit(1);
		}

	}
	else{
		print_debug("Error: Systax error while parsing body, LBRACE expected\n");
		exit(1);
	}
}

struct statementNode* parse_program(){
	struct statementNode* stl;
	ttype = getToken();
	if(ttype == VAR){
		parse_var();
		parse_var_array();
		//printVarList();
		ttype = getToken();
		if(ttype == LBRACE){
			ungetToken();
			stl = parse_body();
			return stl;
		}
		else{
			print_debug("Error: Syntax error while parsing parse_program, LBRACE expected\n");
			exit(1);
		}
		return NULL;
	}
	else{
		print_debug("Error: Syntax error while parsing program, missing VAR section\n");
		exit(1);
	}
}

struct statementNode * parse_program_and_generate_intermediate_representation(){
	struct statementNode* st = parse_program();
	return st;
}