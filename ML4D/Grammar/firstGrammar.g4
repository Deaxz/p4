grammar firstGrammar;

prog: lines EOF;

lines:
    (dcl SEMICOLON lines |
     stmt SEMICOLON lines)?;

dcl:
    type ID (ASSIGN expr_stmt)? |
    (type | VOID) ID LPAREN (type ID (COMMA type ID)*)? RPAREN LBRACE lines RBRACE;

stmt:
    ID ASSIGN expr_stmt |
    WHILE LPAREN expr_stmt RPAREN LBRACE lines RBRACE |
    ID BACKWARDS |
    RETURN expr_stmt |
    expr_stmt;

expr_stmt:
    logical_OR_expr;

logical_OR_expr:
    logical_OR_expr OR logical_AND_expr |
    logical_AND_expr;

logical_AND_expr:
    logical_AND_expr AND equality_expr |
    equality_expr;

equality_expr:
    plinus_expr (EQUALS | NOTEQUALS) plinus_expr |
    relational_expr;

relational_expr:
    plinus_expr (GTHAN | LTHAN | GETHAN | LETHAN) plinus_expr |
    plinus_expr;

plinus_expr:
    plinus_expr (PLUS | MINUS) muldiv_expr |
    muldiv_expr;

muldiv_expr:
    muldiv_expr (MUL | DIV) power_expr |
    power_expr;

power_expr:
    factor POWER power_expr |
    factor;

factor:
    ID |
    ID DOT GRAD |
    INUM |
    FNUM |
    BOOL |
    LPAREN expr_stmt RPAREN |
    method_invocation;

method_invocation:
    ID LPAREN argument_list RPAREN;

argument_list:
    (expr_stmt (COMMA expr_stmt)*)?;

type:
    INT |
    BOOL |
    DOUBLE;

GRAD:'grad';
AND: 'and';
OR: 'or';  
VOID: 'void';
DOT: '.';
WHILE: 'while';
DOUBLE: 'double';
INT: 'int';
BOOL: ('true'|'false');
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