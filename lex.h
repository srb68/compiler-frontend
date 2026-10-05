#ifndef LEX_H_
#define LEX_H_

#include <iostream>
#include <string>
using namespace std;

enum Token {
    // keywords
    AND, BEGIN, BOOLEAN, CHAR, CONST, END, ELSE, FALSE, IF,
    INTEGER, IDIV, MOD, NOT, OR, PROGRAM, READLN, REAL, STRING,
    THEN, TRUE, VAR, WRITE, WRITELN,
    // operators
    PLUS, MINUS, MULT, DIV, ASSOP, EQ, LTHAN, GTHAN,
    // delimiters
    COMMA, SEMICOL, LPAREN, RPAREN, COLON, DOT, LBRACE, RBRACE,
    // constants
    ICONST, RCONST, SCONST, BCONST, CCONST,
    // identifier
    IDENT,
    // special
    ERR, DONE
};

class LexItem {
    Token token;
    string lexeme;
    int lnum;
public:
    LexItem() : token(ERR), lexeme(""), lnum(-1) {}
    LexItem(Token t, const string& lex, int line) : token(t), lexeme(lex), lnum(line) {}
    Token GetToken() const { return token; }
    string GetLexeme() const { return lexeme; }
    int GetLinenum() const { return lnum; }
    bool operator==(const Token t) const { return token == t; }
    bool operator!=(const Token t) const { return token != t; }
};

extern ostream& operator<<(ostream& out, const LexItem& tok);
extern LexItem id_or_kw(const string& lexeme, int linenum);
extern LexItem getNextToken(istream& in, int& linenum);

#endif
