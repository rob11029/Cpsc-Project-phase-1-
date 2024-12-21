#include <iostream>
using namespace std;

class Node {
  string lexeme, token, value;
  string data_type; // added field for type (int, float, void, etc.)
  int block_id; // block number
  int line_number, char_start_num, length; // Preserved location information
  Node* next;

public:
  Node() {
    next = NULL;
    block_id = 0; // initial block id
  }

  // Node needs lexemes, token, lexeme value / lexeme itself, line number, character start number, and lexeme length
  Node (string lexeme, string token, string value, int line_number, int char_start_num, int length) {
    this->lexeme = lexeme;
    this->token = token;
    this->value = value;
    this->data_type = ""; // phase 4 update
    this->block_id = 0; // phase 4 update
    this->line_number = line_number;
    this->char_start_num = char_start_num;
    this->length = length;
    this->next = NULL;
  }

  // FOR TESTING, COULD BE REMOVE WHEN USING THE PARSER
  void print() {
    cout << " Lexeme: " << lexeme
         << "\n Token: " << token
         << "\n Token Value: " << value
         << "\n Data Type: " << data_type
         << "\n Block #: " << block_id
         << "\n Line #: " << line_number
         << "\n Character Start number: " << char_start_num
         << "\n Length: " << length << endl << endl;
  }
  friend class SymbolTable;
};

class SymbolTable {
  Node* head[100];
  int current_block;

public:
  SymbolTable() {
    for (int i = 0; i < 100; i++) {
      head[i] = NULL;
    }
    current_block = 0; // initialize block number
  }

  // Block management
  void enterBlock() {
    current_block++;
  }

  void exitBlock() {
    if (current_block > 0) {
      current_block--;
    }
  }
  // Get current block
  int getCurrentBlock() {
    return current_block;
  }

  // Modified insert to only store identifiers and keywords
  bool insert(string lexeme, string token, string value, 
              int line_no, int char_start_num, int length) {
    // Only store identifiers and keywords
    if (token != "IDENTIFIER" && token != "KEYWORD") {
      return false;
    }

    int index = getIndex(lexeme);
    Node* newNode = new Node(lexeme, token, value, line_no, char_start_num, length);
    newNode->block_id = current_block;

    if (head[index] == NULL) {
      head[index] = newNode;
    } else {
      Node* current = head[index];
      while (current->next != NULL) {
        if (current->lexeme == lexeme && current->block_id == current_block) {
          delete newNode;
          return false; // Identifier already exists in the current block
        }
        current = current->next;
      }
      current->next = newNode;
    }
    return true;
  }

  // Set type for an identifier
  bool setType(string lexeme, string type) {
    int index = getIndex(lexeme);
    Node* current = head[index];

    while (current != NULL) {
      if (current->lexeme == lexeme && 
          current->block_id == current_block) {
        current->data_type = type;
        return true;
      }
      current = current->next;
    }
    return false;
  }

  string find(string lexeme) {
    int index = getIndex(lexeme);
    Node* current = head[index];

    for (int searchBlock = current_block; searchBlock >= 0; searchBlock--) {
      Node* temp = current;
      while (temp != NULL) {
        if (temp->lexeme == lexeme && temp->block_id <= searchBlock) {
          temp->print();
          return temp->lexeme;
        }
        temp = temp->next;
      }
    }
    return "-1";
  }

  string getType(string lexeme) {
    int index = getIndex(lexeme);
    Node* current = head[index];

    while (current != NULL) {
      if (current->lexeme == lexeme && 
          current->block_id <= current_block) {
        return current->data_type;
      }
      current = current->next;
    }
    return "unknown";
  }

  int getIndex(string lexeme) {
    int index = 0;
    for (int i = 0; i < lexeme.length(); i++) {
      index = index + lexeme[i];
    }
    return (index % 100);
  }
};