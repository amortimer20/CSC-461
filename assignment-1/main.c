#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


/* -----------------------------------------
   Character classes
----------------------------------------- */
enum CharClass {
    LETTER,          // Letter or underscore
    DIGIT,           // Digit 0-9
    UNKNOWN,         // Operator, punctuation, whitespace, etc.
    END_OF_INPUT     // End of file
};


/* -----------------------------------------
   Token types
----------------------------------------- */
enum TokenType {
    INT_LIT,             // Integer literal, e.g. 25
    IDENT,               // Valid identifier, e.g. sum
    ASSIGN_OP,           // Assignment operator =
    ADD_OP,              // Addition operator +
    SUB_OP,              // Subtraction operator -
    MULT_OP,             // Multiplication operator *
    DIV_OP,              // Division operator /
    LEFT_PAREN,          // Left parenthesis (
    RIGHT_PAREN,         // Right parenthesis )
    SEMICOLON,           // Semicolon ;
    END_TOKEN,           // End of input file
    INVALID_TOKEN,       // Unsupported symbol, e.g. @
    INVALID_IDENTIFIER   // Invalid identifier, e.g. 2sum
};


/* -----------------------------------------
   Global variables
----------------------------------------- */
enum CharClass charClass;      // Class of current character
enum TokenType nextToken;      // Current token

char lexeme[100];              // Stores current lexeme
char nextChar;                 // Current character

int lexLen;                    // Length of current lexeme

FILE *inputFile;               // Input file


/* -----------------------------------------
   Function declarations
----------------------------------------- */

/*
   addChar()

   Purpose:
       Adds nextChar to lexeme.

   Input:
       nextChar

   Output:
       Updates lexeme and lexLen.

   Important:
       Keep lexeme properly terminated with '\0'.
*/
void addChar(void)
{
    
}


/*
   getChar()

   Purpose:
       Reads one character from the input file and
       determines its character class.

   Uses:
       inputFile

   Produces:
       Updates nextChar and charClass

   Character classes:
       LETTER       - letter or underscore '_'
       DIGIT        - digit 0-9
       UNKNOWN      - operators, punctuation,
                      whitespace, etc.
       END_OF_INPUT - end of file

   Important:
       getc() returns an int because it must also
       be able to return EOF. After checking for
       EOF, the character can be stored in nextChar.
*/
void getChar(void);


/*
   skipWhitespaceAndComments()

   Purpose:
       Skips characters that should not become tokens
       before the lexer processes the next token.

   Uses:
       nextChar, charClass, inputFile

   Produces:
       Positions nextChar at the beginning of
       the next token.

   Must skip:
       - spaces
       - tabs
       - newlines
       - // single-line comments
       - block comments

   Important:
       When '/' is found, determine whether it is:
           /   division operator
           //  single-line comment
           block comment beginning

       Comments must not produce tokens.

       If a block comment is not closed before EOF,
       report an error.
*/
void skipWhitespaceAndComments(void);


/*
   isValidIdentifier()

   Purpose:
       Checks whether a string follows the identifier
       rules used in this assignment.

   Input:
       name - the identifier to check

   Returns:
       1 - valid identifier
       0 - invalid identifier

   Rules:
       - First character must be a letter or '_'.
       - Remaining characters may be letters,
         digits, or '_'.

   Examples:
       sum        -> valid
       sum2       -> valid
       _count     -> valid
       value_2    -> valid

       2sum       -> invalid
       4_score    -> invalid
*/
int isValidIdentifier(const char *name);


/*
   lookup()

   Purpose:
       Determines the token type for operators
       and punctuation symbols.

   Input:
       ch - one character

   Returns:
       Corresponding TokenType

   Must recognize:
       (   LEFT_PAREN
       )   RIGHT_PAREN
       +   ADD_OP
       -   SUB_OP
       *   MULT_OP
       /   DIV_OP
       =   ASSIGN_OP
       ;   SEMICOLON

   Important:
       Any unsupported symbol should be classified
       as INVALID_TOKEN.
*/
enum TokenType lookup(char ch);


/*
   lex()

   Purpose:
       Reads and recognizes the next token from
       the input file.

   Uses:
       nextChar, charClass, lexeme, inputFile

   Produces:
       - Updates nextToken
       - Builds the current lexeme
       - Returns the recognized TokenType

   Must recognize:
       - identifiers
       - integer literals
       - invalid identifiers
       - operators
       - parentheses
       - semicolons
       - invalid symbols
       - end of file

   Important:
       - Skip whitespace and comments first.
       - For this assignment, 2sum should be
         one INVALID_IDENTIFIER, not:
             INT_LIT 2
             IDENT sum
*/
enum TokenType lex(void);


/*
   tokenName()

   Purpose:
       Converts a TokenType into readable text
       for displaying the output.

   Input:
       token - a TokenType value

   Returns:
       String containing the token name

   Example:
       IDENT      -> "IDENT"
       INT_LIT    -> "INT_LIT"
       ADD_OP     -> "ADD_OP"

   Note:
       This function is only used to make the
       lexer output easier to read.
*/
const char *tokenName(enum TokenType token);


/* =========================================
   MAIN PROGRAM
========================================= */

int main(void)
{
    inputFile = fopen("test.txt", "r");

    if (inputFile == NULL) {
        printf("Error: could not open the input file.\n");
        return EXIT_FAILURE;
    }

    // Read the first character
    getChar();

    // Continue calling lex() until EOF
    do {
        lex();
    }
    while (nextToken != END_TOKEN);

    fclose(inputFile);

    return EXIT_SUCCESS;
}







/* =========================================
   tokenName
   Provided for you
========================================= */

const char *tokenName(enum TokenType token)
{
    switch (token) {

        case INT_LIT:
            return "INT_LIT";

        case IDENT:
            return "IDENT";

        case ASSIGN_OP:
            return "ASSIGN_OP";

        case ADD_OP:
            return "ADD_OP";

        case SUB_OP:
            return "SUB_OP";

        case MULT_OP:
            return "MULT_OP";

        case DIV_OP:
            return "DIV_OP";

        case LEFT_PAREN:
            return "LEFT_PAREN";

        case RIGHT_PAREN:
            return "RIGHT_PAREN";

        case SEMICOLON:
            return "SEMICOLON";

        case END_TOKEN:
            return "EOF";

        case INVALID_TOKEN:
            return "INVALID_TOKEN";

        case INVALID_IDENTIFIER:
            return "INVALID_IDENTIFIER";

        default:
            return "UNKNOWN_TOKEN";
    }
}
