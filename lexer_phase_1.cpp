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
    IF, ELSE, WHILE, BREAK, MAIN // update token phase 2
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

        // Check for comments (//)
        if (i + 1 < code.length() && code[i] == '/' && code[i + 1] == '/') {
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
            continue;
        }

        // Handle numbers (integers and reals)
        if (isdigit(ch) || (ch == '.' && i + 1 < code.length() && isdigit(code[i + 1]))) {
            string number;
            bool isReal = false;

            // Handle leading digits
            while (i < code.length() && isdigit(code[i])) {
                number += code[i];
                i++;
            }

            // Check for decimal point
            if (i < code.length() && code[i] == '.') {
                isReal = true;
                number += code[i];
                i++;

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
            continue;
        }
        // Handle identifiers
        if (isalpha(ch)) {
            string identifier;
            identifier += ch;
            i++;
            while (i < code.length() && (isalnum(code[i]))) {
                identifier += code[i];
                i++;
            }
            if (identifier == "if") {
                tokens.push_back({IF, identifier});
            } else if (identifier == "else") {
                tokens.push_back({ELSE, identifier});
            } else if (identifier == "while") {
                tokens.push_back({WHILE, identifier});
            } else if (identifier == "break") {
                tokens.push_back({BREAK, identifier});
            } else if (identifier == "main") {
                tokens.push_back({MAIN, identifier});
            } else if (isBasicType(identifier)) {
                tokens.push_back({BASIC, identifier});
            } else if (isKeyword(identifier)) {
                tokens.push_back({KEYWORD, identifier});
            } else {
                tokens.push_back({IDENTIFIER, identifier});
            }
            continue;
        }

        // Handle delimiters and operators
        if (ch == '(') tokens.push_back({LEFT_PAREN, "("});
        else if (ch == ')') tokens.push_back({RIGHT_PAREN, ")"});
        else if (ch == '[') tokens.push_back({LEFT_BRACKET, "["});
        else if (ch == ']') tokens.push_back({RIGHT_BRACKET, "]"});
        else if (ch == '{') tokens.push_back({LEFT_BRACE, "{"});
        else if (ch == '}') tokens.push_back({RIGHT_BRACE, "}"});
        else if (ch == ';') tokens.push_back({SEMICOLON, ";"});
        else if (ch == ',') tokens.push_back({COMMA, ","});
        else if (ch == '+') {
            if (i + 1 < code.length() && code[i + 1] == '+') {
                tokens.push_back({INCREMENT, "++"});
                i++;
            } else {
                tokens.push_back({PLUS, "+"});
            }
        }
        else if (ch == '-') {
            if (i + 1 < code.length() && code[i + 1] == '-') {
                tokens.push_back({DECREMENT, "--"});
                i++;
            } else {
                tokens.push_back({MINUS, "-"});
            }
        }
        else if (ch == '*') tokens.push_back({MULTIPLY, "*"});
        else if (ch == '/') tokens.push_back({DIVIDE, "/"});
        else if (ch == '%') tokens.push_back({MODULUS, "%"});
        else if (ch == '<') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back({LESS_THAN_EQ, "<="});
                i++;
            } else {
                tokens.push_back({LESS_THAN, "<"});
            }
        }
        else if (ch == '>') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back({GREATER_THAN_EQ, ">="});
                i++;
            } else {
                tokens.push_back({GREATER_THAN, ">"});
            }
        }
        else if (ch == '=') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back({LOGIC_EQUAL, "=="});
                i++;
            } else {
                tokens.push_back({ASSIGNMENT, "="});
            }
        }
        else if (ch == '&') {
            if (i + 1 < code.length() && code[i + 1] == '&') {
                tokens.push_back({LOGIC_AND, "&&"});
                i++;
            } else {
                tokens.push_back({BIT_AND, "&"});
            }
        }
        else if (ch == '|') {
            if (i + 1 < code.length() && code[i + 1] == '|') {
                tokens.push_back({LOGIC_OR, "||"});
                i++;
            } else {
                tokens.push_back({BIT_OR, "|"});
            }
        }
        else if (ch == '!'){
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back({LOGIC_NOT_EQUAL, "!="});
                i++;
            } else {
                tokens.push_back({LOGIC_NOT, "!"});
            }
        }
        else if (ch != '.') { // Skip standalone dots as they're handled in number parsing
            tokens.push_back({INVALID, string(1, ch)});
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
            case INVALID: tokenType = "INVALID"; break;
        }
        cout << tokenType << ": " << token.second << endl;

        // Insert into symbol table
        SymbolTable symbol;
        #line 284
        symbol.insert(token.second, tokenType, token.second, __LINE__, token.second[0], token.second.length());
        symbol.find(token.second);
    }
}

int main() {
    // Test case
    string code = R"(
        int main() {
        int zero;
        zero = 1;
        return 0;
        }
    )";

    vector<pair<TokenType, string>> tokens = lexer(code);
    printTokens(tokens);
    return 0;
}
