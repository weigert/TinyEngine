#ifndef TINYENGINE_PARSE
#define TINYENGINE_PARSE

#include <set>
#include <map>
#include <functional>
#include <sstream>
#include <tuple>

namespace parse {
using namespace std;

/*
===================================================
            Generalized Stream Object
===================================================
*/

template<typename T>
struct stream {

    size_t pos = 0;
    vector<T> buf;

    inline size_t size(){ 
        return buf.size(); 
    }

    inline bool eof(){ 
        return (pos >= size()-1); 
    }

    inline void reset(){
        pos = 0;
    }

    virtual inline void inc(){
        this->pos++;
    }

    // Error Function

    void err(string msg){
        printf("(%d) Stream Error: %s", pos, msg.c_str());
    }

    // Increment Operators
    
    stream<T>& operator++(){        // Prefix Increment
        inc();
        return *this;
    }

    stream<T> operator++(int){      // Postfix Increment
        stream<T> old = *this; 
        operator++();
        return old;
    }

    stream<T>& operator+(int n){
        while(--n >= 0) operator++();
        return *this;
    }

    stream<T>& operator+=(int n){
        operator+(n);
        return *this;
    }

    // Indexing Operators

    T operator[](size_t N){
        return this->buf[this->pos+N];
    }

    T operator()(size_t N){
        T val = operator[](N);
        operator+=(N+1);
        return val;
    }

    T operator()(){
        return operator()(0);
    }

};

// Output Operator

template<typename T>
ostream& operator<<(ostream& os, stream<T>& s){
    if(s.size() > 0)
    for(auto& e: s.buf)
        os << e;
    return os; 
}

/*
===================================================
            Specialized Character Stream
===================================================
*/

struct cstream : stream<char> {

    size_t line = 1;
    size_t col = 0;

    cstream(const char* filename){
        load(filename);
    }

    bool load(const char* filename){

        ifstream f(filename);
        if(!f.is_open())
            return false;

        string line;
        while(getline(f, line)){
            for(auto& c: line)
                buf.push_back(c);
            buf.push_back('\n');
        }

        f.close();
        return true;

    }

    void err(string msg){
        printf("(%d, %d) Stream Error: %s", line, col, msg.c_str());
    }

    inline void inc(){
        if((*this)[0] == '\n'){
            line++;
            col=0;
        }
        this->pos++;
    }

};

/*
===================================================
            Specialized Token Stream
===================================================

Currently, my token is basically just a glorified pair.
I could easily replace it with the standard pair structure.

I could also add methods to it which allow me to easily append
stuff to it...

The methods which I use to construct different types of tokens
are static though, which I don't like.
But it allows me to implement two methods in the same "namespace",
which is convenient for the check and read methods.

Of course, tokens have different content and length and are just
some base unit.

I could have smaller tokens and combine them to bigger token types.
This is ultimately what a parser does.

Of course if I had some "iterable structure" of tokens, then reading
all of the tokens would be much more efficient.

But I would have to have pattern matching somehow...
Also I have tokens inside tokens...

There is actually a ton of ways this could be implemented
Which is what makes this so difficult!
Which is the most compact and flexible?

Idea:
    Get the next element of the string stream,
    try to add it to a token.

    If the token fits the "read" function, add to the token and return
    itself.

    If the token doesn't, it we create a new token.

    Then we keep doing this with the last token element continually.

    The tokenstream itself is what has the function which keeps adding
    the next token.

I could do a recursive approach,
where I parse base character types and then try to find nested solutions
directly.

I could also do a simple approach which only looks at
characters and bracket types for instance...

Basically a full parse tree approach where every single character is
a separate type of token.

Then I just write the rules which make the parse tree with different node types.
Of course every MAJOR node type can have a set of sub-node types.

so it would be a mixture of both.

I would write rules which simultaneously do basic concatenation
but also a form of "now we are in the sub-tree"

AAAAAAAA

{A,A,A}

we start with the bracket, notice an element A,
which then 

We can write a simple lexer using regular expressions:
    While a regular expression matches the current string + the next character...
    This is what I should do!

Then I use an LR parser to generate my tree.

First of all I need a better lexer.
Forget the parsing / abstract syntax tree generation for now.
I want it to be abstract enough that I can 

We can use bit operations if we make binary assignments... 
*/

namespace token {

    // Binary Token Base Type
    enum type {
        NONE        = 0b0,
        WHITESPACE  = 0b100,
        LINEBREAK   = 0b1000,
        CHARACTER   = 0b1,
        NUMBER      = 0b10,
        PERIOD      = 0b10000,
        COMMA       = 0b0,
        COLON       = 0b100000,
        DASH        = 0b1000000,
        PIPE        = 0b0,
        HASH        = 0b0,
        QUOTE       = 0b0,
        LCURLY      = 0b0,
        RCURLY      = 0b0,
        LSQUARE     = 0b0,
        RSQUARE     = 0b0,
        MISC        = 0b0,
        //...
    };

    const map<type, string> base_match = {
        {NONE, ""},
        {WHITESPACE, " \t"},
        {LINEBREAK, "\n"},
        {CHARACTER, "ÄÖÜABCDEFGHIJKLMNOPQRSTUVWXYZäöüabcdefghijklmnopqrstuvwxyz_"},
        {NUMBER, "0123456789"},
        {PERIOD, "."},
        {COMMA, ","}
        {COLON, ":"},
        {DASH, "-"},
        {PIPE, "|"},
        {HASH, "#"},
        {LCURLY, "{"},
        {RCURLY, "}"},
        {LSQUARE, "["},
        {RSQUARE, "]"},
        {QUOTE, "\""},
        {MISC, "+*/&%$§!^()=?;<>"},
    };

    /*
        I wont make it hyper efficient,
        I will just make it pass multiple times.
        One time to get the base characters,
        Another time to extract the next possible level of tokens
        Then the next possible level of tokens
        and so on.

        Create identifiers, numbers,
        Then create lists from numbers,
        and when I get a sequence which I can't reduce to a new node
        then it fails!
        like an unexpected sequence.

        Like [aa, 1.2, bb]
        would be reduced to string, number, string,
        then bracket, list, bracket
        then bracketed list?

        But that's just what regular expressions do...?
        I don't know what to do.
    */

    // Simple Token, Which Contains the Type and Char
    typedef pair<type, char> base;

    /*
        I can create a simple token matcher,
        which returns a pair with a type and
        the generated char using a map.
    */



    base match(char c){
        return make_pair(NONE, c);
    }

    /*
        I can generate a stream of tokens using
        the token stream, which simply matches the tokens.
    */

    /*
        Finally, I can use the binary matching rules of the token types
        to decide whether or not tokens should be concatenated or not,
        and to what type of token.
    */

};



const string alpha = "abcdefghijklmnopqrstuvwxyz_";
const string ALPHA = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_";
const string numeric = "0123456789";
const string OPERATOR = ":-|,";
const string whitespace = " \t\n";

struct Token {
    string type = "NONE";
    string value = "";
    Token(){}
    Token(string _type, string _value){
        type = _type; value = _value;
    }
};

ostream& operator<<(ostream& os, Token& t){
    return os << t.value;
}

// Token Implementations

struct TokenComment {
    static bool check(char c){
        if(string("#").find(c) != string::npos) return true;
        return false;
    }
    static Token read(cstream& cs){
        stringstream s;
        s << cs();
        while(cs[0] != '\n')
            s << cs();
        return Token("COMMENT", s.str());
    }
};

struct TokenIdentifier {
    static bool check(char c){
        if(alpha.find(c) != string::npos) return true;
        if(ALPHA.find(c) != string::npos) return true;
        return false;
    }
    static Token read(cstream& cs){
        stringstream s;
        s << cs();
        while(check(cs[0]) || numeric.find(cs[0]) != string::npos)
            s << cs();
        return Token("IDENTIFIER", s.str());
    }
};

struct TokenOperator {
    static bool check(char c){
        if(OPERATOR.find(c) != string::npos) return true;
        return false;
    }
    static Token read(cstream& cs){
        stringstream s;
        s << cs();
        while(check(cs[0]))
            s << cs();
        return Token("OPERATOR", s.str());
    }
};

struct TokenNumber {
    static bool check(char c){
        if(numeric.find(c) != string::npos) return true;
        return false;
    }
    static Token read(cstream& cs){
        stringstream s;
        s << cs();
        bool founddot = false;
        while(check(cs[0]) || cs[0] == '.'){
            if(cs[0] == '.'){
                if(founddot) break;
                founddot = true;
            }
            s << cs();
        }
        return Token("NUMBER", s.str());
    }
};


struct TokenWhitespace {
    static bool check(char c){
        if(string(" \t\n").find(c) != string::npos) return true;
        return false;
    }
    static Token read(cstream& cs){
        stringstream s;
        s << cs();
        while(check(cs[0]))
            s << cs();
        return Token("WHITESPACE", s.str());
    }
};


struct TokenString {
    static bool check(char c){
        return c == '"';
    }
    static Token read(cstream& cs){
        stringstream s;
        s << cs();
        bool escaped = false;
        while (!cs.eof()) {
            char c = cs();
            if (escaped) {
                s << c;
                escaped = false;
            } else if (c == '\\') {
                escaped = true;
            } else if (c == '\"') {
                s << c;
                break;
            } else {
                s << c;
            }
        }
        return Token("STRING", s.str());
    }
};

struct tstream: stream<Token> {

    cstream* input = NULL;

    tstream(cstream& l){
        input = &l;
        while(!input->eof())
            buf.push_back(readtoken());
        input->reset();
    }

    Token readtoken(){

        if(TokenOperator::check((*input)[0])) 
            return TokenOperator::read(*input);

        else if(TokenComment::check((*input)[0])) 
            return TokenComment::read(*input);

        else if(TokenWhitespace::check((*input)[0])) 
            return TokenWhitespace::read(*input);

        else if(TokenIdentifier::check((*input)[0])) 
            return TokenIdentifier::read(*input);

        else if(TokenString::check((*input)[0])) 
            return TokenString::read(*input);

        else if(TokenNumber::check((*input)[0])) 
            return TokenNumber::read(*input);

        stringstream s;
        s << (*input)();
        err("Token Not Recognized: "+s.str()+"\n");
        return Token("UNKNOWN", s.str());

    }

    void err(string msg){
    //    printf("Token Stream Error: %s\n", msg.c_str());
        input->err(msg);
    }

};

/*
    Currently, this basically has very simple rules for contructing compound tokens from characters.
    For instance, a comment is given by a # followed by whatever until linebreak.
    So a comment is ended by linebreak.
    If my state is comment, then I only return to state null IF I encounter a token linebreak

    and so on and so forth
*/

/*

namespace node {

enum type {
    NONE,
    VALUE,
    PAIR,
    ARRAY,
    SET
};

struct node {
    const type t = NONE;
    string val;
    node(){}
};

struct node_val : node {
    const type t = VALUE;
    string val;
};

struct node_pair : node {
    const type t = PAIR;
    node* key;
    node* val;
    node_pair(){}
    ~node_pair(){
        delete key;
        delete val;
    }
};

struct node_arr : node {
    const type t = ARRAY;
    vector<node*> s;
    ~node_arr(){
        for(auto& _s: s)
            delete _s;
    }
};

struct node_set : node {
    const type t = SET;
    set<node*> s;
    ~node_set(){
        for(auto& _s: s)
            delete _s;
    }
};

// Parsing Function

bool try_parse_comment(tstream& ts){

    int N = 0;
    if(ts[N]->t != COMMENT) 
        return false;
    N++;

    while(ts[N]->t != LINEBREAK)
        N++; 

    ts += N;
    return true;

}

node_pair* try_parse_pair(tstream& ts){

    int N = 0;
    token key;
    token val;

    if(ts[N]->t != IDENTIFIER) 
        return NULL;
    key = ts[N++];

    while(ts[N]->t == WHITESPACE)
        N++;

    if(ts[N]->t != COLON) 
        return NULL;
    N++;

    while(ts[N]->t == WHITESPACE)
        N++;

    if(ts[N]->t != IDENTIFIER)
        return NULL;
    val = ts[N++];

    // Successfully Found Pair

    ts += N;
    N = 0;

    // Remove Whitespace / Comment

    while(ts[N]->t == WHITESPACE)
        N++;

    try_parse_comment(ts);

    // Remove Trailing Whitespace, Comments
    // Check for duplicate tokens?

    // If I have whitespace, go until whitespace.
    // If I have a linebreak, we are done.
    // If I have a 

    node_pair* n = new node_pair();
    
    n->key = new node_val();
    n->key->val = key->value;

    n->val = new node_val();
    n->val->val = val->value;

    ts += N;
    return n;

}

*/
    /*

node* parse(tstream& ts){


    node_set* root = new node_set();

    while(!ts.eof()){

        cout<<"AYY"<<endl;

        node* pair = try_parse_pair(ts);
        if(pair != NULL){
            root->s.insert(pair);
            continue;
        }

        ts++;
        
    }

    return root;


    return NULL;

}

    */
};


#endif