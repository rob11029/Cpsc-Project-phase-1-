#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Class for a TAC instruction (think of it like a line of code)
class TACInstruction {
public:
    string op;        // Operator, like "+", "-", etc.
    string result;    // Result, like t1, x, etc.
    string operand1;  // Operand 1, like x, 5, etc.
    string operand2;  // Operand 2 (optional)

    TACInstruction(string op, string result, string operand1, string operand2 = "")
        : op(op), result(result), operand1(operand1), operand2(operand2) {}

    // Prints the TAC instruction nicely
    void print() const {
        if (operand2.empty())
            cout << result << " = " << operand1 << " " << op << endl;
        else
            cout << result << " = " << operand1 << " " << op << " " << operand2 << endl;
    }
};

// Class to generate TAC instructions
class TACGenerator {
private:
    int tempVarCount; // Counter for generating temporary vars (t1, t2, etc.)
    int labelCount;   // Counter for generating unique labels (L1, L2, etc.)
    vector<TACInstruction> instructions; // All TAC instructions we generate

    // Helper method to create temporary variables like t1, t2, t3
    string generateTempVar() {
        return "t" + to_string(tempVarCount++);
    }

    // Helper method to generate unique labels like L1, L2
    string generateLabel() {
        return "L" + to_string(labelCount++);
    }

public:
    TACGenerator() : tempVarCount(1), labelCount(1) {}

    // Generate TAC for expressions like t1 = x + y
    void generateTACForExpression(string op, string operand1, string operand2) {
        string result = generateTempVar(); // Create a new temporary variable
        TACInstruction instr(op, result, operand1, operand2); // Create the TAC instruction
        instructions.push_back(instr); // Save it
    }

    // Generate TAC for unary expressions like t1 = -x
    void generateTACForUnaryExpression(string op, string operand) {
        string result = generateTempVar();
        TACInstruction instr(op, result, operand); // Create the TAC instruction
        instructions.push_back(instr);
    }

    // Generate TAC for assignments like x = t1
    void generateTACForAssignment(string var, string expr) {
        TACInstruction instr("=", var, expr); // Create the assignment instruction
        instructions.push_back(instr);
    }

    // Generate TAC for if statement (if condition goto label)
    void generateTACForIf(string condition, string label) {
        TACInstruction instr("if", condition, label); // If condition is true, jump to label
        instructions.push_back(instr);
    }

    // Generate TAC for while loop (while condition goto label)
    void generateTACForWhile(string condition, string label) {
        TACInstruction instr("while", condition, label); // While condition is true, jump to label
        instructions.push_back(instr);
    }

    // Generate TAC for return statement like return t1
    void generateTACForReturn(string expr) {
        TACInstruction instr("return", expr, ""); // Just return the expression
        instructions.push_back(instr);
    }

    // Traverse the AST and generate TAC for each node
    void generateTACForAST(ASTNode* astNode) {
        if (astNode == nullptr) return;

        // Handle different node types like "Program", "Statement", etc.
        if (astNode->nodeType == "Program") {
            for (auto* child : astNode->children) {
                generateTACForAST(child); // Handle all the program's children (e.g., functions)
            }
        }
        else if (astNode->nodeType == "Block") {
            for (auto* child : astNode->children) {
                generateTACForAST(child); // Handle all block statements (like declarations and assignments)
            }
        }
        else if (astNode->nodeType == "Declaration") {
            // Declarations don't generate TAC, but they affect symbol table
        }
        else if (astNode->nodeType == "Statement") {
            if (astNode->children.size() == 1) {
                // Handle assignment (e.g., t1 = x + y)
                string var = astNode->children[0]->value;
                string expr = astNode->children[1]->value;
                generateTACForAssignment(var, expr);
            }
            else if (astNode->children.size() == 2) {
                // Handle control flow: if and while statements
                string condition = astNode->children[0]->value;
                string label = astNode->children[1]->value;
                if (condition == "if") {
                    generateTACForIf(condition, label);
                } else if (condition == "while") {
                    generateTACForWhile(condition, label);
                }
            }
        }
        else if (astNode->nodeType == "Expression") {
            string op = astNode->value;
            if (astNode->children.size() == 2) {
                // Binary expression (e.g., t1 = x + y)
                string operand1 = astNode->children[0]->value;
                string operand2 = astNode->children[1]->value;
                generateTACForExpression(op, operand1, operand2);
            } else if (astNode->children.size() == 1) {
                // Unary expression (e.g., t1 = -x)
                string operand = astNode->children[0]->value;
                generateTACForUnaryExpression(op, operand);
            }
        }
    }

    // Print out all TAC instructions (so we know what was generated)
    void printTAC() const {
        for (const auto& instr : instructions) {
            instr.print();
        }
    }
};

// Simple class to represent AST nodes (abstract syntax tree)
class ASTNode {
public:
    string nodeType; // Type of node (e.g., "Program", "Statement", "Expression")
    string value;    // Value of the node (e.g., "x", "+", etc.)
    vector<ASTNode*> children; // Child nodes (sub-expressions or sub-statements)

    ASTNode(string type) : nodeType(type) {}
    ASTNode(string type, string val) : nodeType(type), value(val) {}

    void addChild(ASTNode* child) {
        if (child != nullptr) {
            children.push_back(child);
        }
    }

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

    ~ASTNode() {
        for (auto* child : children) {
            delete child;
        }
    }
};

int main() {
    // Create a sample AST for the second test case
    ASTNode* programNode = new ASTNode("Program");

    // Add a block for the main function
    ASTNode* blockNode = new ASTNode("Block");
    programNode->addChild(blockNode);

    // Example: x = a + b
    ASTNode* assignmentNode = new ASTNode("Statement");
    ASTNode* varNode = new ASTNode("Identifier", "x");
    ASTNode* exprNode = new ASTNode("Expression", "+");
    exprNode->addChild(new ASTNode("Identifier", "a"));
    exprNode->addChild(new ASTNode("Identifier", "b"));
    assignmentNode->addChild(varNode);
    assignmentNode->addChild(exprNode);
    blockNode->addChild(assignmentNode);

    // Example: if (x < 10) goto L1
    ASTNode* ifNode = new ASTNode("Statement");
    ASTNode* conditionNode = new ASTNode("Expression", "<");
    conditionNode->addChild(new ASTNode("Identifier", "x"));
    conditionNode->addChild(new ASTNode("Integer", "10"));
    ifNode->addChild(new ASTNode("if"));
    ifNode->addChild(conditionNode);
    ifNode->addChild(new ASTNode("L1")); // Label for if block
    blockNode->addChild(ifNode);

    // Instantiate the TAC generator
    TACGenerator tacGen;
    tacGen.generateTACForAST(programNode);

    // Print out the TAC instructions we generated
    cout << "Generated TAC:" << endl;
    tacGen.printTAC();

    // Clean up memory
    delete programNode;

    return 0;
}
