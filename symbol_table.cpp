#include <iostream>
using namespace std;

class Node {
  string lexeme, token, value, char_start_num;
  int line_number, length;
  Node* next;

public:
  Node() {
    next = NULL;
  }

  // Node needs lexemes, token, lexeme value / lexeme itself, line number, character start number, and lexeme length
  Node (string lexeme, string token, string value, int line_number, char char_start_num, int length) {
    this->lexeme = lexeme;
    this->token = token;
    this->value = value;
    this->line_number = line_number;
    this->char_start_num = char_start_num;
    this->length = length;
    next = NULL;
  }

  // FOR TESTING, COULD BE REMOVE WHEN USING THE PARSER
  void print() {
    cout << " Lexeme: " << lexeme
         << "\n Token: " << token
         << "\n Token Value: " << value
         << "\n Line #: " << line_number
         << "\n Character Start number: " << char_start_num
         << "\n Length: " << length << endl << endl;
  }
  friend class SymbolTable;
};

class SymbolTable {
  Node* head[100];

public:
  SymbolTable() {
    for (int i = 0; i < 100; i++) head[i] = NULL;
  }

// Function to find the lexeme in the symbol table
string find(string l) {
  int index = getIndex(l);
  Node* start = head[index];
  if (start == NULL) return "-1";

  while (start != NULL) {
    if (start->lexeme == l) {
      start->print();
      return start->lexeme;
    }
    start = start->next;
  }
  return "-1";
}

// Function to insert lexemes, token, lexeme value / lexeme itself, line number, character start number,
// and lexeme length into Symbol Table
bool insert(string l, string t, string v, int lNo, char c_s_n, int lgh) {
  int index = getIndex(l);
  Node* x = new Node(l, t, v, lNo, c_s_n, lgh);

  if (head[index] == NULL) {
    head[index] = x;
    cout << " Inserted " << l << " in the symbol table" << endl;
    return true;
  } else {
    Node* start = head[index];
    while (start->next != NULL) start = start->next;

    start->next = x;
    cout << " Inserted " << l << " in the symbol table" << endl;
    return true;
  }
  return false;
}

// Function to get index based on the lexeme
int getIndex(string l) {
  int index = 0;
  for (int i = 0; i < l.length(); i++) {
    index = index + l[i];
  }
  return (index % 100);
}
};
