#include "scanner.h"
#include "testing_utils.h"
#include <ctype.h>
#include <string.h>


token_t deal_with_prolog();

token_t get_token(int token_num)
{
    int c; // store char
    token_t token;
    State_t state = STATE_START;

    
    // dealing with prolog: declare(...) separatly outside from fsm
    if(token_num == 1){
        return deal_with_prolog();
    }


    // has to be freed with dyn_string_free(string)
    // unless token with *string in it is returned
    // then caller has to call free_token() on it
    Dyn_String *string = dyn_string_init();


    while(true){
        
        c = getc(stdin);
        switch(state)
        {
            case STATE_START:                
                if(c== EOF){
                    token.type = TOKEN_EOF;
                    dyn_string_free(string);
                    return token;
                }
                // whitespace
                else if(isspace(c)){
                    break;
                }
                else if(isdigit(c)){
                    state=STATE_INT;
                    dyn_string_add_char(string,c);
                    break;
                }

                else if(isalpha(c) || c == '_'){
                    state = STATE_ID;
                    dyn_string_add_char(string,c);
                    break;
                }

                else if(c == '/'){
                    state = STATE_COMMENT;
                    break;
                }

                else if(c == '"'){
                    state = STATE_STRING_READ;
                    break;
                }
                
                else if(c == '*'){
                    token.type = TOKEN_MULTIPLY;
                    dyn_string_free(string);
                    return token;

                    break;
                }

                else if(c == '-'){
                    token.type = TOKEN_MINUS;
                    dyn_string_free(string);
                    return token;
                }

                else if(c == '+'){
                    token.type = TOKEN_PLUS;
                    dyn_string_free(string);
                    return token;
                }

                else if(c == '('){
                    token.type = TOKEN_L_PAR;
                    dyn_string_free(string);
                    return token;
                }
                else if(c == ')'){
                    token.type = TOKEN_R_PAR;
                    dyn_string_free(string);
                    return token;
                }
                else if(c == '{'){
                    token.type = TOKEN_L_BRAC;
                    dyn_string_free(string);
                    return token;
                }
                else if(c == '}'){
                    token.type = TOKEN_R_BRAC;
                    dyn_string_free(string);
                    return token;
                }

                else if(c == '!'){
                    state = STATE_NOT_EQUAL1;
                    break;
                }

                else if(c == '='){
                    state = STATE_EQUAL;
                    break;
                }

                else if(c == '<'){
                    state = STATE_SMALLER;
                    break;
                }

                else if(c == '>'){
                    state = STATE_BIGGER;
                    break;
                }

                else if(c == '.'){
                    token.type = TOKEN_DOT;
                    dyn_string_free(string);
                    return token;
                }
                else if(c == ','){
                    token.type = TOKEN_COMMA;
                    dyn_string_free(string);
                    return token;
                }
                else if(c == ':'){
                    token.type = TOKEN_COLON;
                    dyn_string_free(string);
                    return token;
                }
                else if(c == ';'){
                    token.type = TOKEN_SEMICOLON;
                    dyn_string_free(string);
                    return token;
                }

                else if(c == '?'){
                    state = STATE_QUEST_MARK;
                    break;
                }

                else if(c == '$'){
                    state = STATE_VAR_ID_START;
                    break;
                }

                else{
                    state = STATE_BLANK3;
                    break;
                }

            break;

            case STATE_INT:
                if(isdigit(c)){
                    dyn_string_add_char(string,c);
                    break;
                }
                else if(c=='.'){
                    dyn_string_add_char(string,c);
                    state = STATE_FLOAT;
                    break;
                }
                else{
                    ungetc(c,stdin);
                    token.type = TOKEN_INT;
                    token.int_value =strtol(string->string,NULL,10);
                    dyn_string_free(string);
                    return token;
                }
            break;

            case STATE_FLOAT:
                if(isdigit(c)){
                    dyn_string_add_char(string,c);
                    break;
                }
                else{
                    ungetc(c,stdin);
                    token.type = TOKEN_FLOAT;
                    token.float_value = strtod(string->string,NULL);
                    dyn_string_free(string);
                    return token;
                }
                break;
            
            case STATE_ID:
                if(isalnum(c)|| c == '_'){
                    dyn_string_add_char(string,c);
                    break;
                }
                else{
                    ungetc(c,stdin);
                    if(!strcmp((const char*) string->string,"else")){
                        dyn_string_free(string);
                        token.type = TOKEN_KEYWORD;
                        token.keyword = KEYWORD_ELSE;
                        return token;
                    }if(!strcmp((const char*) string->string,"float")){
                        dyn_string_free(string);
                        token.type = TOKEN_KEYWORD;
                        token.keyword = KEYWORD_FLOAT;
                        return token;
                    }if(!strcmp((const char*) string->string,"function")){
                        dyn_string_free(string);
                        token.type = TOKEN_KEYWORD;
                        token.keyword = KEYWORD_FUNCTION;
                        return token;
                    }if(!strcmp((const char*) string->string,"if")){
                        dyn_string_free(string);
                        token.type = TOKEN_KEYWORD;
                        token.keyword = KEYWORD_IF;
                        return token;
                    }if(!strcmp((const char*) string->string,"null")){
                        dyn_string_free(string);
                        token.type = TOKEN_KEYWORD;
                        token.keyword = KEYWORD_NULL;
                        return token;
                    }if(!strcmp((const char*) string->string,"return")){
                        dyn_string_free(string);
                        token.type = TOKEN_KEYWORD;
                        token.keyword = KEYWORD_RETURN;
                        return token;
                    }if(!strcmp((const char*) string->string,"string")){
                        dyn_string_free(string);
                        token.type = TOKEN_KEYWORD;
                        token.keyword = KEYWORD_STRING;
                        return token;
                    }if(!strcmp((const char*) string->string,"void")){
                        dyn_string_free(string);
                        token.type = TOKEN_KEYWORD;
                        token.keyword = KEYWORD_VOID;
                        return token;
                    }if(!strcmp((const char*) string->string,"while")){
                        dyn_string_free(string);
                        token.type = TOKEN_KEYWORD;
                        token.keyword = KEYWORD_WHILE;
                        return token;
                    }
                    // asi bude problem s free nekdy

                    token.type = TOKEN_FUNC_ID;
                    token.string = string;
                    return token;
                }
                break;
            
            case STATE_COMMENT:
                if(c=='/'){
                    state = STATE_LINE_COMMENT;
                }
                else if(c == '*'){
                    state = STATE_BLOCK_COM;
                }
                else{
                    state = STATE_BLANK0; 
                    ungetc(c,stdin);
                }
                break;
            
            case STATE_LINE_COMMENT:
                if(c == EOF){
                    state = STATE_BLANK0;
                    ungetc(c,stdin);
                }
                else if(c == '\n'){
                    state = STATE_START;
                }
                break;

            case STATE_BLOCK_COM:
                if(c == EOF){
                    state = STATE_BLANK0;
                    ungetc(c,stdin);
                }
                else if(c== '*'){
                    state = STATE_BLOCK_COM1;
                }
                break;
            
            case STATE_BLOCK_COM1:
                if(c == '/'){
                    state = STATE_START;
                }
                else{
                    state = STATE_BLOCK_COM1;
                }
                break;
            
            case STATE_STRING_READ:
                if(c == EOF){
                    ungetc(c,stdin);
                    state = STATE_BLANK0;
                }
                else if(c == 92){ // 92 == '\'
                    state = STATE_STRING_ESCAPE;
                }
                // white space or sharp
                // convert to escpae sequence \xzy
                else if(c <= 32 || c == 35)
                {
                    char tmp[3];
                    sprintf(tmp,"%d",(int)c);
                    if (tmp[1] == '\0')
                    {
                        tmp[1] = tmp[0];
                        tmp[0] = '0';
                        tmp[2] = '\0';
                    }
                    dyn_string_add_char(string,92);
                    dyn_string_add_char(string,'0');
                    dyn_string_add_string(string,tmp);                    

                }
                else if(c == '"'){
                    token.type = TOKEN_STRING;
                    token.string = string;
                    return token;
                }
                else{
                    dyn_string_add_char(string,c);
                }
                break;
            
            case STATE_STRING_ESCAPE:
                if(c == EOF)
                {
                    ungetc(c,stdin);
                    state = STATE_BLANK0;
                    break;
                }
                // \$ \" just add signs to string
                if (c == '"' || c == '$')
                {
                    state = STATE_STRING_READ;
                    dyn_string_add_char(string,c);
                    break;
                }
                // backslash
                if ( c == 92)
                {
                    state = STATE_STRING_READ;
                    dyn_string_add_char(string,92);
                    dyn_string_add_string(string,"\092");
                    break;                    
                }
                // \xhh
                if ( c == 'x')
                {
                    char tmp[3];
                    tmp[2]='\0';
                    c = getc(stdin);
                    if(c == EOF)
                    {
                        token.type = STATE_BLANK0;
                        ungetc(c,stdin);
                        return token;
                    }
                    if(!isxdigit(c))
                    {
                        dyn_string_add_char(string,92);
                        dyn_string_add_string(string,"092x");
                        ungetc(c,stdin);
                        state = STATE_STRING_READ;
                        break;
                    }
                    tmp[0] = c;
                    c = getc(stdin);
                    if(c == EOF)
                    {
                        token.type = STATE_BLANK0;
                        ungetc(c,stdin);
                        return token;
                    }
                    if(!isxdigit(c))
                    {
                        dyn_string_add_char(string,92);
                        dyn_string_add_string(string,"092x");
                        ungetc(tmp[0],stdin);
                        ungetc(c,stdin);
                        state = STATE_STRING_READ;
                        break;
                    }
                    tmp[1] = c;
                    char new = hex_to_dec(tmp);
                    // target language specific
                    if (new == 92)
                    {
                        dyn_string_add_char(string,92);
                        dyn_string_add_string(string,"092");
                    }
                    else if (new == '#')
                    {
                        dyn_string_add_char(string,92);
                        dyn_string_add_string(string,"035");
                    }
                    // source language specific
                    else if (new == '"')
                    {
                        dyn_string_add_char(string,'"');
                    }
                    else if (new == '$')
                    {
                        dyn_string_add_char(string,'$');
                    }
                    else{
                        ungetc(new,stdin);
                    }
                    state = STATE_STRING_READ;
                    break;
                }
                if (c <= '7' && c >= '0')
                {
                    char tmp[4];
                    tmp[3] = '\0';
                    tmp[0] = c;
                    c = getc(stdin);
                    if(c == EOF)
                    {
                        token.type = STATE_BLANK0;
                        ungetc(c,stdin);
                        return token;
                    }
                    if(c > '7' || c < '0')
                    {
                        dyn_string_add_char(string,92);
                        dyn_string_add_string(string,"092");
                        ungetc(c,stdin);
                        ungetc(tmp[0],stdin);
                        state = STATE_STRING_READ;
                        break;
                    }
                    tmp[1] = c;
                    c = getc(stdin);
                    if(c == EOF)
                    {
                        token.type = STATE_BLANK0;
                        ungetc(c,stdin);
                        return token;
                    }
                    if(c > '7' || c < '0')
                    {
                        dyn_string_add_char(string,92);
                        dyn_string_add_string(string,"092");
                        ungetc(c,stdin);
                        ungetc(tmp[1],stdin);
                        ungetc(tmp[0],stdin);
                        state = STATE_STRING_READ;
                        break;
                    }
                    tmp[2] = c;
                    char new = oct_to_dec(tmp);
                    // target language specific
                    if (new == 92)
                    {
                        dyn_string_add_char(string,92);
                        dyn_string_add_string(string,"092");
                    }
                    else if (new == '#')
                    {
                        dyn_string_add_char(string,92);
                        dyn_string_add_string(string,"035");
                    }
                    // source language specific
                    else if (new == '"')
                    {
                        dyn_string_add_char(string,'"');
                    }
                    else if (new == '$')
                    {
                        dyn_string_add_char(string,'$');
                    }
                    else{
                        ungetc(new,stdin);
                    }
                    state = STATE_STRING_READ;
                    break;
                }
                // else
                dyn_string_add_char(string,92);
                dyn_string_add_string(string,"092");
                ungetc(c,stdin);
                state = STATE_STRING_READ;


                break;
            
            case STATE_NOT_EQUAL1:
                if(c=='='){
                    state = STATE_NOT_EQUAL2;
                }
                else{
                    ungetc(c,stdin);
                    state = STATE_BLANK2;
                }
                break;

            case STATE_NOT_EQUAL2:
                if(c=='='){
                    token.type = TOKEN_NOT_EQUAL;
                    dyn_string_free(string);
                    return token;
                }
                else{
                    state = STATE_BLANK2;
                }
                break;

            case STATE_EQUAL:
                if(c == '='){
                    state = STATE_EQUAL1;
                }
                else{
                    ungetc(c,stdin);
                    token.type = TOKEN_EQUAL;
                    dyn_string_free(string);
                    return token;
                }
                break;
            
            case STATE_EQUAL1:
                if(c == '='){
                    token.type = TOKEN_EQUAL2;
                    dyn_string_free(string);
                    return token;
                }
                else{
                    ungetc(c,stdin);
                    state = STATE_BLANK2;
                }
                break;

            case STATE_SMALLER:
                if (c == '='){
                    token.type = TOKEN_SMALL_EQ;
                    dyn_string_free(string);
                    return token;
                }
                else if(c == '?'){
                    token.type = TOKEN_START_TAG;
                    dyn_string_free(string);
                    return token;
                }
                else{
                    token.type = TOKEN_EQUAL;
                    ungetc(c,stdin);
                    dyn_string_free(string);
                    return token;
                }
                break;
            
            case STATE_BIGGER:
                if(c == '='){
                    token.type = TOKEN_BIGGER_EQ;
                    ungetc(c,stdin);
                    dyn_string_free(string);
                    return token;
                }
                else{
                    token.type = TOKEN_BIGGER;
                    dyn_string_free(string);
                    return token;
                }
                break;
            
            case STATE_QUEST_MARK:
                if(c == '>'){
                    token.type = TOKEN_END_TAG;
                    dyn_string_free(string);
                    return token;
                }
                else{
                    token.type = TOKEN_QUEST_MARK;
                    ungetc(c,stdin);
                    dyn_string_free(string);
                    return token;
                }
                break;
            
            case STATE_VAR_ID_START:
                if( isalpha(c)||c=='_'){
                    state = STATE_VAR_ID;
                    dyn_string_add_char(string,c);
                }
                else{
                    state = STATE_BLANK1;
                    ungetc(c,stdin);
                }
                break;
            
            case STATE_VAR_ID:
                if(isalnum(c)||c=='_'){
                    dyn_string_add_char(string,c);
                }
                else{
                    ungetc(c,stdin);
                    token.type = TOKEN_VAR_ID;
                    token.string = string;
                    return token;                    
                }
                break;
            

            //TODO
            case STATE_BLANK0:
                dyn_string_free(string);
                token.type = TOKEN_BLANK0;
                return token;
            case STATE_BLANK1:
                dyn_string_free(string);
                token.type = TOKEN_BLANK1;
                return token;
            case STATE_BLANK2:
                dyn_string_free(string);
                token.type = TOKEN_BLANK2;
                return token;
            // default = blank3
            default:
                token.type = TOKEN_BLANK3;
                return token;
        }       


    }
}



// i use free_token() on every token, in case some had dyn_string in it
token_t deal_with_prolog()
{
    int c;
    token_t token;
    token.type = TOKEN_PROLOG_FAIL;
    c = getc(stdin);

    // after start tag "<?" white space have to follow
    if(!isspace(c))
    {
        free_token(token);
        token.type = TOKEN_PROLOG_FAIL;
        return token;
    }

    // now we have to check that "declare(strict_types=1);"
    // was send to stdin, with any amount of white spaces or comments in between

    // we check if get_token() return right tokens for "declare(strict_types=1);"
    // otherwise return token leading to lexical error

    // "declare"
    token = get_token(5);
    if ( token.type == TOKEN_FUNC_ID)
    {
        if(!strcmp((const char*) token.string->string,"declare"));
        else
        {   
            free_token(token);
            token.type = TOKEN_PROLOG_FAIL;
            return token;
        }
    }
    else
    {
        free_token(token);
        token.type = TOKEN_PROLOG_FAIL;
        return token;
    }
    free_token(token);

    // '('
    token = get_token(5);
    if(token.type != TOKEN_L_PAR)
    {
        free_token(token);
        token.type = TOKEN_PROLOG_FAIL;
        return token;
    }

    // strict_types
    token = get_token(5);
    if ( token.type == TOKEN_FUNC_ID)
    {
        if(!strcmp((const char*) token.string->string,"strict_types"));
        else
        {   
            free_token(token);
            token.type = TOKEN_PROLOG_FAIL;
            return token;
        }
    }
    else
    {
        free_token(token);
        token.type = TOKEN_PROLOG_FAIL;
        return token;
    }    
    free_token(token);
    

    // '='
    token = get_token(5);
    if(token.type != TOKEN_EQUAL)
    {
        free_token(token);
        token.type = TOKEN_PROLOG_FAIL;
        return token;
    }

    // 1
    token = get_token(5);
    if ( token.type == TOKEN_INT)
    {
        if(token.int_value == 1);
        else
        {   
        free_token(token);
            token.type = TOKEN_PROLOG_FAIL;
            return token;
        }
    }
    else
    {
        free_token(token);
        token.type = TOKEN_PROLOG_FAIL;
        return token;
    }


    // ')'
    token = get_token(5);
    if(token.type != TOKEN_R_PAR)
    {
        free_token(token);
        token.type = TOKEN_PROLOG_FAIL;
        return token;
    }    

    // ')'
    token = get_token(5);
    if(token.type != TOKEN_SEMICOLON)
    {
        free_token(token);
        token.type = TOKEN_PROLOG_FAIL;
        return token;
    }    

    // everything was alrigt
    token.type = TOKEN_PROLOG;
    return token;

}



void free_token(token_t token){
    if(token.type == TOKEN_STRING || token.type == TOKEN_VAR_ID || token.type == TOKEN_FUNC_ID)
    {
        dyn_string_free(token.string);
    }
    return;
}