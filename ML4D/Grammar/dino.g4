grammar dino;

prog: lines EOF; // Kan erstatte prog med lines eller omvendt
 
lines
    :   (dcl SEMICOLON right=lines
    |   stmt SEMICOLON right=lines)?
    ;
  
dcl
    :   type=types left=ID (op=ASSIGN right=bool_expr)? //# dclExpr
    |   type=types func=ID LPAREN (types ID (COMMA types ID)*)? RPAREN LBRACE lines RBRACE // Måske overvej no multiple parameter i starten
    ;

stmt
    :   id=ID op='=' right=bool_expr                       //# infixStmtExpr
    |   WHILE LPAREN predicate=bool_expr RPAREN LBRACE body=lines RBRACE
    |   id=ID op=BACKWARDS    
    |   op=RETURN right=bool_expr                       //# returnExpr
    |   bool_expr // Nødvendig for at kunne kalde void metoder, i.e. zero();
    
//  |   func=ID '(' (bool_expr (COMMA bool_expr)*)? ')' // Overvej i stedet for den over. Ikke sikkert det er smartere tho.  
    ;

bool_expr 
    :    left=bool_expr op='and' right=bool_expr     //# infixBoolExpr
    |    left=bool_expr op='or'  right=bool_expr     //# infixBoolExpr
    |    expr                                       
    ;
    
expr
    :   '(' expr ')'                                    # parensExpr
    |   op='not' ID                                     # unaryExpr   
    |   <assoc=right> left=expr op='**' right=expr      # infixExpr
    |   left=expr op=('*'|'/') right=expr               # infixExpr
    |   left=expr op=('+'|'-') right=expr               # infixExpr
    |   left=expr op=('=='|'!=') right=expr             # infixExpr
    |   func=ID '(' (bool_expr (COMMA bool_expr)*)? ')' # funcExpr  // Måske overvej no multiple parameter i starten
    |   left=expr op=('<'|'<='|'>'|'>=') left=expr      # infixExpr
    
    // Derivation types
    |   value=(INUM|FNUM|BOOLVAL)                       # numberExpr
    |   left=ID                                         # idExpr
    ;
    
    
types
    :   INT 
    |   BOOL 
    |   DOUBLE
    |   VOID
    ;

GRAD:'grad';
NOT: 'not';
AND: 'and';
OR: 'or';  
VOID: 'void';
DOT: '.';
WHILE: 'while';
DOUBLE: 'double';
INT: 'int';
BOOL: 'bool';
BOOLVAL: ('true'|'false');
RETURN: 'return';
BACKWARDS: '<-';
ASSIGN: '=';
POWER: '**';
MUL: '*';
DIV: '/';
PLUS: '+';
MINUS: '-';
COMMA: ',';
LPAREN: '(';
RPAREN: ')';
LBRACE: '{';
RBRACE: '}';
LBRACK: '[';
RBRACK: ']';
EQUALS: '==';
NOTEQUALS: '!=';
GTHAN: '>';
LTHAN: '<';
GETHAN: '>=';
LETHAN: '<=';
SEMICOLON: ';';
WS: [ \t\r\n]+ -> skip;
INUM: [-]?[0-9]+; // Unary?
FNUM: [-]?[0-9]+ [.][0-9]+; // Unary?
ID: [A-z]([0-9A-z])*;