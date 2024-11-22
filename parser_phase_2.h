#ifndef PARSER_PHASE_2_H
#define PARSER_PHASE_2_H

#include <iostream>
#include <string>
#include <vector>
#include "lexer_phase_1.cpp"

// Node types based on our grammar
enum class NodeType {
    PROGRAM, BLOCK, DECLS, DECL, TYPE, STMTS, STMT, LOC,
    BOOL, JOIN, EQUALITY, REL, EXPR, TERM, UNARY, FACTOR,
    TERMINAL, EPSILON
};

// Forward declaration of functions
std::string nodeTypeToString(NodeType type);

class CSTNode {
private:
    NodeType type;                    // Type of node
    std::string value;                // Value for terminals (lexeme)
    TokenType tokenType;              // Token type for terminals
    std::vector<CSTNode*> children;   // Child nodes

public:
    // Constructors
    CSTNode(NodeType t);
    CSTNode(TokenType tt, const std::string& val);
    static CSTNode* createEpsilon();

    // Tree operations
    void addChild(CSTNode* child);
    void printTree(int depth = 0) const;

    // Getters
    NodeType getType() const;
    std::string getValue() const;
    TokenType getTokenType() const;
    const std::vector<CSTNode*>& getChildren() const;

    // Destructor
    ~CSTNode();
};

class Parser {
private:
    std::vector<std::pair<TokenType, std::string>> tokens;
    size_t currentPos;
    SymbolTable& symbolTable;

    // Helper functions
    CSTNode* createTerminal();
    bool match(TokenType type);
    bool peek(TokenType type);
    void expect(TokenType type);
    void error(const std::string& message);
    std::string tokenTypeToString(TokenType type);

    // Grammar production functions
    CSTNode* parseProgram(); // productions 1
    CSTNode* parseBlock(); // productions 2
    CSTNode* parseDecls(); // productions 3
    CSTNode* parseDecl(); // productions 4
    CSTNode* parseType(); // productions 5
    CSTNode* parseStmts(); // productions 6
    CSTNode* parseStmt(); // productions 7-13
    CSTNode* parseLoc(); // productions 14
    CSTNode* parseBool(); // productions 15
    CSTNode* parseJoin(); // productions 16
    CSTNode* parseEquality(); // productions 17
    CSTNode* parseRel(); // productions 18
    CSTNode* parseExpr(); // productions 19
    CSTNode* parseTerm(); // productions 20
    CSTNode* parseUnary(); // productions 21-23
    CSTNode* parseFactor(); // productions 24-29
    
    // Prime and double-prime productions
    CSTNode* parseBlockPrime(); // productions 30-37
    CSTNode* parseBlockDoublePrime(); // productions 38-46
    CSTNode* parseStmtsPrime(); // productions 47-54
    CSTNode* parseStmtPrime(); // productions 55-56
    CSTNode* parseEqualityPrime(); // productions 57-58
    CSTNode* parseRelPrime(); // productions 59-63
    CSTNode* parseExprPrime(); // productions 64-65
    CSTNode* parseTermPrime(); // productions 66-67
    CSTNode* parseDeclsPrime(); // productions 68-69
    CSTNode* parseTypePrime(); // productions 70-71
    CSTNode* parseLocPrime(); // productions 72-73
    CSTNode* parseBoolPrime(); // productions 74-75
    CSTNode* parseJoinPrime(); // productions 76-77
    CSTNode* parseEqualityDoublePrime(); // productions 78-79
    CSTNode* parseExprDoublePrime(); // productions 80-81
    CSTNode* parseTermDoublePrime(); // productions 82-83

public:
    Parser(const std::vector<std::pair<TokenType, std::string>>& tokenStream, SymbolTable& symTable);
    CSTNode* parse();
};

#endif 