#include <cstdlib>
#include <sys/mman.h>
#include <cstdlib>
#include <iostream>

#include "nlc_strings.h"
#include "nlc_integers.h"
#include "codegen.h"
#include "codegen_structures.h"
#include "types.h"
#include "scope_stack.h"
#include "tree_eval.h"

// Two meg should suffice
static constexpr size_t PROG_SIZE{2*1024*1024};

// Entry into the programs address space
static Byte* prog;

// Offset into the address space
static size_t p_offset;

size_t byte_count{};

void load_byte(Byte byte) {
    prog[p_offset++] = byte;
    ++byte_count;
}

void load_byte_at(size_t pos, Byte byte) {
    prog[pos] = byte;
    ++byte_count;
}

void dump() {
    for (size_t i{}; i<p_offset; ++i) {
        std::cout << std::hex << prog[i];
    }
}

void print_int(int a) {
    std::cout << a; 
}

void print_bool(bool b) {
    std::cout << std::boolalpha << b; 
}

void print_string(const char* c) {
    std::cout << c;
}

int read_int() {
    int a; std::cin >> a; 
    return a;
}

void load_imm8(int x) {
    load_byte(x & 0xff);
}

// Loads a 32-bit integer into the program to be used as an operand
void load_imm32(int x) {
    for (int i=0; i<4; ++i) {
        load_byte(x & 0xff);
        x>>=0x8;
    } 
}

void load_imm32_at(size_t pos, long long x) {
    for (int i=0; i<4; ++i) {
        load_byte_at(pos++, x & 0xff);
        x>>=0x8;
    }
}

// Loads a 64-bit integer into the program
void load_imm64(long long x) {
    for (int i=0; i<8; ++i) {
        load_byte(x & 0xff);
        x>>=0x8;
    } 
}

int pspace_init() {
    p_offset = 0;

    // Ask the kernel to hand over read, write, and executable memory 
    prog = (unsigned char *) mmap(nullptr, 
            PROG_SIZE,
            PROT_EXEC | PROT_READ | PROT_WRITE, 
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0
    );

    if (prog == MAP_FAILED) {
        return -1;
    }

    return 0;
}

// FF /6 PUSH r/m32, push a value onto the stack
void x86_push_imm32(int x) {
    ++pushed;

    load_byte(0x68);
    load_imm32(x); 
}

// FF /6 PUSH r/m32
void x86_pushr32(REGISTER src) {
    ++pushed;

    if (src >= REGISTER::R8) {
        load_byte(gen_rex_r(WIDE_OFF, src));
    }

    load_byte(0xFF);
    load_byte(gen_modrm_rr(src, (REGISTER)(6)));
}

// FF /6 PUSH r/m64
void x86_pushr64(REGISTER src) {
    ++pushed;

    load_byte(gen_rex_r(WIDE_ON, src));
    load_byte(0xFF);
    load_byte(gen_modrm_rr(src, (REGISTER)6));
}

// FF /6 PUSH r/m32
void x86_pushm32(REGISTER src) {
    ++pushed;

    if (src >= REGISTER::R8) {
        load_byte(gen_rex_r(WIDE_OFF, src));
    }

    load_byte(0xFF);
    load_byte(gen_modrm_norr_nodisp(src, (REGISTER)(6)));
}

// 8F /0 POP r/m32, pop the top of the stack into dest
void x86_popr32(REGISTER dest) {
    --pushed;

    if (dest >= REGISTER::R8) {
        load_byte(gen_rex_r(WIDE_OFF, dest));
    }

    load_byte(0x8F);
    load_byte(gen_modrm_rr(dest, (REGISTER)0));
}

// 8F /0 POP r/m64
void x86_popr64(REGISTER dest) {
    --pushed;

    load_byte(gen_rex_r(WIDE_ON, dest));
    load_byte(0x8F);
    load_byte(gen_modrm_rr(dest, (REGISTER)0));
}

// 89 /r MOV r/m32 r32
void x86_mov_rr32(REGISTER dest, REGISTER src) {
    if (dest >= REGISTER::R8 || src >= REGISTER::R8) {
        load_byte(gen_rex_rr(WIDE_OFF, dest, src));
    }

    load_byte(0x89);
    load_byte(gen_modrm_rr(dest, src));
}

// REX.W 89 /r MOV r/m64 r64
void x86_mov_rr64(REGISTER dest, REGISTER src) {
    load_byte(gen_rex_rr(WIDE_ON, dest, src));
    load_byte(0x89);
    load_byte(gen_modrm_rr(dest, src));
}

// B8+rd id MOV r32, imm32
void x86_mov_rimm32(REGISTER dest, int src) {
    if (dest >= REGISTER::R8) {
        load_byte(gen_rex_r(WIDE_OFF, dest));
    }

    load_byte(0xB8 + (dest & 0x7));
    load_imm32(src);
}

// REX.W + B8+ rd io MOV r64, imm64
// NOTE: If dest is R8-R15, REX.B = 1, zero otherwise. In any case, 
// REX.R = REX.X = 0, and REX.W = 1
void x86_mov_rimm64(REGISTER dest, long long src) {
    load_byte(gen_rex_r(WIDE_ON, dest));
    load_byte(0xB8 + (dest & 0x7));
    load_imm64(src);
}

// REX.W + B8+ rd io MOV r64, imm64
void x86_mov_rimm64_sizet(REGISTER dest, size_t src) {
    load_byte(gen_rex_r(WIDE_ON, dest));
    load_byte(0xB8 + (dest & 0x7));
    load_imm64(src);
}

// REX.W + B8+ rd io MOV r64, imm64
void x86_mov_rimm64_ptr(REGISTER dest, std::uintptr_t src) {
    load_byte(gen_rex_r(WIDE_ON, dest));
    load_byte(0xB8 + (dest & 0x7));
    load_imm64(src);
}

// 89 /r MOV r/m32, r32
void x86_mov_mr32_nodisp(REGISTER dest, REGISTER src) {
    if (dest >= REGISTER::R8 || src >= REGISTER::R8) {
        load_byte(gen_rex_rr(WIDE_OFF, dest, src));
    }
    load_byte(0x89);
    load_byte(gen_modrm_norr_nodisp(dest, src));
}

// REX.W + 89 /r MOV r/m64, r64
//
// Note: Specifically when dest is a memory location 
// inside a register (no displacement), so mod = 00
void x86_mov_mr64_nodisp(REGISTER dest, REGISTER src) {
    load_byte(gen_rex_rr(WIDE_ON, dest, src));
    load_byte(0x89);
    load_byte(gen_modrm_norr_nodisp(dest, src));
}

// 8B /r MOV r64, r/m64
void x86_mov_rm32_nodisp(REGISTER dest, REGISTER src) {
    if (dest >= REGISTER::R8 || src >= REGISTER::R8) {
        load_byte(gen_rex_rr(WIDE_OFF, src, dest));
    }
    load_byte(0x8B);
    load_byte(gen_modrm_norr_nodisp(src, dest));
}

// REX.W + 89 /r MOV r/m64, r64
//
// Note: Specifically when dest is a memory location 
// inside a register (no displacement), so mod = 01
void x86_mov_mr64_disp8(REGISTER dest, REGISTER src, int disp) {
    load_byte(gen_rex_rr(WIDE_ON, dest, src));
    load_byte(0x89);
    load_byte(gen_modrm_norr_disp8(dest, src));
    load_imm8(disp);
}

// 29 /r SUB r/m32, r32
void x86_sub_rr32(REGISTER dest, REGISTER src) {
    if (dest >= REGISTER::R8 || src >= REGISTER::R8) {
        load_byte(gen_rex_rr(WIDE_OFF, dest, src));
    }

    load_byte(0x29);
    load_byte(gen_modrm_rr(dest,src));
}

// REX.W + 81 /5 id SUB r/m64, imm32
void x86_sub_r64_imm32(REGISTER dest, long long x) {
    load_byte(gen_rex_r(WIDE_ON, dest));
    load_byte(0x81);
    load_byte(gen_modrm_rr(dest, (REGISTER)5));
    load_imm32(x);
}

// 01 /r ADD r/m32, r32
void x86_add_rr32(REGISTER dest, REGISTER src) {
    if (dest >= REGISTER::R8 || src >= REGISTER::R8) {
        load_byte(gen_rex_rr(WIDE_OFF, dest, src));
    }

    load_byte(0x01);
    load_byte(gen_modrm_rr(dest, src));
}

// 0F AF /r IMUL r32, r/m32
void x86_mult_rr32(REGISTER dest, REGISTER src) {
    if (dest >= REGISTER::R8 || src >= REGISTER::R8) {
        load_byte(gen_rex_rr(WIDE_OFF, dest, src));
    }

    load_byte(0x0F);
    load_byte(0xAF);
    load_byte(gen_modrm_rr(src, dest));
}


// F7 /7 IDIV r/m32
void x86_div_rr32(REGISTER dest, REGISTER src) {
    // MOV EAX, dest
    x86_mov_rr32(REGISTER::EAX, dest);

    // CDQ
    load_byte(0x99);

    if (src >= REGISTER::R8) {
        load_byte(gen_rex_r(WIDE_OFF, src));
    }

    // IDIV src
    load_byte(0xF7); 
    load_byte(gen_modrm_rr(src, (REGISTER)(7)));
}

void x86_modulo_rr32(REGISTER dest, REGISTER src) {
    // DIV src (dest in eax)
    x86_div_rr32(dest, src);

    // Move remainder into accumulator
    x86_mov_rr32(REGISTER::EAX, REGISTER::EDX);
}

// The fast exponentiation algorithm in IA-32e
void x86_fast_exp() {
    load_byte(0x45);  // XOR R8, R8  // e = 0
    load_byte(0x31);
    load_byte(0xc0);
    load_byte(0x41);  // POP R10      // get a
    load_byte(0x8f);
    load_byte(0xc2);
    load_byte(0x41);  // POP R9     // get b
    load_byte(0x8f);
    load_byte(0xc1);
    load_byte(0x45);  // TEST R10, R10 // if (b < 0)
    load_byte(0x85);
    load_byte(0xd2);
    load_byte(0x7c);  // JL          // if (b < 0) jump to end
    load_byte(0x1e);
    load_byte(0x41);  // INC R8      // e = 1
    load_byte(0xff);
    load_byte(0xc0);
    load_byte(0x45);  // TEST R10, R10 // start of while loop
    load_byte(0x85);
    load_byte(0xd2);
    load_byte(0x74);  // JE          // if (b = 0) exit loop, 
    load_byte(0x16);
    load_byte(0x41);  // TEST R10, 1 // if (b & 1)
    load_byte(0xf7);
    load_byte(0xc2);
    load_byte(0x01);
    load_byte(0x00);
    load_byte(0x00);
    load_byte(0x00);
    load_byte(0x74); // JE  if(b & 1) is 0, jump over next IMUL
    load_byte(0x04);
    load_byte(0x45); // IMUL R8, R9  // e = e * a
    load_byte(0x0f);
    load_byte(0xaf);
    load_byte(0xc1);
    load_byte(0x45); // IMUL R9, R9  // a = a * a
    load_byte(0x0f);
    load_byte(0xaf);
    load_byte(0xc9);
    load_byte(0x41); // SAR R10, 1   // b >>= 1
    load_byte(0xd1);
    load_byte(0xfa);
    load_byte(0xeb); // JMP   // back to start of while loop
    load_byte(0xe5);
    load_byte(0x41); // MOV EAX, R8  // move result (e = a^b) to EAX
    load_byte(0x8b);
    load_byte(0xc0);

    // Account for two pops
    pushed-=2;
}

// 31 /r XOR r/m32, r32
void x86_xor_rr32(REGISTER dest, REGISTER src) {
    if (dest >= REGISTER::R8 || src >= REGISTER::R8) {
        load_byte(gen_rex_rr(WIDE_OFF, dest, src));
    }

    load_byte(0x31);
    load_byte(gen_modrm_rr(dest, src));
}

// 87 /r XCHG r/m32, r32
void x86_xchg32(REGISTER r1, REGISTER r2) {
    if (r1 >= REGISTER::R8 || r2 >= REGISTER::R8) {
        load_byte(gen_rex_rr(WIDE_OFF, r1, r2));
    }

    load_byte(0x87);
    load_byte(gen_modrm_rr(r1, r2));
}


// C3 RET 
void x86_construct_ret() {
    load_byte(0xc3);
}

// FF /2
void x86_call(REGISTER reg) {
    if (reg >= REGISTER::R8) {
        load_byte(gen_rex_r(WIDE_ON, reg)); 
    }

    load_byte(0xFF);
    load_byte(gen_modrm_rr(reg, (REGISTER) 2));
}

// Call some C++ function that takes a single integer argument, and returns void
// FF /2
void x86_call_void_sia(void(*f)(int), REGISTER src) {
    x86_mov_rr32(REGISTER::EDI, src);
    x86_mov_rimm64_ptr(REGISTER::RCX, (std::uintptr_t) f);

    x86_call(REGISTER::RCX);
}

void x86_call_void_sba(void(*f)(bool), REGISTER src) {
    x86_mov_rr32(REGISTER::EDI, src);
    x86_mov_rimm64_ptr(REGISTER::RCX, (std::uintptr_t) f);

    x86_call(REGISTER::RCX);
}

// Get the argument through the memory address in a register
void x86_call_void_sia_indirect(void(*f)(int), REGISTER src) {
    x86_mov_rm32_nodisp(REGISTER::EDI, src);
    x86_mov_rimm64_ptr(REGISTER::RCX, (std::uintptr_t) f);

    x86_call(REGISTER::RCX);
}

void x86_call_int_zia(int(*f)(void)) {
    x86_mov_rimm64_ptr(REGISTER::ECX, (std::uintptr_t) f);
    x86_call(REGISTER::ECX);
}

// Call some C++ function that takes a single char* argument, and returns void
// FF /2
void x86_call_void_sca(void(*f)(const char*), STR_TABLE_ENTRY& st_entry) {
    const char* c = STR_TABLE::emit_string(st_entry);

    x86_mov_rimm64_ptr(REGISTER::RCX, (std::uintptr_t) f); 
    x86_mov_rimm64_ptr(REGISTER::RDI, (std::uintptr_t) c); 

    x86_call(REGISTER::RCX);
}

// Calls INT_TABLE::emit_int(size_t offset), which returns a pointer to the region in memory in which the variable
// in question resides. When this function returns, that pointer will be in r10 and we can update the value
// that it points to.
//
// mov  rdi, offset 
// mov  r10, INT_TABLE::emit_int
// call r10
// mov  r10,eax
void x86_get_int_for_assign(size_t offset) {
    x86_mov_rimm64_sizet(REGISTER::RDI, offset);
    x86_mov_rimm64_ptr(REGISTER::R10, (std::uintptr_t)(int*(*)(size_t))INT_TABLE::emit_int);
    x86_call(REGISTER::R10);

    x86_mov_rr64(REGISTER::R10, REGISTER::RAX);
}

// Use R11
void x86_get_int_for_expr(size_t offset) {
    x86_mov_rimm64_sizet(REGISTER::RDI, offset);
    x86_mov_rimm64_ptr(REGISTER::R11, (std::uintptr_t)(int*(*)(size_t))INT_TABLE::emit_int);
    x86_call(REGISTER::R11);

    x86_mov_rr64(REGISTER::R11, REGISTER::RAX);
}

// 39 /r CMP r/m32, r32
void x86_cmp_rr32(REGISTER dest, REGISTER src) {
    if (dest >= REGISTER::R8 || src >= REGISTER::R8) {
        load_byte(gen_rex_rr(WIDE_OFF, dest, src));
    }
    load_byte(0x39);
    load_byte(gen_modrm_rr(dest, src));
}

// 0F 9C /0 SETL r/m8
void x86_setl_al() {
    load_byte(0x0F);
    load_byte(0x9C);
    load_byte(gen_modrm_rr(REGISTER_8BIT::AL, (REGISTER_8BIT)0));
}

// 0F 9E /0 SETLE r/m8
void x86_setle_al() {
    load_byte(0x0F);
    load_byte(0x9E);
    load_byte(gen_modrm_rr(REGISTER_8BIT::AL, (REGISTER_8BIT)0));
}

// 0F 9F /0 SETG r/m8
void x86_setg_al() {
    load_byte(0x0F);
    load_byte(0x9F);
    load_byte(gen_modrm_rr(REGISTER_8BIT::AL, (REGISTER_8BIT)0));
}

// 0F 9D /0 SETGE r/m8
void x86_setge_al() {
    load_byte(0x0F);
    load_byte(0x9D);
    load_byte(gen_modrm_rr(REGISTER_8BIT::AL, (REGISTER_8BIT)0));
}

// 0F 94 /0 SETE r/m8
void x86_sete_al() {
    load_byte(0x0F);
    load_byte(0x94);
    load_byte(gen_modrm_rr(REGISTER_8BIT::AL, (REGISTER_8BIT)0));
}

// 0F 95 /0 SETNE r/m8
void x86_setne_al() {
    load_byte(0x0F);
    load_byte(0x95);
    load_byte(gen_modrm_rr(REGISTER_8BIT::AL, (REGISTER_8BIT)0));
}

// 0F 95 SETNZ r/m8
void x86_setnz_al() {
    load_byte(0x0F);
    load_byte(0x95);
    load_byte(gen_modrm_rr(REGISTER_8BIT::AL, (REGISTER_8BIT)0));
}

// 0F B6 /r MOVZX r32, r/m8
void x86_movzx_r32_r8_al(REGISTER dest) {
    if (dest >= REGISTER::R8) {
        load_byte(gen_rex_r(WIDE_OFF, dest));
    }
    load_byte(0x0F);
    load_byte(0xB6);
    load_byte(gen_modrm_rr((REGISTER) 0, dest));
}

// 34 ib XOR AL, imm8
void x86_al_flip() {
    load_byte(0x34);
    load_imm8(1);
}

// A8 ib TEST AL, imm8
void x86_test_al_imm8(int x) {
    load_byte(0xA8);
    load_imm8(x);
}

// 74 cb JZ rel8
void x86_jz_rel8(int disp) {
    load_byte(0x74);
    load_imm8(disp);
}

// 0F 84 cd JZ rel32
size_t x86_jz_rel32_missing() {
    load_byte(0x0F);
    load_byte(0x84);
    size_t tmp = p_offset;
    load_imm32(0);

    return tmp;
}

// 0F 85 cd JNZ rel32
size_t x86_jnz_rel32_missing() {
    load_byte(0x0F);
    load_byte(0x85);
    size_t tmp = p_offset;
    load_imm32(0);

    return tmp;
}

// E9 cd JMP rel32
size_t x86_jmp_rel32_missing() {
    load_byte(0xE9);
    size_t tmp = p_offset;
    load_imm32(0);

    return tmp;
}

// 75 cb JNZ rel8
void x86_jnz_rel8(int disp) {
    load_byte(0x75);
    load_imm8(disp);
}

// 0F 85 cd JNZ rel32
void x86_jnz_rel32(int disp) {
    load_byte(0x0F);
    load_byte(0x85);
    load_imm32(disp);
}

// F6 /0 ib TEST r/m8, imm8
void x86_test_rm8_imm8(REGISTER_8BIT rm, int x) {
    load_byte(0xF6);
    load_byte(gen_modrm_rr(rm, (REGISTER_8BIT)0));
    load_imm8(x);
}

size_t move_program_pointer(size_t dx) {
    for (size_t i{}; i < dx; ++i) {
        ++p_offset;
    } 

    return p_offset;
}

size_t get_current_position() {
    return p_offset;
}

// push rbp
// mov rbp, rsp
// sub rsp, k
void emit_function_prologue(const SymbolBucket& scope_stack_frame) {
    x86_pushr64(REGISTER::RBP); 
    x86_mov_rr64(REGISTER::RBP, REGISTER::RSP);
    x86_sub_r64_imm32(
        REGISTER::RSP, 
        Scope::get_size_of_stack_frame(scope_stack_frame)
    );
}

// mov rsp ,rbp
// pop rbp
// ret
void emit_function_epilogue() {
    x86_mov_rr64(REGISTER::RSP, REGISTER::RBP);
    x86_popr64(REGISTER::RBP);
    x86_construct_ret();
}

// Add a return instruction and execute program, returns value in the accumulator
int x86_exec() {
    x86_popr64(REGISTER::R15);
    x86_popr64(REGISTER::R12);
    load_byte(0xc3);
    // dump();
    // return 0;
    return ((int(*)(void))prog)();
}

// Reclaim programs memory
int pspace_reclaim() {
    return munmap(prog, PROG_SIZE);
}

