#include "lex.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cctype>

using namespace std;

// Returns true if the token is a reserved keyword (not including true/false — those are booleans)
bool isKeyword(Token t) {
    if (t == IF)      return true;
    if (t == ELSE)    return true;
    if (t == WRITELN) return true;
    if (t == WRITE)   return true;
    if (t == READLN)  return true;
    if (t == INTEGER) return true;
    if (t == REAL)    return true;
    if (t == BOOLEAN) return true;
    if (t == CHAR)    return true;
    if (t == STRING)  return true;
    if (t == BEGIN)   return true;
    if (t == END)     return true;
    if (t == VAR)     return true;
    if (t == CONST)   return true;
    if (t == THEN)    return true;
    if (t == PROGRAM) return true;
    if (t == AND)     return true;
    if (t == OR)      return true;
    if (t == NOT)     return true;
    if (t == IDIV)    return true;
    if (t == MOD)     return true;
    return false;
}

int main(int argc, char* argv[]) {

    // ----- Step 1: Parse command-line arguments -----
    bool flagAll = false;
    bool flagNum = false;
    bool flagStr = false;
    bool flagIds = false;
    string filename = "";

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg[0] == '-') {
            // It's a flag
            if (arg == "-all") {
                flagAll = true;
            } else if (arg == "-num") {
                flagNum = true;
            } else if (arg == "-str") {
                flagStr = true;
            } else if (arg == "-ids") {
                flagIds = true;
            } else {
                cout << "UNRECOGNIZED FLAG {" << arg << "}" << endl;
                return 1;
            }
        } else {
            // It should be the filename
            if (!filename.empty()) {
                cout << "ONLY ONE FILE NAME IS ALLOWED." << endl;
                return 1;
            }
            filename = arg;
        }
    }

    // Make sure a filename was given
    if (filename.empty()) {
        cout << "NO SPECIFIED INPUT FILE." << endl;
        return 1;
    }

    // Try to open the file
    ifstream infile(filename);
    if (!infile.is_open()) {
        cout << "CANNOT OPEN THE FILE " << filename << endl;
        return 1;
    }

    // ----- Step 2: Tokenize the file -----
    int linenum = 1;

    // Counters for the summary
    int totalTokens = 0;
    int numIdKw     = 0;
    int numNumbers  = 0;
    int numBooleans = 0;
    int numStrings  = 0;

    // Maps to track unique values and how many times each appears
    map<string, int> identifiers;  // user-defined names
    map<string, int> keywords;     // reserved words
    map<string, int> intConsts;    // integer constants
    map<string, int> realConsts;   // real (float) constants
    map<string, int> strings;      // string literals

    bool hasTokens   = false;
    bool errOccurred = false;

    while (true) {
        LexItem tok = getNextToken(infile, linenum);
        Token t = tok.GetToken();

        // End of file — stop the loop
        if (t == DONE) {
            break;
        }

        // Lexer found an error — print it and stop
        if (t == ERR) {
            cout << tok;
            errOccurred = true;
            break;
        }

        // Count and categorize this token
        hasTokens = true;
        totalTokens++;

        // If -all flag is set, print every token as we see it
        if (flagAll) {
            cout << tok;
        }

        if (t == IDENT) {
            numIdKw++;
            identifiers[tok.GetLexeme()]++;

        } else if (isKeyword(t)) {
            numIdKw++;
            // Store keyword in lowercase
            string kw = tok.GetLexeme();
            for (int i = 0; i < (int)kw.size(); i++) kw[i] = tolower(kw[i]);
            keywords[kw]++;

        } else if (t == ICONST) {
            numNumbers++;
            intConsts[tok.GetLexeme()]++;

        } else if (t == RCONST) {
            numNumbers++;
            realConsts[tok.GetLexeme()]++;

        } else if (t == TRUE || t == FALSE || t == BCONST) {
            numBooleans++;

        } else if (t == SCONST) {
            numStrings++;
            strings[tok.GetLexeme()]++;
        }
    }

    // ----- Step 3: Handle special cases before printing summary -----

    // Empty file
    if (!hasTokens && !errOccurred) {
        cout << "Empty File." << endl;
        return 0;
    }

    // Error already printed above
    if (errOccurred) {
        return 1;
    }

    // ----- Step 4: Print the summary -----
    // linenum - 1 gives the actual number of lines in the file
    cout << endl;
    cout << "Lines: "                 << linenum - 1  << endl;
    cout << "Total Tokens: "          << totalTokens  << endl;
    cout << "Identifiers & Keywords: "<< numIdKw      << endl;
    cout << "Numbers: "               << numNumbers   << endl;
    cout << "Booleans: "              << numBooleans  << endl;
    cout << "Strings: "               << numStrings   << endl;

    // ----- Step 5: Handle flags (always in this order: -ids, -num, -str) -----

    if (flagIds) {
        // Print unique identifiers in alphabetical order
        if (!identifiers.empty()) {
            cout << "IDENTIFIERS:" << endl;
            bool first = true;
            for (auto& entry : identifiers) {
                if (!first) cout << ", ";
                cout << entry.first << " (" << entry.second << ")";
                first = false;
            }
            cout << endl;
        }

        // Print unique keywords in alphabetical order
        if (!keywords.empty()) {
            // If there were no identifiers, print a blank line as separator
            if (identifiers.empty()) {
                cout << endl;
            }
            cout << "KEYWORDS:" << endl;
            bool first = true;
            for (auto& entry : keywords) {
                if (!first) cout << ", ";
                cout << entry.first << " (" << entry.second << ")";
                first = false;
            }
            cout << endl;
        }
    }

    if (flagNum) {
        // Print unique integer constants in alphabetical order
        if (!intConsts.empty()) {
            cout << "INTEGERS:" << endl;
            bool first = true;
            for (auto& entry : intConsts) {
                if (!first) cout << ", ";
                cout << entry.first << " (" << entry.second << ")";
                first = false;
            }
            cout << endl;
        }

        // Print unique real constants in alphabetical order
        if (!realConsts.empty()) {
            cout << "REALS:" << endl;
            bool first = true;
            for (auto& entry : realConsts) {
                if (!first) cout << ", ";
                cout << entry.first << " (" << entry.second << ")";
                first = false;
            }
            cout << endl;
        }
    }

    if (flagStr) {
        // Print unique string literals in alphabetical order
        if (!strings.empty()) {
            cout << "STRINGS:" << endl;
            bool first = true;
            for (auto& entry : strings) {
                if (!first) cout << ", ";
                cout << "'" << entry.first << "' (" << entry.second << ")";
                first = false;
            }
            cout << endl;
        }
    }

    return 0;
}
