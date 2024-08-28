#include <iostream>
#include <cctype>
#include <cstring>
#include<fstream>

#define MAX_TOKEN_LEN 100

enum Tokentype{
    TOKEN_INT, TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_ASSIGN,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_IF, TOKEN_EQUAL, TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_SEMICOLON, TOKEN_UNKNOWN, TOKEN_EOF, TOKEN_MUL, TOKEN_DIV
};
struct Token{
    Tokentype type;
    char text[MAX_TOKEN_LEN];
};
void getNextToken(std::fstream &file, Token &token) {
    int c;
    while ((c = file.get()) != EOF) {
        if (isspace(c)) continue;
        if (isalpha(c)) {
            int len = 0;
            token.text[len++] = c;
            while (isalnum(c = file.get())) {
            if (len < MAX_TOKEN_LEN - 1) token.text[len++] = c;
        }
        file.unget();
        token.text[len] = '\0';
        if (strcmp(token.text, "int") == 0) token.type = TOKEN_INT;
        else if (strcmp(token.text, "if") == 0) token.type = TOKEN_IF;
        else token.type = TOKEN_IDENTIFIER;
        return;
        }
        if (isdigit(c)) {
            int len = 0;
            token.text[len++] = c;
            while (isdigit(c = file.get())) {
            if (len < MAX_TOKEN_LEN - 1) token.text[len++] = c;
        }
        file.unget();
        token.text[len] = '\0';
        token.type = TOKEN_NUMBER;
        return;
        }
        switch (c) {
        case '=': {
            if(file.peek()=='='){
                file.get();
                token.type = TOKEN_EQUAL;
                token.text[0] = '=';
                token.text[1] = '=';
                token.text[2] = '\0';
            }else{
                token.type = TOKEN_ASSIGN;
                token.text[0]='=';
                token.text[1] = '\0';
            }
            return;
        };
        case '+': token.type = TOKEN_PLUS; token.text[0] = '+'; token.text[1] = '\0'; return;
        case '-': token.type = TOKEN_MINUS; token.text[0] = '-'; token.text[1] = '\0'; return;
        case '*': token.type = TOKEN_MUL; token.text[0] = '*'; token.text[1] = '\0'; return;
        case '/': token.type = TOKEN_DIV; token.text[0] = '/'; token.text[1] = '\0'; return;
        case '(': token.type = TOKEN_LBRACE; token.text[0] = '('; token.text[1] = '\0'; return;
        case ')': token.type = TOKEN_RBRACE; token.text[0] = ')'; token.text[1] = '\0'; return;
        case '{': token.type = TOKEN_LBRACE; token.text[0] = '{'; token.text[1] = '\0'; return;
        case '}': token.type = TOKEN_RBRACE; token.text[0] = '}'; token.text[1] = '\0'; return;
        case ';': token.type = TOKEN_SEMICOLON; token.text[0] = ';'; token.text[1] = '\0'; return;
        default: token.type = TOKEN_UNKNOWN; token.text[0] = c; token.text[1] ='\0'; return;
        }
    }
    token.type = TOKEN_EOF;
    token.text[0] = '\0';
}

void generateAssemblyForDeclaration(const char *varName){
    std::cout<<"MOD : "<<varName<<'\n';
}
void generateAssemblyForAssignment(const char*varName, const char *value){
    std::cout<<"LOAD : "<<value<<'\n';
    std::cout<<"STORE : "<<varName<<'\n';
}

void parseDeclaration(std::fstream &file){
    Token token;
    getNextToken(file,token);
    while(token.type == TOKEN_INT){
        getNextToken(file,token);
        if(token.type != TOKEN_IDENTIFIER){
            std::cout<<"syntax error : expected var name after 'int'\n";
            return;
        }
        generateAssemblyForDeclaration(token.text);

        getNextToken(file,token);
        if(token.type != TOKEN_SEMICOLON){
            std::cout<<"syntax error : expected ';' after var declaration\n";
            return;
        }
        getNextToken(file,token);
    }
    file.unget();
}

void parseArithmetic(std::fstream &file, const char *varName){
    Token token;
    getNextToken(file,token);
    while(token.type == TOKEN_PLUS || token.type == TOKEN_MINUS || token.type == TOKEN_MUL || token.type == TOKEN_DIV){
        char op = token.text[0];

        getNextToken(file,token);
        if(token.type != TOKEN_IDENTIFIER && token.type != TOKEN_NUMBER){
            std::cout<<"syntax error : expected number or val after "<<op <<'\n';
            return;
        }

        if(op == '+'){
            std::cout<<"ADD : "<<token.text<<'\n';
        }else if(op == '-'){
            std::cout<<"SUB : "<<token.text<<'\n';
        }else if(op == '*'){
            std::cout<<"MUL : "<<token.text<<'\n';
        }else if(op == '/'){
            std::cout<<"DIV : "<<token.text<<'\n';
        }
        getNextToken(file,token);
    }
    std::cout<<"STORE : "<<varName << '\n';
    file.unget();
}

void parseExpression(std::fstream &file){
    Token token;
    getNextToken(file,token);
    if(token.type != TOKEN_IDENTIFIER){
        std::cout<<"syntax error : expected var name at start of expression\n";
        return;
    }

    char varName[MAX_TOKEN_LEN];
    strcpy(varName,token.text);

    getNextToken(file,token);
    if(token.type != TOKEN_ASSIGN){
        std::cout<<"syntax Error : expected '=' after var name\n";
        return;
    }

    getNextToken(file,token);
    if(token.type == TOKEN_NUMBER){
        generateAssemblyForAssignment(varName,token.text);

    }else if(token.type == TOKEN_IDENTIFIER){
        std::cout<<"LOAD : "<<token.text<<'\n';
        parseArithmetic(file,varName);
    }else{
        std::cout<<"syntax error : unexpected token in expression\n";
        return;
    }
    getNextToken(file,token);
    if(token.type != TOKEN_SEMICOLON){
        std::cout<<"syntax error : expected ';' at end of expression\n";
        return;
    }
}

void parseIfStatement(std::fstream &file){
    Token token;
    getNextToken(file,token);
    if(token.type != TOKEN_LBRACE){
        std::cout<<"syntax error : expected '(' after 'if'\n";
        return;
    }
    getNextToken(file,token);
    if(token.type != TOKEN_IDENTIFIER){
        std::cout<<"syntax error : expected var name in condition\n";
        return;
    }

    char varName[MAX_TOKEN_LEN];
    strcpy(varName,token.text);

    getNextToken(file,token);
    if(token.type != TOKEN_EQUAL){
        std::cout<<"syntax error : expected '==' in condition\n ";
        return;
    }
    getNextToken(file,token);
    if(token.type != TOKEN_NUMBER){
        std::cout<<"syntax error : expected a number after '=='\n";
        return;
    }

    std::cout<<"CMP : "<<token.text<<'\n';
    std::cout<<"JNE : if\n";

    getNextToken(file,token);
    if(token.type != TOKEN_RBRACE){
        std::cout<<"syntax error : ')' at end of condition\n";
        return;
    }
    getNextToken(file,token);
    if(token.type != TOKEN_LBRACE){
        std::cout<<"syntax error : '{' after 'if' condition\n";
        return;
    }

    parseExpression(file);

    std::cout<<"end_if : \n";
}

int main() {
    std::fstream file("../input/input.txt");
    if (!file) {
    perror("Failed to open file");
    return 1;
    }

    parseDeclaration(file);

    parseExpression(file);
    parseExpression(file);
    parseExpression(file);

    Token token;
    getNextToken(file,token);
    if(token.type == TOKEN_IF){
        parseIfStatement(file);
    }

 file.close();
 return 0;
}