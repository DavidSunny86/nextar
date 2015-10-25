%option reentrant
%option noyywrap
%option prefix="neo_"
%option never-interactive
%{

#include <ParserContext.h>
#include <parse.neo.hpp>
#define YY_NO_UNISTD_H
#define YY_EXTRA_TYPE nextar::ParserContext*
#define YY_INPUT(buf,result,max_size)       \
	result = yyextra->Read(buf, max_size);	\
	NEX_ASSERT(result <= max_size);			\
	buf[result] = 0;						\

#define yyterminate() return nextar::neo_parser_impl::make_END(b.Loc())	
#define YY_USER_ACTION                     b.Loc().Columns((nextar::uint32)yyleng);
void * neo_alloc (size_t bytes, void * yyscanner);
void * neo_realloc (void * ptr, size_t bytes, void * yyscanner);
void   neo_free (void * ptr, void * yyscanner);
%}

%x STATE_REGION STATE_TEXT STATE_STR STATE_CONTENT

CH      [$#_a-zA-Z]
BLANK   [ \t]
%%

"@" {
  BEGIN(STATE_REGION);
}

<STATE_REGION>{CH}*":neo"?[ \n\t]+       {
  BEGIN(STATE_CONTENT);
  return nextar::neo_parser_impl::make_REGION_ID(&yytext[1], b.Loc());
}

<STATE_REGION>{CH}*":text"[ \n\t]+       {
  BEGIN(STATE_TEXT);
  b.StartContent();
  return nextar::neo_parser_impl::make_TEXT_REGION_ID(&yytext[1], b.Loc());
}

<STATE_TEXT>"@"       {
  BEGIN(STATE_REGION);
  return nextar::neo_parser_impl::make_TEXT_CONTENTS(b.GetContent(), b.Loc());
}

<STATE_TEXT>\\@  {
  b.PutContent('@');
}

<STATE_TEXT>[^\\@\n]+  {
  b.PutContent(yytext);
}

<STATE_TEXT>\\[^@] {
  b.PutContent(yytext);
}

\"             {
  BEGIN(STATE_STR);
  b.StartContent();
}

<STATE_STR>\"        {
 BEGIN(INITIAL);
 return nextar::neo_parser_impl::make_STRING_LITERAL(b.GetContent(), b.Loc());
}

<STATE_STR>\\[0-7]{1,3}   {
  int result;
  std::sscanf( yytext + 1, "%o", &result);
  if ( result > 0xff )
	  b.PutContent(result);
}

<STATE_STR>\\[0-9]+     {
 b.Error(nextar::ParserContext::BAD_ESCAPE_SEQ);
}

<STATE_STR>\\n  b.PutContent('\n');
<STATE_STR>\\t  b.PutContent('\t');
<STATE_STR>\\r  b.PutContent('\r');
<STATE_STR>\\b  b.PutContent('\b');
<STATE_STR>\\f  b.PutContent('\f');

<STATE_STR>\\(.|\n)  b.PutContent(yytext[1]);

<STATE_STR>[^\\\n\"]+   b.PutContent(yytext);

<STATE_CONTENT>[^ \t\n\r]+ {
  return nextar::neo_parser_impl::make_COMMAND(yytext, b.Loc());
}

<STATE_CONTENT>[\n]+  {
 b.Loc().Lines((nextar::uint32)yyleng);
 b.Loc().Step();
}

<STATE_CONTENT>{BLANK}+ {
  b.Loc().Step();
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