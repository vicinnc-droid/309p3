// src/tools/calculator_eval.cpp — YOU IMPLEMENT (spec §4.6).
//
// calc_evaluate: evaluate a well-formed token list with two stacks
// (one for numbers, one for operators and '('). See spec §4.6 and the
// worked trace in Appendix C. std::stack or std::vector are both fine.

#include "tools/calculator.h"
#include <stack>

// Determine the priority of an operator
static int precedence(CalcToken::Kind op) {

    if (op == CalcToken::Kind::Times ||
        op == CalcToken::Kind::Divide) {
        return 2;
    }

    if (op == CalcToken::Kind::Plus ||
        op == CalcToken::Kind::Minus) {
        return 1;
    }

    return 0;
}

// Apply the operator at the top of the stack
static bool apply_operator(
    std::stack<double>& values,
    std::stack<CalcToken::Kind>& ops,
    std::string* error) {

    // Make sure we have an operator and two numbers
    if (ops.empty() || values.size() < 2) {
        *error = "invalid expression";
        return false;
    }

    // Get and remove the operator
    CalcToken::Kind op = ops.top();
    ops.pop();

    // Get the right operand first
    double right = values.top();
    values.pop();

    // Get the left operand second
    double left = values.top();
    values.pop();

    double result = 0.0;

    // Perform the operation
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

    // Store the result for later calculations
    values.push(result);
    return true;
}

bool calc_evaluate(const std::vector<CalcToken>& tokens, double* out,
                   std::string* error) {

    // Stack to store numbers
    std::stack<double> values;

    // Stack to store operators
    std::stack<CalcToken::Kind> ops;

    // Process each token in the expression
for (const CalcToken& token : tokens) {

    // If the token is a number, push it onto the values stack
    if (token.kind == CalcToken::Kind::Number) {
        values.push(token.value);
    }

    // If the token is an opening parenthesis, push it onto ops
    else if (token.kind == CalcToken::Kind::LParen) {
        ops.push(token.kind);
    }

    // If the token is a closing parenthesis
    else if (token.kind == CalcToken::Kind::RParen) {

        // Calculate everything inside the parentheses
        while (!ops.empty() && ops.top() != CalcToken::Kind::LParen) {
            if (!apply_operator(values, ops, error)) {
                return false;
            }
        }

        // Remove the opening parenthesis
        if (ops.empty()) {
            *error = "mismatched parentheses";
            return false;
        }

        ops.pop();
    }

    // Otherwise, the token is an arithmetic operator
    else {

        // Apply operators with higher or equal precedence
        while (!ops.empty() &&
               ops.top() != CalcToken::Kind::LParen &&
               precedence(ops.top()) >= precedence(token.kind)) {

            if (!apply_operator(values, ops, error)) {
                return false;
            }
        }

        // Save the current operator
        ops.push(token.kind);
    }
}

// Apply all remaining operators
while (!ops.empty()) {

    // Check for unmatched parentheses
    if (ops.top() == CalcToken::Kind::LParen) {
        *error = "mismatched parentheses";
        return false;
    }

    // Perform the next operation
    if (!apply_operator(values, ops, error)) {
        return false;
    }
}

// Make sure exactly one answer remains
if (values.size() != 1) {
    *error = "invalid expression";
    return false;
}

// Store the final answer
*out = values.top();

return true;

}
