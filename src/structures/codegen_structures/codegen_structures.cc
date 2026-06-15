#include "codegen_structures.h"
#include "tree_eval.h"
#include "ast_structures.h"

#include "types.h"

extern const NodeToCodegenFnMap node_type_to_codegen {
    {NODE_TYPE::PRINT, evaluate_print},
    {NODE_TYPE::READ, process_read},
    {NODE_TYPE::DECL, init_var},
    {NODE_TYPE::ASSIGN, update_var},
    {NODE_TYPE::IF, process_if},
    {NODE_TYPE::WHILE, process_while},
    {NODE_TYPE::BLOCK, process_block},
    {NODE_TYPE::FUNCTION, process_fn},
    {NODE_TYPE::CALL, process_call}
};

bool node_type_has_codegen_fn(NODE_TYPE node_type) {
    return node_type_to_codegen.find(node_type) 
        != node_type_to_codegen.end();
}

CodegenFn get_codegen_fn(NODE_TYPE node_type) {
    return node_type_has_codegen_fn(node_type) 
        ? node_type_to_codegen.at(node_type) : nullptr;
}
