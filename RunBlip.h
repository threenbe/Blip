#include <iostream>
#include "Parse.h"
#include "String.h"
#include "BST.h"
#include <stack>

static int32_t operate(String& op, int32_t arg1, int32_t arg2) {
	String plus = String("+");
	String minus = String("-");
	String times = String("*");
	String div = String("/");
	String mod = String("%");
	String and_ = String ("&&");
	String or_ = String("||");
	String less = String("<");
	String more = String(">");
	String eq = String("==");
	String uneq = String("!=");
	String less_eq = String("<=");
	String more_eq = String(">=");

	if (op == plus) 
		return arg1 + arg2;
	if (op == minus)
		return arg1 - arg2;
	if (op == times)
		return arg1 * arg2;
	if (op == div)
		return arg1 / arg2;
	if (op == mod)
		return arg1 % arg2;
	if (op == and_)
		return arg1 && arg2;
	if (op == or_)
		return arg1 || arg2;
	if (op == less)
		return arg1 < arg2;
	if (op == more)
		return arg1 > arg2;
	if (op == eq)
		return arg1 == arg2;
	if (op == uneq)
		return arg1 != arg2;
	if (op == less_eq)
		return arg1 <= arg2;
	if (op == more_eq)
		return arg1 >= arg2;
}

/***************************************execute***************************************
This function actually solves an expression. It is only called to solve a single expression
at a time. The result is pushed onto the stack.
INPUTS: references to value and operator stacks
OUTPUTS: none
*/
static void execute(std::stack<int32_t>& value_stack, std::stack<String>& operator_stack) {
	String not_ = String("!");
	String neg = String("~");
	int32_t pop1;
	int32_t pop2;
	String op;

	op = operator_stack.top();
	operator_stack.pop();
	pop2 = value_stack.top();
	value_stack.pop();

	if (op == not_) {
		pop2 = !(pop2);
		value_stack.push(pop2);
	}
	else if (op == neg) {
		pop2 *= -1;
		value_stack.push(pop2);
	}
	else {
		pop1 = value_stack.top();
		value_stack.pop();
		int32_t result = operate(op, pop1, pop2);
		value_stack.push(result);		
	}
}

/***************************************evaluate***************************************
This function evaluates part of an expression. It parses an expression until it either finds
two operands (constant number or variable) after a binary operator or one operand after a unary
operator and then solves the expression by calling the execute function. If there are symbols in place of where
operators are supposed to be, then the expressions with those symbols are evaluated first, and the preceding operators
are evaluated recursively. The final result of an expression should be the sole value left on the value_stack.
INPUTS: references to value_stack, operator_stack, symbol_table
OUTPUTS: none
*/
static void evaluate(std::stack<int32_t>& value_stack, std::stack<String>& operator_stack, BST<String>& symbol_table) {
	String neg = String("~");
	String not_ = String("!");

	read_next_token();
	String tok = String(next_token());
	if (next_token_type == NUMBER) {
		value_stack.push(token_number_value);
	}
	else if (next_token_type == NAME) {
		value_stack.push(symbol_table.getVar(tok));
	}

	else if (next_token_type == SYMBOL) {

		operator_stack.push(tok);

		if (tok == neg || tok == not_) {
			String peek = String(peek_next_token());

			if (next_token_type == NUMBER) {
				read_next_token();
				value_stack.push(token_number_value);
			} else if (next_token_type == NAME) {
				read_next_token();
				tok = String(next_token());
				value_stack.push(symbol_table.getVar(tok));
			} else if (next_token_type == SYMBOL) {
				evaluate(value_stack, operator_stack, symbol_table);
			}
			execute(value_stack, operator_stack);
		}

		else {
			String peek = String(peek_next_token());

			if (next_token_type == NUMBER) {
				read_next_token();
				value_stack.push(token_number_value);
			} else if (next_token_type == NAME) {
				read_next_token();
				tok = String(next_token());
				value_stack.push(symbol_table.getVar(tok));
			} else if (next_token_type == SYMBOL) {
				evaluate(value_stack, operator_stack, symbol_table);
			}

			peek = String(peek_next_token());

			if (next_token_type == NUMBER) {
				read_next_token();
				value_stack.push(token_number_value);
			} else if (next_token_type == NAME) {
				read_next_token();
				tok = String(next_token());
				value_stack.push(symbol_table.getVar(tok));
			} else if (next_token_type == SYMBOL) {
				evaluate(value_stack, operator_stack, symbol_table);
			}
			execute(value_stack, operator_stack);
		}
	}
}

/***************************************begin_eval***************************************
This helper function begins the evaluation of an expression. It keeps calling the evaluate
function until the expression has been solved in its entirety.
INPUTS: references to the value stack, operator stack, and symbol table
OUTPUTS: none
*/
static void begin_eval(std::stack<int32_t>& value_stack, std::stack<String>& operator_stack, BST<String>& symbol_table) {
	String comment = String("//");
	String text = String("text");
	String output = String("output");
	String var = String("var");
	String set = String("set");

	evaluate(value_stack, operator_stack, symbol_table);
	String peek = String(peek_next_token());

	while (peek != var && peek != set && peek != output && peek != text && next_token_type != END && peek != comment) {
		evaluate(value_stack, operator_stack, symbol_table);
		peek = String(peek_next_token());
	}
}

/***************************************run***************************************
This function is to be called by a main function wants to read and execute a blip file.
Parses the file and carries out instructions accordingly.
INPUTS: none
OUTPUTS: none
*/
void run() {
	std::stack<int32_t> value_stack;
	std::stack<String> operator_stack;
	BST<String> symbol_table;

	String comment = String("//");
	String text = String("text");
	String output = String("output");
	String var = String("var");
	String set = String("set");

	read_next_token();
	String tok;
		
	while (next_token_type != END) {

		if (next_token_type == SYMBOL) {
			tok = String(next_token());

			if (tok == comment) {
				skip_line();
			} 
		}

		else if (next_token_type == NAME) {
			tok = String(next_token());

			if (tok == text) {
				read_next_token();
				String tok_text = String(next_token());
				std::cout << tok_text.c_str();
			}

			else if (tok == output) {
				begin_eval(value_stack, operator_stack, symbol_table);
				std::cout << value_stack.top();
				value_stack.pop();
			}

			else if (tok == var) {
				read_next_token();
				String var_tok = String(next_token());

				if (symbol_table.varExists(var_tok)) {

					std::cout << "variable " << var_tok.c_str() << " incorrectly re-initialized\n";

					begin_eval(value_stack, operator_stack, symbol_table);

					symbol_table.updateVar(var_tok, value_stack.top());
					value_stack.pop();
				} else {

					begin_eval(value_stack, operator_stack, symbol_table);

					symbol_table.insert(var_tok, value_stack.top());
					value_stack.pop();
				}
			}

			else if (tok == set) {
				read_next_token();
				String var_tok = String(next_token());

				if (symbol_table.varExists(var_tok)) {

					begin_eval(value_stack, operator_stack, symbol_table);

					symbol_table.updateVar(var_tok, value_stack.top());
					value_stack.pop();
				} else {

					std::cout << "variable " << var_tok.c_str() << " not declared\n";

					begin_eval(value_stack, operator_stack, symbol_table);

					symbol_table.insert(var_tok, value_stack.top());
					value_stack.pop();
				}
			}
		}

		read_next_token();
	}
}
