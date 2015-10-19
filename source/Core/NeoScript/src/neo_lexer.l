%option reentrant
%option noyywrap
%option prefix="neo_"

%{

#include <ParserContext.h>
#include <parse.neo.hpp>

#define YY_EXTRA_TYPE nextar::ParserContext*
#define YY_INPUT(buf,result,max_size)       \
	result = yyextra->Read(buf, max_size);	\
	NEX_ASSERT(result <= max_size);			\
	buf[result] = 0;						\

#define yyterminate() return nextar::neo_parser_impl::make_END(b.Loc())	
#define YY_USER_ACTION                     b.Loc().Columns(yyleng);
void * neo_alloc (size_t bytes, void * yyscanner);
void * neo_realloc (void * ptr, size_t bytes, void * yyscanner);
void   neo_free (void * ptr, void * yyscanner);
%}

%x REGION TEXT STR CONTENT

CH      [$#_a-zA-Z]
BLANK   [ \t]
%%

"@" {
  BEGIN(REGION);
}

<REGION>{CH}*":neo"?[ \n\t]+       {
  BEGIN(CONTENT);
  return nextar::neo_parser_impl::make_REGION_ID(&yytext[1], b.Loc());
}

<REGION>{CH}*":text"[ \n\t]+       {
  BEGIN(TEXT);
  b.StartContent();
  return nextar::neo_parser_impl::make_TEXT_REGION_ID(&yytext[1], b.Loc());
}

<TEXT>"@"       {
  BEGIN(REGION);
  return nextar::neo_parser_impl::make_TEXT_CONTENTS(b.GetContent(), b.Loc());
}

<TEXT>\\@  {
  b.PutContent('@');
}

<TEXT>[^\\@\n]+  {
  b.PutContent(yytext);
}

<TEXT>\\[^@] {
  b.PutContent(yytext);
}

\"             {
  BEGIN(STR);
  b.StartContent();
}

<STR>\"        {
 BEGIN(INITIAL);
 return nextar::neo_parser_impl::make_STRING_LITERAL(b.GetContent(), b.Loc());
}

<STR>\\[0-7]{1,3}   {
  int result;
  std::sscanf( yytext + 1, "%o", &result);
  if ( result > 0xff )
     b.PutContent(result);
}

<STR>\\[0-9]+     {
 b.Error(nextar::ParserContext::BAD_ESCAPE_SEQ);
}

<STR>\\n  b.PutContent('\n');
<STR>\\t  b.PutContent('\t');
<STR>\\r  b.PutContent('\r');
<STR>\\b  b.PutContent('\b');
<STR>\\f  b.PutContent('\f');

<STR>\\(.|\n)  b.PutContent(yytext[1]);

<STR>[^\\\n\"]+   b.PutContent(yytext);

<CONTENT>[^ \t\n\r]+ {
  return nextar::neo_parser_impl::make_COMMAND(yytext, b.Loc());
}

<CONTENT>[\n]+  {
 b.Loc().Lines(yyleng);
 b.Loc().Step();
}

<CONTENT>{BLANK}+ {
  b.Loc().Step();
}

<TEXT,STR>\n      {
  // no error, just increase the line number
  // @todo
  b.Loc().Lines(yyleng);
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