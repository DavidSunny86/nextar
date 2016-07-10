%option reentrant
%option noyywrap
%option prefix="neo_"
%option never-interactive
%{
#include <cstdint>
#include <ParserContext.h>
#include <parse.neo.hpp>
#define YY_NO_UNISTD_H
#define YY_EXTRA_TYPE nextar::ParserContext*
#define YY_INPUT(buf,result,max_size)       \
	result = yyextra->Read(buf, max_size);	\
	NEX_ASSERT(result <= max_size);			\
	NEX_ASSERT(buf[result] == 0);				\

#define yyterminate() return nextar::neo_parser_impl::make_END(b.Loc())
#define YY_USER_ACTION                     b.Loc().Columns((nextar::uint32)yyleng);
void * neo_alloc (size_t bytes, void * yyscanner);
void * neo_realloc (void * ptr, size_t bytes, void * yyscanner);
void   neo_free (void * ptr, void * yyscanner);
%}

%x STATE_REGION STATE_TEXT STATE_STR STATE_CONTENT STATE_COMMENT

REG      [a-zA-Z#_$]+
BLANK   [ \t]
%%


<STATE_TEXT>{

\\\[ 		 	b.PutContent('[');
^\[\[          {
	BEGIN(STATE_REGION); 
	nextar::String content = b.GetContent(); 
	b.StartContent(); 
	return nextar::neo_parser_impl::make_TEXT_CONTENTS(content, b.Loc()); 
}

[^\\\[\n]+       b.PutContent(yytext);
\\[^\[]+			 b.PutContent(yytext);

<<EOF>>  {
	BEGIN(INITIAL); 
	return nextar::neo_parser_impl::make_TEXT_CONTENTS(b.GetContent(), b.Loc()); 
}

}


<STATE_STR>{
\"     			{
	BEGIN(STATE_CONTENT);  
	return nextar::neo_parser_impl::make_STRING_LITERAL(b.GetContent(), b.Loc());
}
\\[0-7]{1,3}  {  
	int result;  
	std::sscanf( yytext + 1, "%o", &result);  
	if ( result > 0xff )	  
		b.PutContent(result); 
}

\\[0-9]+     	b.Error(nextar::ParserContext::BAD_ESCAPE_SEQ);
\\n  					b.PutContent('\n');
\\t  					b.PutContent('\t');
\\r  					b.PutContent('\r');
\\b  					b.PutContent('\b');
\\f  					b.PutContent('\f');
\\(.|\n)  		b.PutContent(yytext[1]);
[^\\\n\"]+   	b.PutContent(yytext);
}

<INITIAL,STATE_CONTENT>{
^\[\[          BEGIN(STATE_REGION); b.StartContent();
"/*" 		  		BEGIN(STATE_COMMENT);
"{"            return nextar::neo_parser_impl::make_LBRACKET(b.Loc());
"}"            return nextar::neo_parser_impl::make_RBRACKET(b.Loc());
"["            return nextar::neo_parser_impl::make_LSQBRACKET(b.Loc());
"]"            return nextar::neo_parser_impl::make_RSQBRACKET(b.Loc());
","            return nextar::neo_parser_impl::make_COMMA(b.Loc());
";"            return nextar::neo_parser_impl::make_SEMICOLON(b.Loc());
"("            return nextar::neo_parser_impl::make_LBRACES(b.Loc());
")"            return nextar::neo_parser_impl::make_RBRACES(b.Loc());
"="            return nextar::neo_parser_impl::make_ASSIGN(b.Loc());
\"             BEGIN(STATE_STR);  b.StartContent();
[\n]+  		  	b.Loc().Lines((nextar::uint32)yyleng); b.Loc().Step();
{BLANK}+   	   b.Loc().Step();
[^ \"\{\}\[\],;()\t\n\r]+    return nextar::neo_parser_impl::make_IDENTIFIER(yytext, b.Loc());

}

<STATE_REGION>{

{REG} {
	b.PutContent(yytext);
}

{BLANK}+  b.Loc().Step();

(\.script\]\])|\]\] {
	BEGIN(STATE_CONTENT);
	return nextar::neo_parser_impl::make_REGION_ID(b.GetContent(), b.Loc());
}

\.text\]\]  {
	BEGIN(STATE_TEXT);
	nextar::String value = b.GetContent();
	b.StartContent();
	return nextar::neo_parser_impl::make_TEXT_REGION_ID(value, b.Loc());
}

}


<STATE_COMMENT>{
"*/" 			BEGIN(STATE_CONTENT);
[^*\n]+		// eat comment in chunks
\*          // eat it up
\n		      b.Loc().Lines((nextar::uint32)1);
}

<STATE_TEXT,STATE_STR>\n      {
  // no error, just increase the line number
  // @todo
  b.Loc().Lines((nextar::uint32)yyleng);
  b.Loc().Step();
  b.PutContent('\n');
}

%%

namespace nextar {

void ParserContext::BeginScan() {
  neo_lex_init_extra(this, &_scanner);
  neo_set_debug(_traceScanning, _scanner);
}

void ParserContext::EndScan() {
  neo_lex_destroy(_scanner);
}

}
