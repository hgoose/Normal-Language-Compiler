#include "tree_eval.h"
#include "ast_node.h"
#include "codegen.h"
#include "token.h"
#include "nlc_integers.h"
#include "error.h"
#include "ast_utils.h"
#include "parserutils.h"

#include <cstdlib>
#include <algorithm>
#include <iostream>

static void r_evaluate_expr(AST_NODE* p, int& pushed) {
    // NOOP
    if (!p) return;

    if (p->token.id == TOKEN_AND) {
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

    if (p->token.id == TOKEN_OR) {
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
    } else if (p->node_type == NODE_TYPE::BOOL) {
        x86_push_imm32(p->boolean); ++pushed;
    }
    // RUN CODE FOR UNARY NEGATION
    else if (p->token.id == TOKEN_UNEG) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_xor_rr32(REGISTER::ECX, REGISTER::ECX);
        x86_xchg32(REGISTER::EAX, REGISTER::ECX);
        x86_sub_rr32(REGISTER::EAX, REGISTER::ECX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    } 
    // RUN CODE FOR BINARY ADDITION
    else if (p->token.id == TOKEN_PLUS) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_popr32(REGISTER::ECX); --pushed;
        // x86_xchg32(REGISTER::EAX, REGISTER::ECX);
        x86_add_rr32(REGISTER::EAX, REGISTER::ECX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    }
    // RUN CODE FOR BINARY SUBTRACTION
    else if (p->token.id == TOKEN_MINUS) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_popr32(REGISTER::ECX); --pushed;
        x86_xchg32(REGISTER::EAX, REGISTER::ECX);
        x86_sub_rr32(REGISTER::EAX, REGISTER::ECX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    }
    // RUN CODE FOR BINARY MULTIPLICATION
    else if (p->token.id == TOKEN_MULT) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_popr32(REGISTER::ECX); --pushed;
        // x86_xchg32(REGISTER::EAX, REGISTER::ECX);
        x86_mult_rr32(REGISTER::EAX, REGISTER::ECX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    }
    // RUN CODE FOR BINARY DIVISION
    else if (p->token.id == TOKEN_DIV) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_popr32(REGISTER::ECX); --pushed;
        x86_xchg32(REGISTER::EAX, REGISTER::ECX);
        x86_div_rr32(REGISTER::EAX, REGISTER::ECX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    }
    // RUN CODE FOR BINARY MODULUS
    else if (p->token.id == TOKEN_MOD) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_popr32(REGISTER::ECX); --pushed;
        x86_xchg32(REGISTER::EAX, REGISTER::ECX);
        x86_modulo_rr32(REGISTER::EAX, REGISTER::ECX);
        x86_pushr32(REGISTER::EAX); ++pushed;
    }
    // RUN CODE FOR SUPER FAST EXPONENTIATION
    else if (p->token.id == TOKEN_EXP) {
        x86_fast_exp();

        // fast_exp makes two pops
        pushed-=2;

        x86_pushr32(REGISTER::EAX); ++pushed;
    } 
    else if (p->token.id == TOKEN_NOT) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_al_flip();
        x86_pushr32(REGISTER::EAX); ++pushed;
    }
    else if (p->token.id == TOKEN_LESS ||
            p->token.id == TOKEN_LESS_EQ ||
            p->token.id == TOKEN_GREATER ||
            p->token.id == TOKEN_GREATER_EQ ||
            p->token.id == TOKEN_EQUAL ||
            p->token.id == TOKEN_NOT_EQUAL
    ) {
        x86_popr32(REGISTER::EAX); --pushed;
        x86_popr32(REGISTER::ECX); --pushed;
        x86_cmp_rr32(REGISTER::ECX, REGISTER::EAX);

        if (p->token.id == TOKEN_LESS) {
            x86_setl_al();
        } 
        else if (p->token.id == TOKEN_LESS_EQ) {
            x86_setle_al();
        } 
        else if (p->token.id == TOKEN_GREATER) {
            x86_setg_al();
        }
        else if (p->token.id == TOKEN_GREATER_EQ) {
            x86_setge_al();
        }
        else if (p->token.id == TOKEN_EQUAL) {
            x86_sete_al();
        }
        // TOKEN_NOT_EQUAL
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

    x86_popr32(REGISTER::EAX); --pushed;

    // Result in EAX
    if (root->is_type_integral()) {
        // Call print_int with value in accumulator
        x86_call_void_sia(print_int, REGISTER::EAX);
    } else if (root->is_type_logical()) {
        // Call print_bool with value in accumulator (al)
        x86_call_void_sba(print_bool, REGISTER::EAX);
    }

    for (int i{}; i<pushed; ++i) {
        x86_popr32(REGISTER::EBX);
    }
}

bool evaluate_print(AST_NODE* root) {
    if (!root) return false;

    std::for_each(root->children.begin(), root->children.end(), [](AST_NODE* it) -> void {
        if (!it) return;

        if (it->entry.vi) {
            x86_call_void_sca(print_string, it->entry);             
        } else {
            evaluate_print_expr(it);
        }
    });

    return true;
}

bool init_var(AST_NODE* root) {
    if (!root) return false;

    AST_NODE* var = root->children.front();
    INT_TABLE_ENTRY entry = INT_TABLE::add_int(0);

    // Could not get space for variable.
    // add_int will print overflow error.
    if (!entry.vi) {
        return false;
    }

    var->syminfo->location.int_table_offset = entry.offset;
    var->syminfo->location.address = entry.get_addr();
    var->syminfo->location.location_type = LOCATION_TYPE::MEMORY;

    return true;
}

bool update_var(AST_NODE* root) {
    if (!root) return false;

    auto child = root->children.begin();

    AST_NODE* var, *ast_expr;

    if (*child) var = *child;
    ++child;
    if (*child) ast_expr = *child;

    SYMINFO* symbol = SYMTABLE::get_symbol(var->token.identifier, var->symbol_type);

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

    AST_NODE* var = root->children.front();

    SYMINFO* symbol = SYMTABLE::get_symbol(var->token.identifier, var->symbol_type);

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
    if (!root) return;
    
    if (root->node_type == NODE_TYPE::PRINT) {
        evaluate_print(root);
    } else if (root->node_type == NODE_TYPE::READ) {
        process_read(root);
    } else if (root->node_type == NODE_TYPE::DECL) {
        init_var(root);
    } else if (root->node_type == NODE_TYPE::ASSIGN) {
        update_var(root);
    } else if (root->node_type == NODE_TYPE::IF) {
        process_if(root);
    } else if (root->node_type == NODE_TYPE::WHILE) {
        process_while(root);
    }
}

// Eval condition -> al
// test al, 1
// jz   ____ <- save location, generate code for statements (plus uncond jump)
//      ^       and learn the total size. Then, insert the rel32 
//              jump size. (Save first byte then increment poffset by four)
//
//  Then, below all statements inside the if, we co
bool process_if(AST_NODE* root) {
    if (!root) return false;

    AST_NODE* condition{};

    auto if_child = root->children.begin();
    if (if_child != root->children.end()) {
        condition = *(if_child++);
    } else return false;

    evaluate_expr(condition);
    x86_test_al_imm8(0x1);
    size_t jz_rel32_start = x86_jz_rel32_missing();

    int start = get_current_position();

    bool has_else{};
    AST_NODE* else_node{};
    while (if_child != root->children.end()) {
        dispatch_statement(*if_child);
        if ((*if_child)->node_type == NODE_TYPE::ELSE) {
            has_else = true;
            else_node = (*if_child);
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
