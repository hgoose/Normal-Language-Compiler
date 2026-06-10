#ifndef NLC_CODEGEN_H
#define NLC_CODEGEN_H

#include "codegen_structures.h"

#include <cstdlib>
#include <cstdint>

struct STR_TABLE_ENTRY;
struct INT_TABLE_ENTRY;

inline constexpr int WIDE_ON{1};
inline constexpr int WIDE_OFF{0};

extern size_t byte_count;

inline unsigned int gen_modrm_rr(REGISTER rm, REGISTER r) {
    return ((0b11 << 0x3) | (r & 0x7)) << 0x3 | (rm & 0x7);
}

inline unsigned int gen_modrm_rr(REGISTER_8BIT rm, REGISTER_8BIT r) {
    return ((0b11 << 0x3) | (r & 0x7)) << 0x3 | (rm & 0x7);
}

inline unsigned int gen_modrm_norr_nodisp(REGISTER rm, REGISTER r) {
    return ((0b00 << 0x3) | (r & 0x7)) << 0x3 | (rm & 0x7);
}

inline unsigned int gen_modrm_norr_disp8(REGISTER rm, REGISTER r) {
    return ((0b01 << 0x3) | (r & 0x7)) << 0x3 | (rm & 0x7);
}

inline unsigned int gen_rex_rr(bool w, REGISTER rm, REGISTER reg) {
    bool r = reg >= REGISTER::R8, b = rm >= REGISTER::R8;

    return ((0x4 << 0x4) | (w << 0x3)) | (r << 0x2) | b;
}

inline unsigned int gen_rex_r(bool w, REGISTER reg) {
    return ((0x4 << 0x4) | (w << 0x3)) | ((reg >> 3) & 0x1);
}

int pspace_init();
int pspace_reclaim();

void load_byte(unsigned char byte);
void load_byte_at(size_t pos, unsigned char byte);

void dump();

void print_int(int a);
void print_bool(bool b);
void print_string(const char* c);

int read_int();

void load_imm8(int x);
void load_imm32(int x);
void load_imm32_at(size_t pos, long long x);
void load_imm64(long long x);

void x86_push_imm32(int x);
void x86_pushr32(REGISTER src);
void x86_pushr64(REGISTER src);
void x86_pushm32(REGISTER src);

void x86_popr32(REGISTER dest);
void x86_popr64(REGISTER dest);

void x86_mov_rr32(REGISTER dest, REGISTER src);
void x86_mov_rr64(REGISTER dest, REGISTER src);
void x86_mov_rimm32(REGISTER dest, int src);
void x86_mov_rimm64(REGISTER dest, long long src);
void x86_mov_rimm64_sizet(REGISTER dest, size_t src);
void x86_mov_rimm64_ptr(REGISTER dest, std::uintptr_t src);
void x86_mov_mr32_nodisp(REGISTER dest, REGISTER src);
void x86_mov_mr64_nodisp(REGISTER dest, REGISTER src);
void x86_mov_mr64_disp8(REGISTER dest, REGISTER src, int disp);
void x86_movzx_r32_r8_al(REGISTER dest);

void x86_sub_rr32(REGISTER dest, REGISTER src);
void x86_add_rr32(REGISTER dest, REGISTER src);
void x86_mult_rr32(REGISTER dest, REGISTER src);
void x86_div_rr32(REGISTER dest, REGISTER src);
void x86_modulo_rr32(REGISTER dest, REGISTER src);
void x86_fast_exp();
void x86_xor_rr32(REGISTER dest, REGISTER src);

void x86_xchg32(REGISTER r1, REGISTER r2);

void x86_construct_ret();

void x86_call_void_sia(void(*)(int), REGISTER);
void x86_call_void_sba(void(*)(bool), REGISTER);
void x86_call_void_sia_indirect(void(*)(int), REGISTER);
void x86_call_int_zia(int(*f)(void));
void x86_call_void_sca(void(*f)(const char*), STR_TABLE_ENTRY&);

void x86_init_int_var(INT_TABLE_ENTRY*);

void x86_call(REGISTER);

void x86_get_int_for_assign(size_t);
void x86_get_int_for_expr(size_t);

void x86_cmp_rr32(REGISTER dest, REGISTER SRC);
void x86_setl_al();
void x86_setle_al();
void x86_setg_al();
void x86_setge_al();
void x86_sete_al();
void x86_setne_al();
void x86_setnz_al();

void x86_al_flip();

void x86_test_al_imm8(int x);
void x86_test_rm8_imm8(REGISTER_8BIT rm, int x);

void x86_jz_rel8(int disp);
size_t x86_jz_rel32_missing();
size_t x86_jnz_rel32_missing();
size_t x86_jmp_rel32_missing();
void x86_jnz_rel8(int disp);
void x86_jnz_rel32(int disp);

size_t move_program_pointer(size_t dx);
size_t get_current_position();

int x86_exec();

#endif
