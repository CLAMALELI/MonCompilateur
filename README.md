# CERIcompiler

A simple compiler.
From : Pascal-like imperative LL(k) langage
To : 64 bit 80x86 assembly langage (AT&T)

**Le GitHub**
< https://github.com/CLAMALELI/MonCompilateur

**Download the repository :**

> git clone git@framagit.org:jourlin/cericompiler.git

**Build the compiler and test it :**

> make test

**Have a look at the output :**

> gedit test.s

**Debug the executable :**

> ddd ./test

**Commit the new version :**

> git commit -a -m "What's new..."

**Send to your framagit :**

> git push -u origin master

**Get from your framagit :**

> git pull -u origin master

**This version Can handle :**

// Program := [DeclarationPart] [VarDeclarationPart] StatementPart
// DeclarationPart := "[" Identifier {"," Identifier} "]"
// VarDeclarationPart := "VAR" VarDeclaration {";" VarDeclaration} "."
// VarDeclaration := Identifier {"," Identifier} ":" Type

// Type := "INTEGER" | "BOOLEAN" | "DOUBLE" | "CHAR" | "STRING"
// AdditiveOperator := "+" | "-" | "||"
// MultiplicativeOperator := "*" | "/" | "%" | "&&"
// RelationalOperator := "==" | "!=" | "<" | ">" | "<=" | ">="
// empty ::=
// Digit := "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"
// Letter := "a"|...|"z"|"A"|...|"Z"

// Number := Digit{Digit} | Digit{Digit}"."Digit{Digit}
// Identifier := Letter {(Letter|Digit)}
// Const := Number | CharConst | StringConst

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
// SimpleExpression := Term {AdditiveOperator Term}
// Term := Factor {MultiplicativeOperator Factor}
// Factor := Number | Identifier | "(" Expression ")" | CharConst | StringConst

// StatementPart := Statement {";" Statement} "."
// Statement := AssignementStatement | IfStatement | WhileStatement | DoWhileStatement
//            | ForStatement | BlockStatement | DisplayStatement | CaseStatement | RepeatStatement
// AssignementStatement := Identifier ":=" Expression
// IfStatement := "IF" Expression "THEN" Statement ["ELSE" Statement]
// WhileStatement := "WHILE" Expression "DO" Statement
// DoWhileStatement := "DO" Statement "WHILE" Expression
// ForStatement := "FOR" AssignementStatement "TO" Expression "DO" Statement
//               | "FOR" AssignementStatement "DOWNTO" Expression "DO" Statement
// BlockStatement := "BEGIN" Statement {";" Statement} "END"
// DisplayStatement := "DISPLAY" Expression
// CaseStatement := "CASE" Expression "OF" CaseListElement {";" CaseListElement} "END"
// CaseListElement := CaseLabelList ":" Statement | empty
// CaseLabelList := Const {"," Const}
// RepeatStatement := "REPEAT" Statement {";" Statement} "UNTIL" Expression


