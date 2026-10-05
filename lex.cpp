#include "lex.h"
#include <cctype>

using namespace std;

// Maps lowercase keyword strings to their Token values
static map<string, Token> kwMap = {
    {"and", AND},     {"begin", BEGIN},   {"boolean", BOOLEAN},
    {"char", CHAR},   {"const", CONST},   {"div", IDIV},
    {"else", ELSE},   {"end", END},       {"false", FALSE},
    {"if", IF},       {"integer", INTEGER},{"mod", MOD},
    {"not", NOT},     {"or", OR},         {"program", PROGRAM},
    {"readln", READLN},{"real", REAL},    {"string", STRING},
    {"then", THEN},   {"true", TRUE},     {"var", VAR},
    {"write", WRITE}, {"writeln", WRITELN}
};

// Given a word, return the matching keyword token, or IDENT if it's not a keyword
LexItem id_or_kw(const string& lexeme, int linenum) {
    // Convert to lowercase for case-insensitive keyword matching
    string lower = lexeme;
    for (int i = 0; i < (int)lower.size(); i++) {
        lower[i] = tolower(lower[i]);
    }

    // Look it up in the keyword map
    if (kwMap.count(lower) > 0) {
        return LexItem(kwMap[lower], lower, linenum);
    }

    // Not a keyword, so it's a user-defined identifier
    return LexItem(IDENT, lexeme, linenum);
}

// Print a token in the required format
ostream& operator<<(ostream& out, const LexItem& tok) {
    Token t   = tok.GetToken();
    string lex = tok.GetLexeme();

    if      (t == ICONST)  out << "ICONST: (" << lex << ")\n";
    else if (t == RCONST)  out << "RCONST: (" << lex << ")\n";
    else if (t == BCONST)  out << "BCONST: (" << lex << ")\n";
    else if (t == TRUE)    out << "TRUE: (" << lex << ")\n";
    else if (t == FALSE)   out << "FALSE: (" << lex << ")\n";
    else if (t == IDENT)   out << "IDENT: <" << lex << ">\n";
    else if (t == SCONST)  out << "SCONST: '" << lex << "'\n";
    else if (t == CCONST)  out << "CCONST: '" << lex << "'\n";
    else if (t == ERR)     out << "ERR: Error in line (" << tok.GetLinenum() << ") " << lex << "\n";
    else if (t == AND)     out << "AND: \"and\"\n";
    else if (t == BEGIN)   out << "BEGIN: \"begin\"\n";
    else if (t == BOOLEAN) out << "BOOLEAN: \"boolean\"\n";
    else if (t == CHAR)    out << "CHAR: \"char\"\n";
    else if (t == CONST)   out << "CONST: \"const\"\n";
    else if (t == END)     out << "END: \"end\"\n";
    else if (t == ELSE)    out << "ELSE: \"else\"\n";
    else if (t == IF)      out << "IF: \"if\"\n";
    else if (t == INTEGER) out << "INTEGER: \"integer\"\n";
    else if (t == IDIV)    out << "IDIV: \"div\"\n";
    else if (t == MOD)     out << "MOD: \"mod\"\n";
    else if (t == NOT)     out << "NOT: \"not\"\n";
    else if (t == OR)      out << "OR: \"or\"\n";
    else if (t == PROGRAM) out << "PROGRAM: \"program\"\n";
    else if (t == READLN)  out << "READLN: \"readln\"\n";
    else if (t == REAL)    out << "REAL: \"real\"\n";
    else if (t == STRING)  out << "STRING: \"string\"\n";
    else if (t == THEN)    out << "THEN: \"then\"\n";
    else if (t == VAR)     out << "VAR: \"var\"\n";
    else if (t == WRITE)   out << "WRITE: \"write\"\n";
    else if (t == WRITELN) out << "WRITELN: \"writeln\"\n";
    else if (t == PLUS)    out << "PLUS: \"+\"\n";
    else if (t == MINUS)   out << "MINUS: \"-\"\n";
    else if (t == MULT)    out << "MULT: \"*\"\n";
    else if (t == DIV)     out << "DIV: \"/\"\n";
    else if (t == ASSOP)   out << "ASSOP: \":=\"\n";
    else if (t == EQ)      out << "EQ: \"=\"\n";
    else if (t == LTHAN)   out << "LTHAN: \"<\"\n";
    else if (t == GTHAN)   out << "GTHAN: \">\"\n";
    else if (t == COMMA)   out << "COMMA: \",\"\n";
    else if (t == SEMICOL) out << "SEMICOL: \";\"\n";
    else if (t == LPAREN)  out << "LPAREN: \"(\"\n";
    else if (t == RPAREN)  out << "RPAREN: \")\"\n";
    else if (t == COLON)   out << "COLON: \":\"\n";
    else if (t == DOT)     out << "DOT: \".\"\n";
    else if (t == LBRACE)  out << "LBRACE: \"{\"\n";
    else if (t == RBRACE)  out << "RBRACE: \"}\"\n";
    else if (t == DONE)    out << "DONE\n";

    return out;
}

// Read and return the next token from the input stream
LexItem getNextToken(istream& in, int& linenum) {
    char ch;

    // ----- Step 1: Skip whitespace, keep track of line numbers -----
    while (in.get(ch)) {
        if (ch == '\n') {
            linenum++;
        } else if (ch == '\r') {
            // Handle Windows-style \r\n line endings
            if (in.peek() == '\n') {
                char tmp;
                in.get(tmp);
            }
            linenum++;
        } else if (isspace(ch)) {
            // Regular whitespace (space, tab) — just skip
        } else {
            // Found a non-whitespace character, stop skipping
            break;
        }
    }

    // If we hit the end of the file, return DONE
    if (in.eof()) {
        return LexItem(DONE, "", linenum);
    }

    // ----- Step 2: Handle { ... } style comments -----
    if (ch == '{') {
        bool closed = false;
        while (in.get(ch)) {
            if (ch == '\n') linenum++;
            else if (ch == '\r') {
                if (in.peek() == '\n') { char tmp; in.get(tmp); }
                linenum++;
            }
            if (ch == '}') {
                closed = true;
                break;
            }
        }
        if (!closed) {
            return LexItem(ERR, "Missing closing symbol(s) for a comment \"{\"", linenum);
        }
        // Comment consumed — get the next real token
        return getNextToken(in, linenum);
    }

    // ----- Step 3: Handle '(' — either (* comment or left parenthesis -----
    if (ch == '(') {
        if (in.peek() == '*') {
            // It's a (* ... *) comment
            char star;
            in.get(star);  // consume the '*'

            char prev = 0;
            bool closed = false;

            while (in.get(ch)) {
                if (ch == '\n') linenum++;
                else if (ch == '\r') {
                    if (in.peek() == '\n') { char tmp; in.get(tmp); }
                    linenum++;
                }
                // Detect closing *)
                if (prev == '*' && ch == ')') {
                    closed = true;
                    break;
                }
                // Detect nested (* — this is an error
                if (prev == '(' && ch == '*') {
                    return LexItem(ERR, "Invalid nesting of comments \"(*(*\"", linenum);
                }
                prev = ch;
            }
            if (!closed) {
                return LexItem(ERR, "Missing closing symbol(s) for a comment \"(*\"", linenum);
            }
            return getNextToken(in, linenum);
        }
        // Not a comment — just a left parenthesis
        return LexItem(LPAREN, "(", linenum);
    }

    // ----- Step 4: Right brace (standalone, not part of a comment) -----
    if (ch == '}') {
        return LexItem(RBRACE, "}", linenum);
    }

    // ----- Step 5: String literal  'like this' -----
    if (ch == '\'') {
        string content = "";   // the characters inside the quotes
        string full = "'";     // used for error messages (includes opening quote)
        bool closed = false;

        while (in.get(ch)) {
            // Newline inside a string is an error
            if (ch == '\n' || ch == '\r') {
                return LexItem(ERR, "New line is not allowed within string literal \"" + full + "\"", linenum);
            }
            full += ch;
            if (ch == '\'') {
                closed = true;
                break;
            }
            content += ch;
        }

        if (!closed) {
            return LexItem(ERR, "New line is not allowed within string literal \"" + full + "\"", linenum);
        }
        return LexItem(SCONST, content, linenum);
    }

    // ----- Step 6: Identifier or keyword (starts with a letter or underscore) -----
    if (isalpha(ch) || ch == '_') {
        string lexeme = "";
        lexeme += ch;

        // Keep reading letters, digits, underscores, or '$'
        while (!in.eof() && (isalnum(in.peek()) || in.peek() == '_' || in.peek() == '$')) {
            in.get(ch);
            lexeme += ch;
        }

        // Check if it's a reserved keyword or just an identifier
        return id_or_kw(lexeme, linenum);
    }

    // ----- Step 7: Number (starts with a digit) -----
    if (isdigit(ch)) {
        string lexeme = "";
        lexeme += ch;

        // Read all the integer digits
        while (!in.eof() && isdigit(in.peek())) {
            in.get(ch);
            lexeme += ch;
        }

        // Check if there's a decimal point next
        if (in.peek() == '.') {
            char dot;
            in.get(dot);

            // A real number requires at least one digit after the dot
            if (isdigit(in.peek())) {
                lexeme += dot;

                // Read the fractional digits
                while (!in.eof() && isdigit(in.peek())) {
                    in.get(ch);
                    lexeme += ch;
                }

                // A second dot means an invalid float (e.g., 27.57.5)
                if (in.peek() == '.') {
                    char dot2;
                    in.get(dot2);
                    lexeme += dot2;
                    // Read one more digit to include in the error message
                    if (!in.eof() && isdigit(in.peek())) {
                        in.get(ch);
                        lexeme += ch;
                    }
                    return LexItem(ERR, "Invalid floating-point constant \"" + lexeme + "\"", linenum);
                }

                // Check for optional exponent (E or e)
                if (in.peek() == 'E' || in.peek() == 'e') {
                    char echar;
                    in.get(echar);
                    string expLex = lexeme + echar;
                    char next = in.peek();

                    if (next == '+' || next == '-') {
                        // Exponent has a sign
                        char sign;
                        in.get(sign);
                        expLex += sign;

                        if (isdigit(in.peek())) {
                            // Read exponent digits
                            while (!in.eof() && isdigit(in.peek())) {
                                in.get(ch);
                                expLex += ch;
                            }
                            // A trailing e/E after the exponent is invalid
                            if (in.peek() == 'e' || in.peek() == 'E') {
                                char bad;
                                in.get(bad);
                                expLex += bad;
                                return LexItem(ERR, "Invalid exponent for floating-point constant \"" + expLex + "\"", linenum);
                            }
                            return LexItem(RCONST, expLex, linenum);
                        } else {
                            // Sign not followed by a digit (e.g., 0.75E-+)
                            char bad = in.peek();
                            if (!in.eof() && !isspace((unsigned char)bad)) {
                                in.get(bad);
                                expLex += bad;
                            }
                            return LexItem(ERR, "Invalid exponent for floating-point constant \"" + expLex + "\"", linenum);
                        }

                    } else if (isdigit(next)) {
                        // Exponent with no sign (e.g., 3.0e2)
                        while (!in.eof() && isdigit(in.peek())) {
                            in.get(ch);
                            expLex += ch;
                        }
                        // Trailing e/E is invalid
                        if (in.peek() == 'e' || in.peek() == 'E') {
                            char bad;
                            in.get(bad);
                            expLex += bad;
                            return LexItem(ERR, "Invalid exponent for floating-point constant \"" + expLex + "\"", linenum);
                        }
                        return LexItem(RCONST, expLex, linenum);

                    } else {
                        // E not followed by digit or sign (e.g., 0.75Ee)
                        char bad = in.peek();
                        if (!in.eof() && !isspace((unsigned char)bad)) {
                            in.get(bad);
                            expLex += bad;
                        }
                        return LexItem(ERR, "Invalid exponent for a floating-point constant \"" + expLex + "\"", linenum);
                    }
                }

                // Valid real number with no exponent (e.g., 12.3)
                return LexItem(RCONST, lexeme, linenum);

            } else {
                // Dot not followed by a digit (e.g., "375.") — put the dot back, return integer
                in.putback(dot);
                return LexItem(ICONST, lexeme, linenum);
            }
        }

        // No decimal point — plain integer constant
        return LexItem(ICONST, lexeme, linenum);
    }

    // ----- Step 8: Standalone dot -----
    if (ch == '.') {
        return LexItem(DOT, ".", linenum);
    }

    // ----- Step 9: Operators and single-character delimiters -----
    if (ch == '+') return LexItem(PLUS,    "+",  linenum);
    if (ch == '-') return LexItem(MINUS,   "-",  linenum);
    if (ch == '*') return LexItem(MULT,    "*",  linenum);
    if (ch == '/') return LexItem(DIV,     "/",  linenum);
    if (ch == '=') return LexItem(EQ,      "=",  linenum);
    if (ch == '<') return LexItem(LTHAN,   "<",  linenum);
    if (ch == '>') return LexItem(GTHAN,   ">",  linenum);
    if (ch == ',') return LexItem(COMMA,   ",",  linenum);
    if (ch == ';') return LexItem(SEMICOL, ";",  linenum);
    if (ch == ')') return LexItem(RPAREN,  ")",  linenum);

    // Colon — could be ':' alone or ':=' assignment operator
    if (ch == ':') {
        if (in.peek() == '=') {
            in.get(ch);  // consume the '='
            return LexItem(ASSOP, ":=", linenum);
        }
        return LexItem(COLON, ":", linenum);
    }

    // ----- Step 10: Anything else is an invalid character -----
    string errMsg = "Invalid character for starting a token \"";
    errMsg += ch;
    errMsg += "\"";
    return LexItem(ERR, errMsg, linenum);
}
