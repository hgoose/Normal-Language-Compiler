if exists("b:current_syntax")
  finish
endif

syntax keyword nlKeyword if else while for return break continue call invoke fn proc procedure
syntax keyword nlType int void
syntax keyword nlBoolean true false

syntax match nlNumber '[0-9]\+'
syntax match nlIdentifier '[A-Za-z_][A-Za-z0-9_]*'

syntax match nlOperator '==\|!=\|<=\|>=\|&\||\|+\|-\|*\|/\|=\|<\|>\|!\|mod\|}\|{\|(\|)\|\[\|\]'

syntax region nlString start=+"+ skip=+\\"+ end=+"+
syntax region nlComment start="//" end="$"
syntax region nlBlockComment start="/\*" end="\*/"

highlight default link nlKeyword Keyword
highlight default link nlType Type
highlight default link nlBoolean Boolean
highlight default link nlNumber Number
highlight default link nlString String
highlight default link nlComment Comment
highlight default link nlBlockComment Comment
highlight default link nlOperator Operator
highlight default link nlIdentifier Identifier

let b:current_syntax = "nl"
