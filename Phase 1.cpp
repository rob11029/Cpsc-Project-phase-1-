#include <iostream>  
#include <string>   
#include <cctype>


enum TokenType {
    IDENTIFIER,
    NUMBER,
    COMMENT,

    // Parentheses, Braces, and Brackets from the first portion of the list
    // (
    LEFT_PAREN,
    // )
    RIGHT_PAREN,
    // {
    LEFT_BRACE,
    // }
    RIGHT_BRACE,
    // [
    LEFT_BRACKET,
    // ]
    RIGHT_BRACKET,

    // Bringing in the math in here
    // .
    DOT,
    // +
    PLUS,
    // -
    MINUS,
    // *
    MULTIPLY,
    // /
    DIVIDE,
    // %
    MODULUS,
    // <
    LESS_THAN,
    // >
    GREATER_THAN,
    // =
    ASSIGNMENT,
    // ;
    SEMICOLON,
    // ,
    COMMA,

    // Some operators from the list
    // ++
    INCREMENT,
    // --
    DECREMENT,
    // <=
    LESS_THAN_EQ,
    // >=
    GREATER_THAN_EQ,
    // ==
    LOGIC_EQUAL,
    // &&
    LOGIC_AND,
    // ||
    LOGIC_OR,
    // !
    LOGIC_NOT,
    // &
    BIT_AND,
    // |
    BIT_OR,

    // Keywords as i think were on the list
    INT,
    FLOAT,
    CHAR,
    MAIN,
    RETURN,
    WHILE,
    IF,
    ELSE,
    FOR,
    BREAK,
    CONTINUE,
    SWITCH,
    CASE,
    GOTO,
    UNSIGNED,
    VOID,
    // anything else that I'm not sure of
    UNKNOWN
};

