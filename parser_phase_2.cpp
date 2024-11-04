#include <iostream>
#include <vector>
#include <string>
#include "lexer_phase_1.cpp"
using namespace std;

class Parser {
private:
    vector<pair<TokenType, string>> tokens;
    size_t currentPos;
    SymbolTable& symbolTable;
    
    // Helper functions for parsing
    bool match(TokenType type) {
        if (currentPos < tokens.size() && tokens[currentPos].first == type) {
            currentPos++;
            return true;
        }
        return false;
    }

    // New function to peek at the next token
    bool peek(TokenType type) {
        if (currentPos < tokens.size()) {
            return tokens[currentPos].first == type;
        }
        return false;
    }
    
    // New function to handle expected tokens
    void expect(TokenType type) {
        if (!match(type)) {
            error("Expected " + tokenTypeToString(type));
        }
    }
    
    // New function to handle syntax errors
    void error(const string& message) {
        cout << "Syntax Error: " << message;
        if (currentPos < tokens.size()) {
            cout << " at token '" << tokens[currentPos].second << "'";
        }
        cout << endl;
        exit(1);
    }

    // New function to convert token types to strings
    string tokenTypeToString(TokenType type) {
        switch (type) {
            case BASIC: return "basic type";
            case IDENTIFIER: return "identifier";
            case SEMICOLON: return "semicolon";
            case LEFT_PAREN: return "(";
            case RIGHT_PAREN: return ")";
            case LEFT_BRACE: return "{";
            case RIGHT_BRACE: return "}";
            case ASSIGNMENT: return "=";
            case IF: return "if";
            case ELSE: return "else";
            case WHILE: return "while";
            case DO: return "do";
            case INTEGER: return "integer";
            case REAL: return "real";
            default: return "unknown token";
        }
    }

    // New function to handle program parsing
    void parseProgram() {
        // Handle function return type
        if (!match(BASIC)) error("Expected basic");
        
        // Handle main function
        if (!match(MAIN)) error("Expected 'main'");
        
        // Handle parameters
        expect(LEFT_PAREN);
        parseParameters();  // New function to handle parameters
        expect(RIGHT_PAREN);
        
        // Handle function body
        expect(LEFT_BRACE);
        parseBlock();
        expect(RIGHT_BRACE);
    }

    // New function to handle function parameters
    void parseParameters() {
        // Handle void parameter
        if (peek(BASIC) && tokens[currentPos].second == "void") {
            match(BASIC);
            return;
        }
        
        // Handle other parameter types (can be extended for multiple parameters)
        if (peek(BASIC)) {
            match(BASIC);
            expect(IDENTIFIER);
            while (match(COMMA)) {
                expect(BASIC);
                expect(IDENTIFIER);
            }
        }
    }
    // New function to handle block parsing
    void parseBlock() {
        while (currentPos < tokens.size() && !peek(RIGHT_BRACE)) {
            if (peek(BASIC)) {
                parseDeclaration();
            } else {
                parseStatement();
            }
        }
    }
    // New function to handle variable declarations
    void parseDeclaration() {
        match(BASIC);
        string varType = tokens[currentPos-1].second;
        
        if (!match(IDENTIFIER)) error("Expected identifier after type");
        string varName = tokens[currentPos-1].second;
        
        symbolTable.insert(varName, "VARIABLE", varType, 0, 0, varName.length());
        
        if (match(ASSIGNMENT)) {
            parseExpression();
        }
        
        expect(SEMICOLON);
    }
    // New function to handle statements
    void parseStatement() {
        if (peek(IDENTIFIER)) {
            parseAssignmentStatement();
        }
        else if (match(IF)) {
            parseIfStatement();
        }
        else if (match(WHILE)) {
            parseWhileStatement();
        }
        else if (match(DO)) {
            parseDoWhileStatement();
        }
        else if (match(BREAK)) {
            expect(SEMICOLON);
        }
        else if (peek(KEYWORD) && tokens[currentPos].second == "return") {
            match(KEYWORD);
            if (!peek(INTEGER)) {
                parseExpression();  // Allow expressions in return statements
            } else {
                match(INTEGER);
            }
            expect(SEMICOLON);
        }
        else {
            parseExpression();  // Allow standalone expressions
            expect(SEMICOLON);
        }
    }
    // New function to handle assignment statements
    void parseAssignmentStatement() {
        expect(IDENTIFIER);
        string varName = tokens[currentPos-1].second;
        
        if (symbolTable.find(varName) == "-1") {
            error("Undefined variable: " + varName);
        }
        
        expect(ASSIGNMENT);
        parseExpression();
        expect(SEMICOLON);
    }
    // New function to handle if statements
    void parseIfStatement() {
        // if (Expression) Statement [else Statement]
        expect(LEFT_PAREN);
        parseExpression();
        expect(RIGHT_PAREN);
        
        if (match(LEFT_BRACE)) {
            parseBlock();
            expect(RIGHT_BRACE);
        } else {
            parseStatement();
        }
        
        if (match(ELSE)) {
            if (match(LEFT_BRACE)) {
                parseBlock();
                expect(RIGHT_BRACE);
            } else {
                parseStatement();
            }
        }
    }
    // New function to handle while statements
    void parseWhileStatement() {
        expect(LEFT_PAREN);
        parseExpression();
        expect(RIGHT_PAREN);
        
        if (match(LEFT_BRACE)) {
            parseBlock();
            expect(RIGHT_BRACE);
        } else {
            parseStatement();
        }
    }
    // New function to handle do-while statements
    void parseDoWhileStatement() {
        if (match(LEFT_BRACE)) {
            parseBlock();
            expect(RIGHT_BRACE);
        } else {
            parseStatement();
        }
        
        expect(WHILE);
        expect(LEFT_PAREN);
        parseExpression();
        expect(RIGHT_PAREN);
        expect(SEMICOLON);
    }
    // New function to handle expressions
    void parseExpression() {
        // Handle comparison expressions
        parseRelationalExpression();
        
        while (peek(LOGIC_EQUAL) || peek(LOGIC_NOT_EQUAL)) {
            match(tokens[currentPos].first);
            parseRelationalExpression();
        }
    }
    // New function to handle relational expressions
    void parseRelationalExpression() {
        // Handle relational operators
        parseArithmeticExpression();
        
        while (peek(LESS_THAN) || peek(LESS_THAN_EQ) || 
               peek(GREATER_THAN) || peek(GREATER_THAN_EQ)) {
            match(tokens[currentPos].first);
            parseArithmeticExpression();
        }
    }
    // New function to handle arithmetic expressions
    void parseArithmeticExpression() {
        parseTerm();
        
        while (peek(PLUS) || peek(MINUS)) {
            match(tokens[currentPos].first);
            parseTerm();
        }
    }
    // New function to handle term parsing
    void parseTerm() {
        parseFactor();
        // Handle multiplication, division, and modulus
        while (peek(MULTIPLY) || peek(DIVIDE) || peek(MODULUS)) {
            match(tokens[currentPos].first);
            parseFactor();
        }
    }
    // New function to handle factor parsing
    void parseFactor() {
        if (match(IDENTIFIER)) {
            string varName = tokens[currentPos-1].second;
            if (symbolTable.find(varName) == "-1") {
                error("Undefined variable: " + varName);
            }
        }
        else if (match(INTEGER) || match(REAL)) {
            // Valid literal
        }
        else if (match(LEFT_PAREN)) {
            parseExpression();
            expect(RIGHT_PAREN);
        }
        else {
            error("Invalid expression factor");
        }
    }

public:
    Parser(const vector<pair<TokenType, string>>& tokenStream, SymbolTable& symTable) 
        : tokens(tokenStream), currentPos(0), symbolTable(symTable) {}

    void parse() {
        parseProgram();
        if (currentPos < tokens.size()) {
            error("Unexpected tokens after program end");
        }
        cout << "Parsing completed successfully!" << endl;
    }
};

int main() {
    SymbolTable symbolTable;

    // Test case
    string code = R"(
        int main()
        {
            int counter; int sum;
            counter = 0; sum = 0;
            while(counter < 10)
            {
                if (sum % 2 == 0)
                {
                    sum = sum + 2;
                }
                else
                {
                    sum = sum + 1;
                }
                counter = counter + 1;
            }
            return 0;
        }
    )";

    // Lexical Analysis
    vector<pair<TokenType, string>> tokens = lexer(code, symbolTable);
    cout << "Tokens:" << endl;
    printTokens(tokens);
    
    // Parsing 
    Parser parser(tokens, symbolTable);
    try {
        parser.parse();
    } catch (const exception& error) {
        cout << "Parsing failed: " << error.what() << endl;
    }
    return 0;
}