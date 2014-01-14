/*
CSE340 Project4
Ravs, FALL2013

Note for ASU students: Please do not copy the code. You are free to
learn the concept and implement it in your way but copying as it is
is plagiarism.

*/

/*---------------------------------------
	Parse Tree Node function
----------------------------------------*/
struct varNode* make_varNode();
struct primaryVarNode* make_primaryVarNode();
struct arrayVarNode* make_arrayVarNode();
struct exprNode* make_exprNode();
struct statementNode* make_statementNode();
struct assignmentStatement* make_assignmentStatement();
struct printStatement* make_printStatement();
struct ifStatement* make_ifStatement();
struct gotoStatement* make_gotoStatement();

/*---------------------------------------
	Parse function
----------------------------------------*/
void parse_var();
struct statementNode* parse_body();
struct statementNode* parse_stmt_list();
struct statementNode* parse_stmt();
struct exprNode* expr();
struct exprNode* term();
struct exprNode* factor();
