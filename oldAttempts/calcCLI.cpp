#include <iostream>
#include <cmath>
#include <math.h>
#include <string>

using namespace std;

void bootInfo() {
cout
<< "      ___           ___                         ___                    ___                                \n"
<< "     /  /\\         /  /\\                       /  /\\                  /  /\\                      ___      \n"
<< "    /  /:/        /  /::\\                     /  /:/                 /  /:/                     /  /\\     \n"
<< "   /  /:/        /  /:/\\:\\    ___     ___    /  /:/                 /  /:/       ___     ___   /  /:/     \n"
<< "  /  /:/  ___   /  /:/~/::\\  /__/\\   /  /\\  /  /:/  ___            /  /:/  ___  /__/\\   /  /\\ /__/::\\     \n"
<< " /__/:/  /  /\\ /__/:/ /:/\\:\\ \\  \\:\\ /  /:/ /__/:/  /  /\\          /__/:/  /  /\\ \\  \\:\\ /  /:/ \\__\\/\\:\\__  \n"
<< " \\  \\:\\ /  /:/ \\  \\:\\/:/__\\/  \\  \\:\\  /:/  \\  \\:\\ /  /:/          \\  \\:\\ /  /:/  \\  \\:\\  /:/     \\  \\:\\/\\ \n"
<< "  \\  \\:\\  /:/   \\  \\::/        \\  \\:\\/:/    \\  \\:\\  /:/            \\  \\:\\  /:/    \\  \\:\\/:/       \\__\\::/ \n"
<< "   \\  \\:\\/:/     \\  \\:\\         \\  \\::/      \\  \\:\\/:/              \\  \\:\\/:/      \\  \\::/        /__/:/  \n"
<< "    \\  \\::/       \\  \\:\\         \\__\\/        \\  \\::/                \\  \\::/        \\__\\/         \\__\\/   \n"
<< "     \\__\\/         \\__\\/                       \\__\\/                  \\__\\/                               \n"
<< "A light weight command line interface scientific calculator - calc is short for calculator - version 0.0.1 \n"
<< "Type \"exit \" to quit \n";
}

bool validInputCheck(const string& s) {
    const string symbolsAllowedIn = "0123456789()[]-+/*^!";
    return s.find_first_not_of(symbolsAllowedIn) == string::npos;
    // npos is a special constant meaning 'not found'
}
bool containsParenthesis(const string& s) {
    const string exponentAndFactorial = "()[]";
    return s.find_first_not_of(exponentAndFactorial) == string::npos;
}

bool containsExponentOrFactorial(const string& s) {
    const string exponentAndFactorial = "^!";
    return s.find_first_not_of(exponentAndFactorial) == string::npos;
}

bool containsAimlessOperators(const string& s) {
    const string operators = "-+/*^!";
    return operators.find(s.front()) == string::npos || operators.find(s.back()) == string::npos;
}

// bool containsMultipleOperators(const string& s) {

// }

long double compute(const string& eq){

    size_t functionParsePos = 1;
    string currentOperator = "";
    string firstSlot = "";
    string secondSlot = "";

    // Find the first number
    do {
        if ( isdigit(eq.at(functionParsePos)) ) {
            firstSlot += eq.at(functionParsePos);
            ++functionParsePos;
        }
        else {
            break;
        }   
    } while ( isdigit(eq.at(functionParsePos)) );

        // TODO: if it parses too far it spits an error
        if ( !isdigit(eq.at(functionParsePos)) ){
            currentOperator += eq.at(functionParsePos);
            ++functionParsePos;
        }
        else {
            cout << currentOperator;
            return 0;
        }

    do {
        if ( isdigit(eq.at(functionParsePos)) && functionParsePos != eq.size()-1 ) {
            cout << functionParsePos << " versus " << eq.size() << "\n";
            secondSlot += eq.at(functionParsePos);
            ++functionParsePos;
            }
        else {
            break;
            }
        } while ( isdigit(eq.at(functionParsePos)) ); // Problem Line

        // Division
        if ( currentOperator == "/" ) {
            return stold(firstSlot) / stold(secondSlot);
        }

        // Multiplication
        else if ( currentOperator == "*" ) {
            return stold(firstSlot) * stold(secondSlot);
        }

        // Subtraction
        else if ( currentOperator == "-" ) {
            return stold(firstSlot) - stold(secondSlot);
        }

        // Addition
        else if ( currentOperator == "+" ) {
            // cout << "1st" << firstSlot << "2nd" << secondSlot;
            return stold(firstSlot) + stold(secondSlot);
        }

        else {
            cout << "No operator found";
            return 0;
        }
}

long double longCompute(const string& eq){
    long double output {};

    string example1 = "(12+45)^13";
    string example2 = "8!";

    return output;
}

int equationInput() {
    string equationIn; 
    
    cout << "\n";
    cin >> equationIn;

    if (equationIn == "exit" || equationIn == "Exit") {
        return 1;
    }

    else if ( !validInputCheck(equationIn) ) {
        cout << "Invalid Argument: illegal symbol";
        return 0;
    }

    // Aimless Operator Error
    else if ( !containsAimlessOperators(equationIn) ) {
        cout << "Invalid Argument: detatched operator";
        return 0;
    }

    else {
        if ( !containsExponentOrFactorial(equationIn) ) {
            cout << compute(equationIn);
        }
        else {
            cout << longCompute(equationIn);
        }
        return 0;
    };
}

int main() {

    bootInfo();

    int status; // Storing if user typed exit

    do {
        status = equationInput();
    } while ( status != 1 );

    return 0;
}