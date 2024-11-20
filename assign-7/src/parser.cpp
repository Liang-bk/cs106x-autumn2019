/*
 * File: parser.cpp
 * ----------------
 * This file implements the parser.h interface.
 */

#include <iostream>
#include <string>
#include "error.h"
#include "exp.h"
#include "parser.h"
#include "strlib.h"
#include "tokenscanner.h"
#include "set.h"
using namespace std;

static Expression *readE(TokenScanner& scanner, const SSModel& model, int prec = 0);
static Expression *readT(TokenScanner& scanner, const SSModel& model);
static int precedence(const std::string& token);

static const Set<string> s = {
    "average",
    "sum",
    "product",
    "max",
    "min",
    "median",
    "stdev",
    "mean"
};
/**
 * Implementation notes: parseExp
 * ------------------------------
 * This code just reads an expression and then checks for extra tokens.
 */

Expression *parseExp(TokenScanner& scanner, const SSModel& model) {
   Expression *exp = readE(scanner, model, 0);
   if (scanner.hasMoreTokens()) {
      error("Unexpected token \"" + scanner.nextToken() + "\"");
   }
   return exp;
}

/**
 * Implementation notes: readE
 * Usage: exp = readE(scanner, prec);
 * ----------------------------------
 * The implementation of readE uses precedence to resolve the ambiguity in
 * the grammar.  At each level, the parser reads operators and subexpressions
 * until it finds an operator whose precedence is greater than the prevailing
 * one.  When a higher-precedence operator is found, readE calls itself
 * recursively to read that subexpression as a unit.
 */

Expression *readE(TokenScanner& scanner, const SSModel& model, int prec) {
   Expression *exp = readT(scanner, model);
   string token;
   while (true) {
      token = scanner.nextToken();
      int tprec = precedence(token);
      if (tprec <= prec) break;
      Expression *rhs = readE(scanner, model, tprec);
      exp = new CompoundExp(token, exp, rhs);
   }
   scanner.saveToken(token);
   return exp;
}

/**
 * Implementation notes: readT
 * ---------------------------
 * This function scans a term, which is either an integer, an identifier,
 * or a parenthesized subexpression.
 */
Expression *readT(TokenScanner& scanner, const SSModel& model) {
   string token = scanner.nextToken();
   TokenScanner::TokenType type = scanner.getTokenType(token);
   if (type == TokenScanner::WORD) {
       location cell;
       if (stringToLocation(token, cell) && model.nameIsValid(token)) return new IdentifierExp(token);

       if (s.contains(toLowerCase(token))) {
           string funcName = toLowerCase(token);

           if (scanner.nextToken() != "(") error("Expected token \"(\"");

           // first cellname
           token = scanner.nextToken();
           if (token == "") error("Expected token cellname");
           type = scanner.getTokenType(token);
           if (type != TokenScanner::WORD) error("Unexpected token \"" + token + "\"");
           if (!stringToLocation(token, cell) || !model.nameIsValid(token)) error("Invalid name " + token);
           location startCell = cell;

           // symbol ":"
           if (scanner.nextToken() != ":") error("Expected token :");

           // second cellname
           token = scanner.nextToken();
           if (token == "") error("Expected token cellname");
           type = scanner.getTokenType(token);
           if (type != TokenScanner::WORD) error("Unexpected token \"" + token + "\"");
           if (!stringToLocation(token, cell) || !model.nameIsValid(token)) error("Invalid name " + token);
           location endCell = cell;

           // make sure that startCell is less than endCell so that there won't have situation like {A2:A1}
           if (endCell.col < startCell.col || endCell.row < startCell.row) error("startCell is bigger than endCell, please check input cells");
           if (scanner.nextToken() != ")") error("Unbalanced parentheses");

           return new FormulaExp(funcName, range{startCell, endCell});
       } else {
           error("Undefined word");
       }

   }
   if (type == TokenScanner::NUMBER) return new DoubleExp(stringToReal(token));
   if (type == TokenScanner::STRING) return new TextStringExp(token.substr(1, token.length() - 2));
   if (token != "(") error("Unexpected token \"" + token + "\"");
   Expression *exp = readE(scanner, model);
   if (scanner.nextToken() != ")") {
      error("Unbalanced parentheses");
   }
   return exp;
}

/**
 * Implementation notes: precedence
 * --------------------------------
 * This function checks the token against each of the defined operators
 * and returns the appropriate precedence value.
 */
int precedence(const string& token) {
   if (token == "+" || token == "-") return 1;
   if (token == "*" || token == "/") return 2;
   return 0;
}
