#include "parser_phase_2.cpp"
#include <unordered_set>
using namespace std;

// AST Node for Abstract Syntax Tree
class ASTNode {
public:
    string nodeType;
    string value;
    vector<ASTNode*> children;

    // Constructors
    ASTNode(const string& type) : nodeType(type) {}
    ASTNode(const string& type, const string& val) : nodeType(type), value(val) {}

    // Add a child node
    void addChild(ASTNode* child) {
        if (child != nullptr) {
            children.push_back(child);
        }
    }

    // Print the tree, kinda ugly but works
    void printTree(int depth = 0) const {
        string indent(depth * 2, ' ');
        cout << indent << nodeType;
        if (!value.empty()) {
            cout << " (" << value << ")";
        }
        cout << endl;
        for (auto* child : children) {
            if (child != nullptr) {
                child->printTree(depth + 1);
            }
        }
    }

    // Destructor to clean up
    ~ASTNode() {
        for (auto* child : children) {
            delete child;
        }
    }
};

// Semantic analyzer that makes an AST and checks stuff
class SemanticAnalyzer {
private:
    unordered_set<string> declaredVariables;

    // Check if a variable was declared
    void checkVariableDeclared(const string& varName) {
        if (declaredVariables.find(varName) == declaredVariables.end()) {
            cerr << "Error: Variable '" << varName << "' is not declared." << endl;
            exit(1);
        }
    }

    void errorchecking() {
      cout << "Error here" << endl;
    }

    // Transform CST to AST
    ASTNode* transformToAST(CSTNode* cstNode) {
        if (!cstNode) return nullptr;

        switch (cstNode->getType()) {
            case NodeType::PROGRAM: {
                ASTNode* programNode = new ASTNode("Program");
                for (CSTNode* child : cstNode->getChildren()) {
                    ASTNode* astChild = transformToAST(child);
                    if (astChild) programNode->addChild(astChild);
                }
                return programNode;
            }
            case NodeType::BLOCK: {
              ASTNode* blockNode = new ASTNode("Block");
              blockNode->addChild(transformToAST(cstNode->getChildren()[0]));
              blockNode->addChild(transformToAST(cstNode->getChildren()[1]));
              blockNode->addChild(transformToAST(cstNode->getChildren()[2]));
              blockNode->addChild(transformToAST(cstNode->getChildren()[3]));
              return blockNode;
            }
            case NodeType::DECLS: {
              ASTNode* firstChild = transformToAST(cstNode->getChildren()[0]);
              return firstChild;
            }
            case NodeType::DECL: {
                ASTNode* declNode = new ASTNode("Declaration");
                for (CSTNode* child : cstNode->getChildren()) {
                    ASTNode* astChild = transformToAST(child);
                    if (astChild) declNode->addChild(astChild);
                }
                return declNode;
            }
            case NodeType::TYPE: {
               ASTNode* firstChild = transformToAST(cstNode->getChildren()[0]);
               return firstChild;

            }
            case NodeType::STMTS: {
              ASTNode* stmtsNode = new ASTNode("Statements");
                for (CSTNode* child : cstNode->getChildren()) {
                    ASTNode* astChild = transformToAST(child);
                    if (astChild) stmtsNode->addChild(astChild);
                }
              return stmtsNode;
            }

            case NodeType::STMT: {
                CSTNode* firstChild = cstNode->getChildren()[0];
                ASTNode* stmtNode = new ASTNode("Statement");
                if (firstChild->getTokenType() == RETURN) {
                for (CSTNode* child : cstNode->getChildren()) {
                    ASTNode* astChild = transformToAST(child);
                    if (astChild) stmtNode->addChild(astChild);
                }
                  return stmtNode;
                } else if (firstChild->getTokenType() == IF) {
                    ASTNode* ifNode = new ASTNode("if");
                    ifNode->addChild(transformToAST(cstNode->getChildren()[1]));
                    ifNode->addChild(transformToAST(cstNode->getChildren()[2]));
                    if (cstNode->getChildren().size() > 3) {
                        ifNode->addChild(transformToAST(cstNode->getChildren()[3]));
                    }
                    return ifNode;
                } else if (firstChild->getTokenType() == WHILE) {
                for (CSTNode* child : cstNode->getChildren()) {
                    ASTNode* astChild = transformToAST(child);
                    if (astChild) stmtNode->addChild(astChild);
                }
                  return stmtNode;
                } else if (firstChild->getTokenType() == BREAK) {
                for (CSTNode* child : cstNode->getChildren()) {
                    ASTNode* astChild = transformToAST(child);
                    if (astChild) stmtNode->addChild(astChild);
                }
                  return stmtNode;
                } else if (firstChild->getTokenType() == DO) {
                for (CSTNode* child : cstNode->getChildren()) {
                    ASTNode* astChild = transformToAST(child);
                    if (astChild) stmtNode->addChild(astChild);
                }
                  return stmtNode;
                } else if (firstChild->getTokenType() == IDENTIFIER) {
                for (CSTNode* child : cstNode->getChildren()) {
                    ASTNode* astChild = transformToAST(child);
                    if (astChild) stmtNode->addChild(astChild);
                }
                  return stmtNode;
                }
            }
            case NodeType::LOC: {
                ASTNode* locNode = new ASTNode("Loc");
                for (CSTNode* child : cstNode->getChildren()) {
                    ASTNode* astChild = transformToAST(child);
                    if (astChild) locNode->addChild(astChild);
                }
                return locNode;
            }
            case NodeType::BOOL: {
              ASTNode* boolNode = new ASTNode("Bool");
              boolNode->addChild(transformToAST(cstNode->getChildren()[0]));
              return boolNode;
            }
            case NodeType::JOIN: {
              ASTNode* joinNode = new ASTNode("Join");
              joinNode->addChild(transformToAST(cstNode->getChildren()[0]));
              return joinNode;
            }
            case NodeType::EQUALITY: {
              ASTNode* equalityNode = new ASTNode("Equality");
              equalityNode->addChild(transformToAST(cstNode->getChildren()[0]));
              return equalityNode;
            }
            case NodeType::REL: {
              ASTNode* relNode = new ASTNode("Rel");
              relNode->addChild(transformToAST(cstNode->getChildren()[0]));
              return relNode;
            }

            case NodeType::EXPR: {
                ASTNode* exprNode = new ASTNode("Expression");
                exprNode->addChild(transformToAST(cstNode->getChildren()[0]));
                exprNode->addChild(new ASTNode("Operator", cstNode->getChildren()[1]->getValue()));
                exprNode->addChild(transformToAST(cstNode->getChildren()[2]));
                return exprNode;
            }
            case NodeType::TERM: {
              ASTNode* termNode = new ASTNode("Term");
              termNode->addChild(transformToAST(cstNode->getChildren()[0]));
              return termNode;
            }
            case NodeType::UNARY: {
              ASTNode* unaryNode = new ASTNode("Unary");
              unaryNode->addChild(transformToAST(cstNode->getChildren()[0]));
              return unaryNode;
            }
            case NodeType::FACTOR: {
              ASTNode* factorNode = new ASTNode("Factor");
              factorNode->addChild(transformToAST(cstNode->getChildren()[0]));
              return factorNode;
            }
            case NodeType::EPSILON: {
              ASTNode* epsilonNode = new ASTNode("Epsilon");
              epsilonNode->addChild(transformToAST(cstNode->getChildren()[0]));
              return epsilonNode;
            }
            default:
                if (cstNode->getType() == NodeType::TERMINAL) {
                    return new ASTNode("Terminal", cstNode->getValue());
                }
        }
        return nullptr;
    }

public:
    // This is the main function to analyze the CST
    ASTNode* analyze(CSTNode* cstRoot) {
        if (!cstRoot) {
            cerr << "Error: Empty syntax tree." << endl;
            exit(1);
        }
        return transformToAST(cstRoot);
    }
};

// Test the semantic analyzer
int main() {
    SymbolTable symbolTable;

    // Testing this mess
    string code = R"(
    int main() {
    int zero;
    zero = 0;
    return 0;
    }
    )";

    // Lexical analysis
    vector<pair<TokenType, string>> tokens = lexer(code, symbolTable);
    cout << "Tokens:" << endl;
    printTokens(tokens);

    // Parsing
    Parser parser(tokens, symbolTable);
    CSTNode* syntaxTree = parser.parse();
    cout << "\nConcrete Syntax Tree:" << endl;
    syntaxTree->printTree();

    // Semantic analysis
    SemanticAnalyzer analyzer;
    ASTNode* ast = analyzer.analyze(syntaxTree);
    cout << "\nAbstract Syntax Tree:" << endl;
    ast->printTree();

    // Clean up time
    delete syntaxTree;
    delete ast;

    return 0;
}
