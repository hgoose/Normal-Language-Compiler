CXX      = g++
CXXFLAGS = -std=c++23 -g \
			-Isrc -Itests -Itests/drivers \
			-Isrc/buffio -Isrc/err -Isrc/lexer \
			-Isrc/parser -Isrc/codegen -Isrc/utils \
			-Isrc/nlc_symbols -Isrc/nlc_strings -Isrc/ast_node \
			-Isrc/nlc_integers \
			-Isrc/parser/LL1 \
			-Isrc/types \
			-Isrc/structures/token_structures \
			-Isrc/structures/parser_structures \
			-Isrc/structures/ast_structures  \
			-Isrc/structures/codegen_structures  \
			-Isrc/structures/table_structures  \
			-Isrc/structures/table_structures/symtable_structures  \
			-Isrc/structures/lex_structures  \
			-Isrc/structures/error_structures  \
			-Isrc/state/bufstate \
			-Isrc/state/lexstate \
			-Isrc/scope \
			-Isrc/functions \

TARGET   = bin/nlc
OBJDIR   = obj

SRC = \
	src/main.cc \
	src/utils/util.cc \
	src/utils/parserutils.cc \
	src/utils/ast_utils.cc \
	src/utils/lex_utils.cc \
	tests/drivers/parsertest.cc \
	src/err/error.cc \
	src/lexer/lex.cc \
	src/lexer/token.cc \
	src/buffio/buffio.cc \
	src/parser/parser.cc \
	src/ast_node/ast_node.cc \
	src/codegen/codegen.cc \
	src/codegen/tree_eval.cc \
	src/nlc_symbols/symtable.cc \
	src/nlc_strings/nlc_strings.cc \
	src/nlc_integers/nlc_integers.cc \
	src/parser/LL1/sets.cc \
	src/structures/token_structures/token_structures.cc \
	src/structures/parser_structures/parser_structures.cc \
	src/structures/ast_structures/ast_structures.cc \
	src/structures/codegen_structures/codegen_structures.cc \
	src/structures/lex_structures/lex_structures.cc \
	src/structures/error_structures/error_structures.cc \
	src/state/bufstate/bufstate.cc \
	src/state/lexstate/lexstate.cc \
	src/scope/scope_stack.cc \
	src/functions/functions.cc

OBJ = $(SRC:%=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p bin
	$(CXX) $(OBJ) -o $(TARGET)

$(OBJDIR)/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf bin obj
