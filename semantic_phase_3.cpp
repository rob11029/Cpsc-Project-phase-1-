#include "parser_phase_2.h"
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
            case NodeType::DECL: {
                string type = cstNode->getChildren()[0]->getValue();
                string name = cstNode->getChildren()[1]->getValue();
                declaredVariables.insert(name);
                ASTNode* declNode = new ASTNode("Declaration");
                declNode->addChild(new ASTNode("Type", type));
                declNode->addChild(new ASTNode("Identifier", name));
                return declNode;
            }
            case NodeType::STMT: {
                CSTNode* firstChild = cstNode->getChildren()[0];
                if (firstChild->getTokenType() == RETURN) {
                    ASTNode* returnNode = new ASTNode("Return");
                    returnNode->addChild(new ASTNode("Value", cstNode->getChildren()[1]->getValue()));
                    return returnNode;
                } else if (firstChild->getTokenType() == IF) {
                    ASTNode* ifNode = new ASTNode("If");
                    ifNode->addChild(transformToAST(cstNode->getChildren()[1]));
                    ifNode->addChild(transformToAST(cstNode->getChildren()[2]));
                    if (cstNode->getChildren().size() > 3) {
                        ifNode->addChild(transformToAST(cstNode->getChildren()[3]));
                    }
                    return ifNode;
                } else if (firstChild->getTokenType() == WHILE) {
                    ASTNode* whileNode = new ASTNode("While");
                    whileNode->addChild(transformToAST(cstNode->getChildren()[1]));
                    whileNode->addChild(transformToAST(cstNode->getChildren()[2]));
                    return whileNode;
                }
                break;
            }
            case NodeType::EXPR: {
                ASTNode* exprNode = new ASTNode("Expression");
                exprNode->addChild(transformToAST(cstNode->getChildren()[0]));
                exprNode->addChild(new ASTNode("Operator", cstNode->getChildren()[1]->getValue()));
                exprNode->addChild(transformToAST(cstNode->getChildren()[2]));
                return exprNode;
            }
            case NodeType::LOC: {
                string varName = cstNode->getChildren()[0]->getValue();
                checkVariableDeclared(varName);
                return new ASTNode("Variable", varName);
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
            int x;
            x = 5 + 3;
            if (x > 0) {
                return x;
            }
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
