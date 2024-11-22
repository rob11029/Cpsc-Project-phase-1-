#include "parser_phase_2.h"
using namespace std;

// NodeType to string conversion implementation
string nodeTypeToString(NodeType type) {
    switch(type) {
        case NodeType::PROGRAM: return "Program";
        case NodeType::BLOCK: return "Block";
        case NodeType::DECLS: return "Decls";
        case NodeType::DECL: return "Decl";
        case NodeType::TYPE: return "Type";
        case NodeType::STMTS: return "Stmts";
        case NodeType::STMT: return "Stmt";
        case NodeType::LOC: return "Loc";
        case NodeType::BOOL: return "Bool";
        case NodeType::JOIN: return "Join";
        case NodeType::EQUALITY: return "Equality";
        case NodeType::REL: return "Rel";
        case NodeType::EXPR: return "Expr";
        case NodeType::TERM: return "Term";
        case NodeType::UNARY: return "Unary";
        case NodeType::FACTOR: return "Factor";
        case NodeType::TERMINAL: return "Terminal";
        case NodeType::EPSILON: return "epsilon";
        default: return "Unknown";
    }
}

// CSTNode implementation
CSTNode::CSTNode(NodeType t) 
    : type(t), tokenType(INVALID) {}

CSTNode::CSTNode(TokenType tt, const string& val) 
    : type(NodeType::TERMINAL), value(val), tokenType(tt) {}

CSTNode* CSTNode::createEpsilon() {
    return new CSTNode(NodeType::EPSILON);
}

void CSTNode::addChild(CSTNode* child) {
    if (child) children.push_back(child);
}

void CSTNode::printTree(int depth) const {
    string indent(depth * 2, ' ');  // Reduced indentation for compactness
    
    cout << indent;
    if (depth > 0) {
        cout << "|-- ";  // Simplified tree connector
    }
    
    if (type == NodeType::TERMINAL) {
        cout << "[" << value << "]" << endl;
    } else {
        cout << "[" << nodeTypeToString(type) << "]" << endl;
    }
    
    // Print children without epsilon nodes
    for (const auto* child : children) {
        if (child && child->type != NodeType::EPSILON) {
            child->printTree(depth + 1);
        }
    }
}


NodeType CSTNode::getType() const { return type; }
string CSTNode::getValue() const { return value; }
TokenType CSTNode::getTokenType() const { return tokenType; }
const vector<CSTNode*>& CSTNode::getChildren() const { return children; }

CSTNode::~CSTNode() {
    for (auto* child : children) {
        delete child;
    }
    children.clear();
}

// Parser implementation
Parser::Parser(const vector<pair<TokenType, string>>& tokenStream, SymbolTable& symTable) 
    : tokens(tokenStream), currentPos(0), symbolTable(symTable) {}

CSTNode* Parser::createTerminal() {
    if (currentPos < tokens.size()) {
        TokenType type = tokens[currentPos].first;
        string value = tokens[currentPos].second;
        currentPos++;
        return new CSTNode(type, value);
    }
    return nullptr;
}

bool Parser::match(TokenType type) {
    if (currentPos < tokens.size() && tokens[currentPos].first == type) {
        currentPos++;
        return true;
    }
    return false;
}

bool Parser::peek(TokenType type) {
    if (currentPos < tokens.size()) {
        return tokens[currentPos].first == type;
    }
    return false;
}

void Parser::expect(TokenType type) {
    if (!match(type)) {
        error("Expected " + tokenTypeToString(type));
    }
}

void Parser::error(const string& message) {
    cout << "Syntax Error: " << message;
    if (currentPos < tokens.size()) {
        cout << " at token '" << tokens[currentPos].second << "'";
    }
    cout << endl;
    exit(1);
}

// Grammar production functions
CSTNode* Parser::parseProgram() {
    CSTNode* node = new CSTNode(NodeType::PROGRAM);
    
    if (!peek(BASIC)) {
        error("Expected basic type (int, float, char, void)");
        return nullptr;
    }
    
    CSTNode* typeNode = new CSTNode(BASIC, tokens[currentPos].second);
    node->addChild(typeNode);
    currentPos++;
    
    if (!peek(MAIN)) {
        error("Expected 'main'");
        return nullptr;
    }
    
    CSTNode* mainNode = new CSTNode(MAIN, tokens[currentPos].second);
    node->addChild(mainNode);
    currentPos++;
    
    expect(LEFT_PAREN);
    node->addChild(new CSTNode(LEFT_PAREN, "("));
    
    expect(RIGHT_PAREN);
    node->addChild(new CSTNode(RIGHT_PAREN, ")"));
    
    CSTNode* blockNode = parseBlock();
    if (!blockNode) {
        error("Invalid block");
        return nullptr;
    }
    node->addChild(blockNode);
    
    return node;
}


CSTNode* Parser::parseBlock() {
    CSTNode* node = new CSTNode(NodeType::BLOCK);
    
    expect(LEFT_BRACE);
    node->addChild(new CSTNode(LEFT_BRACE, "{"));
    
    // Parse declarations if they exist
    if (peek(BASIC)) {
        CSTNode* declsNode = parseDecls();
        if (!declsNode) {
            error("Invalid declarations");
            return nullptr;
        }
        node->addChild(declsNode);
    }
    
    // Parse statements
    CSTNode* stmtsNode = parseStmts();
    if (!stmtsNode) {
        error("Invalid statements");
        return nullptr;
    }
    node->addChild(stmtsNode);
    
    expect(RIGHT_BRACE);
    node->addChild(new CSTNode(RIGHT_BRACE, "}"));
    
    return node;
}

CSTNode* Parser::parseDecls() {
    CSTNode* node = new CSTNode(NodeType::DECLS);
    
    CSTNode* declNode = parseDecl();
    if (!declNode) return nullptr;
    node->addChild(declNode);
    
    CSTNode* declsPrimeNode = parseDeclsPrime();
    if (!declsPrimeNode) return nullptr;
    node->addChild(declsPrimeNode);
    
    return node;
}

CSTNode* Parser::parseDecl() {
    CSTNode* node = new CSTNode(NodeType::DECL);
    
    CSTNode* typeNode = parseType();
    if (!typeNode) return nullptr;
    node->addChild(typeNode);
    
    if (!match(IDENTIFIER)) {
        error("Expected identifier");
        return nullptr;
    }
    node->addChild(new CSTNode(IDENTIFIER, tokens[currentPos-1].second));
    
    expect(SEMICOLON);
    node->addChild(new CSTNode(SEMICOLON, ";"));
    
    return node;
}

CSTNode* Parser::parseType() {
    CSTNode* node = new CSTNode(NodeType::TYPE);
    
    if (!match(BASIC)) {
        error("Expected basic type");
        return nullptr;
    }
    node->addChild(new CSTNode(BASIC, tokens[currentPos-1].second));
    
    CSTNode* typePrimeNode = parseTypePrime();
    if (!typePrimeNode) return nullptr;
    node->addChild(typePrimeNode);
    
    return node;
}

CSTNode* Parser::parseStmts() {
    if (!(peek(IF) || peek(IDENTIFIER) || peek(WHILE) || peek(DO) || 
          peek(BREAK) || peek(RETURN) || peek(LEFT_BRACE))) {
        return nullptr;
    }

    CSTNode* node = new CSTNode(NodeType::STMTS);
    
    CSTNode* stmtNode = parseStmt();
    if (!stmtNode) return nullptr;
    node->addChild(stmtNode);
    
    // Recursively parse more statements if they exist
    CSTNode* moreStmts = parseStmts();
    if (moreStmts) {
        node->addChild(moreStmts);
    }
    
    return node;
}

CSTNode* Parser::parseStmt() {
    CSTNode* node = new CSTNode(NodeType::STMT);
    
    if (match(IF)) {
        expect(LEFT_PAREN);
        node->addChild(new CSTNode(LEFT_PAREN, "("));
        
        CSTNode* boolNode = parseBool();
        if (!boolNode) return nullptr;
        node->addChild(boolNode);
        
        expect(RIGHT_PAREN);
        node->addChild(new CSTNode(RIGHT_PAREN, ")"));
        
        CSTNode* stmtNode = parseStmt();
        if (!stmtNode) return nullptr;
        node->addChild(stmtNode);
        
        CSTNode* stmtPrimeNode = parseStmtPrime();
        if (!stmtPrimeNode) return nullptr;
        node->addChild(stmtPrimeNode);
    } 
    else if (peek(IDENTIFIER)) {
        CSTNode* locNode = parseLoc();
        if (!locNode) return nullptr;
        node->addChild(locNode);
        
        expect(ASSIGNMENT);
        node->addChild(new CSTNode(ASSIGNMENT, "="));
        
        CSTNode* boolNode = parseBool();
        if (!boolNode) return nullptr;
        node->addChild(boolNode);
        
        expect(SEMICOLON);
        node->addChild(new CSTNode(SEMICOLON, ";"));
    }
    else if (match(WHILE)) {
        expect(LEFT_PAREN);
        node->addChild(new CSTNode(LEFT_PAREN, "("));
        
        CSTNode* boolNode = parseBool();
        if (!boolNode) return nullptr;
        node->addChild(boolNode);
        
        expect(RIGHT_PAREN);
        node->addChild(new CSTNode(RIGHT_PAREN, ")"));
        
        CSTNode* stmtNode = parseStmt();
        if (!stmtNode) return nullptr;
        node->addChild(stmtNode);
    }
    else if (match(DO)) {
        CSTNode* stmtNode = parseStmt();
        if (!stmtNode) return nullptr;
        node->addChild(stmtNode);
        
        expect(WHILE);
        node->addChild(new CSTNode(WHILE, "while"));
        
        expect(LEFT_PAREN);
        node->addChild(new CSTNode(LEFT_PAREN, "("));
        
        CSTNode* boolNode = parseBool();
        if (!boolNode) return nullptr;
        node->addChild(boolNode);
        
        expect(RIGHT_PAREN);
        node->addChild(new CSTNode(RIGHT_PAREN, ")"));
        
        expect(SEMICOLON);
        node->addChild(new CSTNode(SEMICOLON, ";"));
    }
    else if (match(RETURN)) {
        if (!match(INTEGER)) {
            error("Expected number after return");
            return nullptr;
        }
        node->addChild(new CSTNode(INTEGER, tokens[currentPos-1].second));
        
        expect(SEMICOLON);
        node->addChild(new CSTNode(SEMICOLON, ";"));
    }
    else if (match(BREAK)) {
        expect(SEMICOLON);
        node->addChild(new CSTNode(SEMICOLON, ";"));
    }
    else if (peek(LEFT_BRACE)) {
        CSTNode* blockNode = parseBlock();
        if (!blockNode) return nullptr;
        node->addChild(blockNode);
    }
    else {
        error("Invalid statement");
        return nullptr;
    }
    
    return node;
}

CSTNode* Parser::parseLoc() {
    CSTNode* node = new CSTNode(NodeType::LOC);
    
    if (!match(IDENTIFIER)) {
        error("Expected identifier");
        return nullptr;
    }
    node->addChild(new CSTNode(IDENTIFIER, tokens[currentPos-1].second));
    
    CSTNode* locPrime = parseLocPrime();
    if (locPrime) {
        node->addChild(locPrime);
    }
    
    return node;
}

CSTNode* Parser::parseBool() {
    CSTNode* joinNode = parseJoin();
    if (!joinNode) return nullptr;
    
    // If there's a logical OR, create a Bool node
    if (peek(LOGIC_OR)) {
        CSTNode* node = new CSTNode(NodeType::BOOL);
        node->addChild(joinNode);
        
        match(LOGIC_OR);
        node->addChild(new CSTNode(LOGIC_OR, "||"));
        
        CSTNode* rightJoin = parseJoin();
        if (!rightJoin) {
            delete node;
            return nullptr;
        }
        node->addChild(rightJoin);
        return node;
    }
    
    return joinNode;
}

CSTNode* Parser::parseJoin() {
    CSTNode* equalityNode = parseEquality();
    if (!equalityNode) return nullptr;
    
    // If there's a logical AND, create a Join node
    if (peek(LOGIC_AND)) {
        CSTNode* node = new CSTNode(NodeType::JOIN);
        node->addChild(equalityNode);
        
        match(LOGIC_AND);
        node->addChild(new CSTNode(LOGIC_AND, "&&"));
        
        CSTNode* rightEquality = parseEquality();
        if (!rightEquality) {
            delete node;
            return nullptr;
        }
        node->addChild(rightEquality);
        return node;
    }
    
    return equalityNode;
}

CSTNode* Parser::parseEquality() {
    CSTNode* relNode = parseRel();
    if (!relNode) return nullptr;
    
    // If there's an equality operator, create an Equality node
    if (peek(LOGIC_EQUAL) || peek(LOGIC_NOT_EQUAL)) {
        CSTNode* node = new CSTNode(NodeType::EQUALITY);
        node->addChild(relNode);
        
        TokenType op = tokens[currentPos].first;
        match(op);
        node->addChild(new CSTNode(op, tokens[currentPos-1].second));
        
        CSTNode* rightRel = parseRel();
        if (!rightRel) {
            delete node;
            return nullptr;
        }
        node->addChild(rightRel);
        return node;
    }
    
    return relNode;
}

CSTNode* Parser::parseRel() {
    CSTNode* exprNode = parseExpr();
    if (!exprNode) return nullptr;
    
    // If there's a relational operator, create a Rel node
    if (peek(LESS_THAN) || peek(LESS_THAN_EQ) || 
        peek(GREATER_THAN) || peek(GREATER_THAN_EQ)) {
        CSTNode* node = new CSTNode(NodeType::REL);
        node->addChild(exprNode);
        
        TokenType op = tokens[currentPos].first;
        match(op);
        node->addChild(new CSTNode(op, tokens[currentPos-1].second));
        
        CSTNode* rightExpr = parseExpr();
        if (!rightExpr) {
            delete node;
            return nullptr;
        }
        node->addChild(rightExpr);
        return node;
    }
    
    // No relational operator, just return the expression
    return exprNode;
}

CSTNode* Parser::parseExpr() {
    CSTNode* termNode = parseTerm();
    if (!termNode) return nullptr;

    // If there are no operators, just return the term
    if (!peek(PLUS) && !peek(MINUS)) {
        return termNode;
    }

    // Create expression node only if we have operators
    CSTNode* node = new CSTNode(NodeType::EXPR);
    node->addChild(termNode);
    
    while (peek(PLUS) || peek(MINUS)) {
        TokenType op = tokens[currentPos].first;
        match(op);
        node->addChild(new CSTNode(op, tokens[currentPos-1].second));
        
        CSTNode* nextTerm = parseTerm();
        if (!nextTerm) {
            delete node;
            return nullptr;
        }
        node->addChild(nextTerm);
    }
    
    return node;
}

CSTNode* Parser::parseTerm() {
    CSTNode* unaryNode = parseUnary();
    if (!unaryNode) return nullptr;

    // If there are no operators, just return the unary
    if (!peek(MULTIPLY) && !peek(DIVIDE)) {
        return unaryNode;
    }

    // Create term node only if we have operators
    CSTNode* node = new CSTNode(NodeType::TERM);
    node->addChild(unaryNode);
    
    while (peek(MULTIPLY) || peek(DIVIDE)) {
        TokenType op = tokens[currentPos].first;
        match(op);
        node->addChild(new CSTNode(op, tokens[currentPos-1].second));
        
        CSTNode* nextUnary = parseUnary();
        if (!nextUnary) {
            delete node;
            return nullptr;
        }
        node->addChild(nextUnary);
    }
    
    return node;
}

CSTNode* Parser::parseUnary() {
    CSTNode* node = new CSTNode(NodeType::UNARY);
    
    if (match(LOGIC_NOT)) {
        node->addChild(new CSTNode(LOGIC_NOT, "!"));
        
        CSTNode* unaryNode = parseUnary();
        if (!unaryNode) return nullptr;
        node->addChild(unaryNode);
    }
    else if (match(MINUS)) {
        node->addChild(new CSTNode(MINUS, "-"));
        
        CSTNode* unaryNode = parseUnary();
        if (!unaryNode) return nullptr;
        node->addChild(unaryNode);
    }
    else {
        CSTNode* factorNode = parseFactor();
        if (!factorNode) return nullptr;
        node->addChild(factorNode);
    }
    
    return node;
}

CSTNode* Parser::parseFactor() {
    if (match(LEFT_PAREN)) {
        CSTNode* node = new CSTNode(NodeType::FACTOR);
        node->addChild(new CSTNode(LEFT_PAREN, "("));
        
        CSTNode* boolNode = parseBool();
        if (!boolNode) {
            delete node;
            return nullptr;
        }
        node->addChild(boolNode);
        
        expect(RIGHT_PAREN);
        node->addChild(new CSTNode(RIGHT_PAREN, ")"));
        return node;
    }
    else if (match(INTEGER)) {
        return new CSTNode(INTEGER, tokens[currentPos-1].second);
    }
    else if (match(REAL)) {
        return new CSTNode(REAL, tokens[currentPos-1].second);
    }
    else if (peek(IDENTIFIER)) {
        return parseLoc();
    }
    
    error("Invalid factor");
    return nullptr;
}

CSTNode* Parser::parseBlockPrime() {
    CSTNode* node = new CSTNode(NodeType::BLOCK);
    
    if (match(RIGHT_BRACE)) {
        node->addChild(new CSTNode(RIGHT_BRACE, "}"));
        return node;
    }
    
    CSTNode* stmtsNode = parseStmts();
    if (!stmtsNode) return nullptr;
    node->addChild(stmtsNode);
    
    expect(RIGHT_BRACE);
    node->addChild(new CSTNode(RIGHT_BRACE, "}"));
    
    return node;
}

CSTNode* Parser::parseBlockDoublePrime() {
    CSTNode* node = new CSTNode(NodeType::BLOCK);
    
    if (peek(BASIC)) {
        CSTNode* declsNode = parseDecls();
        if (!declsNode) return nullptr;
        node->addChild(declsNode);
    }
    
    CSTNode* stmtsNode = parseStmts();
    if (!stmtsNode) return nullptr;
    node->addChild(stmtsNode);
    
    return node;
}

CSTNode* Parser::parseStmtsPrime() {
    CSTNode* node = new CSTNode(NodeType::STMTS);
    
    if (peek(IF) || peek(IDENTIFIER) || peek(WHILE) || peek(DO) || 
        peek(BREAK) || peek(RETURN) || peek(LEFT_BRACE)) {
        
        CSTNode* stmtNode = parseStmt();
        if (!stmtNode) return nullptr;
        node->addChild(stmtNode);
        
        CSTNode* stmtsPrimeNode = parseStmtsPrime();
        if (!stmtsPrimeNode) return nullptr;
        node->addChild(stmtsPrimeNode);
    }
    else {
        node->addChild(CSTNode::createEpsilon());
    }
    
    return node;
}

CSTNode* Parser::parseStmtPrime() {
    CSTNode* node = new CSTNode(NodeType::STMT);
    
    if (match(ELSE)) {
        node->addChild(new CSTNode(ELSE, "else"));
        
        CSTNode* stmtNode = parseStmt();
        if (!stmtNode) return nullptr;
        node->addChild(stmtNode);
    }
    else {
        node->addChild(CSTNode::createEpsilon());
    }
    
    return node;
}

CSTNode* Parser::parseEqualityPrime() {
    CSTNode* node = new CSTNode(NodeType::EQUALITY);
    
    if (match(LOGIC_EQUAL)) {
        node->addChild(new CSTNode(LOGIC_EQUAL, "=="));
        
        CSTNode* relNode = parseRel();
        if (!relNode) return nullptr;
        node->addChild(relNode);
    }
    else if (match(LOGIC_NOT_EQUAL)) {
        node->addChild(new CSTNode(LOGIC_NOT_EQUAL, "!="));
        
        CSTNode* relNode = parseRel();
        if (!relNode) return nullptr;
        node->addChild(relNode);
    }
    
    return node;
}

CSTNode* Parser::parseRelPrime() {
    CSTNode* node = new CSTNode(NodeType::REL);
    
    if (match(LESS_THAN)) {
        node->addChild(new CSTNode(LESS_THAN, "<"));
        
        CSTNode* exprNode = parseExpr();
        if (!exprNode) return nullptr;
        node->addChild(exprNode);
    }
    else if (match(LESS_THAN_EQ)) {
        node->addChild(new CSTNode(LESS_THAN_EQ, "<="));
        
        CSTNode* exprNode = parseExpr();
        if (!exprNode) return nullptr;
        node->addChild(exprNode);
    }
    else if (match(GREATER_THAN_EQ)) {
        node->addChild(new CSTNode(GREATER_THAN_EQ, ">="));
        
        CSTNode* exprNode = parseExpr();
        if (!exprNode) return nullptr;
        node->addChild(exprNode);
    }
    else if (match(GREATER_THAN)) {
        node->addChild(new CSTNode(GREATER_THAN, ">"));
        
        CSTNode* exprNode = parseExpr();
        if (!exprNode) return nullptr;
        node->addChild(exprNode);
    }
    else {
        node->addChild(CSTNode::createEpsilon());
    }
    return node;
}

CSTNode* Parser::parseExprPrime() {
    CSTNode* node = new CSTNode(NodeType::EXPR);
    
    if (match(PLUS)) {
        node->addChild(new CSTNode(PLUS, "+"));
        
        CSTNode* termNode = parseTerm();
        if (!termNode) return nullptr;
        node->addChild(termNode);
    }
    else if (match(MINUS)) {
        node->addChild(new CSTNode(MINUS, "-"));
        
        CSTNode* termNode = parseTerm();
        if (!termNode) return nullptr;
        node->addChild(termNode);
    }
    
    return node;
}

CSTNode* Parser::parseTermPrime() {
    CSTNode* node = new CSTNode(NodeType::TERM);
    
    if (match(MULTIPLY)) {
        node->addChild(new CSTNode(MULTIPLY, "*"));
        
        CSTNode* unaryNode = parseUnary();
        if (!unaryNode) return nullptr;
        node->addChild(unaryNode);
    }
    else if (match(DIVIDE)) {
        node->addChild(new CSTNode(DIVIDE, "/"));
        
        CSTNode* unaryNode = parseUnary();
        if (!unaryNode) return nullptr;
        node->addChild(unaryNode);
    }
    
    return node;
}

CSTNode* Parser::parseDeclsPrime() {
    CSTNode* node = new CSTNode(NodeType::DECLS);
    
    if (peek(BASIC)) {
        CSTNode* declNode = parseDecl();
        if (!declNode) return nullptr;
        node->addChild(declNode);
        
        CSTNode* declsPrimeNode = parseDeclsPrime();
        if (!declsPrimeNode) return nullptr;
        node->addChild(declsPrimeNode);
    }
    else {
        node->addChild(CSTNode::createEpsilon());
    }
    
    return node;
}

CSTNode* Parser::parseTypePrime() {
    CSTNode* node = new CSTNode(NodeType::TYPE);
    
    if (match(LEFT_BRACKET)) {
        node->addChild(new CSTNode(LEFT_BRACKET, "["));
        
        if (!match(INTEGER)) {
            error("Expected number in array declaration");
            return nullptr;
        }
        node->addChild(new CSTNode(INTEGER, tokens[currentPos-1].second));
        
        expect(RIGHT_BRACKET);
        node->addChild(new CSTNode(RIGHT_BRACKET, "]"));
        
        CSTNode* typePrimeNode = parseTypePrime();
        if (!typePrimeNode) return nullptr;
        node->addChild(typePrimeNode);
    }
    else {
        node->addChild(CSTNode::createEpsilon());
    }
    
    return node;
}

CSTNode* Parser::parseLocPrime() {
    if (!match(LEFT_BRACKET)) {
        return nullptr;  // Don't create epsilon nodes for empty loc primes
    }
    
    CSTNode* node = new CSTNode(NodeType::LOC);
    node->addChild(new CSTNode(LEFT_BRACKET, "["));
    
    CSTNode* boolNode = parseBool();
    if (!boolNode) return nullptr;
    node->addChild(boolNode);
    
    expect(RIGHT_BRACKET);
    node->addChild(new CSTNode(RIGHT_BRACKET, "]"));
    
    CSTNode* nextPrime = parseLocPrime();
    if (nextPrime) {
        node->addChild(nextPrime);
    }
    
    return node;
}


CSTNode* Parser::parseBoolPrime() {
    CSTNode* node = new CSTNode(NodeType::BOOL);
    
    if (match(LOGIC_OR)) {
        node->addChild(new CSTNode(LOGIC_OR, "||"));
        
        CSTNode* joinNode = parseJoin();
        if (!joinNode) return nullptr;
        node->addChild(joinNode);
        
        CSTNode* boolPrimeNode = parseBoolPrime();
        if (!boolPrimeNode) return nullptr;
        node->addChild(boolPrimeNode);
    }
    else {
        node->addChild(CSTNode::createEpsilon());
    }
    
    return node;
}

CSTNode* Parser::parseJoinPrime() {
    CSTNode* node = new CSTNode(NodeType::JOIN);
    
    if (match(LOGIC_AND)) {
        node->addChild(new CSTNode(LOGIC_AND, "&&"));
        
        CSTNode* equalityNode = parseEquality();
        if (!equalityNode) return nullptr;
        node->addChild(equalityNode);
        
        CSTNode* joinPrimeNode = parseJoinPrime();
        if (!joinPrimeNode) return nullptr;
        node->addChild(joinPrimeNode);
    }
    else {
        node->addChild(CSTNode::createEpsilon());
    }
    
    return node;
}

CSTNode* Parser::parseEqualityDoublePrime() {
    CSTNode* node = new CSTNode(NodeType::EQUALITY);
    
    if (peek(LOGIC_EQUAL) || peek(LOGIC_NOT_EQUAL)) {
        CSTNode* equalityPrimeNode = parseEqualityPrime();
        if (!equalityPrimeNode) return nullptr;
        node->addChild(equalityPrimeNode);
        
        CSTNode* equalityDoublePrimeNode = parseEqualityDoublePrime();
        if (!equalityDoublePrimeNode) return nullptr;
        node->addChild(equalityDoublePrimeNode);
    }
    else {
        node->addChild(CSTNode::createEpsilon());
    }
    
    return node;
}

CSTNode* Parser::parseExprDoublePrime() {
    CSTNode* node = new CSTNode(NodeType::EXPR);
    
    if (peek(PLUS) || peek(MINUS)) {
        CSTNode* exprPrimeNode = parseExprPrime();
        if (!exprPrimeNode) return nullptr;
        node->addChild(exprPrimeNode);
        
        CSTNode* exprDoublePrimeNode = parseExprDoublePrime();
        if (!exprDoublePrimeNode) return nullptr;
        node->addChild(exprDoublePrimeNode);
    }
    else {
        node->addChild(CSTNode::createEpsilon());
    }
    
    return node;
}

CSTNode* Parser::parseTermDoublePrime() {
    CSTNode* node = new CSTNode(NodeType::TERM);
    
    if (peek(MULTIPLY) || peek(DIVIDE)) {
        CSTNode* termPrimeNode = parseTermPrime();
        if (!termPrimeNode) return nullptr;
        node->addChild(termPrimeNode);
        
        CSTNode* termDoublePrimeNode = parseTermDoublePrime();
        if (!termDoublePrimeNode) return nullptr;
        node->addChild(termDoublePrimeNode);
    }
    else {
        node->addChild(CSTNode::createEpsilon());
    }
    
    return node;
}

// Convert TokenType to string for error messages
string Parser::tokenTypeToString(TokenType type) {
    switch (type) {
        case KEYWORD: return "keyword";
        case IDENTIFIER: return "identifier"; 
        case COMMENT: return "comment";
        case INVALID: return "invalid";
        case LEFT_PAREN: return "(";
        case RIGHT_PAREN: return ")";
        case LEFT_BRACKET: return "[";
        case RIGHT_BRACKET: return "]";
        case LEFT_BRACE: return "{";
        case RIGHT_BRACE: return "}";
        case DOT: return ".";
        case SEMICOLON: return ";";
        case COMMA: return ",";
        case PLUS: return "+";
        case MINUS: return "-";
        case MULTIPLY: return "*";
        case DIVIDE: return "/";
        case MODULUS: return "%";
        case ASSIGNMENT: return "=";
        case INCREMENT: return "++";
        case DECREMENT: return "--";
        case LESS_THAN: return "<";
        case LESS_THAN_EQ: return "<=";
        case GREATER_THAN: return ">";
        case GREATER_THAN_EQ: return ">=";
        case LOGIC_EQUAL: return "==";
        case LOGIC_AND: return "&&";
        case LOGIC_OR: return "||";
        case LOGIC_NOT: return "!";
        case BIT_AND: return "&";
        case BIT_OR: return "|";
        case LOGIC_NOT_EQUAL: return "!=";
        case BASIC: return "basic";
        case INTEGER: return "integer";
        case REAL: return "real";
        case IF: return "if";
        case ELSE: return "else";
        case WHILE: return "while";
        case BREAK: return "break";
        case MAIN: return "main";
        case DO: return "do";
        case RETURN: return "return";
        default: return "unknown";
    }
}

// Public parse method
CSTNode* Parser::parse() {
    CSTNode* root = parseProgram();
    if (currentPos < tokens.size()) {
        error("Unexpected tokens after program end");
        return nullptr;
    }
    return root;
}

// Add main function for testing if needed
int main() {
    SymbolTable symbolTable;

    // Test case
    string code = R"(
        int main() {
        int zero;
        zero = 1;
        return 0;
        }
    )";

    // Lexical Analysis
    vector<pair<TokenType, string>> tokens = lexer(code, symbolTable);
    cout << "Tokens:" << endl;
    printTokens(tokens);
    
    // Parsing with CST generation
    Parser parser(tokens, symbolTable);
    try {
        CSTNode* syntaxTree = parser.parse();
        if (syntaxTree) {
            cout << "\n=== Concrete Syntax Tree ===" << endl;
            cout << "-------------------------" << endl;
            syntaxTree->printTree(0);
            cout << "-------------------------" << endl;
            delete syntaxTree;
        }
    } catch (const exception& e) {
        cout << "Parsing failed: " << e.what() << endl;
    }
    return 0;
}