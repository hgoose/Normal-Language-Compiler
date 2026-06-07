CXX      = g++
CXXFLAGS = -std=c++23 -g \
			-Isrc -Itests -Itests/drivers \
			-Isrc/buffio -Isrc/err -Isrc/lexer \
			-Isrc/parser -Isrc/codegen -Isrc/utils \
			-Isrc/ncc_symbols -Isrc/ncc_strings -Isrc/ast_node \
			-Isrc/ncc_integers

TARGET   = bin/nlc
OBJDIR   = obj

SRC = \
	src/main.cc \
	src/utils/util.cc \
	src/utils/parserutils.cc \
	src/utils/ast_utils.cc \
	tests/drivers/parsertest.cc \
	src/err/error.cc \
	src/lexer/lex.cc \
	src/lexer/token.cc \
	src/buffio/buffio.cc \
	src/parser/parser.cc \
	src/ast_node/ast_node.cc \
	src/codegen/codegen.cc \
	src/codegen/tree_eval.cc \
	src/ncc_symbols/symtable.cc \
	src/ncc_strings/ncc_strings.cc \
	src/ncc_integers/ncc_integers.cc
	

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
