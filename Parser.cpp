#include <iostream>
#include <vector>
#include "lexer_phase_1.cpp" // Your lexer header file
#include "symbol_table.cpp"  // Your symbol table

using namespace std;

// Set up token tracking
Token currentToken;
int tokenIndex = 0;
vector<Token> tokens;

// Move to the next token in the list
void getNextToken() {
    if (tokenIndex < tokens.size()) {
        currentToken = tokens[tokenIndex++];
    } else {
        // Assuming there's a token for end-of-file in your lexer
        currentToken = Token(END_OF_FILE, "");
    }
}

// Check if the current token matches expected and move to the next one
bool expect(TokenType type) {
    if (currentToken.type == type) {
        getNextToken();
        return true;
    } else {
        cout << "Error: Expected " << type << " but got " << currentToken.lexeme << endl;
        return false;
    }
}

// Report an error if parsing goes wrong
void reportError(const string& message) {
    cout << "Error: " << message << " at token " << currentToken.lexeme << endl;
    exit(1);
}

// Functions for each rule in your grammar
void parseProgram();
void parseBlock();
void parseDeclarations();
void parseDeclaration();
void parseType();
void parseStatements();
void parseStatement();
void parseExpression();
void parseTerm();
void parseFactor();

// Program -> main block
void parseProgram() {
    expect(KEYWORD);  // Expect "main"
    parseBlock();
}

// Block -> { declarations statements }
void parseBlock() {
    expect(LEFT_BRACE);
    parseDeclarations();
    parseStatements();
    expect(RIGHT_BRACE);
}

// Declarations -> Declaration Declarations | ε
void parseDeclarations() {
    if (currentToken.type == BASIC || currentToken.type == IDENTIFIER) {
        parseDeclaration();
        parseDeclarations();
    }
    // If not BASIC or IDENTIFIER, we just exit this function, as it could be ε
}

// Declaration -> Type IDENTIFIER ;
void parseDeclaration() {
    parseType();
    expect(IDENTIFIER);
    expect(SEMICOLON);
}

// Type -> BASIC | [ BASIC ]
void parseType() {
    if (currentToken.type == BASIC) {
        expect(BASIC);
    } else if (currentToken.type == LEFT_BRACKET) {
        expect(LEFT_BRACKET);
        expect(NUMBER);
        expect(RIGHT_BRACKET);
    } else {
        reportError("Expected a type");
    }
}

// Statements -> Statement Statements | ε
void parseStatements() {
    if (currentToken.type == LEFT_BRACE || currentToken.type == IDENTIFIER || 
        currentToken.type == IF || currentToken.type == WHILE || currentToken.type == RETURN) {
        parseStatement();
        parseStatements();
    }
    // If it doesn't match any statement, it could be ε
}

// Statement -> IF | WHILE | RETURN | Block | etc.
void parseStatement() {
    if (currentToken.type == IF) {
        expect(IF);
        expect(LEFT_PAREN);
        parseExpression();
        expect(RIGHT_PAREN);
        parseStatement();
    } else if (currentToken.type == WHILE) {
        expect(WHILE);
        expect(LEFT_PAREN);
        parseExpression();
        expect(RIGHT_PAREN);
        parseStatement();
    } else if (currentToken.type == RETURN) {
        expect(RETURN);
        parseExpression();
        expect(SEMICOLON);
    } else if (currentToken.type == LEFT_BRACE) {
        parseBlock();
    } else {
        reportError("Expected a statement");
    }
}

// Expression -> Term { + Term }
void parseExpression() {
    parseTerm();
    while (currentToken.type == PLUS || currentToken.type == MINUS) {
        getNextToken(); // Handle "+" or "-"
        parseTerm();
    }
}

// Term -> Factor { * Factor }
void parseTerm() {
    parseFactor();
    while (currentToken.type == MULTIPLY || currentToken.type == DIVIDE || currentToken.type == MODULUS) {
        getNextToken(); // Handle "*", "/", or "%"
        parseFactor();
    }
}

// Factor -> ( Expression ) | NUMBER | IDENTIFIER
void parseFactor() {
    if (currentToken.type == LEFT_PAREN) {
        expect(LEFT_PAREN);
        parseExpression();
        expect(RIGHT_PAREN);
    } else if (currentToken.type == NUMBER) {
        expect(NUMBER);
    } else if (currentToken.type == IDENTIFIER) {
        expect(IDENTIFIER);
    } else {
        reportError("Expected a factor");
    }
}

// Main function
int main() {
    // Example code to parse
    string code = "int main() { int x; if (x > 0) { x = x + 1; } }";
    
    // Get tokens from lexer
    tokens = lexer(code);
    getNextToken(); // Start with the first token

    // Start parsing
    parseProgram();

    if (currentToken.type == END_OF_FILE) {
        cout << "Parsing completed successfully!" << endl;
    } else {
        cout << "Parsing failed." << endl;
    }

    return 0;
}
