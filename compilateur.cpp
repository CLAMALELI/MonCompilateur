// A compiler from a very simple Pascal-like structured language LL(k)
// to 64-bit 80x86 Assembly langage
// Copyright (C) 2019 Pierre Jourlin
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
// Build with "make compilateur" 


#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

char current = EOF;             
char nextcar;               

void ReadChar(void){        
    if(current == EOF)
        cin.get(nextcar);
    do{
        current = nextcar;
        cin.get(nextcar);
    }while(current!= EOF &&(current==' '||current=='\t'||current=='\n'||current=='\r'));
}

void Error(string s){
    cerr << s << "  , current : "<< current << endl;
    exit(-1);
}

void Expression(void);
void Term(void);
void Factor(void);
void SimpleExpression(void);

// AdditiveOperator := "+" | "-" | "||" 
void AdditiveOperator(void){
    if(current=='+'||current=='-'){
        ReadChar();
    } else if (current == '|' && nextcar == '|') {
        ReadChar();
        ReadChar();
    } else {
        Error("Opérateur additif attendu");
    }
}

// Digit := "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"     
void Digit(void){
    if((current < '0') || (current > '9'))
        Error("Chiffre attendu");
    else {
        ReadChar();
    }
}

// Number := Digit{Digit}
void Number(void) {
    int val = 0;
    if (current >= '0' || current <= '9'){
		val = (current - '0');
		ReadChar();
		while (current >= '0' && current <= '9') {
        	val = val * 10 + (current - '0');
        	ReadChar();
    	}
	}else{
		Error("Chiffre attendu1");
	}
    cout << "\tpushq $" << val << endl; 
}

// MultiplicativeOperator := "*" | "/" | "%" | "&&"
void MultiplicativeOperator(void){
    if(current=='*'||current=='/' ||current=='%'){
        ReadChar();
    } else if (current == '&' && nextcar == '&') {
        ReadChar();
        ReadChar();
    } else {
        Error("Opérateur multiplicatif attendu");
    }
}

// Letter := "A"|...|"Z" || "a"|...|"z"
void Letter(void){
    if ((current >= 'A' && current <= 'Z') || (current >= 'a' && current <= 'z')){
        cout << "\tpushq $" << current << endl;
		ReadChar();
    } else {
        Error("Lettre attendueLe");
    }
}

void ArithmeticExpression(void){
	char adop;
	Term();
	while(current=='+'||current=='-'){
		adop=current;		// Save operator in local variable
		AdditiveOperator();
		Term();
		cout << "\tpop %rbx"<<endl;	// get first operand
		cout << "\tpop %rax"<<endl;	// get second operand
		if(adop=='+')
			cout << "\taddq	%rbx, %rax"<<endl;	// add both operands
		else
			cout << "\tsubq	%rbx, %rax"<<endl;	// substract both operands
		cout << "\tpush %rax"<<endl;			// store result
	}

}


// <RelationalOperator> ::= '<' | '>' | '<=' | '>=' | '==' | '!='
string RelationalOperator(void){
    if(current!='<'&&current!='>'&&current!='!'&&current!='='){
		return NULL;
	}
	if(nextcar=='='){
		switch(current){
			case '=':
				ReadChar(); ReadChar(); 
				return "equ";
			case '!': 
				ReadChar(); ReadChar(); 
				return "diff";
			case '<': 
				ReadChar(); ReadChar(); 
				return "infe";
			case '>': 
				ReadChar(); ReadChar(); 
				return "supe";
		}
	}
	switch(current){
		case '<':	ReadChar();
				return "inf";
		case '>':	ReadChar();
				return "sup";
		default:	Error("opérateur relationnel inconnu");
		
	}
	return NULL;
}

// Factor := Number | Letter | "(" Expression ")" | "!" Factor
void Factor(void){
    if (current >= '0' && current <= '9'){
        Number();
    }
    else if ((current >= 'A' && current <= 'Z') || (current >= 'a' && current <= 'z')){
        Letter();
    }
    else if (current == '('){
        ReadChar();
        Expression();
        if (current == ')') ReadChar();
        else Error("')' attendue");
    }
    else if (current == '!'){
        ReadChar();
        Factor();
    }
    else {
        Error("Facteur attendu");
    }
}

// Term := Factor {MultiplicativeOperator Factor}
void Term(void){
	char ope;
    Factor();
    while(current=='*'||current=='/' || current=='%' || (current == '&' && nextcar == '&')){
		ope = current;
        MultiplicativeOperator();
        Factor();
		cout << "\tpop %rbx"<<endl;
		cout << "\tpop %rax"<<endl;

		switch(ope){
			case '*':
				cout << "\timulq %rbx, %rax" << endl;
                cout << "\tpushq %rax" << endl;
                break;
			case '&':
				cout << "\tmulq	%rbx"<<endl;
				cout << "\tpush %rax"<<endl;
				break;
			case '/':
				cout << "\tmovq $0, %rdx"<<endl;  
				cout << "\tdiv %rbx"<<endl;	
				cout << "\tpush %rax"<<endl;
				break;
			case '%':
				cout << "\tmovq $0, %rdx"<<endl;
				cout << "\tdiv %rbx"<<endl;
				cout << "\tpush %rdx"<<endl;
				break;
			default:
				Error("opérateur additif attendu");
		}

    }
}

// SimpleExpression := Term {AdditiveOperator Term}
void SimpleExpression(void){
	char Sexpr;
    Term();
    while(current=='+'||current=='-' || (current == '|' && nextcar == '|')){
		Sexpr=current;
        AdditiveOperator();
        Term();

		cout << "\tpop %rbx"<<endl;
		cout << "\tpop %rax"<<endl;
		if(Sexpr=='+')
			cout << "\taddq	%rbx, %rax"<<endl;
		else
			cout << "\tsubq	%rbx, %rax"<<endl;
		cout << "\tpush %rax"<<endl;	

    }
}

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
void Expression(void){
	char expr2;
    SimpleExpression();
    if(current == '<' || current == '>' || current == '=' || (current == '!' && nextcar == '=')){
        RelationalOperator();
        SimpleExpression();
    }
}

// AssignementStatement := Letter "=" Expression
void AssignementStatement(void){
    if ((current >= 'A' && current <= 'Z') || (current >= 'a' && current <= 'z')){
        Letter();
        if (current == '='){
            ReadChar();
            Expression();
        } else Error("'=' attendu");
    } else Error("Lettre attendue");
}

//Statement := AssignementStatement
void Statement(void){
    AssignementStatement();
}

// DeclarationPart := "[" Letter {"," Letter} "]"
void DeclarationPart(void){
    if (current == '['){
		cout << "\t.data"<<endl;
		cout << "\t.align 8"<<endl;

		ReadChar();
		if ((current >= 'A' && current <= 'Z') || (current >= 'a' && current <= 'z')){
			cout << current << ":\t.quad 0"<<endl;
			Letter();
			while (current == ','){
				ReadChar();
				if ((current >= 'A' && current <= 'Z') || (current >= 'a' && current <= 'z')){
					cout << current << ":\t.quad 0"<<endl;
					ReadChar();
					
				}
    		}
		}
    	if (current == ']') {
			ReadChar();
		}else Error("']' attendu");
	}
}

// StatementPart := Statement {";" Statement} "."
void StatementPart(void){
    Statement();
    while (current == ';'){
        ReadChar();
        Statement();
    }
    if (current != '.') Error("'.' attendu");
}

// Program := [DeclarationPart] StatementPart
void Program(void){
    if (current == '[') {
		DeclarationPart();
	}
	StatementPart();
}

int main(void){
    cout << "\t\t\t# Code produit par le compilateur" << endl;
    cout << "\t.text" << endl;
    cout << "\t.globl main" << endl;
    cout << "main:" << endl;
    cout << "\tpush %rbp" << endl;
    cout << "\tmovq %rsp, %rbp" << endl;

    ReadChar();
    Program();

    cout << "\tmovq %rbp, %rsp" << endl;
    cout << "\tpop %rbp" << endl;
    cout << "\tret" << endl;

    return 0;
}