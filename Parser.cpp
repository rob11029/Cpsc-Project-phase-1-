#include <iostream>
#include <vector>
#include "lexer_phase_1.cpp"
#include "symbol_table.cpp"

using namespace std;

// Set up token tracking hre
Token currentToken;
int tokenIndex = 0;
vector<Token> tokens;

// Move the next one
void getNextToken() {
    if (tokenIndex < tokens.size()) {
        currentToken = tokens[tokenIndex++];
    } else {
        currentToken = Token(END_OF_FILE, "");
    }
}

// Check if the current token matches what we want and move to the next
bool expect(TokenType type) {
    if (currentToken.type == type) {
        getNextToken();
        return true;
    } else {
        cout << "Error: Expected " << type << " but got " << currentToken.lexeme << endl;
        return false;
    }
}

// Report if something goes wrong
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

// Program
void parseProgram() {
    expect(KEYWORD);
    parseBlock();
}

// declarations statements
void parseBlock() {
    expect(LEFT_BRACE);
    parseDeclarations();
    parseStatements();
    expect(RIGHT_BRACE);
}

// Parses a list of variable declarations
void parseDeclarations() {
    if (currentToken.type == BASIC || currentToken.type == IDENTIFIER) {
        parseDeclaration();
        parseDeclarations();
    }
    // We can exit here if we need to
}

// Parses a variable declaration with a type, name, and semicolon
void parseDeclaration() {
    parseType();
    expect(IDENTIFIER);
    expect(SEMICOLON);
}

// Parses a data type, like basic types or arrays
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

// Parses a list of statements, like if, while, return
void parseStatements() {
    if (currentToken.type == LEFT_BRACE || currentToken.type == IDENTIFIER || 
        currentToken.type == IF || currentToken.type == WHILE || currentToken.type == RETURN) {
        parseStatement();
        parseStatements();
    }
    // if it doesnt match we assume its epsi
}

// Statement
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

// Parses a part of an expression with + or - operators
void parseExpression() {
    parseTerm();
    while (currentToken.type == PLUS || currentToken.type == MINUS) {
        getNextToken();
        parseTerm();
    }
}

// Parses a part of an expression with *, /, or % operators
void parseTerm() {
    parseFactor();
    while (currentToken.type == MULTIPLY || currentToken.type == DIVIDE || currentToken.type == MODULUS) {
        getNextToken();
        parseFactor();
    }
}

// Parses a single number, variable, or expression in parentheses
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
    SymbolTable symbol_table;
    std::string code = "int main() { int x; if (x > 0) { x = x + 1; } }";

    // Pass code and symbol table to lexer and get tokens
    tokens = lexer(code, symbol_table);
    getNextToken();  // Initialize the first token

    // Begin parsing
    parseProgram();

    if (currentToken.type == END_OF_FILE) {
        std::cout << "Parsing completed successfully!" << std::endl;
    } else {
        std::cout << "Parsing failed." << std::endl;
    }

    return 0;
}
