#include "../lib/Analizer_ACL.hpp"

using namespace std;

Analizer_ACL::Analizer_ACL(){
    myfile = new ifstream( "base.dnl" );
    map<string,string> * global_scope = new map<string,string>;
    scopes.push_back(global_scope);
    scope_iterator = scopes.begin();
    cout << "Constructor: start getting tokens" << endl;
    get_tokens();
}

Analizer_ACL::Analizer_ACL( string file_path ){
    myfile = new ifstream( file_path );
    map<string,string> * global_scope = new map<string,string>;
    scopes.push_back(global_scope);
    scope_iterator = scopes.begin();
    cout << "Constructor: start getting tokens" << endl;
    get_tokens();
}

bool Analizer_ACL::check_token_sequence( vector<string> type_sequence ){
    vector<Token *>::iterator after_state = token_iterator;
    if(equal( type_sequence.begin(), type_sequence.end(), token_iterator, [&](string first, Token * second){
        ++after_state;
        if( first.at(0) == '_'){
            first.erase(0,1);
            return second->content == first;
        }else{
            return second->type == first;
        }
    })){
        token_iterator = after_state;//Eat the tokens.
        return true;
    }else{
        return false;
    }
}

/*
 *  @brief
 *  @param Type pointer ifstream that point to myfile file.
 */
int Analizer_ACL::get_tokens(){
    
    Token * readed_token;
    string token;
    char c = myfile->peek();

    while( c == 42 ){ 

        do{
           myfile->get();
        }while( myfile->peek() != '\n' );
        c = myfile->peek();
    }

    if( myfile->is_open() ){
        do{
            readed_token = new Token();
            token="";

            c = myfile->peek();

            while( c == 42 ){ 
                do{
                myfile->get();
                }while( myfile->peek() != '\n' );
                c = myfile->peek();
            }

            if( is_letter(c) ){ 
                do{
                    token += myfile->get();
                    c = myfile->peek();
                }while( is_letter(c) || is_number(c) );

                readed_token->content = token;

                if( commands.find(token) != commands.end() ){
                    readed_token->type ="COMMAND";
                }else{
                    readed_token->type ="WORD";
                }

            }else if( is_number(c) ){ 
                do{
                    token += myfile->get();
                }while( is_number(myfile->peek()) );

                readed_token->content = token;
                readed_token->type = "NUMBER";

            }else if( is_operator(c) ){ 
                do{
                    token += myfile->get();
                }while( is_operator(myfile->peek()) );

                readed_token->content = token;
                readed_token->type = "OPERATOR";

            }else if( c == '"' ){
                do{
                    token += myfile->get();
                }while( is_allowed_in_string(myfile->peek() ) && token.size()<= 40 );
                if( myfile->peek()!= '"' ){
                    //THROW ERROR
                    cout << "sintax error!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                    token = myfile->peek();
                    readed_token->content = token;
                    readed_token->type = "ERROR";     
                    return -1;       
                }else{
                    token+=myfile->get();
                }

                readed_token->content = token;
                readed_token->type = "STRING";

            }else if( c == '[' ){
                do{
                    token += myfile->get();
                }while( is_number(myfile->peek()));
                if( myfile->peek() != ']'){
                    //THROW ERROR
                    cout << "sintax error!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                    token = myfile->peek();
                    readed_token->content = token;
                    readed_token->type = "ERROR";
                    return -2;
                }else{
                    token += myfile->get();
                }

                readed_token->content = token;
                readed_token->type = "INDEX";

            }else if( c == '\n'  ){
                do{
                    token += myfile->get();
                }while( myfile->peek() == '\n' );

                readed_token->content = token;
                readed_token->type ="NEW LINE";

            }else if( c == EOF){
                readed_token->content = "";        
                readed_token->type ="END";

            }else if( c == ' ' || c == '\t' ){
                do{
                    token += myfile->get();
                    c = myfile->peek();
                }while( c==' ' || c == '\t' );

                readed_token->content = token;
                readed_token->type ="WHITE SPACE";

            }else{
                cout<< "None of the above!!     :  " << (int)c ;

                token = c;
                readed_token->content = token;
                readed_token->type ="ERROR";

            }   
            cout << "size of token vector is: " << token_vector.size() <<  endl;
            token_vector.push_back(readed_token);
            cout << "size of token vector is: " << token_vector.size() <<  endl;
        }while( token!="" );
    }else{
        cout << "The file is not open." << endl;
    }    
    cout << "The number of tokens readed were " << token_vector.size() << endl;
    return 1; 
}

void Analizer_ACL::lexer(){
    int line_count =0;
    token_iterator=token_vector.begin();

    int iterador = 0;

    do{
        cout << "\n\nPosicaoao do iterador no comeco da analise  "<<std::distance(token_vector.begin(), token_iterator) << endl;
        cout<< "Start decoding token:    "<< (**token_iterator).content << " ------- " << (**token_iterator).type <<endl;
        if ( check_token_sequence({"WHITE SPACE"}) ){

        }else if( check_token_sequence({"NEW LINE"}) ){

            ++line_count;

        }else if( check_token_sequence({"_dimg","WHITE SPACE","WORD","INDEX", "NEW LINE"})){
            string map_key, index;
            int number_of_indexes;

            ++line_count;

            map_key = "var_" + (*(token_iterator-3))->content +"[";
            index = (*(token_iterator-2))->content;
            index.erase(0,1); // remove '[' from index (first char)
            index.pop_back(); // remove ']' from index (last char)
            number_of_indexes = atoi(index.c_str());

            for(int i =0; i <= number_of_indexes; ++i ){
                cout << "\nDeclaring global variable named   " << map_key + to_string(i) + "]" << endl;
                (**scopes.begin())[ map_key + to_string(i) + "]" ] = "";
            }

        // }else if( check_token_sequence({"_if", "WHITE SPACE", "WORD", "INDEX", "WHITE SPACE", "_=", "WHITE SPACE", "WORD", "INDEX", "NEW LINE" })){
        }else if( check_token_sequence({"_if", "WHITE SPACE"})){
            string first_key, first_value, second_key, second_value, used_operator;
            bool condition;

            if( check_token_sequence({"WORD","INDEX","WHITE SPACE"}) ){
                first_key = "var_" + (*(token_iterator-3))->content + (*(token_iterator-2))->content;
            }else if( check_token_sequence({"WORD", "WHITE SPACE"}) ){
                first_key = "var_" + (*(token_iterator-2))->content;
            }else{
                cout << "\n!Expecting variable. Sintax error." << line_count << endl;
            }
            cout << "First key is  " << first_key << endl;
            first_value = (**scope_iterator)[first_key];
            
            if( check_token_sequence({"OPERATOR"}) ){
                used_operator = (*(token_iterator-1))->content;
            }else{
                cout << "\n!!Expecting operator. Sintax error." << line_count << endl;
            }

            if( check_token_sequence({ "WHITE SPACE", "WORD", "INDEX", "NEW LINE" }) ){
                second_key = "var_" + (*(token_iterator-3))->content + (*(token_iterator-2))->content;

            }else if( check_token_sequence({ "WHITE SPACE", "WORD", "NEW LINE" }) ){
                second_key = "var_" + (*(token_iterator-2))->content;

            }else{
                cout << "\n!Expecting variable. Sintax error." << line_count << endl;
            }
            second_value = (**scope_iterator)[second_key];

            ++line_count;

            //first_key = "var_" + (*(token_iterator-8))->content + (*(token_iterator-7))->content;
            //second_key = "var_" + (*(token_iterator-3))->content + (*(token_iterator-2))->content;

            cout << "The first value is " << first_value << "  the second value is " << second_value << endl;
            if( used_operator == "=" ){
                condition = first_value == second_value;
            }else if( used_operator == "<"){
                condition = atoi(first_value.c_str()) < atoi(second_value.c_str()); 
            }else if( used_operator == ">"){
                condition = atoi(first_value.c_str()) > atoi(second_value.c_str());
            }

            cout << "\nif condition is " << condition << endl;
            if( condition ){
                //Enter if scope.
                scopes.push_back( new map<string,string> );
                ++scope_iterator;
            }else{
                //If case the condition wasent met, skip the if block.
                do{
                    ++token_iterator;
                    if((*token_iterator)->type == "END"){
                        cout << "coudn`t find the end of if block\n";
                        break;
                    }
                }while( (*token_iterator)->content != "else" && (*token_iterator)->content != "endif" );
                ++token_iterator;//The token skiped was a else to run instead of an if, or it was the endif in case the if didn`t had an else.
            }
                
        }else if( check_token_sequence({"_else"}) ){//If is reading else at this point is because it was inside an if, so the else block must be skipped.
            do{
                ++token_iterator;
                if((*token_iterator)->type == "END"){
                    cout << "coudn`t find the end of if block\n";
                    break;
                }
            }while( (*token_iterator)->content != "endif");
            ++token_iterator;//Skip the token that sinalizes the end of the block;

        }else if( check_token_sequence({ "_println", "WHITE SPACE", "STRING", "NEW LINE" })){
            ++line_count;

            cout << (*(token_iterator-2))->content;

        }else if( check_token_sequence({ "_endif" }) ){    
            //If it hit an endif here it is geting out of a scope, then pop the scope
            --scope_iterator;
            scopes.pop_back();

        }else if( check_token_sequence({ "_set", "WHITE SPACE", "WORD" }) ){
            string key, value;

            if( check_token_sequence({ "INDEX", "WHITE SPACE", "_=", "WHITE SPACE", "NUMBER" }) ){
                key = "var_" + (*(token_iterator-6))->content + (*(token_iterator-5))->content;
                value = (*(token_iterator-1))->content;
            }else if( check_token_sequence({ "WHITE SPACE", "_=", "WHITE SPACE", "NUMBER" }) ){
                key = "var_" + (*(token_iterator-5))->content;
                value = (*(token_iterator-1))->content;
            }

            (**scope_iterator)[key]= value;
            cout << "new value is " << key << " = " << (**scope_iterator)[key] << endl;

        }else if((*token_iterator)->type == "END"){
            break;
        }
        ++iterador;

    }while( iterador < 20);

}