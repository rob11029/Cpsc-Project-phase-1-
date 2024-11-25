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
                for (CSTNode* child : cstNode->getChildren()) {
                    ASTNode* astChild = transformToAST(child);
                    if (astChild) blockNode->addChild(astChild);
                }
                return blockNode;
            }

            // Skip through DECLS
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

            // Skip through TYPE
            case NodeType::TYPE: {
               ASTNode* firstChild = transformToAST(cstNode->getChildren()[0]);
               return firstChild;

            }
            case NodeType::STMTS: {
              ASTNode* stmtsNode = transformToAST(cstNode->getChildren()[0]);
              stmtsNode->addChild(transformToAST(cstNode->getChildren()[1]));
              return stmtsNode;
            }

            case NodeType::STMT: {
              CSTNode* firstChild = cstNode->getChildren()[0];
              ASTNode* stmtNode = new ASTNode("Statement");
                for (CSTNode* child : cstNode->getChildren()) {
                    ASTNode* astChild = transformToAST(child);
                    if (astChild) stmtNode->addChild(astChild);
                }
                  return stmtNode;
            }

            // Skip through these
            case NodeType::LOC:
            case NodeType::BOOL:
            case NodeType::JOIN:
            case NodeType::EQUALITY:
            case NodeType::REL:
            case NodeType::EXPR:
            case NodeType::TERM:
            case NodeType::UNARY:
            case NodeType::FACTOR: {
               ASTNode* firstChild = transformToAST(cstNode->getChildren()[0]);
               return firstChild;
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
    zero = 1;
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
