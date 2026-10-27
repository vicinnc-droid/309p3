// src/tools/calculator_eval.cpp — YOU IMPLEMENT (spec §4.6).
//
// calc_evaluate: evaluate a well-formed token list with two stacks
// (one for numbers, one for operators and '('). See spec §4.6 and the
// worked trace in Appendix C. std::stack or std::vector are both fine.

#include "tools/calculator.h"
#include <stack>

//look at operator
static int precedence(CalcToken::Kind op) {

    if (op== CalcToken::Kind::Times ||
        op == CalcToken::Kind::Divide) {


        return 2;
    }       



    if (op ==CalcToken::Kind::Plus ||
        op ==CalcToken::Kind::Minus) {


        return 1;
    }
    return 0;//return 0 in case
}

// operator on top
static bool apply_operator(
    std::stack<double>& values,
    std::stack<CalcToken::Kind>& ops,
    std::string* error) {

    // makes sure there is an operator with 2 nums
    if (ops.empty() || values.size() < 2) {

        *error = "invalid expression";


        return false;
    }

    // remove operator

    CalcToken::Kind op = ops.top();
    ops.pop();

    // Gget right operand
    double right = values.top();
    values.pop();

    //get left
    double left = values.top();
    values.pop();

    double result = 0.0;

    //do the operation with the numbers
    switch (op) {
        case CalcToken::Kind::Plus:


            result = left + right;

            break;

        case CalcToken::Kind::Minus:
            result = left - right;
            break;


        case CalcToken::Kind::Times:
            result = left * right;

            break;

        case CalcToken::Kind::Divide:
            if (right == 0.0) {

                *error = "division by zero";


                return false;
            }
            result = left / right;
            break;

        default:
            *error = "invalid operator";
            return false;
    }

    //keep result
    values.push(result);

    return true;
}

bool calc_evaluate(const std::vector<CalcToken>& tokens, double* out,
                   std::string* error) {

    //store numbers on stack
    std::stack<double> values;

    //operator stack
    std::stack<CalcToken::Kind> ops;

    //look at each token
for (const CalcToken& token : tokens) {

    //token = number
    if (token.kind == CalcToken::Kind::Number) {
        values.push(token.value);
    }

    // token = (
    else if (token.kind == CalcToken::Kind::LParen) {
        ops.push(token.kind);
    }

    // token = )
    else if (token.kind == CalcToken::Kind::RParen) {

        
        while (!ops.empty() && ops.top() != CalcToken::Kind::LParen) {


            if (!apply_operator(values, ops, error)) {

                return false;
            }
        }

        // get rid of parenthesis
        if (ops.empty()) {

            *error = "not the right parenthessi";
            return false;
        }

        ops.pop();
    }

    // token is operator
    else {

        //apply pemdas
        while (!ops.empty()&&
               ops.top()!= CalcToken::Kind::LParen &&
               precedence(ops.top()) >= precedence(token.kind)) {

            if (!apply_operator(values, ops, error)) {



                return false;
            }
        }
        ops.push(token.kind);
    }
}

// apply ops that are left
while (!ops.empty()) {

    // look through the single parentheses
    if (ops.top() == CalcToken::Kind::LParen) {
        *error = "mismatched parentheses";


        return false;
    }

    //next operation
    if (!apply_operator(values, ops, error)) {


        return false;
    }
}

//make sure there is only 1 answer 
if (values.size() != 1) {
    
    *error = "invalid expression";


    return false;
}

//final answer
*out = values.top();

return true;

}
