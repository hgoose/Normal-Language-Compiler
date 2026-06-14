#include "tree_eval.h"
#include "ast_node.h"
#include "codegen.h"
#include "token.h"
#include "nlc_integers.h"
#include "error.h"
#include "ast_utils.h"
#include "parserutils.h"
#include "codegen_structures.h"
#include "function_structures.h"

#include <cstdlib>
#include <algorithm>
#include <iostream>

static void r_evaluate_expr(AST_NODE* p, int& pushed) {
    if (!p) return;

    if (p->token.is(TOKEN_AND)) {
        AST_NODE* left{}, *right{};

        auto it = p->children.begin();
        if (it != p->children.end()) {
            left = *it;
            ++it;
        } else return;

        if (it != p->children.end()) {
            right = *it;
        } else return;

        // evaluate lhs
        r_evaluate_expr(left, pushed);
        x86_popr32(REGISTER::EAX); --pushed;

        // if lhs == 0, result is false; skip rhs
        x86_test_al_imm8(1);
        size_t jz_false = x86_jz_rel32_missing();
        int jz_start = get_current_position();

        // evaluate rhs only if lhs was true
        r_evaluate_expr(right, pushed);
        x86_popr32(REGISTER::EAX); --pushed;

        int jz_end = get_current_position();

        x86_pushr32(REGISTER::EAX); ++pushed;

        int jz_size = jz_end - jz_start;
        load_imm32_at(jz_false, jz_size);

        return;
    }

    if (p->token.is(TOKEN_OR)) {
        AST_NODE* left{}, *right{};

        auto it = p->children.begin();
        if (it != p->children.end()) {
            left = *it;
            ++it;
        } else return;

        if (it != p->children.end()) {
            right = *it;
        } else return;

        // evaluate lhs
        r_evaluate_expr(left, pushed);
        x86_popr32(REGISTER::EAX); --pushed;

        // if lhs != 0, result is true, skip rhs
        x86_test_al_imm8(1);
        size_t jnz_true = x86_jnz_rel32_missing();

        int jnz_start = get_current_position();

        // evaluate rhs only if lhs was false
        r_evaluate_expr(right, pushed);
        x86_popr32(REGISTER::EAX); --pushed;

        x86_test_al_imm8(1);
        x86_setnz_al();
        x86_movzx_r32_r8_al(REGISTER::EAX);
        int jnz_end = get_current_position();

        x86_pushr32(REGISTER::EAX); ++pushed;

        int jnz_jump_size = jnz_end - jnz_start;

        load_imm32_at(jnz_true, jnz_jump_size);

        return;
    }

    std::for_each(p->children.begin(), p->children.end(), [&pushed](auto it) -> void {
        r_evaluate_expr(it, pushed);
    });

    // PUSH INTEGERS TO THE STACK
    if (p->node_type == NODE_TYPE::INT) {
        x86_push_imm32(p->token.integer); ++pushed;
    } 

    else if (p->node_type == NODE_TYPE::VAR) {
        // Get a pointer to the variable in r11, 
        // then push that value to the stack
        x86_get_int_for_expr(p->syminfo->location.int_table_offset);
        x86_pushm32(REGISTER::R11); ++pushed;
    } 

    else if (p->node_type == NODE_TYPE::BOOL) {
        x86_push_imm32(p->boolean); ++pushed;
    }

    // RUN CODE FOR UNARY NEGATION
    else if (p->token.is(TOKEN_UNEG)) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_xor_rr32(REGISTER::ECX, REGISTER::ECX);
        x86_xchg32(REGISTER::EAX, REGISTER::ECX);
        x86_sub_rr32(REGISTER::EAX, REGISTER::ECX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    } 

    // RUN CODE FOR BINARY ADDITION
    else if (p->token.is(TOKEN_PLUS)) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_popr32(REGISTER::ECX); --pushed;
        // x86_xchg32(REGISTER::EAX, REGISTER::ECX);
        x86_add_rr32(REGISTER::EAX, REGISTER::ECX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    }

    // RUN CODE FOR BINARY SUBTRACTION
    else if (p->token.is(TOKEN_MINUS)) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_popr32(REGISTER::ECX); --pushed;
        x86_xchg32(REGISTER::EAX, REGISTER::ECX);
        x86_sub_rr32(REGISTER::EAX, REGISTER::ECX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    }

    // RUN CODE FOR BINARY MULTIPLICATION
    else if (p->token.is(TOKEN_MULT)) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_popr32(REGISTER::ECX); --pushed;
        // x86_xchg32(REGISTER::EAX, REGISTER::ECX);
        x86_mult_rr32(REGISTER::EAX, REGISTER::ECX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    }

    // RUN CODE FOR BINARY DIVISION
    else if (p->token.is(TOKEN_DIV)) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_popr32(REGISTER::ECX); --pushed;
        x86_xchg32(REGISTER::EAX, REGISTER::ECX);
        x86_div_rr32(REGISTER::EAX, REGISTER::ECX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    }

    // RUN CODE FOR BINARY MODULUS
    else if (p->token.is(TOKEN_MOD)) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_popr32(REGISTER::ECX); --pushed;
        x86_xchg32(REGISTER::EAX, REGISTER::ECX);
        x86_modulo_rr32(REGISTER::EAX, REGISTER::ECX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    }

    // RUN CODE FOR SUPER FAST EXPONENTIATION
    else if (p->token.is(TOKEN_EXP)) {
        x86_fast_exp();

        // fast_exp makes two pops
        pushed-=2;

        x86_pushr32(REGISTER::EAX); ++pushed;
    } 

    else if (p->token.is(TOKEN_NOT)) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_al_flip();
        x86_pushr32(REGISTER::EAX); ++pushed;
    }

    else if (p->operator_is_relational()) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_popr32(REGISTER::ECX); --pushed;
        x86_cmp_rr32(REGISTER::ECX, REGISTER::EAX);

        if (p->token.is(TOKEN_LESS)) {
            x86_setl_al();
        } 

        else if (p->token.is(TOKEN_LESS_EQ)) {
            x86_setle_al();
        } 

        else if (p->token.is(TOKEN_GREATER)) {
            x86_setg_al();
        }

        else if (p->token.is(TOKEN_GREATER_EQ)) {
            x86_setge_al();
        }

        else if (p->token.is(TOKEN_EQUAL)) {
            x86_sete_al();
        }

        // Not equal
        else { 
            x86_setne_al();
        }

        x86_movzx_r32_r8_al(REGISTER::EAX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    }

}

void evaluate_expr(AST_NODE* root) {
    // Noop on empty tree
    if (!root) return;

    int pushed{};

    r_evaluate_expr(root, pushed);

    if (pushed == 0) return;

    if (pushed > 0) {
        x86_popr32(REGISTER::EAX); --pushed;
    }

    for (int i{}; i<pushed; ++i) {
        x86_popr32(REGISTER::EBX);
    }
}

void evaluate_print_expr(AST_NODE* root) {
    // Noop on empty tree
    if (!root) return;

    int pushed{};

    // Evaluate expression
    r_evaluate_expr(root, pushed);

    if (pushed == 0) return;

    // Result in EAX
    x86_popr32(REGISTER::EAX); --pushed;

    if (root->is_type_integral()) {
        // Call print_int with value in accumulator
        x86_call_void_sia(print_int, REGISTER::EAX);
    } 

    else if (root->is_type_logical()) {
        // Call print_bool with value in accumulator (al)
        x86_call_void_sba(print_bool, REGISTER::EAX);
    }

    for (int i{}; i<pushed; ++i) {
        x86_popr32(REGISTER::EBX);
    }
}

bool evaluate_print(AST_NODE* root) {
    if (!root) return false;

    bool exit{};
    std::for_each(root->children.begin(), root->children.end(), [&exit](AST_NODE* child) -> void {
        if (!child || exit) return;

        if (child->is_var() && child->symbol_freed()) {
            print_error(
                Error{NLC_INVALID_IDENTIFIER,
                child->token.line_no,
                child->token.col_no}
            ); 
            exit = true;
        }

        else if (child->entry.vi) {
            x86_call_void_sca(print_string, child->entry);             
        } 

        else {
            evaluate_print_expr(child);
        }
    });

    return true;
}

bool init_var(AST_NODE* root) {
    if (!root) return false;

    auto child = root->children.begin();
    auto children_end = root->children.end();

    AST_NODE* var{}, *value{};
    if (child != children_end) {
        var = *child;
        ++child;
    }

    if (child != children_end) {
        value = *child;
        ++child;
    }

    INT_TABLE_ENTRY entry = INT_TABLE::add_int(0);

    // Could not get space for variable.
    // add_int will print overflow error.
    if (!entry.vi) {
        return false;
    }

    var->syminfo->location.int_table_offset = entry.offset;
    var->syminfo->location.address = entry.get_addr();
    var->syminfo->location.location_type = LOCATION_TYPE::MEMORY;

    if (value) {
        AST_NODE* assignment = create_assign(var, value);
        update_var(assignment);
        free_tree(assignment);
    }

    return true;
}

bool update_var(AST_NODE* root) {
    if (!root) return false;

    auto child = root->children.begin();
    auto children_end = root->children.end();

    AST_NODE* var, *ast_expr;
    if (child != children_end) {
        var = *child;
        ++child;
    }

    if (child != children_end) {
        ast_expr = *child;
    }

    SYMINFO* symbol = SYMTABLE::get_symbol(var->token.identifier, var->symbol_type, root->statement_scope_level);

    // No symbol found
    if (!symbol) {
        set_print_token_error(Error{}, var->token, NLC_UNKNOWN_VARIABLE);
        return false;
    }

    var->syminfo = symbol;

    // Otherwise, the symbol is found in the symbol table, so we update it
    
    // After this call, address of symbol is in r10
    x86_get_int_for_assign(symbol->location.int_table_offset);
    
    // Fast exp uses R10, evaluate_expr could clobber it 
    x86_mov_rr64(REGISTER::R15, REGISTER::R10);

    // Result in eax
    evaluate_expr(ast_expr);

    x86_mov_mr32_nodisp(REGISTER::R15, REGISTER::EAX);

    return true;
}

bool process_read(AST_NODE* root) {
    if (!root) return false;

    auto child = root->children.begin();
    auto children_end = root->children.end();

    AST_NODE* var;
    if (child != children_end) {
        var = *child;
    }

    SYMINFO* symbol = SYMTABLE::get_symbol(var->token.identifier, var->symbol_type, root->statement_scope_level);

    if (!symbol) {
        set_print_token_error(Error{}, var->token, NLC_UNKNOWN_VARIABLE);
        return false;
    }

    var->syminfo = symbol;

    // Return value in EAX
    x86_call_int_zia(read_int);

    // Move return value to a non call clobbered register (r12)
    x86_mov_rr32(REGISTER::R12, REGISTER::EAX);

    // Call x86_get_int to get the location of the variable. 
    // Pointer to this location in r10 after call
    x86_get_int_for_assign(symbol->location.int_table_offset);

    // Now, mov [r10], r12
    x86_mov_mr32_nodisp(REGISTER::R10, REGISTER::R12);

    return true;
}

void dispatch_statement(AST_NODE* root) {
    if (!root || !node_type_has_codegen_fn(root->node_type)) return;

    get_codegen_fn(root->node_type)(root);
}

// Eval condition -> al
// test al, 1
// jz   ____ <- save location, generate code for statements (plus uncond jump)
//      ^       and learn the total size. Then, insert the rel32 
//              jump size. (Save first byte then increment poffset by four)
bool process_if(AST_NODE* root) {
    if (!root) return false;

    AST_NODE* condition{};

    auto if_child = root->children.begin();
    auto children_end = root->children.end();

    if (if_child != children_end) {
        condition = *if_child;
        ++if_child;
    } 

    else return false;

    evaluate_expr(condition);
    x86_test_al_imm8(0x1);
    size_t jz_rel32_start = x86_jz_rel32_missing();

    int start = get_current_position();

    bool has_else{};
    AST_NODE* else_node{};
    while (if_child != children_end) {
        dispatch_statement(*if_child);

        if ((*if_child)->node_type == NODE_TYPE::ELSE) {
            has_else = true;
            else_node = *if_child;
            break;
        }

        ++if_child;
    }
    size_t jmp_rel32_start = x86_jmp_rel32_missing();

    int end = get_current_position();
    int jz_jump_size = end - start;

    load_imm32_at(jz_rel32_start, jz_jump_size);

    if (has_else) {
        int else_start = get_current_position();
        auto else_child = else_node->children.begin(); 
        while (else_child != else_node->children.end()) {
            dispatch_statement(*else_child);
            ++else_child;
        }

        int else_end = get_current_position();
        int jmp_size = else_end - else_start;

        load_imm32_at(jmp_rel32_start, jmp_size);
    }

    return true;
}

bool process_while(AST_NODE* root) {
    if (!root) return false;

    AST_NODE* condition{};

    auto while_child = root->children.begin();
    if (while_child != root->children.end()) {
        condition = *(while_child++);
    } else return false;

    size_t jmp_start = x86_jmp_rel32_missing();

    int body_start = get_current_position();
    while (while_child != root->children.end()) {
        dispatch_statement(*while_child);
        ++while_child;
    }
    int body_end = get_current_position();
    int body_size = body_end - body_start;

    load_imm32_at(jmp_start, body_size);

    int condition_start = get_current_position();

    evaluate_expr(condition);
    x86_test_al_imm8(0x1);

    int condition_end = get_current_position();

    int condition_plus_test_size = condition_end - condition_start;

    // +6 is the size of the jnz itself
    x86_jnz_rel32(-(body_size + condition_plus_test_size + 6));

    return true;
}

bool process_block(AST_NODE* root) {
    if (!root) return false;

    for (auto& child : root->children) {
        dispatch_statement(child); 
    }

    Scope::tear_down_frame(root->scope_stack_frame, root->statement_scope_level);

    return true;
}

bool process_fn(AST_NODE* root) {
    AST_NODE* name, *ppack, *rv, *block;

    auto child = root->children.begin();
    auto end = root->children.end();

    if (child++ != end) name = *child;
    if (child++ != end) ppack = *child;
    if (child++ != end) rv = *child;
    if (child++ != end) block = *child;

    std::string function_name = name->token.identifier;
    Byte label_byte = get_current_position();

    label_create(function_name, label_byte);

    // SYMINFO* syminfo = SYMTABLE::get_symbol(name->syminfo.name, name->syminfo.)

    for (auto& child : block->children) {
        dispatch_statement(child);
    }

    return true; 
}
