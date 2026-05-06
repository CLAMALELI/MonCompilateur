//  A compiler from a very simple Pascal-like structured language LL(k)
//  to 64-bit 80x86 Assembly langage
//  Copyright (C) 2019 Pierre Jourlin
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Build with "make compilateur"


#include <string>
#include <iostream>
#include <cstdlib>
#include <set>
#include <map>
#include <FlexLexer.h>
#include "tokeniser.h"
#include <cstring>
#include <vector>

using namespace std;

enum OPREL {EQU, DIFF, INF, SUP, INFE, SUPE, WTFR};
enum OPADD {ADD, SUB, OR, WTFA};
enum OPMUL {MUL, DIV, MOD, AND ,WTFM};

TOKEN current;				// Current token


FlexLexer* lexer = new yyFlexLexer; // This is the flex tokeniser
// tokens can be read using lexer->yylex()
// lexer->yylex() returns the type of the lexicon entry (see enum TOKEN in tokeniser.h)
// and lexer->YYText() returns the lexicon entry as a string

	
map<string, TYPES> DeclaredVariables;
unsigned long TagNumber=0;

bool IsDeclared(const char *id){
    return DeclaredVariables.find(id) != DeclaredVariables.end();
}


void Error(string s){
	cerr << "Ligne n°"<<lexer->lineno()<<", lu : '"<<lexer->YYText()<<"'("<<current<<"), mais ";
	cerr<< s << endl;
	exit(-1);
}

enum TYPES Identifier(void){
    string name = lexer->YYText();
    if (!IsDeclared(name.c_str())) {
        Error("Variable non déclarée : " + name);
    }
    enum TYPES type = DeclaredVariables[name];
    cout << "\tmovq " << name << ", %rax" << endl;
    cout << "\tpush %rax" << endl;

    current = (TOKEN) lexer->yylex();
    return type;
}

// Number := Digit{Digit}
enum TYPES Number(void){
	cout <<"\tpush $"<<atoi(lexer->YYText())<<endl;
	current=(TOKEN) lexer->yylex();
	return INTEGER;
}

enum TYPES Expression(void);			// Called by Term() and calls Term()

// Factor := Number | Letter | "(" Expression ")"| "!" Factor
enum TYPES Factor(void){
    enum TYPES type;
    if(current==RPARENT){
        current=(TOKEN) lexer->yylex();
        type=Expression();
        if(current!=LPARENT)
            Error("')' était attendu");
        else
            current=(TOKEN) lexer->yylex();
    }
    else if(current==NUMBER){
        if(strchr(lexer->YYText(), '.') != nullptr){
            double f = atof(lexer->YYText());
            long long unsigned int *i = (long long unsigned int *) &f;
            cout << "\tmovq $" << *i << ", %rax\t# charge le flottant " << f << endl;
            cout << "\tpush %rax" << endl;
            current=(TOKEN) lexer->yylex();
            type = DOUBLE;
        } else {
            type = Number();
        }
    }
    else if(current==ID)
        type=Identifier();
    else
        Error("'(' ou chiffre ou lettre attendue");
    return type;
}

// MultiplicativeOperator := "*" | "/" | "%" | "&&"
OPMUL MultiplicativeOperator(void){
	OPMUL opmul;
	if(strcmp(lexer->YYText(),"*")==0)
		opmul=MUL;
	else if(strcmp(lexer->YYText(),"/")==0)
		opmul=DIV;
	else if(strcmp(lexer->YYText(),"%")==0)
		opmul=MOD;
	else if(strcmp(lexer->YYText(),"&&")==0)
		opmul=AND;
	else opmul=WTFM;
	current=(TOKEN) lexer->yylex();
	return opmul;
}

// Term := Factor {MultiplicativeOperator Factor}
enum TYPES Term(void){
    enum TYPES type1, type2;
    OPMUL mulop;
    type1 = Factor();
    while(current==MULOP){
        mulop=MultiplicativeOperator();
        type2 = Factor();
        if(type1!=type2){
            Error("types incompatibles");
        }
        if(type1 == DOUBLE){
            cout << "\tfldl (%rsp)" << endl;
            cout << "\taddq $8, %rsp" << endl;
            cout << "\tfldl (%rsp)" << endl;
            cout << "\taddq $8, %rsp" << endl;

            switch(mulop){
                case MUL:
                    cout << "\tfmulp\t# FMUL" << endl;
                    break;
                case DIV:
                    cout << "\tfdivp\t# FDIV" << endl;
                    break;
                default:
                    Error("opérateur non supporté pour DOUBLE");
            }
            cout << "\tsubq $8, %rsp" << endl;
            cout << "\tfstpl (%rsp)" << endl;
        } else {
            cout << "\tpop %rbx"<<endl;
            cout << "\tpop %rax"<<endl;
            switch(mulop){
                case AND:
                    cout << "\tandq %rbx, %rax\t# AND"<<endl;
                    break;
                case MUL:
                    cout << "\timulq %rbx, %rax\t# MUL"<<endl;
                    break;
                case DIV:
                    cout << "\tmovq $0, %rdx"<<endl;
                    cout << "\tdiv %rbx"<<endl;
                    break;
                case MOD:
                    cout << "\tmovq $0, %rdx"<<endl;
                    cout << "\tdiv %rbx"<<endl;
                    cout << "\tmovq %rdx, %rax"<<endl;
                    break;
            }
            cout << "\tpush %rax"<<endl;
        }
    }
    return type1;
}

// AdditiveOperator := "+" | "-" | "||"
OPADD AdditiveOperator(void){
	OPADD opadd;
	if(strcmp(lexer->YYText(),"+")==0)
		opadd=ADD;
	else if(strcmp(lexer->YYText(),"-")==0)
		opadd=SUB;
	else if(strcmp(lexer->YYText(),"||")==0)
		opadd=OR;
	else opadd=WTFA;
	current=(TOKEN) lexer->yylex();
	return opadd;
}

// SimpleExpression := Term {AdditiveOperator Term}
enum TYPES SimpleExpression(void){
    OPADD adop;
    enum TYPES type1, type2;
    type1 = Term();
    while(current==ADDOP){
        adop=AdditiveOperator();
        type2 = Term();
        if (type1 != type2){
            Error("types incompatibles");
        }
        if(type1 == DOUBLE){
            cout << "\tfldl (%rsp)" << endl;
            cout << "\taddq $8, %rsp" << endl;
            cout << "\tfldl (%rsp)" << endl;
            cout << "\taddq $8, %rsp" << endl;

            switch(adop){
                case ADD:
                    cout << "\tfaddp\t# FADD" << endl;
                    break;
                case SUB:
                    cout << "\tfsubp\t# FSUB" << endl;
                    break;
                default:
                    Error("opérateur non supporté pour DOUBLE");
            }
            cout << "\tsubq $8, %rsp" << endl;
            cout << "\tfstpl (%rsp)" << endl;
        } else {
            cout << "\tpop %rbx"<<endl;
            cout << "\tpop %rax"<<endl;
            switch(adop){
                case OR:
                    cout << "\torq %rbx, %rax\t# OR"<<endl;
                    break;
                case ADD:
                    cout << "\taddq %rbx, %rax\t# ADD"<<endl;
                    break;
                case SUB:
                    cout << "\tsubq %rbx, %rax\t# SUB"<<endl;
                    break;
            }
            cout << "\tpush %rax"<<endl;
        }
    }
    return type1;
}

// DeclarationPart := "[" Ident {"," Ident} "]"
void DeclarationPart(void){
	enum TYPES type;
	if(current!=RBRACKET)
		Error("caractère '[' attendu");
	cout << "\t.data"<<endl;
	cout << "\t.align 8"<<endl;
	
	current=(TOKEN) lexer->yylex();
	if(current!=ID)
		Error("Un identificater était attendu");
	cout << lexer->YYText() << ":\t.quad 0"<<endl;
	DeclaredVariables.insert(make_pair(string(lexer->YYText()), type));
	current=(TOKEN) lexer->yylex();
	while(current==COMMA){
		current=(TOKEN) lexer->yylex();
		if(current!=ID)
			Error("Un identificateur était attendu");
		cout << lexer->YYText() << ":\t.quad 0"<<endl;
		DeclaredVariables.insert(make_pair(string(lexer->YYText()), type));
		current=(TOKEN) lexer->yylex();
	}
	if(current!=LBRACKET)
		Error("caractère ']' attendu");
	current=(TOKEN) lexer->yylex();
}

// RelationalOperator := "==" | "!=" | "<" | ">" | "<=" | ">="  
OPREL RelationalOperator(void){
	OPREL oprel;
	if(strcmp(lexer->YYText(),"==")==0)
		oprel=EQU;
	else if(strcmp(lexer->YYText(),"!=")==0)
		oprel=DIFF;
	else if(strcmp(lexer->YYText(),"<")==0)
		oprel=INF;
	else if(strcmp(lexer->YYText(),">")==0)
		oprel=SUP;
	else if(strcmp(lexer->YYText(),"<=")==0)
		oprel=INFE;
	else if(strcmp(lexer->YYText(),">=")==0)
		oprel=SUPE;
	else oprel=WTFR;
	current=(TOKEN) lexer->yylex();
	return oprel;
}

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
enum TYPES Expression(void){
	OPREL oprel;
	enum TYPES type1, type2;
	type1 = SimpleExpression();
	if(current==RELOP){
		oprel=RelationalOperator();
		type2 = SimpleExpression();
		if (type1 != type2){
			Error("types incompatibles");
		}
		cout << "\tpop %rax"<<endl;
		cout << "\tpop %rbx"<<endl;
		cout << "\tcmpq %rax, %rbx"<<endl;
		switch(oprel){
			case EQU:
				cout << "\tje Vrai"<<++TagNumber<<"\t# If equal"<<endl;
				break;
			case DIFF:
				cout << "\tjne Vrai"<<++TagNumber<<"\t# If different"<<endl;
				break;
			case SUPE:
				cout << "\tjae Vrai"<<++TagNumber<<"\t# If above or equal"<<endl;
				break;
			case INFE:
				cout << "\tjbe Vrai"<<++TagNumber<<"\t# If below or equal"<<endl;
				break;
			case INF:
				cout << "\tjb Vrai"<<++TagNumber<<"\t# If below"<<endl;
				break;
			case SUP:
				cout << "\tja Vrai"<<++TagNumber<<"\t# If above"<<endl;
				break;
			default:
				Error("Opérateur de comparaison inconnu");
		}
		cout << "\tpush $0\t\t# False"<<endl;
		cout << "\tjmp Suite"<<TagNumber<<endl;
		cout << "Vrai"<<TagNumber<<":\tpush $0xFFFFFFFFFFFFFFFF\t\t# True"<<endl;	
		cout << "Suite"<<TagNumber<<":"<<endl;
		return BOOLEAN;
	}
	return type1;
}

// AssignementStatement := Identifier ":=" Expression
void AssignementStatement(void){
    string variable;
    enum TYPES type1, type2;
    if(current!=ID)
        Error("Identificateur attendu");
    if(!IsDeclared(lexer->YYText())){
        cerr << "Erreur : Variable '"<<lexer->YYText()<<"' non déclarée"<<endl;
        exit(-1);
    }
    variable = lexer->YYText();
    type1 = DeclaredVariables[variable];
    current=(TOKEN) lexer->yylex();
    if(current!=ASSIGN)
        Error("caractères ':=' attendus");
    current=(TOKEN) lexer->yylex();
    type2 = Expression();
    if(type1 != type2){
		cerr << "Erreur de type : variable '"<<variable<<"' de type "<<type1<<" assignée à une expression de type "<<type2<<endl;
        Error("types différents");
	}
    if(type1 == DOUBLE){
		cout << "\tfldl (%rsp)\t# Charger le résultat dans le registre FPU" << endl;
		cout << "\taddq $8, %rsp\t# Nettoyer la pile" << endl;
		cout << "\tfstpl " << variable << "\t# Stocker directement dans la variable" << endl;
	} else {
    	cout << "\tpopq " << variable << endl;
	}
}

void Statement(void);

//BlockStatement := "BEGIN" Statement { ";" Statement } "END"
void BlockStatement(void){
	if (current == BEG){
		current=(TOKEN) lexer->yylex();
		Statement();
		while (current == SEMICOLON){
			current=(TOKEN) lexer->yylex();
			Statement();
		}
		if (current != END){
			Error("END requit");
		}
		current=(TOKEN) lexer->yylex();
	}else{
		Error("Begin requit");
	}
}

//ForStatement := "FOR" AssignementStatement "To" Expression "DO" Statement
void ForStatement(void){
	if (current == FOR){
		current=(TOKEN) lexer->yylex();
		AssignementStatement();
		if (current == TO){
			current=(TOKEN) lexer->yylex();
			Expression();
			if (current == DO){
				current=(TOKEN) lexer->yylex();
				Statement();
			}else{
				Error("DO requit");
			}
		}else{
			Error("TO requit");
		}
	}else{
		Error("FOR requit");
	}
}

//WhileStatement := "WHILE" Expression "DO" Statement
void WhileStatement(void){
	enum TYPES type;
	if (current == WHILE){
		current=(TOKEN) lexer->yylex();
		type = Expression();
		if (type != BOOLEAN){
			Error("L'Expression du while doit etre de type boolean");
		}
		if (current == DO){
			current=(TOKEN) lexer->yylex();
			Statement();
		}else{
			Error("DO requit");
		}
	}else{
		Error("While requit");
	}
}

//IfStatement := "IF" Expression "THEN" Statement [ "ELSE" Statement ]
void IfStatement(void){
	enum TYPES type;
	if (current == IF){
		current=(TOKEN) lexer->yylex();
		type = Expression();
		if (type != BOOLEAN){
			Error("L'Expression du if doit etre de type boolean");
		}
		if (current == THEN){
			current=(TOKEN) lexer->yylex();
			Statement();
			if (current == ELSE){
				current=(TOKEN) lexer->yylex();
				Statement();
			}
		}else{
			Error("THEN requit");
		}
	}else{
		Error("IF requit");
	}
}

//  DisplayStatement := "DISPLAY" expression           Affiche expression
void DisplayStatement(void){
    enum TYPES type;
    if (current == DISPLAY){
        current=(TOKEN) lexer->yylex();
        type = Expression();
        
        if(type == DOUBLE) {
            cout << "\tmovsd (%rsp), %xmm0" << endl;
            cout << "\tandq $-16, %rsp\t# Aligne la pile sur 16 octets pour printf" << endl;
            cout << "\tmovq $FormatDouble, %rdi" << endl;
            cout << "\tmovl $1, %eax" << endl;
            cout << "\tcall printf@PLT" << endl;
            cout << "\tmovq %rbp, %rsp\t# On revient à un état connu via le registre de base" << endl;
            cout << "\tsubq $0, %rsp\t# (Optionnel) Ajustement si nécessaire" << endl;
        } else {
            cout << "\tpop %rdx" << endl;
            cout << "\tmovq %rbp, %rsp\t# Sécurité alignement" << endl;
            cout << "\tandq $-16, %rsp" << endl;
            cout << "\tmovq $FormatString1, %rsi" << endl;
            cout << "\tmovl $1, %edi" << endl;
            cout << "\tmovl $0, %eax" << endl;
            cout << "\tcall __printf_chk@PLT" << endl;
            cout << "\tmovq %rbp, %rsp" << endl;
        }
    }
}

enum TYPES check_type(void){
	if (strcmp(lexer->YYText(),"INTEGER")==0){
		return INTEGER;
	}else if(strcmp(lexer->YYText(),"BOOLEAN")==0){
		return BOOLEAN;
	}else if(strcmp(lexer->YYText(),"DOUBLE")==0){
		return DOUBLE;
	}else if(strcmp(lexer->YYText(),"CHAR")==0){
		return CHAR;
	}else{
		Error("Type invalide");
	}
}

// VarDeclaration := Ident {"," Ident} ":" TYPES  // TYPES = {INTEGER, BOOLEAN, DOUBLE, CHAR};
void VarDeclaration(void){
	enum TYPES type;
	vector<string> names;

    if (current != ID)
        Error("Identificateur attendu");
    names.push_back(lexer->YYText());
    current=(TOKEN) lexer->yylex();

    while(current == COMMA){
        current=(TOKEN) lexer->yylex();
        if (current != ID)
            Error("Identificateur attendu");
        names.push_back(lexer->YYText());
        current=(TOKEN) lexer->yylex();
    }

    if(current != COLON)
        Error("caractère ':' attendu");
	current=(TOKEN) lexer->yylex(); 
	type = check_type();
    current = (TOKEN) lexer->yylex();

    string directive;
    switch(type){
        case INTEGER:  directive = ".quad 0";     break;
        case BOOLEAN:  directive = ".quad 0";     break;
        case DOUBLE:   directive = ".double 0.0"; break;
        case CHAR:     directive = ".byte 0";     break;
    }

    for(const string& name : names){
        cout << name << ":\t" << directive << endl;
		DeclaredVariables[name] = type;
	}
}

// VarDeclarationPart := "VAR" VarDeclaration {";" VarDeclaration} "."
void VarDeclarationPart(void){
    if (current == VAR){
        cout << "\t.data" << endl;
        cout << "\t.align 8" << endl;
        current=(TOKEN) lexer->yylex();
        VarDeclaration();
        while(current == SEMICOLON){
            current=(TOKEN) lexer->yylex();
            VarDeclaration();
        }
        if(current != DOT)
            Error("caractère '.' attendu");
        current=(TOKEN) lexer->yylex();
    } else {
        Error("VAR attendu");
    }
}

//Statement := AssignementStatement | IfStatement | WhileStatement | ForStatement | BlockStatement | DisplayStatement | VarDeclarationPart
void Statement(void){
	if (current == ID){
		AssignementStatement();
	}else if (current == IF){
		IfStatement();
	}else if (current == WHILE){
		WhileStatement();
	}else if (current == FOR){
		ForStatement();
	}else if (current == BEG){
		BlockStatement();
	}else if (current == DISPLAY){
		DisplayStatement();
	}else{
		Error("Absence de mot clé");
	}
}

// StatementPart := Statement {";" Statement} "."
void StatementPart(void){
	cout << "\t.text\t\t# The following lines contain the program"<<endl;
	cout << "\t.globl main\t# The main function must be visible from outside"<<endl;
	cout << "main:\t\t\t# The main function body :"<<endl;
	cout << "\tmovq %rsp, %rbp\t# Save the position of the stack's top"<<endl;
	Statement();
	while(current==SEMICOLON){
		current=(TOKEN) lexer->yylex();
		Statement();
	}
	if(current!=DOT){
		Error("caractère '.' attendu2");
	}
	current=(TOKEN) lexer->yylex();
}

// Program := [DeclarationPart] [VarDeclarationPart] StatementPart
void Program(void){
	if(current == RBRACKET)
        DeclarationPart();
	if(current == VAR)
        VarDeclarationPart();
    StatementPart();	
}

int main(void){	// First version : Source code on standard input and assembly code on standard output
	// Header for gcc assembler / linker
	cout << "\t\t\t# This code was produced by the CERI Compiler"<<endl;
	cout << "\t.section .rodata"<<endl;
	cout << "FormatString1:\t.string \"%llu\\n\""<<endl;
	cout << "FormatDouble:\t.string \"%g\\n\"\t# Pour les flottants" << endl;
	// Let's proceed to the analysis and code production
	current=(TOKEN) lexer->yylex();
	Program();
	// Trailer for the gcc assembler / linker
	cout << "\tmovq %rbp, %rsp\t\t# Restore the position of the stack's top"<<endl;
	cout << "\tret\t\t\t# Return from main function"<<endl;
	if(current!=FEOF){
		cerr <<"Caractères en trop à la fin du programme : ["<<current<<"]";
		Error("."); // unexpected characters at the end of program
	}

}