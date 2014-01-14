/*
CSE340 Project4

Author: Prof. Rida Bazi, ASU
Modifications done by Ravs (ravs@ravs.me)

Note for ASU students: Please do not copy the code. You are free to
learn the concept and implement it in your way, but copying as it is
is plagiarism.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

#define DEBUG 1	 // 1 => Turn ON debugging, 0 => Turn OFF debugging

void print_debug(const char * format, ...)
{
	va_list args;
	if (DEBUG)
	{
		va_start (args, format);
		vfprintf (stdout, format, args);
		va_end (args);
	}
}

//---------------------------------------------------------
// Lexer

char token[MAX_TOKEN_LENGTH];	  // token string
int  ttype;						// token type
int  activeToken = FALSE;
int  tokenLength;
int  line_no = 1;

char *reserved[] = 
	 {	"",
		"VAR",
		"IF",
		"WHILE",
		"REPEAT",
		"UNTIL",
		"print",
		"ARRAY",
		"+",
		"-",
		"/",
		"*",
		"=",
		":",
		",",
		";",
		"[",
		"]",
		"(",
		")",
		"{",
		"}",
		"<>",
		">",
		"<",
		"ID",
		"NUM",
		"ERROR"
	 };

void skipSpace()
{
	char c;

	c = getchar(); 
	line_no += (c == '\n');
	while (!feof(stdin) && isspace(c))
	{
		c = getchar(); 
		line_no += (c == '\n');
	}

	// return character to input buffer if eof is not reached
	if (!feof(stdin)) 
		ungetc(c, stdin);
}

int isKeyword(char *s)
{
	int i;

	for (i = 1; i <= KEYWORDS; i++)
		if (strcmp(reserved[i],s) == 0)
			return i;
	return FALSE;
}

int scan_number()
{
	char c;

	c = getchar();
	if (isdigit(c))
	{
		if (c == '0')
		{
			token[tokenLength] = c;
			tokenLength++;
		}
		else
		{
			while (isdigit(c))
			{
				token[tokenLength] = c;
				tokenLength++;;
				c = getchar();
			}
			ungetc(c, stdin);
		}
		token[tokenLength] = '\0';
		return NUM;
	}
	else
		return ERROR;
}


int scan_id_or_keyword()
{
	int ttype;
	char c;

	c = getchar();
	if (isalpha(c))
	{
		while (isalnum(c))
		{
			token[tokenLength] = c;
			tokenLength++;
			c = getchar();
		}
		if (!feof(stdin)) 
			ungetc(c, stdin); 
			 
		token[tokenLength] = '\0';
		ttype = isKeyword(token);
		if (ttype == 0) 
			ttype = ID;
		return ttype;
	} else
		return ERROR;
}

void ungetToken()
{
	activeToken = TRUE;
}

int getToken()
{
	char c;

	if (activeToken)
	{
		activeToken = FALSE;
		return ttype;
	}

	skipSpace();
	tokenLength = 0;
	c = getchar();

	switch (c)
	{
		case '+': ttype = PLUS; break;
		case '-': ttype = MINUS; break;
		case '/': ttype = DIV; break;
		case '*': ttype = MULT; break;
		case '=': ttype = EQUAL; break;
		case ':': ttype = COLON; break;
		case ',': ttype = COMMA; break;
		case ';': ttype = SEMICOLON; break;
		case '[': ttype = LBRAC; break;
		case ']': ttype = RBRAC; break;
		case '(': ttype = LPAREN; break;
		case ')': ttype = RPAREN; break;
		case '{': ttype = LBRACE; break;
		case '}': ttype = RBRACE; break;
		case '<':
			c = getchar();
			if (c == '>')
				ttype = NOTEQUAL;
			else
			{
				ungetc(c, stdin);
				ttype = LESS;
			}
			break;

		case '>':
			ttype = GREATER;
			break;

		default :
			if (isdigit(c))
			{
				ungetc(c, stdin);
				ttype = scan_number();
			}
			else if (isalpha(c)) 
			{
				// token is either a keyword or ID
				ungetc(c, stdin);
				ttype = scan_id_or_keyword();
			}
			else if (c == EOF)
			  ttype = EOF;
			else
			  ttype = ERROR;
			break;
	}	// End Switch
	return ttype;
}

//-----------------------------------------
// Evaluate expression node
// author: ravs
int evaluate_expression(struct exprNode* expr_node){
	if(expr_node!=NULL){
		if(expr_node->operator == 0 & expr_node->tag == 0){
			if(expr_node->var_node->var_type == 0)
				return expr_node->var_node->primary_node->value;
			else if(expr_node->var_node->var_type == 1)
				return expr_node->var_node->array_node->primary_node[evaluate_expression(expr_node->var_node->index)].value;
			else
				return 0;
		}
		else if(expr_node->operator == PLUS){
			return evaluate_expression(expr_node->leftOperand) + evaluate_expression(expr_node->rightOperand);
		}
		else if(expr_node->operator == MULT){
			return evaluate_expression(expr_node->leftOperand) * evaluate_expression(expr_node->rightOperand);
		}
		else
			return 0;
	}
	else
		return 0;
}

//---------------------------------------------------------
// Execute
// with proper modifications, author: ravs
void execute_program(struct statementNode * program)
{
	struct statementNode * pc = program;
	int op1, op2, result;

	while (pc != NULL)
	{
		switch (pc->stmt_type)
		{
			case NOOPSTMT:
				pc = pc->next;
				break;

			case PRINTSTMT:
				if (pc->print_stmt == NULL)
				{
					print_debug("Error: pc points to a print statement but pc->print_stmt is null.\n");
					exit(1);
				}
				if (pc->print_stmt->id == NULL)
				{
					print_debug("Error: print_stmt->id is null.\n");
					exit(1);
				}
				if(pc->print_stmt->id->var_type == 0){
					printf("%d\n", pc->print_stmt->id->primary_node->value);
				}
				else if(pc->print_stmt->id->var_type == 1){
					printf("%d\n", pc->print_stmt->id->array_node->primary_node[evaluate_expression(pc->print_stmt->id->index)].value);
				}
				//printf("%d\n", pc->print_stmt->id->value);
				pc = pc->next;
				break;

			case ASSIGNSTMT:
				if (pc->assign_stmt == NULL)
				{
					print_debug("Error: pc points to an assignment statement but pc->assign_stmt is null.\n");
					exit(1);
				}
				if (pc->assign_stmt->lhs == NULL)
				{
					print_debug("Error: assign_stmt->lhs is null.\n");
					exit(1);
				}
				if (pc->assign_stmt->expr_node == NULL)
				{
					print_debug("Error: assign_stmt->expr_node is null.\n");
					exit(1);
				}
				if(pc->assign_stmt->lhs->var_type == 0){
					pc->assign_stmt->lhs->primary_node->value = evaluate_expression(pc->assign_stmt->expr_node);
				}
				else if(pc->assign_stmt->lhs->var_type == 1){
					pc->assign_stmt->lhs->array_node->primary_node[evaluate_expression(pc->assign_stmt->lhs->index)].value = evaluate_expression(pc->assign_stmt->expr_node);
				}
				//pc->assign_stmt->lhs->value = result;
				pc = pc->next;
				break;

			case IFSTMT:
				if (pc->if_stmt == NULL)
				{
					print_debug("Error: pc points to an if statement but pc->if_stmt is null.\n");
					exit(1);
				}
				if (pc->if_stmt->true_branch == NULL)
				{
					print_debug("Error: if_stmt->true_branch is null.\n");
					exit(1);
				}
				if (pc->if_stmt->false_branch == NULL)
				{
					print_debug("Error: if_stmt->false_branch is null.\n");
					exit(1);
				}
				if (pc->if_stmt->leftOperand == NULL)
				{
					print_debug("Error: if_stmt->leftOperand is null.\n");
					exit(1);
				}
				if (pc->if_stmt->rightOperand == NULL)
				{
					print_debug("Error: if_stmt->rightOperand is null.\n");
					exit(1);
				}
				op1 = evaluate_expression(pc->if_stmt->leftOperand);
				op2 = evaluate_expression(pc->if_stmt->rightOperand);
				switch (pc->if_stmt->relop)
				{
					case GREATER:
						if (op1 > op2)
							pc = pc->if_stmt->true_branch;
						else
							pc = pc->if_stmt->false_branch;
						break;
					case LESS:
						if (op1 < op2)
							pc = pc->if_stmt->true_branch;
						else
							pc = pc->if_stmt->false_branch;
						break;
					case NOTEQUAL:
						if (op1 != op2)
							pc = pc->if_stmt->true_branch;
						else
							pc = pc->if_stmt->false_branch;
						break;
					default:
						print_debug("Error: invalid value for if_stmt->relop (%d).\n", pc->if_stmt->relop);
						exit(1);
						break;
				}
				break;

			case GOTOSTMT:
				if (pc->goto_stmt == NULL)
				{
					print_debug("Error: pc points to a goto statement but pc->goto_stmt is null.\n");
					exit(1);
				}
				if (pc->goto_stmt->target == NULL)
				{
					print_debug("Error: goto_stmt->target is null.\n");
					exit(1);
				}
				pc = pc->goto_stmt->target;
				break;

			default:
				print_debug("Error: invalid value for stmt_type (%d).\n", pc->stmt_type);
				exit(1);
				break;
		}
	}
}

//---------------------------------------------------------
// Entry point
int main()
{
	struct statementNode * program = parse_program_and_generate_intermediate_representation();
	execute_program(program);
	return 0;
}

