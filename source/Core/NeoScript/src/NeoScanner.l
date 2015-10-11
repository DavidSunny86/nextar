
%name NeoScanner
%option noyywrap
%x TEXT_CONTENT_GRAB  STRING_GRAB

%{
#include <ParserContext.h>
%}

DIGIT   [0-9]
CH      [$#_a-ZA-Z]
ALL     [$#_a-ZA-Z0-9]

%%


%%
