#include <iostream>

class Token {
public:
    char kind;                                      // what kind of token
    double value;                                   // for numbers: a value
    Token() :kind{0}, value{0.0} {}         // ADDED: default constructor
    Token(char k) :kind{k}, value{0.0} {}           // construct from one value
    Token(char k, double v) :kind{k}, value{v} {}   // construct from two values
}; // end class Token

class Token_stream {
public:
    Token get();                // get a Token
    void putback(Token t);      // put a Token back
private:
    bool full = false;          // is there a Token in the buffer?  
    Token buffer;               // putback() saves its token here
}; // end class Token_stream

void Token_stream::putback(Token t) {
    buffer = t; 
    full = true; 
}

Token Token_stream::get() {
    if (full) {             // do we already have a Token ready?
        full = false;       // remove Token from buffer
        return buffer; 
    }
    char ch = 0;
    std::cin >> ch;
    if (!std::cin) return Token{'q'}; // ADDED: treat EOF/fail as quit
    switch (ch) {
    case ';':               // for "print"
    case 'q':               // for "quit"
    case '(': case ')': case '+': case '-': case '*': case '/':
        return Token{ch};   // let each character represent itself
    case '.':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
        std::cin.putback(ch);   // put digit back into the input stream
        double val = 0; 
        std::cin >> val;        // read a floating point number
        return Token{'8',val};  // let '8' represent "a number"
    }
    default:
        std::cerr << "Bad Token\n"; 
        return 0; 
    }
}

Token_stream ts;        // provides get() and putback()
double expression();    // declaration so that primary() can call expression()

double primary() {      // deal with numbers and parentheses
    Token t = ts.get();
    switch (t.kind) {
        case '(': {      // handle '(' expression ')'
            double d = expression(); 
            t = ts.get(); 
            if (t.kind != ')')
                std::cerr << "\')\' expected\n";
            return d;
        }
        case '8':
            return t.value; 
        default:
            std::cerr << "Primary expected.\n"; 
            return 0; 
    }
}

double term() {           // deal with * and /
    double left = primary(); 
    Token t = ts.get();
    while (true) {
        switch (t.kind) {
            case '*':
                left *= primary();
                t = ts.get();
                break;
            case '/': {
                double d = primary();
                if (d == 0)
                    std::cerr << "Divide by zero!\n";
                left /= d;
                t = ts.get(); 
                break;
            }
            default:            // ADDED: default case for fall-through
                ts.putback(t); 
                return left;                 
        }
    }
}

double expression() {     // deal with + and -
    double left = term();       //read and evaluate a term
    Token t = ts.get();         // get the next Token from the Token stream
    while(true) {
        switch (t.kind) {
            case '+':
                left += term(); //evaluate term and add
                t = ts.get();
                break; 
            case '-':
                left -= term();
                t = ts.get();
                break;
            default: 
                ts.putback(t);  // put t back into the token stream
                return left;    // finally: no more + or -; return the answer
        }
    }
}

int main() {            //main loop and deal with errors
    double result = 0; 
    
    try {
        while (std::cin) {
            Token t = ts.get(); 
            if (t.kind == 'q') return 0; 
            if (t.kind == ';')
                std::cout << "= " << result << '\n'; // result from prior expression
            else {
                ts.putback(t);
                result = expression(); 
            }
        }
    }
    catch (...) {
        std::cerr << "Exception" << '\n';
        return 1;
    }
} // end main()