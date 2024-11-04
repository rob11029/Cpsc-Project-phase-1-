#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include "symbol_table.cpp"
using namespace std;

// Define token types
enum TokenType {
    KEYWORD, IDENTIFIER, COMMENT, INVALID,
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACKET, RIGHT_BRACKET,
    LEFT_BRACE, RIGHT_BRACE, DOT, SEMICOLON, COMMA,
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULUS, ASSIGNMENT,
    INCREMENT, DECREMENT, LESS_THAN, LESS_THAN_EQ,
    GREATER_THAN, GREATER_THAN_EQ, LOGIC_EQUAL,
    LOGIC_AND, LOGIC_OR, LOGIC_NOT, BIT_AND, BIT_OR, LOGIC_NOT_EQUAL,
    BASIC, INTEGER, REAL, // update basic  phase 2
    IF, ELSE, WHILE, BREAK, MAIN, DO, // update token phase 2
    RETURN, NUMBER, END_OF_FILE,
};

struct Token {
    TokenType type;
    std::string lexeme;

    Token(TokenType type, const std::string &lexeme) : type(type), lexeme(lexeme) {}
};

// Define keywords according to the specification (update basic phase 2)
vector<string> basic = {
    "float", "int", "char", "void"
};

vector<string> keywords = {
    "return", "switch", "case", "for", "goto", "unsigned", "continue",
    "do" // add keywords for phase 2
};

// Function to check if a lexeme is a basic type
bool isBasicType(const string& lexeme) {
    for (const string& type : basic) {
        if (lexeme == type) return true;
    }
    return false;
}
// Function to check if a lexeme is a keyword
bool isKeyword(const string& lexeme) {
    for (const string& keyword : keywords) {
        if (lexeme == keyword) return true;
    }
    return false;
}

// Lexical analyzer function
vector<pair<TokenType, string>> lexer(const string& code, SymbolTable& symbol_table) {
    vector<pair<TokenType, string>> tokens;
    string currentToken;
    int i = 0;
    int line = 1;
    int column = 1;

    while (i < code.length()) {
        char ch = code[i];
        int start_column = column;

        if (ch == '\n') {
          line++;
          column = 1;
          i++;
        }

        // Skip whitespace
        if (isspace(ch)) {
            column++;
            i++;
            continue;
        }

        // Check for comments (//)
        if (i + 1 < code.length() && code[i] == '/' && code[i + 1] == '/') {
            int token_start = column;
            string comment;
            comment += "//";
            i += 2;
            while (i < code.length() && code[i] != '\n') {
                comment += code[i];
                i++;
            }
            if (i < code.length() && code[i] == '\n') {
                comment += '\n';
                i++;
            }
            tokens.push_back({COMMENT, comment});
            symbol_table.insert(comment, "COMMENT", comment, line, token_start, comment.length());
            continue;
        }

        // Handle numbers (integers and reals)
        if (isdigit(ch) || (ch == '.' && i + 1 < code.length() && isdigit(code[i + 1]))) {
            string number;
            int token_start = column;
            bool isReal = false;

            // Handle leading digits
            while (i < code.length() && isdigit(code[i])) {
                number += code[i];
                i++;
                column++;
            }

            // Check for decimal point
            if (i < code.length() && code[i] == '.') {
                isReal = true;
                number += code[i];
                i++;
                column++;

                while(i < code.length() && isdigit(code[i])) {
                    number += code[i];
                    i++;
                }
            }
            // Handle numbers begin with dot
            if (number.empty() && i < code.length() && code[i] == '.') {
                isReal = true;
                number = "0.";
                i++;

                while (i < code.length() && isdigit(code[i])) {
                    number += code[i];
                    i++;
                }
            }
            tokens.push_back({isReal ? REAL : INTEGER, number});
            symbol_table.insert(number, "INTEGER", number, line, token_start, number.length());
            continue;
        }
        // Handle identifiers
        if (isalpha(ch)) {
            string identifier;
            int token_start = column;
            identifier += ch;
            i++;
            while (i < code.length() && (isalnum(code[i]))) {
                identifier += code[i];
                i++;
                column++;
            }
            if (identifier == "if") {
                tokens.push_back({IF, identifier});
                symbol_table.insert(identifier, "IF", identifier, line, token_start, identifier.length());
            } else if (identifier == "else") {
                tokens.push_back({ELSE, identifier});
                symbol_table.insert(identifier, "ELSE", identifier, line, token_start, identifier.length());
            } else if (identifier == "while") {
                tokens.push_back({WHILE, identifier});
                symbol_table.insert(identifier, "WHILE", identifier, line, token_start, identifier.length());
            } else if (identifier == "break") {
                tokens.push_back({BREAK, identifier});
                symbol_table.insert(identifier, "BREAK", identifier, line, token_start, identifier.length());
            } else if (identifier == "main") {
                tokens.push_back({MAIN, identifier});
                symbol_table.insert(identifier, "MAIN", identifier, line, token_start, identifier.length());
            } else if (identifier == "do") {
                tokens.push_back({DO, identifier});
                symbol_table.insert(identifier, "DO", identifier, line, token_start, identifier.length());
            } else if (isBasicType(identifier)) {
                tokens.push_back({BASIC, identifier});
                symbol_table.insert(identifier, "BASIC", identifier, line, token_start, identifier.length());
            } else if (isKeyword(identifier)) {
                tokens.push_back({KEYWORD, identifier});
                symbol_table.insert(identifier, "KEYWORD", identifier, line, token_start, identifier.length());
            } else {
                tokens.push_back({IDENTIFIER, identifier});
                symbol_table.insert(identifier, "IDENTIFIER", identifier, line, token_start, identifier.length());
            }
            continue;
        }

        // Handle delimiters and operators
        int token_start = column;
        if (ch == '(') {
          tokens.push_back({LEFT_PAREN, "("});
          symbol_table.insert("(", "LEFT_PAREN", "(", line, token_start, 1);
          } else if (ch == ')') {
          tokens.push_back({RIGHT_PAREN, ")"});
          symbol_table.insert(")", "RIGHT_PAREN", ")", line, token_start, 1);
        } else if (ch == '[') {
          tokens.push_back({LEFT_BRACKET, "["});
          symbol_table.insert("[", "LEFT_BRACKET", "[", line, token_start, 1);
        } else if (ch == ']') {
          tokens.push_back({RIGHT_BRACKET, "]"});
          symbol_table.insert("]", "RIGHT_BRACKET", "]", line, token_start, 1);
        } else if (ch == '{') {
          tokens.push_back({LEFT_BRACE, "{"});
          symbol_table.insert("{", "LEFT_BRACE", "}", line, token_start, 1);
        }
        else if (ch == '}') {
          tokens.push_back({RIGHT_BRACE, "}"});
          symbol_table.insert("}", "RIGHT_BRACE", "}", line, token_start, 1);
        }
        else if (ch == ';') {
          tokens.push_back({SEMICOLON, ";"});
          symbol_table.insert(";", "SEMICOLON", ";", line, token_start, 1);
        }
        else if (ch == ',') {
          tokens.push_back({COMMA, ","});
          symbol_table.insert(",", "COMMA", ",", line, token_start, 1);
        }
        else if (ch == '+') {
            if (i + 1 < code.length() && code[i + 1] == '+') {
                tokens.push_back({INCREMENT, "++"});
                symbol_table.insert("++", "INCREMENT", "++", line, token_start, 2);
                i++;
            } else {
                tokens.push_back({PLUS, "+"});
                symbol_table.insert("+", "PLUS", "+", line, token_start, 1);
            }
        }
        else if (ch == '-') {
            if (i + 1 < code.length() && code[i + 1] == '-') {
                tokens.push_back({DECREMENT, "--"});
                symbol_table.insert("--", "DECREMENT", "--", line, token_start, 2);
                i++;
            } else {
                tokens.push_back({MINUS, "-"});
                symbol_table.insert("-", "MINUS", "-", line, token_start, 1);
            }
        }
        else if (ch == '*') {
          tokens.push_back({MULTIPLY, "*"});
          symbol_table.insert("*", "MULTIPLY", "*", line, token_start, 1);
        }
        else if (ch == '/') {
          tokens.push_back({DIVIDE, "/"});
          symbol_table.insert("/", "DIVIDE", "/", line, token_start, 1);
        }
        else if (ch == '%') {
          tokens.push_back({MODULUS, "%"});
          symbol_table.insert("%", "MODULUS", "%", line, token_start, 1);
        }
        else if (ch == '<') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back({LESS_THAN_EQ, "<="});
                symbol_table.insert("<=", "LESS_THAN_EQ", "<=", line, token_start, 2);
                i++;
            } else {
                tokens.push_back({LESS_THAN, "<"});
                symbol_table.insert("<", "LESS_THAN", "<", line, token_start, 1);
            }
        }
        else if (ch == '>') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back({GREATER_THAN_EQ, ">="});
                symbol_table.insert(">=", "GREATER_THAN_EQ", ">=", line, token_start, 2);
                i++;
            } else {
                tokens.push_back({GREATER_THAN, ">"});
                symbol_table.insert(">", "GREATER_THAN", ">", line, token_start, 1);
            }
        }
        else if (ch == '=') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back({LOGIC_EQUAL, "=="});
                symbol_table.insert("=", "LOGIC_EQUAL", "=", line, token_start, 2);
                i++;
            } else {
                tokens.push_back({ASSIGNMENT, "="});
                symbol_table.insert("=", "ASSIGNMENT", "=", line, token_start, 1);
            }
        }
        else if (ch == '&') {
            if (i + 1 < code.length() && code[i + 1] == '&') {
                tokens.push_back({LOGIC_AND, "&&"});
                symbol_table.insert("&&", "LOGIC_AND", "&&", line, token_start, 2);
                i++;
            } else {
                tokens.push_back({BIT_AND, "&"});
                symbol_table.insert("&", "BIT_AND", "&", line, token_start, 1);
            }
        }
        else if (ch == '|') {
            if (i + 1 < code.length() && code[i + 1] == '|') {
                tokens.push_back({LOGIC_OR, "||"});
                symbol_table.insert("||", "LOGIC_OR", "||", line, token_start, 2);
                i++;
            } else {
                tokens.push_back({BIT_OR, "|"});
                symbol_table.insert("|", "BIT_OR", "|", line, token_start, 1);
            }
        }
        else if (ch == '!'){
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back({LOGIC_NOT_EQUAL, "!="});
                symbol_table.insert("!=", "LOGIC_NOT_EQUAL", "!=", line, token_start, 2);
                i++;
            } else {
                tokens.push_back({LOGIC_NOT, "!"});
                symbol_table.insert("!", "LOGIC_NOT", "!", line, token_start, 1);
            }
        }
        else if (ch != '.') { // Skip standalone dots as they're handled in number parsing
            tokens.push_back({INVALID, string(1, ch)});
            symbol_table.insert(string(1, ch), "INVALID", string(1, ch), line, token_start, string(1, ch).length());
        }
        i++;
    }
    return tokens;
}

// Function to print tokens
void printTokens(const vector<pair<TokenType, string>>& tokens) {
    for (const auto& token : tokens) {
        string tokenType;
        switch (token.first) {
            case BASIC: tokenType = "Basic"; break;
            case IF: tokenType = "If"; break;
            case ELSE: tokenType = "Else"; break;
            case WHILE: tokenType = "While"; break;
            case BREAK: tokenType = "Break"; break;
            case MAIN: tokenType = "Main"; break;
            case KEYWORD: tokenType = "Keyword"; break;
            case IDENTIFIER: tokenType = "Identifier"; break;
            case REAL: tokenType = "Real"; break;
            case INTEGER: tokenType = "Integer"; break;
            case COMMENT: tokenType = "COMMENT"; break;
            case LEFT_PAREN: tokenType = "leftParen"; break;
            case RIGHT_PAREN: tokenType = "rightParen"; break;
            case LEFT_BRACKET: tokenType = "leftBracket"; break;
            case RIGHT_BRACKET: tokenType = "rightBracket"; break;
            case LEFT_BRACE: tokenType = "leftBrace"; break;
            case RIGHT_BRACE: tokenType = "rightBrace"; break;
            case DOT: tokenType = "dot"; break;
            case SEMICOLON: tokenType = "semicolon"; break;
            case COMMA: tokenType = "comma"; break;
            case PLUS: tokenType = "plus"; break;
            case MINUS: tokenType = "minus"; break;
            case MULTIPLY: tokenType = "multiply"; break;
            case DIVIDE: tokenType = "divide"; break;
            case MODULUS: tokenType = "modulus"; break;
            case ASSIGNMENT: tokenType = "assignment"; break;
            case INCREMENT: tokenType = "increment"; break;
            case DECREMENT: tokenType = "decrement"; break;
            case LESS_THAN: tokenType = "lessThan"; break;
            case LESS_THAN_EQ: tokenType = "lessThanEq"; break;
            case GREATER_THAN: tokenType = "greaterThan"; break;
            case GREATER_THAN_EQ: tokenType = "greaterThanEq"; break;
            case LOGIC_EQUAL: tokenType = "logicEqual"; break;
            case LOGIC_AND: tokenType = "logicAnd"; break;
            case LOGIC_OR: tokenType = "logicOr"; break;
            case LOGIC_NOT: tokenType = "logicNot"; break;
            case BIT_AND: tokenType = "bitAnd"; break;
            case BIT_OR: tokenType = "bitOr"; break;
            case LOGIC_NOT_EQUAL: tokenType = "logicNotEqual"; break;
            case DO: tokenType = "Do"; break;
            case INVALID: tokenType = "INVALID"; break;
        }
        cout << tokenType << ": " << token.second << endl;

        // Insert into symbol table
    }
}

// int main() {
//   SymbolTable symbol_table;

//   // Test case
//   string code = R"(
//     int main() {
//     int zero;
//     zero = 1;
//     return 0;
//     }
//   )";

//      vector<pair<TokenType, string>> tokens = lexer(code, symbol_table);
//      printTokens(tokens);
//      return 0;
//  }
