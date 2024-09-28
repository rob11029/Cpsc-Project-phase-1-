#include <iostream>
#include <string>
#include <cctype>
#include <vector>
using namespace std;

// Define token types
enum TokenType {
    KEYWORD, IDENTIFIER, OPERATOR, NUMBER, DELIMITER, COMMENT, INVALID
};

// Define keywords, operators, and delimiters
vector<string> keywords = {"int", "float", "main", "char", "if", "else", "return", "unsigned", "void", "goto",
                            "for", "while", "do", "break", "continue", "switch", "case", "default"};
vector<string> operators = {".", "+", "-", "*", "/", "%", "++", "--", ">", "<", "<<", ">>", "=", "==", "+=", "-="
                            ">=", "<=", "&&", "!", "||","&", "|}"};
vector<string> delimiters = {"(", ")", "{", "}", "[", "]", ";", ",", ":", "'"};

// Function to check if a lexeme is a keyword
bool isKeyword(const string& lexeme) {
    for (const string& keyword : keywords) {
        if (lexeme == keyword) {
            return true;
        }
    }
    return false;
}

// Function to check if a lexeme is an operator
bool isOperator(const string& lexeme) {
    for (const string& op : operators) {
        if (lexeme == op) {
            return true;
        }
    }
    return false;
}

// Function to check if a lexeme is a delimiter
bool isDelimiter(char ch) {
    for (const string& delim : delimiters) {
        if (delim[0] == ch) {
            return true;
        }
    }
    return false;
}

// Function to check if a lexeme is a number
bool isNumber(const string& lexeme) {
    for (char ch : lexeme) {
        if (!isdigit(ch) && ch != '.') {
            return false;
        }
    }
    return true;
}

// Lexical analyzer function
vector<pair<TokenType, string>> lexer(const string& code) {
    vector<pair<TokenType, string>> tokens;
    string lexeme;
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
           while(i < code.length() && code[i] != '\n'){
               i++;
           }
           string comment = code.substr(commentStart, i - commentStart);
           tokens.push_back({COMMENT, comment});
           continue;
       }

        // Check for delimiters
        if (isDelimiter(ch)) {
            tokens.push_back({DELIMITER, string(1, ch)});
            i++;
            continue;
        }

        // Check for operators
        if (isOperator(string(1, ch))) {
            string op(1, ch);
            if (i + 1 < code.length() && isOperator(op + code[i + 1])) {
                op += code[i + 1];
                i++;
            }
            tokens.push_back({OPERATOR, op});
            i++;
            continue;
        }

        // Handle identifiers and keywords
        if (isalpha(ch) || ch == '_') {
            lexeme.clear();
            lexeme += ch;
            i++;
            while (i < code.length() && (isalnum(code[i]) || code[i] == '_')) {
                lexeme += code[i];
                i++;
            }

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

        // Handle numbers
        if (isdigit(ch)) {
            lexeme.clear();
            while (i < code.length() && (isdigit(code[i]) || code[i] == '.')) {
                lexeme += code[i];
                i++;
            }

            if (isNumber(lexeme)) {
                tokens.push_back({NUMBER, lexeme});
            } else {
                tokens.push_back({INVALID, lexeme});
            }
            continue;
        }

        // If we reach this point, it's an invalid token
        tokens.push_back({INVALID, string(1, ch)});
        i++;

        // This detects if there are any numbers that have multiple decimals and makes it invalid
        bool HasDecimalPoint = false;
        while ( i < code.length() && (isdigit(code[i]) || code[i] == '.')){
            if(code[i] == '.') {
                if (HasDecimalPoint) break;
                HasDecimalPoint = true;
            }
            lexeme += code[i];
            i++;
        }

        // Is used for when a string is not closed correctly this will give it an invalid token
        if (ch == '"') {
            lexeme = "";
            lexeme += ch;
            i++;

            while (i < code.length() && code[i] != '"') {
                lexeme += code[i];
                i++;
            }

            if (i < code.length()) {
                lexeme += '"';
                tokens.push_back({IDENTIFIER, lexeme});
                i++;
            } else {
                tokens.push_back({INVALID, lexeme});
            }

            continue;
        }
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
            case OPERATOR: tokenType = "OPERATOR"; break;
            case NUMBER: tokenType = "NUMBER"; break;
            case DELIMITER: tokenType = "DELIMITER"; break;
            case COMMENT: tokenType = "COMMENT"; break;
            case INVALID: tokenType = "INVALID"; break;
        }
        cout << tokenType << ": " << token.second << endl;
    }
}

int main() {
    string code = R"( 
    // Keywords as identifiers
    int main() {
        int intVar = 1;
        char charVar1 = 'c';
        char charVar2 = 'C';
    }

    // Numbers
    int num1 = 123;
    float num2 = 456.789;
    int num3 = 0;

    // Comment
    //This is a comment.
    )";

    vector<pair<TokenType, string>> tokens = lexer(code);
    printTokens(tokens);

    return 0;
}