#include <iostream>
#include <string>
#include <cctype>
#include <vector>
using namespace std;

// Define token types
enum TokenType {
    KEYWORD, IDENTIFIER, NUMBER, COMMENT, INVALID,
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, SEMICOLON, COMMA,
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULUS, ASSIGNMENT, INCREMENT, DECREMENT,
    LESS_THAN, GREATER_THAN, LOGIC_EQUAL, LOGIC_AND, LOGIC_OR, LOGIC_NOT, 
    BIT_AND, BIT_OR
};

// Define keywords
vector<string> keywords = {"int", "float", "main", "char", "if", "else", "return", 
                           "unsigned", "void", "goto", "for", "while", "break", 
                           "continue", "switch", "case"};

// Function to check if a lexeme is a keyword
bool isKeyword(const string& lexeme) {
    for (const string& keyword : keywords) {
        if (lexeme == keyword) {
            return true;
        }
    }
    return false;
}

//Lexical analyzer function
vector<pair<TokenType, string>> lexer(const string& code) {
    vector<pair<TokenType, string>> tokens;
    int i = 0;

    while (i < code.length()) {
        char ch = code[i];

        // Skip whitespace
        if (isspace(ch)) {
            i++;
            continue;
        }

        // Check for comments
        if (i + 1 < code.length() && code[i] == '/' && code[i + 1] == '/') {
            int commentStart = i;
            while(i < code.length() && code[i] != '\n') {
                i++;
            }
            string comment = code.substr(commentStart, i - commentStart);
            tokens.push_back({COMMENT, comment});
            continue;
        }

        // Handle delimiters and symbols
        if (ch == '(') {
            tokens.push_back({LEFT_PAREN, "("});
            i++;
            continue;
        } else if (ch == ')') {
            tokens.push_back({RIGHT_PAREN, ")"});
            i++;
            continue;
        } else if (ch == '{') {
            tokens.push_back({LEFT_BRACE, "{"});
            i++;
            continue;
        } else if (ch == '}') {
            tokens.push_back({RIGHT_BRACE, "}"});
            i++;
            continue;
        } else if (ch == ';') {
            tokens.push_back({SEMICOLON, ";"});
            i++;
            continue;
        } else if (ch == ',') {
            tokens.push_back({COMMA, ","});
            i++;
            continue;
        }

        // Handle operators
        if (ch == '+') {
            if (i + 1 < code.length() && code[i + 1] == '+') {
                tokens.push_back({INCREMENT, "++"});
                i += 2;
            } else {
                tokens.push_back({PLUS, "+"});
                i++;
            }
            continue;
        } else if (ch == '-') {
            if (i + 1 < code.length() && code[i + 1] == '-') {
                tokens.push_back({DECREMENT, "--"});
                i += 2;
            } else {
                tokens.push_back({MINUS, "-"});
                i++;
            }
            continue;
        } else if (ch == '*') {
            tokens.push_back({MULTIPLY, "*"});
            i++;
            continue;
        } else if (ch == '/') {
            tokens.push_back({DIVIDE, "/"});
            i++;
            continue;
        } else if (ch == '%') {
            tokens.push_back({MODULUS, "%"});
            i++;
            continue;
        }

        // Handle comparisons and logical operators
        if (ch == '<') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back({LESS_THAN_EQ, "<="});
                i += 2;
            } else {
                tokens.push_back({LESS_THAN, "<"});
                i++;
            }
            continue;
        } else if (ch == '>') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back({GREATER_THAN_EQ, ">="});
                i += 2;
            } else {
                tokens.push_back({GREATER_THAN, ">"});
                i++;
            }
            continue;
        } else if (ch == '=') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back({LOGIC_EQUAL, "=="});
                i += 2;
            } else {
                tokens.push_back({ASSIGNMENT, "="});
                i++;
            }
            continue;
        } else if (ch == '&') {
            if (i + 1 < code.length() && code[i + 1] == '&') {
                tokens.push_back({LOGIC_AND, "&&"});
                i += 2;
            } else {
                tokens.push_back({BIT_AND, "&"});
                i++;
            }
            continue;
        } else if (ch == '|') {
            if (i + 1 < code.length() && code[i + 1] == '|') {
                tokens.push_back({LOGIC_OR, "||"});
                i += 2;
            } else {
                tokens.push_back({BIT_OR, "|"});
                i++;
            }
            continue;
        } else if (ch == '!') {
            tokens.push_back({LOGIC_NOT, "!"});
            i++;
            continue;
        }

        // Handle numbers
        if (isdigit(ch)) {
            string lexeme;
            bool decimalPoint = false;
            lexeme += ch;
            i++;
            while (i < code.length() && (isdigit(code[i]) || (!decimalPoint && code[i] == '.'))) {
                if (code[i] == '.') {
                    decimalPoint = true;
                }
                lexeme += code[i];
                i++;
            }
            tokens.push_back({NUMBER, lexeme});
            continue;
        }

        // Handle identifiers and keywords
        if (isalpha(ch) || ch == '_') {
            string lexeme;
            lexeme += ch;
            i++;
            while (i < code.length() && (isalnum(code[i]) || code[i] == '_')) {
                lexeme += code[i];
                i++;
            }
            if (isKeyword(lexeme)) {
                tokens.push_back({KEYWORD, lexeme});
            } else {
                tokens.push_back({IDENTIFIER, lexeme});
            }
            continue;
        }

        // If we reach this point, it's an invalid token
        tokens.push_back({INVALID, string(1, ch)});
        i++;
    }

    return tokens;
}

// Function to print tokens
void printTokens(const vector<pair<TokenType, string>>& tokens) {
    for (const auto& token : tokens) {
        string tokenType;
        switch (token.first) {
            case KEYWORD: tokenType = "KEYWORD"; break;
            case IDENTIFIER: tokenType = "IDENTIFIER"; break;
            case NUMBER: tokenType = "NUMBER"; break;
            case COMMENT: tokenType = "COMMENT"; break;
            case LEFT_PAREN: tokenType = "LEFT_PAREN"; break;
            case RIGHT_PAREN: tokenType = "RIGHT_PAREN"; break;
            case LEFT_BRACE: tokenType = "LEFT_BRACE"; break;
            case RIGHT_BRACE: tokenType = "RIGHT_BRACE"; break;
            case SEMICOLON: tokenType = "SEMICOLON"; break;
            case COMMA: tokenType = "COMMA"; break;
            case PLUS: tokenType = "PLUS"; break;
            case MINUS: tokenType = "MINUS"; break;
            case MULTIPLY: tokenType = "MULTIPLY"; break;
            case DIVIDE: tokenType = "DIVIDE"; break;
            case MODULUS: tokenType = "MODULUS"; break;
            case ASSIGNMENT: tokenType = "ASSIGNMENT"; break;
            case INCREMENT: tokenType = "INCREMENT"; break;
            case DECREMENT: tokenType = "DECREMENT"; break;
            case LESS_THAN: tokenType = "LESS_THAN"; break;
            case GREATER_THAN: tokenType = "GREATER_THAN"; break;
            case LOGIC_EQUAL: tokenType = "LOGIC_EQUAL"; break;
            case LOGIC_AND: tokenType = "LOGIC_AND"; break;
            case LOGIC_OR: tokenType = "LOGIC_OR"; break;
            case LOGIC_NOT: tokenType = "LOGIC_NOT"; break;
            case BIT_AND: tokenType = "BIT_AND"; break;
            case BIT_OR: tokenType = "BIT_OR"; break;
            case INVALID: tokenType = "INVALID"; break;
        }
        cout << tokenType << ": " << token.second << endl;
    }
}

// Main function (your test case)
int main() {
    string code = R"( 
        int main() 
        {
            int 1stint= 0;; 
            charmyChar2; 
            return0; 
        }
    )";

    vector<pair<TokenType, string>> tokens = lexer(code);
    printTokens(tokens);

    return 0;
}