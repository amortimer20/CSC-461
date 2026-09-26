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
    lexeme[lexLen] = nextChar;
    lexLen++;
    lexeme[lexLen] = '\0';
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
void getChar(void)
{
    int charCode = getc(inputFile);
    nextChar = (char)charCode;

    if (charCode == EOF)
        charClass = END_OF_INPUT;
    else if (isalpha(charCode) || charCode == '_')
        charClass = LETTER;
    else if (isdigit(charCode))
        charClass = DIGIT;
    else
        charClass = UNKNOWN;
}


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
void skipWhitespaceAndComments(void)
{
    while (1)
    {
        if (nextChar == ' ' || nextChar == '\t' || nextChar == '\n' || nextChar == '\r')
        {
            getChar();
        }
        else if (nextChar == '/')
        {
            getChar();

            if (nextChar == '/') // Single-line comment
            {
                while (nextChar != '\n')
                    getChar();
            }
            else if (nextChar == '*') // Multi-line comment
            {
                while (1)
                {
                    getChar();

                    if (charClass == END_OF_INPUT)
                    {
                        printf("Error: unclosed multi-line comment\n");
                        break;
                    }

                    if (nextChar == '*')
                    {
                        getChar();

                        if (nextChar == '/')
                        {
                            getChar();
                            break;
                        }
                    }
                }
            }
            else // Division operator
            {
                // Replace division operator as current character
                ungetc(nextChar, inputFile);
                nextChar = '/';
                charClass = UNKNOWN;
                break;
            }
        }
        else // Other symbol
        {
            break;
        }
    }
}


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
int isValidIdentifier(const char *name)
{
    if (!(isalpha(name[0]) || name[0] == '_'))
        return 0;
    

    for (int i = 1; name[i] != '\0'; i++)
    {
        if (!(isalpha(name[i]) || isdigit(name[i]) || name[i] == '_'))
            return 0;
    }
    
    return 1;
}


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
enum TokenType lookup(char ch)
{
    switch(ch)
    {
        case '(':
            return LEFT_PAREN;
        case ')':
            return RIGHT_PAREN;
        case '+':
            return ADD_OP;
        case '-':
            return SUB_OP;
        case '*':
            return MULT_OP;
        case '/':
            return DIV_OP;
        case '=':
            return ASSIGN_OP;
        case ';':
            return SEMICOLON;
        default:
            return INVALID_TOKEN;
    }
}


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
enum TokenType lex(void)
{
    skipWhitespaceAndComments();
    lexLen = 0; // Reset lexeme length before building

    if (charClass == LETTER)
    {
        while (charClass == LETTER || charClass == DIGIT)
        {
            addChar();
            getChar();
        }

        nextToken = isValidIdentifier(lexeme) ? IDENT : INVALID_IDENTIFIER;
    }
    else if (charClass == DIGIT)
    {
        while (charClass == DIGIT)
        {
            addChar();
            getChar();
        }

        nextToken = INT_LIT;
    }
    else if (charClass == UNKNOWN)
    {
        addChar();
        nextToken = lookup(nextChar);
        getChar();
    }
    else // END_OF_INPUT
    {
        strcpy(lexeme, "EOF");
        nextToken = END_TOKEN;
    }

    // printf("Token: %-10s\tLexeme: %s\n", tokenName(nextToken), lexeme);
    return nextToken;
}


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
    inputFile = fopen("test1.txt", "r");

    if (inputFile == NULL) {
        printf("Error: could not open the input file.\n");
        return EXIT_FAILURE;
    }

    // Read the first character
    getChar();

    // Continue calling lex() until EOF
    do {
        lex();
        printf("Token: %-10s\tLexeme: %s\n", tokenName(nextToken), lexeme);
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
