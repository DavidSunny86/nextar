
grammar VitaScript;


script
	: statement* EOF
	;
	  
statement 
	:  COMMAND (WS param)* block? ';'
	;
param
	:  STRING | NAME 
	;
block
	:  '{' statement* '}'
	;
STRING
	:  '"' (ESCAPE | ~('"'|'\\'))* '"'
	;
NAME
	:  (~WS)*
	;
fragment ESCAPE
	:  '\\' ('\\' | '"' )
	;
COMMAND
	:  ('a'..'z'|'A'..'Z')+('0'..'9')*
	;
WS
	:  (' '|'\t'|'\n'|'\r')+ {skip();}
	;

