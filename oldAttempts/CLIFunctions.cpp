#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <list>

using namespace std;

struct formattedEq {
    vector<string> listFormEq; 
    vector<vector<int>> operator_numeric_indices_matrix = vector<vector<int>>(5);
    // 0 | ^
    // 1 | *
    // 2 | /
    // 3 | +
    // 4 | -
};

// FIXES
// restructure the parenthesis solver
// error handling


//  FUNCTIONS

int factorial(const int& numIn) {
    int result = 1;
    int i = 1;

    while(i<numIn+1) {
        result = result*i;
        ++i;
    }
    return result;
}

//  LEGALITY CHECKERS

bool illegalSymbolCheck(const string& s) {
    const string symbolsAllowedIn = "0123456789()-+/*^!";
    return s.find_first_not_of(symbolsAllowedIn) == string::npos;
}


formattedEq eqFormatter(string& eqIn){
    formattedEq currentFormattedEq {};
    int n = 0;
    string curNum = "";


    for (char c : eqIn){
        if(curNum == "" && isdigit(c)){curNum += c;}
        else if(curNum != "" && isdigit(c)){curNum += c;}
        else if(curNum != "" && !isdigit(c)){
            currentFormattedEq.listFormEq.push_back(curNum);
            curNum = "";
        }

        if (c == '('){
            currentFormattedEq.listFormEq.push_back("(");
            n++;
            }
        if (c == ')'){
            currentFormattedEq.listFormEq.push_back(")");
            n++;
            }
        if (c == '^'){
            currentFormattedEq.listFormEq.push_back("^");
            currentFormattedEq.operator_numeric_indices_matrix[0].push_back(n);
            n++;
            }        
        if (c == '*'){
            currentFormattedEq.operator_numeric_indices_matrix[1].push_back(n);
            currentFormattedEq.listFormEq.push_back("*");
            n++;
            }
        if (c == '/'){
            currentFormattedEq.operator_numeric_indices_matrix[2].push_back(n);
            currentFormattedEq.listFormEq.push_back("/");
            n++;
            }
        if (c == '+'){
            currentFormattedEq.operator_numeric_indices_matrix[3].push_back(n);
            currentFormattedEq.listFormEq.push_back("+");
            n++;
            }        
        if (c == '-'){
            currentFormattedEq.operator_numeric_indices_matrix[4].push_back(n);
            currentFormattedEq.listFormEq.push_back("-");
            n++;
        }
    }

    return currentFormattedEq;
}


formattedEq parenthesisParse(formattedEq eq) {
    formattedEq result {};

    // cout << "starting" << "\n";

    int n = eq.listFormEq.size();
    // for(string z : eq.listFormEq){cout << z; }
    // cout << "\n Length: " << n << "\n" ;
    for(vector<int> vec : eq.operator_numeric_indices_matrix){
        for(int bb : vec){
            cout << bb << " ";
        }
        cout << "\n";
    }


    for(int i = n-1; i >= 0; i--) {
        cout << "char parsed" << " : " << i << " : " << eq.listFormEq[i] <<"\n";
        if(eq.listFormEq[i] == "("){
            for(int j = i; j <= n; j++){
                if(eq.listFormEq[j] == ")"){
                    // Make smaller, formatted eq
                    vector<string> subListForm(eq.listFormEq.begin()+i+1, eq.listFormEq.begin()+j);
                    string whyTheFuckNot = "";
                    for(string s : subListForm){
                        whyTheFuckNot += s;
                    }
                    double parenthesisPiece = parenthesisLevelParse(eqFormatter(whyTheFuckNot));
                    eq.listFormEq[i] = parenthesisPiece;
                    // Now we need to condense the equation
                    eq.listFormEq.erase(eq.listFormEq.begin()+i+1, eq.listFormEq.begin()+j);
                    // double replacement_value = parenthesisLevelParse(tempEq);
                    break;
                }
            }
        }
    }


    return result;
}

double parenthesisLevelParse(formattedEq eq) {
    double result {};
    for(int j : eq.operator_numeric_indices_matrix[0]) {
        result = pow(stol(eq.listFormEq[j-1]),stol(eq.listFormEq[j+1]));
        eq.listFormEq[j-1] = to_string(result);
        eq.listFormEq.erase(eq.listFormEq.begin() + j+1);
        eq.listFormEq.erase(eq.listFormEq.begin() + j);
        for(vector<int> vec : eq.operator_numeric_indices_matrix) { 
            for(int l : vec){if(l>j){l =l-2;} }
        }
    }
    for(int j : eq.operator_numeric_indices_matrix[1]) {
        result = stol(eq.listFormEq[j-1])*stol(eq.listFormEq[j+1]);
        eq.listFormEq[j-1] = to_string(result);
        eq.listFormEq.erase(eq.listFormEq.begin() + j+1);
        eq.listFormEq.erase(eq.listFormEq.begin() + j);
        for(vector<int> vec : eq.operator_numeric_indices_matrix) { 
            for(int l : vec){if(l>j){l =l-2;} }
        }
    }
    for(int j : eq.operator_numeric_indices_matrix[2]) {
        result = stol(eq.listFormEq[j-1])/stol(eq.listFormEq[j+1]);
        eq.listFormEq[j-1] = to_string(result);
        eq.listFormEq.erase(eq.listFormEq.begin() + j+1);
        eq.listFormEq.erase(eq.listFormEq.begin() + j);
        for(vector<int> vec : eq.operator_numeric_indices_matrix) { 
            for(int l : vec){if(l>j){l =l-2;} }
        }
    }
    for(int j : eq.operator_numeric_indices_matrix[3]) {
        result = stol(eq.listFormEq[j-1])+stol(eq.listFormEq[j+1]);
        eq.listFormEq[j-1] = to_string(result);
        eq.listFormEq.erase(eq.listFormEq.begin() + j+1);
        eq.listFormEq.erase(eq.listFormEq.begin() + j);
        for(vector<int> vec : eq.operator_numeric_indices_matrix) { 
            for(int l : vec){if(l>j){l =l-2;} }
        }
    }
    for(int j : eq.operator_numeric_indices_matrix[4]) {
        result = stol(eq.listFormEq[j-1])-stol(eq.listFormEq[j+1]);
        eq.listFormEq[j-1] = to_string(result);
        eq.listFormEq.erase(eq.listFormEq.begin() + j+1);
        eq.listFormEq.erase(eq.listFormEq.begin() + j);
        for(vector<int> vec : eq.operator_numeric_indices_matrix) { 
            for(int l : vec){if(l>j){l =l-2;} }
        }
    }
    return result;
}

int main(){
    int status = 2;
    do {
        if (status == 2){
            string input_eq {};
            cout << "Enter Equation: \n";
            cin >> input_eq;
            if(input_eq == "exit" || input_eq == "Exit" || input_eq == "exit()") {
                status = 1;
            }
            else{
                formattedEq eqNow = eqFormatter(input_eq);
                // cout << parenthesisLevelParse(eqNow) << "\n";
                parenthesisParse(eqNow);
                status = 0; 
            }
            }
        else if (status == 0){
            string input_eq {};
            cin >> input_eq;
            if(input_eq == "exit" || input_eq == "Exit" || input_eq == "exit()") {
                status = 1;
            }
            else{
                formattedEq eqNow = eqFormatter(input_eq);
                // cout << parenthesisLevelParse(eqNow) << "\n"; 
                parenthesisParse(eqNow);
                status = 0; 
            }
            }
    } while ( status != 1 );

    cout << "Closing calcCLI";

    return 0;
}