#pragma once
#include <string>
#include <vector>

#include <geek/global.h>
#include <geek/asm/assembler/asm_reg.h>
#include <geek/asm/assembler/asm_op.h>

namespace geek {
class Assembler {
public:
	struct Config {
		bool assert_every_inst = true;
	};

	struct FuncDeleter {
		FuncDeleter() noexcept = default;
		FuncDeleter(const FuncDeleter&) noexcept = default;
		void operator()(void* ptr) const;
	};

	enum ErrorCode : uint32_t;
	class Error;

	Assembler(Arch arch);
	~Assembler();

	Arch GetArch() const;

	const Config& GetConfig() const;
	Config& GetConfig();

	asm_op::Label NewLabel();
	asm_op::Label NewNamedLabel(std::string_view name, asm_op::Label::Type type = asm_op::Label::kGlobal);
	/**
	 * 类似指令使用，在当前位置绑定一个标签
	 */
	Error bind(const asm_op::Label& label);

	size_t CodeSize() const;
	const uint8_t* CodeBuffer() const;

	template<class Func>
	std::unique_ptr<Func, FuncDeleter> PackCodeAsFunctor() const;
	size_t PackCodeTo(uint8_t* ptr, size_t size, uint64_t base_address) const;

    template<class Func, class... Args>
    decltype(auto) InvokeCode(Args&&... args);

	_GEEK_ASM_INST_2X(adc, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(adc, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(adc, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(adc, Mem, Gp);                                    // ANY
	_GEEK_ASM_INST_2X(adc, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(add, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(add, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(add, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(add, Mem, Gp);                                    // ANY
	_GEEK_ASM_INST_2X(add, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(and_, Gp, Gp);                                    // ANY
	_GEEK_ASM_INST_2X(and_, Gp, Mem);                                   // ANY
	_GEEK_ASM_INST_2X(and_, Gp, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(and_, Mem, Gp);                                   // ANY
	_GEEK_ASM_INST_2X(and_, Mem, Imm);                                  // ANY
	_GEEK_ASM_INST_2X(bound, Gp, Mem);									// X86
	_GEEK_ASM_INST_2X(bsf, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(bsf, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(bsr, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(bsr, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_1X(bswap, Gp);										// ANY
	_GEEK_ASM_INST_2X(bt, Gp, Gp);                                       // ANY
	_GEEK_ASM_INST_2X(bt, Gp, Imm);                                      // ANY
	_GEEK_ASM_INST_2X(bt, Mem, Gp);                                      // ANY
	_GEEK_ASM_INST_2X(bt, Mem, Imm);                                     // ANY
	_GEEK_ASM_INST_2X(btc, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(btc, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(btc, Mem, Gp);                                    // ANY
	_GEEK_ASM_INST_2X(btc, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(btr, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(btr, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(btr, Mem, Gp);                                    // ANY
	_GEEK_ASM_INST_2X(btr, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(bts, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(bts, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(bts, Mem, Gp);                                    // ANY
	_GEEK_ASM_INST_2X(bts, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_1X(cbw, Gp_AX);                                      // ANY [EXPLICIT] AX      <- Sign Extend AL
	_GEEK_ASM_INST_2X(cdq, Gp_EDX, Gp_EAX);                             // ANY [EXPLICIT] EDX:EAX <- Sign Extend EAX
	_GEEK_ASM_INST_1X(cdqe, Gp_EAX);                                   // X64 [EXPLICIT] RAX     <- Sign Extend EAX
	_GEEK_ASM_INST_2X(cqo, Gp_RDX, Gp_RAX);                             // X64 [EXPLICIT] RDX:RAX <- Sign Extend RAX
	_GEEK_ASM_INST_2X(cwd, Gp_DX, Gp_AX);                               // ANY [EXPLICIT] DX:AX   <- Sign Extend AX
	_GEEK_ASM_INST_1X(cwde, Gp_EAX);                                   // ANY [EXPLICIT] EAX     <- Sign Extend AX
	_GEEK_ASM_INST_1X(call, Gp);                                       // ANY
	_GEEK_ASM_INST_1X(call, Mem);                                      // ANY
	_GEEK_ASM_INST_1X(call, Label);                                    // ANY
	_GEEK_ASM_INST_1X(call, Imm);                                      // ANY

	_GEEK_ASM_INST_2X(cmp, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(cmp, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(cmp, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(cmp, Mem, Gp);                                    // ANY
	_GEEK_ASM_INST_2X(cmp, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(cmps, DS_ZSI, ES_ZDI);                           // ANY [EXPLICIT]

	_GEEK_ASM_INST_1X(dec, Gp);                                         // ANY
	_GEEK_ASM_INST_1X(dec, Mem);                                        // ANY
	_GEEK_ASM_INST_2X(div, Gp, Gp);                                     // ANY [EXPLICIT]  AH[Rem]: AL[Quot] <- AX / r8
	_GEEK_ASM_INST_2X(div, Gp, Mem);                                    // ANY [EXPLICIT]  AH[Rem]: AL[Quot] <- AX / m8
	_GEEK_ASM_INST_3X(div, Gp, Gp, Gp);                                 // ANY [EXPLICIT] xDX[Rem]:xAX[Quot] <- xDX:xAX / r16|r32|r64
	_GEEK_ASM_INST_3X(div, Gp, Gp, Mem);                                // ANY [EXPLICIT] xDX[Rem]:xAX[Quot] <- xDX:xAX / m16|m32|m64
	_GEEK_ASM_INST_2X(idiv, Gp, Gp);                                   // ANY [EXPLICIT]  AH[Rem]: AL[Quot] <- AX / r8
	_GEEK_ASM_INST_2X(idiv, Gp, Mem);                                  // ANY [EXPLICIT]  AH[Rem]: AL[Quot] <- AX / m8
	_GEEK_ASM_INST_3X(idiv, Gp, Gp, Gp);                               // ANY [EXPLICIT] xDX[Rem]:xAX[Quot] <- xDX:xAX / r16|r32|r64
	_GEEK_ASM_INST_3X(idiv, Gp, Gp, Mem);                              // ANY [EXPLICIT] xDX[Rem]:xAX[Quot] <- xDX:xAX / m16|m32|m64
	_GEEK_ASM_INST_2X(imul, Gp, Gp);                                   // ANY [EXPLICIT] AX <- AL * r8 | ra <- ra * rb
	_GEEK_ASM_INST_2X(imul, Gp, Mem);                                  // ANY [EXPLICIT] AX <- AL * m8 | ra <- ra * m16|m32|m64
	_GEEK_ASM_INST_3X(imul, Gp, Gp, Imm);                              // ANY
	_GEEK_ASM_INST_3X(imul, Gp, Mem, Imm);                             // ANY
	_GEEK_ASM_INST_3X(imul, Gp, Gp, Gp);                               // ANY [EXPLICIT] xDX:xAX <- xAX * r16|r32|r64
	_GEEK_ASM_INST_3X(imul, Gp, Gp, Mem);                              // ANY [EXPLICIT] xDX:xAX <- xAX * m16|m32|m64
	_GEEK_ASM_INST_1X(inc, Gp);                                         // ANY
	_GEEK_ASM_INST_1X(inc, Mem);                                        // ANY
	_GEEK_ASM_INST_1C(j, Label);										 // ANY
	_GEEK_ASM_INST_1C(j, Imm);										// ANY
	_GEEK_ASM_INST_2X(jecxz, Gp, Label);                              // ANY [EXPLICIT] Short jump if CX/ECX/RCX is zero.
	_GEEK_ASM_INST_2X(jecxz, Gp, Imm);                                // ANY [EXPLICIT] Short jump if CX/ECX/RCX is zero.
	_GEEK_ASM_INST_1X(jmp, Gp);                                         // ANY
	_GEEK_ASM_INST_1X(jmp, Mem);                                        // ANY
	_GEEK_ASM_INST_1X(jmp, Label);                                      // ANY
	_GEEK_ASM_INST_1X(jmp, Imm);                                        // ANY
	_GEEK_ASM_INST_2X(lcall, Imm, Imm);                               // ANY
	_GEEK_ASM_INST_1X(lcall, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(lea, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(ljmp, Imm, Imm);                                 // ANY
	_GEEK_ASM_INST_1X(ljmp, Mem);                                      // ANY
	_GEEK_ASM_INST_2X(lods, Gp_ZAX, DS_ZSI);                           // ANY [EXPLICIT]
	_GEEK_ASM_INST_2X(loop, Gp_ZCX, Label);                            // ANY [EXPLICIT] Decrement xCX; short jump if xCX != 0.
	_GEEK_ASM_INST_2X(loop, Gp_ZCX, Imm);                              // ANY [EXPLICIT] Decrement xCX; short jump if xCX != 0.
	_GEEK_ASM_INST_2X(loope, Gp_ZCX, Label);                          // ANY [EXPLICIT] Decrement xCX; short jump if xCX != 0 && ZF == 1.
	_GEEK_ASM_INST_2X(loope, Gp_ZCX, Imm);                            // ANY [EXPLICIT] Decrement xCX; short jump if xCX != 0 && ZF == 1.
	_GEEK_ASM_INST_2X(loopne, Gp_ZCX, Label);                        // ANY [EXPLICIT] Decrement xCX; short jump if xCX != 0 && ZF == 0.
	_GEEK_ASM_INST_2X(loopne, Gp_ZCX, Imm);                          // ANY [EXPLICIT] Decrement xCX; short jump if xCX != 0 && ZF == 0.
	_GEEK_ASM_INST_2X(mov, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(mov, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(mov, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(mov, Mem, Gp);                                    // ANY
	_GEEK_ASM_INST_2X(mov, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(mov, Gp, CReg);                                   // ANY
	_GEEK_ASM_INST_2X(mov, CReg, Gp);                                   // ANY
	_GEEK_ASM_INST_2X(mov, Gp, DReg);                                   // ANY
	_GEEK_ASM_INST_2X(mov, DReg, Gp);                                   // ANY
	_GEEK_ASM_INST_2X(mov, Gp, SReg);                                   // ANY
	_GEEK_ASM_INST_2X(mov, Mem, SReg);                                  // ANY
	_GEEK_ASM_INST_2X(mov, SReg, Gp);                                   // ANY
	_GEEK_ASM_INST_2X(mov, SReg, Mem);                                  // ANY
	_GEEK_ASM_INST_2X(movabs, Gp, Mem);                              // X64
	_GEEK_ASM_INST_2X(movabs, Gp, Imm);                              // X64
	_GEEK_ASM_INST_2X(movabs, Mem, Gp);                              // X64

	_GEEK_ASM_INST_2X(movs, ES_ZDI, DS_ZSI);                           // ANY [EXPLICIT]
	_GEEK_ASM_INST_2X(movsx, Gp, Gp);                                 // ANY
	_GEEK_ASM_INST_2X(movsx, Gp, Mem);                                // ANY
	_GEEK_ASM_INST_2X(movsxd, Gp, Gp);                               // X64
	_GEEK_ASM_INST_2X(movsxd, Gp, Mem);                              // X64
	_GEEK_ASM_INST_2X(movzx, Gp, Gp);                                 // ANY
	_GEEK_ASM_INST_2X(movzx, Gp, Mem);                                // ANY
	_GEEK_ASM_INST_2X(mul, Gp_AX, Gp);                                  // ANY [EXPLICIT] AX      <-  AL * r8
	_GEEK_ASM_INST_2X(mul, Gp_AX, Mem);                                 // ANY [EXPLICIT] AX      <-  AL * m8
	_GEEK_ASM_INST_3X(mul, Gp_ZDX, Gp_ZAX, Gp);                         // ANY [EXPLICIT] xDX:xAX <- xAX * r16|r32|r64
	_GEEK_ASM_INST_3X(mul, Gp_ZDX, Gp_ZAX, Mem);                        // ANY [EXPLICIT] xDX:xAX <- xAX * m16|m32|m64
	_GEEK_ASM_INST_1X(neg, Gp);                                         // ANY
	_GEEK_ASM_INST_1X(neg, Mem);                                        // ANY
	_GEEK_ASM_INST_0X(nop);                                             // ANY
	_GEEK_ASM_INST_1X(nop, Gp);                                         // ANY
	_GEEK_ASM_INST_1X(nop, Mem);                                        // ANY
	_GEEK_ASM_INST_2X(nop, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(nop, Mem, Gp);                                    // ANY
	_GEEK_ASM_INST_1X(not_, Gp);                                        // ANY
	_GEEK_ASM_INST_1X(not_, Mem);                                       // ANY
	_GEEK_ASM_INST_2X(or_, Gp, Gp);                                      // ANY
	_GEEK_ASM_INST_2X(or_, Gp, Mem);                                     // ANY
	_GEEK_ASM_INST_2X(or_, Gp, Imm);                                     // ANY
	_GEEK_ASM_INST_2X(or_, Mem, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(or_, Mem, Imm);                                    // ANY
	_GEEK_ASM_INST_1X(pop, Gp);                                         // ANY
	_GEEK_ASM_INST_1X(pop, Mem);                                        // ANY
	_GEEK_ASM_INST_1X(pop, SReg);;                                      // ANY
	_GEEK_ASM_INST_0X(popa);                                           // X86
	_GEEK_ASM_INST_0X(popad);                                         // X86
	_GEEK_ASM_INST_0X(popf);                                           // ANY
	_GEEK_ASM_INST_0X(popfd);                                         // X86
	_GEEK_ASM_INST_0X(popfq);                                         // X64
	_GEEK_ASM_INST_1X(push, Gp);                                       // ANY
	_GEEK_ASM_INST_1X(push, Mem);                                      // ANY
	_GEEK_ASM_INST_1X(push, SReg);                                     // ANY
	_GEEK_ASM_INST_1X(push, Imm);                                      // ANY
	_GEEK_ASM_INST_0X(pusha);                                         // X86
	_GEEK_ASM_INST_0X(pushad);                                       // X86
	_GEEK_ASM_INST_0X(pushf);                                         // ANY
	_GEEK_ASM_INST_0X(pushfd);                                       // X86
	_GEEK_ASM_INST_0X(pushfq);                                       // X64
	_GEEK_ASM_INST_2X(rcl, Gp, Gp_CL);                                  // ANY
	_GEEK_ASM_INST_2X(rcl, Mem, Gp_CL);                                 // ANY
	_GEEK_ASM_INST_2X(rcl, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(rcl, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(rcr, Gp, Gp_CL);                                  // ANY
	_GEEK_ASM_INST_2X(rcr, Mem, Gp_CL);                                 // ANY
	_GEEK_ASM_INST_2X(rcr, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(rcr, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(rol, Gp, Gp_CL);                                  // ANY
	_GEEK_ASM_INST_2X(rol, Mem, Gp_CL);                                 // ANY
	_GEEK_ASM_INST_2X(rol, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(rol, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(ror, Gp, Gp_CL);                                  // ANY
	_GEEK_ASM_INST_2X(ror, Mem, Gp_CL);                                 // ANY
	_GEEK_ASM_INST_2X(ror, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(ror, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(sbb, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(sbb, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(sbb, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(sbb, Mem, Gp);                                    // ANY
	_GEEK_ASM_INST_2X(sbb, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(sal, Gp, Gp_CL);                                  // ANY
	_GEEK_ASM_INST_2X(sal, Mem, Gp_CL);                                 // ANY
	_GEEK_ASM_INST_2X(sal, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(sal, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(sar, Gp, Gp_CL);                                  // ANY
	_GEEK_ASM_INST_2X(sar, Mem, Gp_CL);                                 // ANY
	_GEEK_ASM_INST_2X(sar, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(sar, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(scas, Gp_ZAX, ES_ZDI);                           // ANY [EXPLICIT]
	_GEEK_ASM_INST_1C(set, Gp);
	_GEEK_ASM_INST_1C(set, Mem);
	_GEEK_ASM_INST_2X(shl, Gp, Gp_CL);                                  // ANY
	_GEEK_ASM_INST_2X(shl, Mem, Gp_CL);                                 // ANY
	_GEEK_ASM_INST_2X(shl, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(shl, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(shr, Gp, Gp_CL);                                  // ANY
	_GEEK_ASM_INST_2X(shr, Mem, Gp_CL);                                 // ANY
	_GEEK_ASM_INST_2X(shr, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(shr, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_3X(shld, Gp, Gp, Gp_CL);                            // ANY
	_GEEK_ASM_INST_3X(shld, Mem, Gp, Gp_CL);                           // ANY
	_GEEK_ASM_INST_3X(shld, Gp, Gp, Imm);                              // ANY
	_GEEK_ASM_INST_3X(shld, Mem, Gp, Imm);                             // ANY
	_GEEK_ASM_INST_3X(shrd, Gp, Gp, Gp_CL);                            // ANY
	_GEEK_ASM_INST_3X(shrd, Mem, Gp, Gp_CL);                           // ANY
	_GEEK_ASM_INST_3X(shrd, Gp, Gp, Imm);                              // ANY
	_GEEK_ASM_INST_3X(shrd, Mem, Gp, Imm);                             // ANY
	_GEEK_ASM_INST_2X(stos, ES_ZDI, Gp_ZAX);                           // ANY [EXPLICIT]
	_GEEK_ASM_INST_2X(sub, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(sub, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(sub, Gp, Imm);                                    // ANY
	_GEEK_ASM_INST_2X(sub, Mem, Gp);                                    // ANY
	_GEEK_ASM_INST_2X(sub, Mem, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(test, Gp, Gp);                                   // ANY
	_GEEK_ASM_INST_2X(test, Gp, Imm);                                  // ANY
	_GEEK_ASM_INST_2X(test, Mem, Gp);                                  // ANY
	_GEEK_ASM_INST_2X(test, Mem, Imm);                                 // ANY
	_GEEK_ASM_INST_2X(ud0, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(ud0, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(ud1, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(ud1, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_0X(ud2);                                             // ANY
	_GEEK_ASM_INST_2X(xadd, Gp, Gp);                                   // ANY
	_GEEK_ASM_INST_2X(xadd, Mem, Gp);                                  // ANY
	_GEEK_ASM_INST_2X(xchg, Gp, Gp);                                   // ANY
	_GEEK_ASM_INST_2X(xchg, Mem, Gp);                                  // ANY
	_GEEK_ASM_INST_2X(xchg, Gp, Mem);                                  // ANY
	_GEEK_ASM_INST_2X(xor_, Gp, Gp);                                    // ANY
	_GEEK_ASM_INST_2X(xor_, Gp, Mem);                                   // ANY
	_GEEK_ASM_INST_2X(xor_, Gp, Imm);                                   // ANY
	_GEEK_ASM_INST_2X(xor_, Mem, Gp);                                   // ANY
	_GEEK_ASM_INST_2X(xor_, Mem, Imm);                                  // ANY

	//! \name Deprecated 32-bit Instructions
	//! \{

	_GEEK_ASM_INST_1X(aaa, Gp);                                         // X86 [EXPLICIT]
	_GEEK_ASM_INST_2X(aad, Gp, Imm);                                    // X86 [EXPLICIT]
	_GEEK_ASM_INST_2X(aam, Gp, Imm);                                    // X86 [EXPLICIT]
	_GEEK_ASM_INST_1X(aas, Gp);                                         // X86 [EXPLICIT]
	_GEEK_ASM_INST_1X(daa, Gp);                                         // X86 [EXPLICIT]
	_GEEK_ASM_INST_1X(das, Gp);                                         // X86 [EXPLICIT]

	//! \name ENTER/LEAVE Instructions
	//! \{

	_GEEK_ASM_INST_2X(enter, Imm, Imm);                               // ANY
	_GEEK_ASM_INST_0X(leave);                                         // ANY

  //! \name IN/OUT Instructions
  //! \{

  // NOTE: For some reason Doxygen is messed up here and thinks we are in cond.

	_GEEK_ASM_INST_2X(in, Gp_ZAX, Imm);                                  // ANY
	_GEEK_ASM_INST_2X(in, Gp_ZAX, Gp_DX);                                // ANY
	_GEEK_ASM_INST_2X(ins, ES_ZDI, Gp_DX);                              // ANY
	_GEEK_ASM_INST_2X(out, Imm, Gp_ZAX);                                // ANY
	_GEEK_ASM_INST_2X(out, Gp_DX, Gp_ZAX);                              // ANY
	_GEEK_ASM_INST_2X(outs, Gp_DX, DS_ZSI);                            // ANY

	//! \name Clear/Set CF/DF Instructions
	//! \{

	_GEEK_ASM_INST_0X(clc);                                             // ANY
	_GEEK_ASM_INST_0X(cld);                                             // ANY
	_GEEK_ASM_INST_0X(cmc);                                             // ANY
	_GEEK_ASM_INST_0X(stc);                                             // ANY
	_GEEK_ASM_INST_0X(std);                                             // ANY
	//! \name Other User-Mode Instructions
//! \{

	_GEEK_ASM_INST_2X(arpl, Gp, Gp);                                   // X86
	_GEEK_ASM_INST_2X(arpl, Mem, Gp);                                  // X86
	_GEEK_ASM_INST_0X(cli);                                             // ANY
	_GEEK_ASM_INST_0X(getsec);                                       // SMX
	_GEEK_ASM_INST_1X(int_, Imm);                                       // ANY
	_GEEK_ASM_INST_0X(int3);                                           // ANY
	_GEEK_ASM_INST_0X(into);                                           // ANY
	_GEEK_ASM_INST_2X(lar, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(lar, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(lds, Gp, Mem);                                    // X86
	_GEEK_ASM_INST_2X(les, Gp, Mem);                                    // X86
	_GEEK_ASM_INST_2X(lfs, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(lgs, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(lsl, Gp, Gp);                                     // ANY
	_GEEK_ASM_INST_2X(lsl, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_2X(lss, Gp, Mem);                                    // ANY
	_GEEK_ASM_INST_0X(pause);                                         // SSE2
	_GEEK_ASM_INST_0X(rsm);                                             // X86
	_GEEK_ASM_INST_1X(sgdt, Mem);                                      // ANY
	_GEEK_ASM_INST_1X(sidt, Mem);                                      // ANY
	_GEEK_ASM_INST_1X(sldt, Gp);                                       // ANY
	_GEEK_ASM_INST_1X(sldt, Mem);                                      // ANY
	_GEEK_ASM_INST_1X(smsw, Gp);                                       // ANY
	_GEEK_ASM_INST_1X(smsw, Mem);                                      // ANY
	_GEEK_ASM_INST_0X(sti);                                             // ANY
	_GEEK_ASM_INST_1X(str, Gp);                                         // ANY
	_GEEK_ASM_INST_1X(str, Mem);                                        // ANY
	_GEEK_ASM_INST_1X(verr, Gp);                                       // ANY
	_GEEK_ASM_INST_1X(verr, Mem);                                      // ANY
	_GEEK_ASM_INST_1X(verw, Gp);                                       // ANY
	_GEEK_ASM_INST_1X(verw, Mem);                                      // ANY

	//! \name Core Privileged Instructions
	//! \{

	_GEEK_ASM_INST_0X(clts);                                           // ANY
	_GEEK_ASM_INST_0X(hlt);                                             // ANY
	_GEEK_ASM_INST_0X(invd);                                           // ANY
	_GEEK_ASM_INST_1X(invlpg, Mem);                                  // ANY
	_GEEK_ASM_INST_2X(invpcid, Gp, Mem);                            // ANY
	_GEEK_ASM_INST_1X(lgdt, Mem);                                      // ANY
	_GEEK_ASM_INST_1X(lidt, Mem);                                      // ANY
	_GEEK_ASM_INST_1X(lldt, Gp);                                       // ANY
	_GEEK_ASM_INST_1X(lldt, Mem);                                      // ANY
	_GEEK_ASM_INST_1X(lmsw, Gp);                                       // ANY
	_GEEK_ASM_INST_1X(lmsw, Mem);                                      // ANY
	_GEEK_ASM_INST_1X(ltr, Gp);                                         // ANY
	_GEEK_ASM_INST_1X(ltr, Mem);                                        // ANY
	_GEEK_ASM_INST_3X(rdmsr, Gp_EDX, Gp_EAX, Gp_ECX);                 // MSR       [EXPLICIT] RDX:EAX <- MSR[ECX]
	_GEEK_ASM_INST_3X(rdpmc, Gp_EDX, Gp_EAX, Gp_ECX);                 // ANY       [EXPLICIT] RDX:EAX <- PMC[ECX]
	_GEEK_ASM_INST_0X(swapgs);                                       // X64
	_GEEK_ASM_INST_0X(wbinvd);                                       // ANY
	_GEEK_ASM_INST_0X(wbnoinvd);                                   // WBNOINVD
	_GEEK_ASM_INST_3X(wrmsr, Gp_EDX, Gp_EAX, Gp_ECX);                 // MSR       [EXPLICIT] RDX:EAX  -> MSR[ECX]
	_GEEK_ASM_INST_3X(xsetbv, Gp_EDX, Gp_EAX, Gp_ECX);               // XSAVE     [EXPLICIT] XCR[ECX] <- EDX:EAX


	_GEEK_ASM_INST_0X(cbw);                                             // ANY       [IMPLICIT] AX      <- Sign Extend AL
	_GEEK_ASM_INST_0X(cdq);                                             // ANY       [IMPLICIT] EDX:EAX <- Sign Extend EAX
	_GEEK_ASM_INST_0X(cdqe);                                           // X64       [IMPLICIT] RAX     <- Sign Extend EAX
	_GEEK_ASM_INST_2X(cmpxchg, Gp, Gp);                             // I486      [IMPLICIT]
	_GEEK_ASM_INST_2X(cmpxchg, Mem, Gp);                            // I486      [IMPLICIT]
	_GEEK_ASM_INST_1X(cmpxchg16b, Mem);                          // CMPXCHG8B [IMPLICIT] m == RDX:RAX ? m <- RCX:RBX
	_GEEK_ASM_INST_1X(cmpxchg8b, Mem);                            // CMPXCHG16B[IMPLICIT] m == EDX:EAX ? m <- ECX:EBX
	_GEEK_ASM_INST_0X(cqo);                                             // X64       [IMPLICIT] RDX:RAX <- Sign Extend RAX
	_GEEK_ASM_INST_0X(cwd);                                             // ANY       [IMPLICIT] DX:AX   <- Sign Extend AX
	_GEEK_ASM_INST_0X(cwde);                                           // ANY       [IMPLICIT] EAX     <- Sign Extend AX
	_GEEK_ASM_INST_1X(div, Gp);                                         // ANY       [IMPLICIT] {AH[Rem]: AL[Quot] <- AX / r8} {xDX[Rem]:xAX[Quot] <- DX:AX / r16|r32|r64}
	_GEEK_ASM_INST_1X(div, Mem);                                        // ANY       [IMPLICIT] {AH[Rem]: AL[Quot] <- AX / m8} {xDX[Rem]:xAX[Quot] <- DX:AX / m16|m32|m64}
	_GEEK_ASM_INST_1X(idiv, Gp);                                       // ANY       [IMPLICIT] {AH[Rem]: AL[Quot] <- AX / r8} {xDX[Rem]:xAX[Quot] <- DX:AX / r16|r32|r64}
	_GEEK_ASM_INST_1X(idiv, Mem);                                      // ANY       [IMPLICIT] {AH[Rem]: AL[Quot] <- AX / m8} {xDX[Rem]:xAX[Quot] <- DX:AX / m16|m32|m64}
	_GEEK_ASM_INST_1X(imul, Gp);                                       // ANY       [IMPLICIT] {AX <- AL * r8} {xAX:xDX <- xAX * r16|r32|r64}
	_GEEK_ASM_INST_1X(imul, Mem);                                      // ANY       [IMPLICIT] {AX <- AL * m8} {xAX:xDX <- xAX * m16|m32|m64}
	_GEEK_ASM_INST_0X(iret);                                           // ANY       [IMPLICIT]
	_GEEK_ASM_INST_0X(iretd);                                         // ANY       [IMPLICIT]
	_GEEK_ASM_INST_0X(iretq);                                         // X64       [IMPLICIT]
	_GEEK_ASM_INST_1X(jecxz, Label);                                  // ANY       [IMPLICIT] Short jump if CX/ECX/RCX is zero.
	_GEEK_ASM_INST_1X(jecxz, Imm);                                    // ANY       [IMPLICIT] Short jump if CX/ECX/RCX is zero.
	_GEEK_ASM_INST_1X(loop, Label);                                    // ANY       [IMPLICIT] Decrement xCX; short jump if xCX != 0.
	_GEEK_ASM_INST_1X(loop, Imm);                                      // ANY       [IMPLICIT] Decrement xCX; short jump if xCX != 0.
	_GEEK_ASM_INST_1X(loope, Label);                                  // ANY       [IMPLICIT] Decrement xCX; short jump if xCX != 0 && ZF == 1.
	_GEEK_ASM_INST_1X(loope, Imm);                                    // ANY       [IMPLICIT] Decrement xCX; short jump if xCX != 0 && ZF == 1.
	_GEEK_ASM_INST_1X(loopne, Label);                                // ANY       [IMPLICIT] Decrement xCX; short jump if xCX != 0 && ZF == 0.
	_GEEK_ASM_INST_1X(loopne, Imm);                                  // ANY       [IMPLICIT] Decrement xCX; short jump if xCX != 0 && ZF == 0.
	_GEEK_ASM_INST_1X(mul, Gp);                                         // ANY       [IMPLICIT] {AX <- AL * r8} {xDX:xAX <- xAX * r16|r32|r64}
	_GEEK_ASM_INST_1X(mul, Mem);                                        // ANY       [IMPLICIT] {AX <- AL * m8} {xDX:xAX <- xAX * m16|m32|m64}
	_GEEK_ASM_INST_0X(ret);
	_GEEK_ASM_INST_1X(ret, Imm);
	_GEEK_ASM_INST_0X(retf);
	_GEEK_ASM_INST_1X(retf, Imm);
	_GEEK_ASM_INST_0X(xlatb);                                         // ANY       [IMPLICIT]


    //! \name FPU Instructions
    //! \{

    _GEEK_ASM_INST_0X(f2xm1);                                         // FPU
	_GEEK_ASM_INST_0X(fabs);                                           // FPU
	_GEEK_ASM_INST_2X(fadd, St, St);                                   // FPU
	_GEEK_ASM_INST_1X(fadd, Mem);                                      // FPU
	_GEEK_ASM_INST_1X(faddp, St);                                     // FPU
	_GEEK_ASM_INST_0X(faddp);                                         // FPU
	_GEEK_ASM_INST_1X(fbld, Mem);                                      // FPU
	_GEEK_ASM_INST_1X(fbstp, Mem);                                    // FPU
	_GEEK_ASM_INST_0X(fchs);                                           // FPU
	_GEEK_ASM_INST_0X(fclex);                                         // FPU
	_GEEK_ASM_INST_1X(fcmovb, St);                                   // FPU
	_GEEK_ASM_INST_1X(fcmovbe, St);                                 // FPU
	_GEEK_ASM_INST_1X(fcmove, St);                                   // FPU
	_GEEK_ASM_INST_1X(fcmovnb, St);                                 // FPU
	_GEEK_ASM_INST_1X(fcmovnbe, St);                               // FPU
	_GEEK_ASM_INST_1X(fcmovne, St);                                 // FPU
	_GEEK_ASM_INST_1X(fcmovnu, St);                                 // FPU
	_GEEK_ASM_INST_1X(fcmovu, St);                                   // FPU
	_GEEK_ASM_INST_1X(fcom, St);                                       // FPU
	_GEEK_ASM_INST_0X(fcom);                                           // FPU
	_GEEK_ASM_INST_1X(fcom, Mem);                                      // FPU
	_GEEK_ASM_INST_1X(fcomp, St);                                     // FPU
	_GEEK_ASM_INST_0X(fcomp);                                         // FPU
	_GEEK_ASM_INST_1X(fcomp, Mem);                                    // FPU
	_GEEK_ASM_INST_0X(fcompp);                                       // FPU
	_GEEK_ASM_INST_1X(fcomi, St);                                     // FPU
	_GEEK_ASM_INST_1X(fcomip, St);                                   // FPU
	_GEEK_ASM_INST_0X(fcos);                                           // FPU
	_GEEK_ASM_INST_0X(fdecstp);                                     // FPU
	_GEEK_ASM_INST_2X(fdiv, St, St);                                   // FPU
	_GEEK_ASM_INST_1X(fdiv, Mem);                                      // FPU
	_GEEK_ASM_INST_1X(fdivp, St);                                     // FPU
	_GEEK_ASM_INST_0X(fdivp);                                         // FPU
	_GEEK_ASM_INST_2X(fdivr, St, St);                                 // FPU
	_GEEK_ASM_INST_1X(fdivr, Mem);                                    // FPU
	_GEEK_ASM_INST_1X(fdivrp, St);                                   // FPU
	_GEEK_ASM_INST_0X(fdivrp);                                       // FPU
	_GEEK_ASM_INST_1X(ffree, St);                                     // FPU
	_GEEK_ASM_INST_1X(fiadd, Mem);                                    // FPU
	_GEEK_ASM_INST_1X(ficom, Mem);                                    // FPU
	_GEEK_ASM_INST_1X(ficomp, Mem);                                  // FPU
	_GEEK_ASM_INST_1X(fidiv, Mem);                                    // FPU
	_GEEK_ASM_INST_1X(fidivr, Mem);                                  // FPU
	_GEEK_ASM_INST_1X(fild, Mem);                                      // FPU
	_GEEK_ASM_INST_1X(fimul, Mem);                                    // FPU
	_GEEK_ASM_INST_0X(fincstp);                                     // FPU
	_GEEK_ASM_INST_0X(finit);                                         // FPU
	_GEEK_ASM_INST_1X(fisub, Mem);                                    // FPU
	_GEEK_ASM_INST_1X(fisubr, Mem);                                  // FPU
	_GEEK_ASM_INST_0X(fninit);                                       // FPU
	_GEEK_ASM_INST_1X(fist, Mem);                                      // FPU
	_GEEK_ASM_INST_1X(fistp, Mem);                                    // FPU
	_GEEK_ASM_INST_1X(fisttp, Mem);                                  // FPU+SSE3
	_GEEK_ASM_INST_1X(fld, Mem);                                        // FPU
	_GEEK_ASM_INST_1X(fld, St);                                         // FPU
	_GEEK_ASM_INST_0X(fld1);                                           // FPU
	_GEEK_ASM_INST_0X(fldl2t);                                       // FPU
	_GEEK_ASM_INST_0X(fldl2e);                                       // FPU
	_GEEK_ASM_INST_0X(fldpi);                                         // FPU
	_GEEK_ASM_INST_0X(fldlg2);                                       // FPU
	_GEEK_ASM_INST_0X(fldln2);                                       // FPU
	_GEEK_ASM_INST_0X(fldz);                                           // FPU
	_GEEK_ASM_INST_1X(fldcw, Mem);                                    // FPU
	_GEEK_ASM_INST_1X(fldenv, Mem);                                  // FPU
	_GEEK_ASM_INST_2X(fmul, St, St);                                   // FPU
	_GEEK_ASM_INST_1X(fmul, Mem);                                      // FPU
	_GEEK_ASM_INST_1X(fmulp, St);                                     // FPU
	_GEEK_ASM_INST_0X(fmulp);                                         // FPU
	_GEEK_ASM_INST_0X(fnclex);                                       // FPU
	_GEEK_ASM_INST_0X(fnop);                                           // FPU
	_GEEK_ASM_INST_1X(fnsave, Mem);                                  // FPU
	_GEEK_ASM_INST_1X(fnstenv, Mem);                                // FPU
	_GEEK_ASM_INST_1X(fnstcw, Mem);                                  // FPU
	_GEEK_ASM_INST_0X(fpatan);                                       // FPU
	_GEEK_ASM_INST_0X(fprem);                                         // FPU
	_GEEK_ASM_INST_0X(fprem1);                                       // FPU
	_GEEK_ASM_INST_0X(fptan);                                         // FPU
	_GEEK_ASM_INST_0X(frndint);                                     // FPU
	_GEEK_ASM_INST_1X(frstor, Mem);                                  // FPU
	_GEEK_ASM_INST_1X(fsave, Mem);                                    // FPU
	_GEEK_ASM_INST_0X(fscale);                                       // FPU
	_GEEK_ASM_INST_0X(fsin);                                           // FPU
	_GEEK_ASM_INST_0X(fsincos);                                     // FPU
	_GEEK_ASM_INST_0X(fsqrt);                                         // FPU
	_GEEK_ASM_INST_1X(fst, Mem);                                        // FPU
	_GEEK_ASM_INST_1X(fst, St);                                         // FPU
	_GEEK_ASM_INST_1X(fstp, Mem);                                      // FPU
	_GEEK_ASM_INST_1X(fstp, St);                                       // FPU
	_GEEK_ASM_INST_1X(fstcw, Mem);                                    // FPU
	_GEEK_ASM_INST_1X(fstenv, Mem);                                  // FPU
	_GEEK_ASM_INST_2X(fsub, St, St);                                   // FPU
	_GEEK_ASM_INST_1X(fsub, Mem);                                      // FPU
	_GEEK_ASM_INST_1X(fsubp, St);                                     // FPU
	_GEEK_ASM_INST_0X(fsubp);                                         // FPU
	_GEEK_ASM_INST_2X(fsubr, St, St);                                 // FPU
	_GEEK_ASM_INST_1X(fsubr, Mem);                                    // FPU
	_GEEK_ASM_INST_1X(fsubrp, St);                                   // FPU
	_GEEK_ASM_INST_0X(fsubrp);                                       // FPU
	_GEEK_ASM_INST_0X(ftst);                                           // FPU
	_GEEK_ASM_INST_1X(fucom, St);                                     // FPU
	_GEEK_ASM_INST_0X(fucom);                                         // FPU
	_GEEK_ASM_INST_1X(fucomi, St);                                   // FPU
	_GEEK_ASM_INST_1X(fucomip, St);                                 // FPU
	_GEEK_ASM_INST_1X(fucomp, St);                                   // FPU
	_GEEK_ASM_INST_0X(fucomp);                                       // FPU
	_GEEK_ASM_INST_0X(fucompp);                                     // FPU
	_GEEK_ASM_INST_0X(fwait);                                         // FPU
	_GEEK_ASM_INST_0X(fxam);                                           // FPU
	_GEEK_ASM_INST_1X(fxch, St);                                       // FPU
	_GEEK_ASM_INST_0X(fxtract);                                     // FPU
	_GEEK_ASM_INST_0X(fyl2x);                                         // FPU
	_GEEK_ASM_INST_0X(fyl2xp1);                                     // FPU
	_GEEK_ASM_INST_1X(fstsw, Gp);                                     // FPU
	_GEEK_ASM_INST_1X(fstsw, Mem);                                    // FPU
	_GEEK_ASM_INST_1X(fnstsw, Gp);                                   // FPU
	_GEEK_ASM_INST_1X(fnstsw, Mem);                                  // FPU

    //! \name MMX & SSE+ Instructions
    //! \{

    _GEEK_ASM_INST_2X(addpd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(addpd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(addps, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(addps, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(addsd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(addsd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(addss, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(addss, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(addsubpd, Xmm, Xmm);                         // SSE3
    _GEEK_ASM_INST_2X(addsubpd, Xmm, Mem);                         // SSE3
    _GEEK_ASM_INST_2X(addsubps, Xmm, Xmm);                         // SSE3
    _GEEK_ASM_INST_2X(addsubps, Xmm, Mem);                         // SSE3
    _GEEK_ASM_INST_2X(andnpd, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(andnpd, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(andnps, Xmm, Xmm);                             // SSE
    _GEEK_ASM_INST_2X(andnps, Xmm, Mem);                             // SSE
    _GEEK_ASM_INST_2X(andpd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(andpd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(andps, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(andps, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_3X(blendpd, Xmm, Xmm, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(blendpd, Xmm, Mem, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(blendps, Xmm, Xmm, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(blendps, Xmm, Mem, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(blendvpd, Xmm, Xmm, XMM0);                   // SSE4_1 [EXPLICIT]
    _GEEK_ASM_INST_3X(blendvpd, Xmm, Mem, XMM0);                   // SSE4_1 [EXPLICIT]
    _GEEK_ASM_INST_3X(blendvps, Xmm, Xmm, XMM0);                   // SSE4_1 [EXPLICIT]
    _GEEK_ASM_INST_3X(blendvps, Xmm, Mem, XMM0);                   // SSE4_1 [EXPLICIT]
    _GEEK_ASM_INST_3X(cmppd, Xmm, Xmm, Imm);                          // SSE2
    _GEEK_ASM_INST_3X(cmppd, Xmm, Mem, Imm);                          // SSE2
    _GEEK_ASM_INST_3X(cmpps, Xmm, Xmm, Imm);                          // SSE
    _GEEK_ASM_INST_3X(cmpps, Xmm, Mem, Imm);                          // SSE
    _GEEK_ASM_INST_3X(cmpsd, Xmm, Xmm, Imm);                          // SSE2
    _GEEK_ASM_INST_3X(cmpsd, Xmm, Mem, Imm);                          // SSE2
    _GEEK_ASM_INST_3X(cmpss, Xmm, Xmm, Imm);                          // SSE
    _GEEK_ASM_INST_3X(cmpss, Xmm, Mem, Imm);                          // SSE
    _GEEK_ASM_INST_2X(comisd, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(comisd, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(comiss, Xmm, Xmm);                             // SSE
    _GEEK_ASM_INST_2X(comiss, Xmm, Mem);                             // SSE
    _GEEK_ASM_INST_2X(cvtdq2pd, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(cvtdq2pd, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(cvtdq2ps, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(cvtdq2ps, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(cvtpd2dq, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(cvtpd2dq, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(cvtpd2pi, Mm, Xmm);                          // SSE2
    _GEEK_ASM_INST_2X(cvtpd2pi, Mm, Mem);                          // SSE2
    _GEEK_ASM_INST_2X(cvtpd2ps, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(cvtpd2ps, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(cvtpi2pd, Xmm, Mm);                          // SSE2
    _GEEK_ASM_INST_2X(cvtpi2pd, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(cvtpi2ps, Xmm, Mm);                          // SSE
    _GEEK_ASM_INST_2X(cvtpi2ps, Xmm, Mem);                         // SSE
    _GEEK_ASM_INST_2X(cvtps2dq, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(cvtps2dq, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(cvtps2pd, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(cvtps2pd, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(cvtps2pi, Mm, Xmm);                          // SSE
    _GEEK_ASM_INST_2X(cvtps2pi, Mm, Mem);                          // SSE
    _GEEK_ASM_INST_2X(cvtsd2si, Gp, Xmm);                          // SSE2
    _GEEK_ASM_INST_2X(cvtsd2si, Gp, Mem);                          // SSE2
    _GEEK_ASM_INST_2X(cvtsd2ss, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(cvtsd2ss, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(cvtsi2sd, Xmm, Gp);                          // SSE2
    _GEEK_ASM_INST_2X(cvtsi2sd, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(cvtsi2ss, Xmm, Gp);                          // SSE
    _GEEK_ASM_INST_2X(cvtsi2ss, Xmm, Mem);                         // SSE
    _GEEK_ASM_INST_2X(cvtss2sd, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(cvtss2sd, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(cvtss2si, Gp, Xmm);                          // SSE
    _GEEK_ASM_INST_2X(cvtss2si, Gp, Mem);                          // SSE
    _GEEK_ASM_INST_2X(cvttpd2pi, Mm, Xmm);                        // SSE2
    _GEEK_ASM_INST_2X(cvttpd2pi, Mm, Mem);                        // SSE2
    _GEEK_ASM_INST_2X(cvttpd2dq, Xmm, Xmm);                       // SSE2
    _GEEK_ASM_INST_2X(cvttpd2dq, Xmm, Mem);                       // SSE2
    _GEEK_ASM_INST_2X(cvttps2dq, Xmm, Xmm);                       // SSE2
    _GEEK_ASM_INST_2X(cvttps2dq, Xmm, Mem);                       // SSE2
    _GEEK_ASM_INST_2X(cvttps2pi, Mm, Xmm);                        // SSE
    _GEEK_ASM_INST_2X(cvttps2pi, Mm, Mem);                        // SSE
    _GEEK_ASM_INST_2X(cvttsd2si, Gp, Xmm);                        // SSE2
    _GEEK_ASM_INST_2X(cvttsd2si, Gp, Mem);                        // SSE2
    _GEEK_ASM_INST_2X(cvttss2si, Gp, Xmm);                        // SSE
    _GEEK_ASM_INST_2X(cvttss2si, Gp, Mem);                        // SSE
    _GEEK_ASM_INST_2X(divpd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(divpd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(divps, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(divps, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(divsd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(divsd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(divss, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(divss, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_3X(dppd, Xmm, Xmm, Imm);                            // SSE4_1
    _GEEK_ASM_INST_3X(dppd, Xmm, Mem, Imm);                            // SSE4_1
    _GEEK_ASM_INST_3X(dpps, Xmm, Xmm, Imm);                            // SSE4_1
    _GEEK_ASM_INST_3X(dpps, Xmm, Mem, Imm);                            // SSE4_1
    _GEEK_ASM_INST_3X(extractps, Gp, Xmm, Imm);                   // SSE4_1
    _GEEK_ASM_INST_3X(extractps, Mem, Xmm, Imm);                  // SSE4_1
    _GEEK_ASM_INST_2X(extrq, Xmm, Xmm);                               // SSE4A
    _GEEK_ASM_INST_3X(extrq, Xmm, Imm, Imm);                          // SSE4A
    _GEEK_ASM_INST_2X(haddpd, Xmm, Xmm);                             // SSE3
    _GEEK_ASM_INST_2X(haddpd, Xmm, Mem);                             // SSE3
    _GEEK_ASM_INST_2X(haddps, Xmm, Xmm);                             // SSE3
    _GEEK_ASM_INST_2X(haddps, Xmm, Mem);                             // SSE3
    _GEEK_ASM_INST_2X(hsubpd, Xmm, Xmm);                             // SSE3
    _GEEK_ASM_INST_2X(hsubpd, Xmm, Mem);                             // SSE3
    _GEEK_ASM_INST_2X(hsubps, Xmm, Xmm);                             // SSE3
    _GEEK_ASM_INST_2X(hsubps, Xmm, Mem);                             // SSE3
    _GEEK_ASM_INST_3X(insertps, Xmm, Xmm, Imm);                    // SSE4_1
    _GEEK_ASM_INST_3X(insertps, Xmm, Mem, Imm);                    // SSE4_1
    _GEEK_ASM_INST_2X(insertq, Xmm, Xmm);                           // SSE4A
    _GEEK_ASM_INST_4X(insertq, Xmm, Xmm, Imm, Imm);                 // SSE4A
    _GEEK_ASM_INST_2X(lddqu, Xmm, Mem);                               // SSE3
    _GEEK_ASM_INST_3X(maskmovq, Mm, Mm, DS_ZDI);                   // SSE  [EXPLICIT]
    _GEEK_ASM_INST_3X(maskmovdqu, Xmm, Xmm, DS_ZDI);             // SSE2 [EXPLICIT]
    _GEEK_ASM_INST_2X(maxpd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(maxpd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(maxps, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(maxps, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(maxsd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(maxsd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(maxss, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(maxss, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(minpd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(minpd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(minps, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(minps, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(minsd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(minsd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(minss, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(minss, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(movapd, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(movapd, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(movapd, Mem, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(movaps, Xmm, Xmm);                             // SSE
    _GEEK_ASM_INST_2X(movaps, Xmm, Mem);                             // SSE
    _GEEK_ASM_INST_2X(movaps, Mem, Xmm);                             // SSE
    _GEEK_ASM_INST_2X(movd, Mem, Mm);                                  // MMX
    _GEEK_ASM_INST_2X(movd, Mem, Xmm);                                 // SSE
    _GEEK_ASM_INST_2X(movd, Gp, Mm);                                   // MMX
    _GEEK_ASM_INST_2X(movd, Gp, Xmm);                                  // SSE
    _GEEK_ASM_INST_2X(movd, Mm, Mem);                                  // MMX
    _GEEK_ASM_INST_2X(movd, Xmm, Mem);                                 // SSE
    _GEEK_ASM_INST_2X(movd, Mm, Gp);                                   // MMX
    _GEEK_ASM_INST_2X(movd, Xmm, Gp);                                  // SSE
    _GEEK_ASM_INST_2X(movddup, Xmm, Xmm);                           // SSE3
    _GEEK_ASM_INST_2X(movddup, Xmm, Mem);                           // SSE3
    _GEEK_ASM_INST_2X(movdq2q, Mm, Xmm);                            // SSE2
    _GEEK_ASM_INST_2X(movdqa, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(movdqa, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(movdqa, Mem, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(movdqu, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(movdqu, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(movdqu, Mem, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(movhlps, Xmm, Xmm);                           // SSE
    _GEEK_ASM_INST_2X(movhpd, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(movhpd, Mem, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(movhps, Xmm, Mem);                             // SSE
    _GEEK_ASM_INST_2X(movhps, Mem, Xmm);                             // SSE
    _GEEK_ASM_INST_2X(movlhps, Xmm, Xmm);                           // SSE
    _GEEK_ASM_INST_2X(movlpd, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(movlpd, Mem, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(movlps, Xmm, Mem);                             // SSE
    _GEEK_ASM_INST_2X(movlps, Mem, Xmm);                             // SSE
    _GEEK_ASM_INST_2X(movmskps, Gp, Xmm);                          // SSE2
    _GEEK_ASM_INST_2X(movmskpd, Gp, Xmm);                          // SSE2
    _GEEK_ASM_INST_2X(movntdq, Mem, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(movntdqa, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(movntpd, Mem, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(movntps, Mem, Xmm);                           // SSE
    _GEEK_ASM_INST_2X(movntsd, Mem, Xmm);                           // SSE4A
    _GEEK_ASM_INST_2X(movntss, Mem, Xmm);                           // SSE4A
    _GEEK_ASM_INST_2X(movntq, Mem, Mm);                              // SSE
    _GEEK_ASM_INST_2X(movq, Mm, Mm);                                   // MMX
    _GEEK_ASM_INST_2X(movq, Xmm, Xmm);                                 // SSE
    _GEEK_ASM_INST_2X(movq, Mem, Mm);                                  // MMX
    _GEEK_ASM_INST_2X(movq, Mem, Xmm);                                 // SSE
    _GEEK_ASM_INST_2X(movq, Mm, Mem);                                  // MMX
    _GEEK_ASM_INST_2X(movq, Xmm, Mem);                                 // SSE
    _GEEK_ASM_INST_2X(movq, Gp, Mm);                                   // MMX
    _GEEK_ASM_INST_2X(movq, Gp, Xmm);                                  // SSE+X64.
    _GEEK_ASM_INST_2X(movq, Mm, Gp);                                   // MMX
    _GEEK_ASM_INST_2X(movq, Xmm, Gp);                                  // SSE+X64.
    _GEEK_ASM_INST_2X(movq2dq, Xmm, Mm);                            // SSE2
    _GEEK_ASM_INST_2X(movsd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(movsd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(movsd, Mem, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(movshdup, Xmm, Xmm);                         // SSE3
    _GEEK_ASM_INST_2X(movshdup, Xmm, Mem);                         // SSE3
    _GEEK_ASM_INST_2X(movsldup, Xmm, Xmm);                         // SSE3
    _GEEK_ASM_INST_2X(movsldup, Xmm, Mem);                         // SSE3
    _GEEK_ASM_INST_2X(movss, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(movss, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(movss, Mem, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(movupd, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(movupd, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(movupd, Mem, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(movups, Xmm, Xmm);                             // SSE
    _GEEK_ASM_INST_2X(movups, Xmm, Mem);                             // SSE
    _GEEK_ASM_INST_2X(movups, Mem, Xmm);                             // SSE
    _GEEK_ASM_INST_3X(mpsadbw, Xmm, Xmm, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(mpsadbw, Xmm, Mem, Imm);                      // SSE4_1
    _GEEK_ASM_INST_2X(mulpd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(mulpd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(mulps, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(mulps, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(mulsd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(mulsd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(mulss, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(mulss, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(orpd, Xmm, Xmm);                                 // SSE2
    _GEEK_ASM_INST_2X(orpd, Xmm, Mem);                                 // SSE2
    _GEEK_ASM_INST_2X(orps, Xmm, Xmm);                                 // SSE
    _GEEK_ASM_INST_2X(orps, Xmm, Mem);                                 // SSE
    _GEEK_ASM_INST_2X(packssdw, Mm, Mm);                           // MMX
    _GEEK_ASM_INST_2X(packssdw, Mm, Mem);                          // MMX
    _GEEK_ASM_INST_2X(packssdw, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(packssdw, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(packsswb, Mm, Mm);                           // MMX
    _GEEK_ASM_INST_2X(packsswb, Mm, Mem);                          // MMX
    _GEEK_ASM_INST_2X(packsswb, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(packsswb, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(packusdw, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(packusdw, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(packuswb, Mm, Mm);                           // MMX
    _GEEK_ASM_INST_2X(packuswb, Mm, Mem);                          // MMX
    _GEEK_ASM_INST_2X(packuswb, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(packuswb, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(pabsb, Mm, Mm);                                 // SSSE3
    _GEEK_ASM_INST_2X(pabsb, Mm, Mem);                                // SSSE3
    _GEEK_ASM_INST_2X(pabsb, Xmm, Xmm);                               // SSSE3
    _GEEK_ASM_INST_2X(pabsb, Xmm, Mem);                               // SSSE3
    _GEEK_ASM_INST_2X(pabsd, Mm, Mm);                                 // SSSE3
    _GEEK_ASM_INST_2X(pabsd, Mm, Mem);                                // SSSE3
    _GEEK_ASM_INST_2X(pabsd, Xmm, Xmm);                               // SSSE3
    _GEEK_ASM_INST_2X(pabsd, Xmm, Mem);                               // SSSE3
    _GEEK_ASM_INST_2X(pabsw, Mm, Mm);                                 // SSSE3
    _GEEK_ASM_INST_2X(pabsw, Mm, Mem);                                // SSSE3
    _GEEK_ASM_INST_2X(pabsw, Xmm, Xmm);                               // SSSE3
    _GEEK_ASM_INST_2X(pabsw, Xmm, Mem);                               // SSSE3
    _GEEK_ASM_INST_2X(paddb, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(paddb, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(paddb, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(paddb, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(paddd, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(paddd, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(paddd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(paddd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(paddq, Mm, Mm);                                 // SSE2
    _GEEK_ASM_INST_2X(paddq, Mm, Mem);                                // SSE2
    _GEEK_ASM_INST_2X(paddq, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(paddq, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(paddsb, Mm, Mm);                               // MMX
    _GEEK_ASM_INST_2X(paddsb, Mm, Mem);                              // MMX
    _GEEK_ASM_INST_2X(paddsb, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(paddsb, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(paddsw, Mm, Mm);                               // MMX
    _GEEK_ASM_INST_2X(paddsw, Mm, Mem);                              // MMX
    _GEEK_ASM_INST_2X(paddsw, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(paddsw, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(paddusb, Mm, Mm);                             // MMX
    _GEEK_ASM_INST_2X(paddusb, Mm, Mem);                            // MMX
    _GEEK_ASM_INST_2X(paddusb, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(paddusb, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(paddusw, Mm, Mm);                             // MMX
    _GEEK_ASM_INST_2X(paddusw, Mm, Mem);                            // MMX
    _GEEK_ASM_INST_2X(paddusw, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(paddusw, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(paddw, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(paddw, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(paddw, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(paddw, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_3X(palignr, Mm, Mm, Imm);                        // SSSE3
    _GEEK_ASM_INST_3X(palignr, Mm, Mem, Imm);                       // SSSE3
    _GEEK_ASM_INST_3X(palignr, Xmm, Xmm, Imm);                      // SSSE3
    _GEEK_ASM_INST_3X(palignr, Xmm, Mem, Imm);                      // SSSE3
    _GEEK_ASM_INST_2X(pand, Mm, Mm);                                   // MMX
    _GEEK_ASM_INST_2X(pand, Mm, Mem);                                  // MMX
    _GEEK_ASM_INST_2X(pand, Xmm, Xmm);                                 // SSE2
    _GEEK_ASM_INST_2X(pand, Xmm, Mem);                                 // SSE2
    _GEEK_ASM_INST_2X(pandn, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(pandn, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(pandn, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(pandn, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(pavgb, Mm, Mm);                                 // SSE
    _GEEK_ASM_INST_2X(pavgb, Mm, Mem);                                // SSE
    _GEEK_ASM_INST_2X(pavgb, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(pavgb, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(pavgw, Mm, Mm);                                 // SSE
    _GEEK_ASM_INST_2X(pavgw, Mm, Mem);                                // SSE
    _GEEK_ASM_INST_2X(pavgw, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(pavgw, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_3X(pblendvb, Xmm, Xmm, XMM0);                   // SSE4_1 [EXPLICIT]
    _GEEK_ASM_INST_3X(pblendvb, Xmm, Mem, XMM0);                   // SSE4_1 [EXPLICIT]
    _GEEK_ASM_INST_3X(pblendw, Xmm, Xmm, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(pblendw, Xmm, Mem, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(pclmulqdq, Xmm, Xmm, Imm);                  // PCLMULQDQ.
    _GEEK_ASM_INST_3X(pclmulqdq, Xmm, Mem, Imm);                  // PCLMULQDQ.
    _GEEK_ASM_INST_6X(pcmpestri, Xmm, Xmm, Imm, Gp_ECX, Gp_EAX, Gp_EDX); // SSE4_2 [EXPLICIT]
    _GEEK_ASM_INST_6X(pcmpestri, Xmm, Mem, Imm, Gp_ECX, Gp_EAX, Gp_EDX); // SSE4_2 [EXPLICIT]
    _GEEK_ASM_INST_6X(pcmpestrm, Xmm, Xmm, Imm, XMM0, Gp_EAX, Gp_EDX);   // SSE4_2 [EXPLICIT]
    _GEEK_ASM_INST_6X(pcmpestrm, Xmm, Mem, Imm, XMM0, Gp_EAX, Gp_EDX);   // SSE4_2 [EXPLICIT]
    _GEEK_ASM_INST_2X(pcmpeqb, Mm, Mm);                             // MMX
    _GEEK_ASM_INST_2X(pcmpeqb, Mm, Mem);                            // MMX
    _GEEK_ASM_INST_2X(pcmpeqb, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(pcmpeqb, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(pcmpeqd, Mm, Mm);                             // MMX
    _GEEK_ASM_INST_2X(pcmpeqd, Mm, Mem);                            // MMX
    _GEEK_ASM_INST_2X(pcmpeqd, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(pcmpeqd, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(pcmpeqq, Xmm, Xmm);                           // SSE4_1
    _GEEK_ASM_INST_2X(pcmpeqq, Xmm, Mem);                           // SSE4_1
    _GEEK_ASM_INST_2X(pcmpeqw, Mm, Mm);                             // MMX
    _GEEK_ASM_INST_2X(pcmpeqw, Mm, Mem);                            // MMX
    _GEEK_ASM_INST_2X(pcmpeqw, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(pcmpeqw, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(pcmpgtb, Mm, Mm);                             // MMX
    _GEEK_ASM_INST_2X(pcmpgtb, Mm, Mem);                            // MMX
    _GEEK_ASM_INST_2X(pcmpgtb, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(pcmpgtb, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(pcmpgtd, Mm, Mm);                             // MMX
    _GEEK_ASM_INST_2X(pcmpgtd, Mm, Mem);                            // MMX
    _GEEK_ASM_INST_2X(pcmpgtd, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(pcmpgtd, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(pcmpgtq, Xmm, Xmm);                           // SSE4_2.
    _GEEK_ASM_INST_2X(pcmpgtq, Xmm, Mem);                           // SSE4_2.
    _GEEK_ASM_INST_2X(pcmpgtw, Mm, Mm);                             // MMX
    _GEEK_ASM_INST_2X(pcmpgtw, Mm, Mem);                            // MMX
    _GEEK_ASM_INST_2X(pcmpgtw, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(pcmpgtw, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_4X(pcmpistri, Xmm, Xmm, Imm, Gp_ECX);          // SSE4_2 [EXPLICIT]
    _GEEK_ASM_INST_4X(pcmpistri, Xmm, Mem, Imm, Gp_ECX);          // SSE4_2 [EXPLICIT]
    _GEEK_ASM_INST_4X(pcmpistrm, Xmm, Xmm, Imm, XMM0);            // SSE4_2 [EXPLICIT]
    _GEEK_ASM_INST_4X(pcmpistrm, Xmm, Mem, Imm, XMM0);            // SSE4_2 [EXPLICIT]
    _GEEK_ASM_INST_3X(pextrb, Gp, Xmm, Imm);                         // SSE4_1
    _GEEK_ASM_INST_3X(pextrb, Mem, Xmm, Imm);                        // SSE4_1
    _GEEK_ASM_INST_3X(pextrd, Gp, Xmm, Imm);                         // SSE4_1
    _GEEK_ASM_INST_3X(pextrd, Mem, Xmm, Imm);                        // SSE4_1
    _GEEK_ASM_INST_3X(pextrq, Gp, Xmm, Imm);                         // SSE4_1
    _GEEK_ASM_INST_3X(pextrq, Mem, Xmm, Imm);                        // SSE4_1
    _GEEK_ASM_INST_3X(pextrw, Gp, Mm, Imm);                          // SSE
    _GEEK_ASM_INST_3X(pextrw, Gp, Xmm, Imm);                         // SSE2
    _GEEK_ASM_INST_3X(pextrw, Mem, Xmm, Imm);                        // SSE4_1
    _GEEK_ASM_INST_2X(phaddd, Mm, Mm);                               // SSSE3
    _GEEK_ASM_INST_2X(phaddd, Mm, Mem);                              // SSSE3
    _GEEK_ASM_INST_2X(phaddd, Xmm, Xmm);                             // SSSE3
    _GEEK_ASM_INST_2X(phaddd, Xmm, Mem);                             // SSSE3
    _GEEK_ASM_INST_2X(phaddsw, Mm, Mm);                             // SSSE3
    _GEEK_ASM_INST_2X(phaddsw, Mm, Mem);                            // SSSE3
    _GEEK_ASM_INST_2X(phaddsw, Xmm, Xmm);                           // SSSE3
    _GEEK_ASM_INST_2X(phaddsw, Xmm, Mem);                           // SSSE3
    _GEEK_ASM_INST_2X(phaddw, Mm, Mm);                               // SSSE3
    _GEEK_ASM_INST_2X(phaddw, Mm, Mem);                              // SSSE3
    _GEEK_ASM_INST_2X(phaddw, Xmm, Xmm);                             // SSSE3
    _GEEK_ASM_INST_2X(phaddw, Xmm, Mem);                             // SSSE3
    _GEEK_ASM_INST_2X(phminposuw, Xmm, Xmm);                     // SSE4_1
    _GEEK_ASM_INST_2X(phminposuw, Xmm, Mem);                     // SSE4_1
    _GEEK_ASM_INST_2X(phsubd, Mm, Mm);                               // SSSE3
    _GEEK_ASM_INST_2X(phsubd, Mm, Mem);                              // SSSE3
    _GEEK_ASM_INST_2X(phsubd, Xmm, Xmm);                             // SSSE3
    _GEEK_ASM_INST_2X(phsubd, Xmm, Mem);                             // SSSE3
    _GEEK_ASM_INST_2X(phsubsw, Mm, Mm);                             // SSSE3
    _GEEK_ASM_INST_2X(phsubsw, Mm, Mem);                            // SSSE3
    _GEEK_ASM_INST_2X(phsubsw, Xmm, Xmm);                           // SSSE3
    _GEEK_ASM_INST_2X(phsubsw, Xmm, Mem);                           // SSSE3
    _GEEK_ASM_INST_2X(phsubw, Mm, Mm);                               // SSSE3
    _GEEK_ASM_INST_2X(phsubw, Mm, Mem);                              // SSSE3
    _GEEK_ASM_INST_2X(phsubw, Xmm, Xmm);                             // SSSE3
    _GEEK_ASM_INST_2X(phsubw, Xmm, Mem);                             // SSSE3
    _GEEK_ASM_INST_3X(pinsrb, Xmm, Gp, Imm);                         // SSE4_1
    _GEEK_ASM_INST_3X(pinsrb, Xmm, Mem, Imm);                        // SSE4_1
    _GEEK_ASM_INST_3X(pinsrd, Xmm, Gp, Imm);                         // SSE4_1
    _GEEK_ASM_INST_3X(pinsrd, Xmm, Mem, Imm);                        // SSE4_1
    _GEEK_ASM_INST_3X(pinsrq, Xmm, Gp, Imm);                         // SSE4_1
    _GEEK_ASM_INST_3X(pinsrq, Xmm, Mem, Imm);                        // SSE4_1
    _GEEK_ASM_INST_3X(pinsrw, Mm, Gp, Imm);                          // SSE
    _GEEK_ASM_INST_3X(pinsrw, Mm, Mem, Imm);                         // SSE
    _GEEK_ASM_INST_3X(pinsrw, Xmm, Gp, Imm);                         // SSE2
    _GEEK_ASM_INST_3X(pinsrw, Xmm, Mem, Imm);                        // SSE2
    _GEEK_ASM_INST_2X(pmaddubsw, Mm, Mm);                         // SSSE3
    _GEEK_ASM_INST_2X(pmaddubsw, Mm, Mem);                        // SSSE3
    _GEEK_ASM_INST_2X(pmaddubsw, Xmm, Xmm);                       // SSSE3
    _GEEK_ASM_INST_2X(pmaddubsw, Xmm, Mem);                       // SSSE3
    _GEEK_ASM_INST_2X(pmaddwd, Mm, Mm);                             // MMX
    _GEEK_ASM_INST_2X(pmaddwd, Mm, Mem);                            // MMX
    _GEEK_ASM_INST_2X(pmaddwd, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(pmaddwd, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(pmaxsb, Xmm, Xmm);                             // SSE4_1
    _GEEK_ASM_INST_2X(pmaxsb, Xmm, Mem);                             // SSE4_1
    _GEEK_ASM_INST_2X(pmaxsd, Xmm, Xmm);                             // SSE4_1
    _GEEK_ASM_INST_2X(pmaxsd, Xmm, Mem);                             // SSE4_1
    _GEEK_ASM_INST_2X(pmaxsw, Mm, Mm);                               // SSE
    _GEEK_ASM_INST_2X(pmaxsw, Mm, Mem);                              // SSE
    _GEEK_ASM_INST_2X(pmaxsw, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(pmaxsw, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(pmaxub, Mm, Mm);                               // SSE
    _GEEK_ASM_INST_2X(pmaxub, Mm, Mem);                              // SSE
    _GEEK_ASM_INST_2X(pmaxub, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(pmaxub, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(pmaxud, Xmm, Xmm);                             // SSE4_1
    _GEEK_ASM_INST_2X(pmaxud, Xmm, Mem);                             // SSE4_1
    _GEEK_ASM_INST_2X(pmaxuw, Xmm, Xmm);                             // SSE4_1
    _GEEK_ASM_INST_2X(pmaxuw, Xmm, Mem);                             // SSE4_1
    _GEEK_ASM_INST_2X(pminsb, Xmm, Xmm);                             // SSE4_1
    _GEEK_ASM_INST_2X(pminsb, Xmm, Mem);                             // SSE4_1
    _GEEK_ASM_INST_2X(pminsd, Xmm, Xmm);                             // SSE4_1
    _GEEK_ASM_INST_2X(pminsd, Xmm, Mem);                             // SSE4_1
    _GEEK_ASM_INST_2X(pminsw, Mm, Mm);                               // SSE
    _GEEK_ASM_INST_2X(pminsw, Mm, Mem);                              // SSE
    _GEEK_ASM_INST_2X(pminsw, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(pminsw, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(pminub, Mm, Mm);                               // SSE
    _GEEK_ASM_INST_2X(pminub, Mm, Mem);                              // SSE
    _GEEK_ASM_INST_2X(pminub, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(pminub, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(pminud, Xmm, Xmm);                             // SSE4_1
    _GEEK_ASM_INST_2X(pminud, Xmm, Mem);                             // SSE4_1
    _GEEK_ASM_INST_2X(pminuw, Xmm, Xmm);                             // SSE4_1
    _GEEK_ASM_INST_2X(pminuw, Xmm, Mem);                             // SSE4_1
    _GEEK_ASM_INST_2X(pmovmskb, Gp, Mm);                           // SSE
    _GEEK_ASM_INST_2X(pmovmskb, Gp, Xmm);                          // SSE2
    _GEEK_ASM_INST_2X(pmovsxbd, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovsxbd, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovsxbq, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovsxbq, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovsxbw, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovsxbw, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovsxdq, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovsxdq, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovsxwd, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovsxwd, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovsxwq, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovsxwq, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovzxbd, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovzxbd, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovzxbq, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovzxbq, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovzxbw, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovzxbw, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovzxdq, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovzxdq, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovzxwd, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovzxwd, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovzxwq, Xmm, Xmm);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmovzxwq, Xmm, Mem);                         // SSE4_1
    _GEEK_ASM_INST_2X(pmuldq, Xmm, Xmm);                             // SSE4_1
    _GEEK_ASM_INST_2X(pmuldq, Xmm, Mem);                             // SSE4_1
    _GEEK_ASM_INST_2X(pmulhrsw, Mm, Mm);                           // SSSE3
    _GEEK_ASM_INST_2X(pmulhrsw, Mm, Mem);                          // SSSE3
    _GEEK_ASM_INST_2X(pmulhrsw, Xmm, Xmm);                         // SSSE3
    _GEEK_ASM_INST_2X(pmulhrsw, Xmm, Mem);                         // SSSE3
    _GEEK_ASM_INST_2X(pmulhw, Mm, Mm);                               // MMX
    _GEEK_ASM_INST_2X(pmulhw, Mm, Mem);                              // MMX
    _GEEK_ASM_INST_2X(pmulhw, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(pmulhw, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(pmulhuw, Mm, Mm);                             // SSE
    _GEEK_ASM_INST_2X(pmulhuw, Mm, Mem);                            // SSE
    _GEEK_ASM_INST_2X(pmulhuw, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(pmulhuw, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(pmulld, Xmm, Xmm);                             // SSE4_1
    _GEEK_ASM_INST_2X(pmulld, Xmm, Mem);                             // SSE4_1
    _GEEK_ASM_INST_2X(pmullw, Mm, Mm);                               // MMX
    _GEEK_ASM_INST_2X(pmullw, Mm, Mem);                              // MMX
    _GEEK_ASM_INST_2X(pmullw, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(pmullw, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(pmuludq, Mm, Mm);                             // SSE2
    _GEEK_ASM_INST_2X(pmuludq, Mm, Mem);                            // SSE2
    _GEEK_ASM_INST_2X(pmuludq, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(pmuludq, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(por, Mm, Mm);                                     // MMX
    _GEEK_ASM_INST_2X(por, Mm, Mem);                                    // MMX
    _GEEK_ASM_INST_2X(por, Xmm, Xmm);                                   // SSE2
    _GEEK_ASM_INST_2X(por, Xmm, Mem);                                   // SSE2
    _GEEK_ASM_INST_2X(psadbw, Mm, Mm);                               // SSE
    _GEEK_ASM_INST_2X(psadbw, Mm, Mem);                              // SSE
    _GEEK_ASM_INST_2X(psadbw, Xmm, Xmm);                             // SSE
    _GEEK_ASM_INST_2X(psadbw, Xmm, Mem);                             // SSE
    _GEEK_ASM_INST_2X(pslld, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(pslld, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(pslld, Mm, Imm);                                // MMX
    _GEEK_ASM_INST_2X(pslld, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(pslld, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(pslld, Xmm, Imm);                               // SSE2
    _GEEK_ASM_INST_2X(pslldq, Xmm, Imm);                             // SSE2
    _GEEK_ASM_INST_2X(psllq, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(psllq, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(psllq, Mm, Imm);                                // MMX
    _GEEK_ASM_INST_2X(psllq, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(psllq, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(psllq, Xmm, Imm);                               // SSE2
    _GEEK_ASM_INST_2X(psllw, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(psllw, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(psllw, Mm, Imm);                                // MMX
    _GEEK_ASM_INST_2X(psllw, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(psllw, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(psllw, Xmm, Imm);                               // SSE2
    _GEEK_ASM_INST_2X(psrad, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(psrad, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(psrad, Mm, Imm);                                // MMX
    _GEEK_ASM_INST_2X(psrad, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(psrad, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(psrad, Xmm, Imm);                               // SSE2
    _GEEK_ASM_INST_2X(psraw, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(psraw, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(psraw, Mm, Imm);                                // MMX
    _GEEK_ASM_INST_2X(psraw, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(psraw, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(psraw, Xmm, Imm);                               // SSE2
    _GEEK_ASM_INST_2X(pshufb, Mm, Mm);                               // SSSE3
    _GEEK_ASM_INST_2X(pshufb, Mm, Mem);                              // SSSE3
    _GEEK_ASM_INST_2X(pshufb, Xmm, Xmm);                             // SSSE3
    _GEEK_ASM_INST_2X(pshufb, Xmm, Mem);                             // SSSE3
    _GEEK_ASM_INST_3X(pshufd, Xmm, Xmm, Imm);                        // SSE2
    _GEEK_ASM_INST_3X(pshufd, Xmm, Mem, Imm);                        // SSE2
    _GEEK_ASM_INST_3X(pshufhw, Xmm, Xmm, Imm);                      // SSE2
    _GEEK_ASM_INST_3X(pshufhw, Xmm, Mem, Imm);                      // SSE2
    _GEEK_ASM_INST_3X(pshuflw, Xmm, Xmm, Imm);                      // SSE2
    _GEEK_ASM_INST_3X(pshuflw, Xmm, Mem, Imm);                      // SSE2
    _GEEK_ASM_INST_3X(pshufw, Mm, Mm, Imm);                          // SSE
    _GEEK_ASM_INST_3X(pshufw, Mm, Mem, Imm);                         // SSE
    _GEEK_ASM_INST_2X(psignb, Mm, Mm);                               // SSSE3
    _GEEK_ASM_INST_2X(psignb, Mm, Mem);                              // SSSE3
    _GEEK_ASM_INST_2X(psignb, Xmm, Xmm);                             // SSSE3
    _GEEK_ASM_INST_2X(psignb, Xmm, Mem);                             // SSSE3
    _GEEK_ASM_INST_2X(psignd, Mm, Mm);                               // SSSE3
    _GEEK_ASM_INST_2X(psignd, Mm, Mem);                              // SSSE3
    _GEEK_ASM_INST_2X(psignd, Xmm, Xmm);                             // SSSE3
    _GEEK_ASM_INST_2X(psignd, Xmm, Mem);                             // SSSE3
    _GEEK_ASM_INST_2X(psignw, Mm, Mm);                               // SSSE3
    _GEEK_ASM_INST_2X(psignw, Mm, Mem);                              // SSSE3
    _GEEK_ASM_INST_2X(psignw, Xmm, Xmm);                             // SSSE3
    _GEEK_ASM_INST_2X(psignw, Xmm, Mem);                             // SSSE3
    _GEEK_ASM_INST_2X(psrld, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(psrld, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(psrld, Mm, Imm);                                // MMX
    _GEEK_ASM_INST_2X(psrld, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(psrld, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(psrld, Xmm, Imm);                               // SSE2
    _GEEK_ASM_INST_2X(psrldq, Xmm, Imm);                             // SSE2
    _GEEK_ASM_INST_2X(psrlq, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(psrlq, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(psrlq, Mm, Imm);                                // MMX
    _GEEK_ASM_INST_2X(psrlq, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(psrlq, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(psrlq, Xmm, Imm);                               // SSE2
    _GEEK_ASM_INST_2X(psrlw, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(psrlw, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(psrlw, Mm, Imm);                                // MMX
    _GEEK_ASM_INST_2X(psrlw, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(psrlw, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(psrlw, Xmm, Imm);                               // SSE2
    _GEEK_ASM_INST_2X(psubb, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(psubb, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(psubb, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(psubb, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(psubd, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(psubd, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(psubd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(psubd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(psubq, Mm, Mm);                                 // SSE2
    _GEEK_ASM_INST_2X(psubq, Mm, Mem);                                // SSE2
    _GEEK_ASM_INST_2X(psubq, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(psubq, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(psubsb, Mm, Mm);                               // MMX
    _GEEK_ASM_INST_2X(psubsb, Mm, Mem);                              // MMX
    _GEEK_ASM_INST_2X(psubsb, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(psubsb, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(psubsw, Mm, Mm);                               // MMX
    _GEEK_ASM_INST_2X(psubsw, Mm, Mem);                              // MMX
    _GEEK_ASM_INST_2X(psubsw, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(psubsw, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(psubusb, Mm, Mm);                             // MMX
    _GEEK_ASM_INST_2X(psubusb, Mm, Mem);                            // MMX
    _GEEK_ASM_INST_2X(psubusb, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(psubusb, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(psubusw, Mm, Mm);                             // MMX
    _GEEK_ASM_INST_2X(psubusw, Mm, Mem);                            // MMX
    _GEEK_ASM_INST_2X(psubusw, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(psubusw, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(psubw, Mm, Mm);                                 // MMX
    _GEEK_ASM_INST_2X(psubw, Mm, Mem);                                // MMX
    _GEEK_ASM_INST_2X(psubw, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(psubw, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(ptest, Xmm, Xmm);                               // SSE4_1
    _GEEK_ASM_INST_2X(ptest, Xmm, Mem);                               // SSE4_1
    _GEEK_ASM_INST_2X(punpckhbw, Mm, Mm);                         // MMX
    _GEEK_ASM_INST_2X(punpckhbw, Mm, Mem);                        // MMX
    _GEEK_ASM_INST_2X(punpckhbw, Xmm, Xmm);                       // SSE2
    _GEEK_ASM_INST_2X(punpckhbw, Xmm, Mem);                       // SSE2
    _GEEK_ASM_INST_2X(punpckhdq, Mm, Mm);                         // MMX
    _GEEK_ASM_INST_2X(punpckhdq, Mm, Mem);                        // MMX
    _GEEK_ASM_INST_2X(punpckhdq, Xmm, Xmm);                       // SSE2
    _GEEK_ASM_INST_2X(punpckhdq, Xmm, Mem);                       // SSE2
    _GEEK_ASM_INST_2X(punpckhqdq, Xmm, Xmm);                     // SSE2
    _GEEK_ASM_INST_2X(punpckhqdq, Xmm, Mem);                     // SSE2
    _GEEK_ASM_INST_2X(punpckhwd, Mm, Mm);                         // MMX
    _GEEK_ASM_INST_2X(punpckhwd, Mm, Mem);                        // MMX
    _GEEK_ASM_INST_2X(punpckhwd, Xmm, Xmm);                       // SSE2
    _GEEK_ASM_INST_2X(punpckhwd, Xmm, Mem);                       // SSE2
    _GEEK_ASM_INST_2X(punpcklbw, Mm, Mm);                         // MMX
    _GEEK_ASM_INST_2X(punpcklbw, Mm, Mem);                        // MMX
    _GEEK_ASM_INST_2X(punpcklbw, Xmm, Xmm);                       // SSE2
    _GEEK_ASM_INST_2X(punpcklbw, Xmm, Mem);                       // SSE2
    _GEEK_ASM_INST_2X(punpckldq, Mm, Mm);                         // MMX
    _GEEK_ASM_INST_2X(punpckldq, Mm, Mem);                        // MMX
    _GEEK_ASM_INST_2X(punpckldq, Xmm, Xmm);                       // SSE2
    _GEEK_ASM_INST_2X(punpckldq, Xmm, Mem);                       // SSE2
    _GEEK_ASM_INST_2X(punpcklqdq, Xmm, Xmm);                     // SSE2
    _GEEK_ASM_INST_2X(punpcklqdq, Xmm, Mem);                     // SSE2
    _GEEK_ASM_INST_2X(punpcklwd, Mm, Mm);                         // MMX
    _GEEK_ASM_INST_2X(punpcklwd, Mm, Mem);                        // MMX
    _GEEK_ASM_INST_2X(punpcklwd, Xmm, Xmm);                       // SSE2
    _GEEK_ASM_INST_2X(punpcklwd, Xmm, Mem);                       // SSE2
    _GEEK_ASM_INST_2X(pxor, Mm, Mm);                                   // MMX
    _GEEK_ASM_INST_2X(pxor, Mm, Mem);                                  // MMX
    _GEEK_ASM_INST_2X(pxor, Xmm, Xmm);                                 // SSE2
    _GEEK_ASM_INST_2X(pxor, Xmm, Mem);                                 // SSE2
    _GEEK_ASM_INST_2X(rcpps, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(rcpps, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(rcpss, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(rcpss, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_3X(roundpd, Xmm, Xmm, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(roundpd, Xmm, Mem, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(roundps, Xmm, Xmm, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(roundps, Xmm, Mem, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(roundsd, Xmm, Xmm, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(roundsd, Xmm, Mem, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(roundss, Xmm, Xmm, Imm);                      // SSE4_1
    _GEEK_ASM_INST_3X(roundss, Xmm, Mem, Imm);                      // SSE4_1
    _GEEK_ASM_INST_2X(rsqrtps, Xmm, Xmm);                           // SSE
    _GEEK_ASM_INST_2X(rsqrtps, Xmm, Mem);                           // SSE
    _GEEK_ASM_INST_2X(rsqrtss, Xmm, Xmm);                           // SSE
    _GEEK_ASM_INST_2X(rsqrtss, Xmm, Mem);                           // SSE
    _GEEK_ASM_INST_3X(shufpd, Xmm, Xmm, Imm);                        // SSE2
    _GEEK_ASM_INST_3X(shufpd, Xmm, Mem, Imm);                        // SSE2
    _GEEK_ASM_INST_3X(shufps, Xmm, Xmm, Imm);                        // SSE
    _GEEK_ASM_INST_3X(shufps, Xmm, Mem, Imm);                        // SSE
    _GEEK_ASM_INST_2X(sqrtpd, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(sqrtpd, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(sqrtps, Xmm, Xmm);                             // SSE
    _GEEK_ASM_INST_2X(sqrtps, Xmm, Mem);                             // SSE
    _GEEK_ASM_INST_2X(sqrtsd, Xmm, Xmm);                             // SSE2
    _GEEK_ASM_INST_2X(sqrtsd, Xmm, Mem);                             // SSE2
    _GEEK_ASM_INST_2X(sqrtss, Xmm, Xmm);                             // SSE
    _GEEK_ASM_INST_2X(sqrtss, Xmm, Mem);                             // SSE
    _GEEK_ASM_INST_2X(subpd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(subpd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(subps, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(subps, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(subsd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(subsd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(subss, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(subss, Xmm, Mem);                               // SSE
    _GEEK_ASM_INST_2X(ucomisd, Xmm, Xmm);                           // SSE2
    _GEEK_ASM_INST_2X(ucomisd, Xmm, Mem);                           // SSE2
    _GEEK_ASM_INST_2X(ucomiss, Xmm, Xmm);                           // SSE
    _GEEK_ASM_INST_2X(ucomiss, Xmm, Mem);                           // SSE
    _GEEK_ASM_INST_2X(unpckhpd, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(unpckhpd, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(unpckhps, Xmm, Xmm);                         // SSE
    _GEEK_ASM_INST_2X(unpckhps, Xmm, Mem);                         // SSE
    _GEEK_ASM_INST_2X(unpcklpd, Xmm, Xmm);                         // SSE2
    _GEEK_ASM_INST_2X(unpcklpd, Xmm, Mem);                         // SSE2
    _GEEK_ASM_INST_2X(unpcklps, Xmm, Xmm);                         // SSE
    _GEEK_ASM_INST_2X(unpcklps, Xmm, Mem);                         // SSE
    _GEEK_ASM_INST_2X(xorpd, Xmm, Xmm);                               // SSE2
    _GEEK_ASM_INST_2X(xorpd, Xmm, Mem);                               // SSE2
    _GEEK_ASM_INST_2X(xorps, Xmm, Xmm);                               // SSE
    _GEEK_ASM_INST_2X(xorps, Xmm, Mem);                               // SSE


	Error db(uint8_t x, size_t repeat_count = 1);
	Error dw(uint16_t x, size_t repeat_count = 1);
	Error dd(uint32_t x, size_t repeat_count = 1);
	Error dq(uint64_t x, size_t repeat_count = 1);

	//! \}

	//! \name Embed
	//! \{

	//! Embeds raw data into the \ref CodeBuffer.
	Error Embed(const void* data, size_t data_size);
	//! Embeds int8_t `value` repeated by `repeat_count`.
	Error EmbedInt8(int8_t value, size_t repeat_count = 1);
	//! Embeds uint8_t `value` repeated by `repeat_count`.
	Error EmbedUInt8(uint8_t value, size_t repeat_count = 1);
	//! Embeds int16_t `value` repeated by `repeat_count`.
	Error EmbedInt16(int16_t value, size_t repeat_count = 1);
	//! Embeds uint16_t `value` repeated by `repeat_count`.
	Error EmbedUInt16(uint16_t value, size_t repeat_count = 1);
	//! Embeds int32_t `value` repeated by `repeat_count`.
	Error EmbedInt32(int32_t value, size_t repeat_count = 1);
	//! Embeds uint32_t `value` repeated by `repeat_count`.
	Error EmbedUInt32(uint32_t value, size_t repeat_count = 1);
	//! Embeds int64_t `value` repeated by `repeat_count`.
	Error EmbedInt64(int64_t value, size_t repeat_count = 1);
	//! Embeds uint64_t `value` repeated by `repeat_count`.
	Error EmbedUInt64(uint64_t value, size_t repeat_count = 1);
	//! Embeds a floating point `value` repeated by `repeat_count`.
	Error EmbedFloat(float value, size_t repeat_count = 1);
	//! Embeds a floating point `value` repeated by `repeat_count`.
	Error EmbedDouble(double value, size_t repeat_count = 1);

	//! Embeds an absolute `label` address as data.
	//!
	//! The `data_size` is an optional argument that can be used to specify the size of the address data. If it's zero
	//! (default) the address size is deduced from the target architecture (either 4 or 8 bytes).
	Error EmbedLabel(const asm_op::Label& label, size_t data_size = 0);

	//! Embeds a delta (distance) between the `label` and `base` calculating it as `label - base`. This function was
	//! designed to make it easier to embed lookup tables where each index is a relative distance of two labels.
	Error EmbedLabelDelta(const asm_op::Label& label, const asm_op::Label& base, size_t data_size = 0);

	//! \}

	//! \name Comment
	//! \{

	//! Emits a comment stored in `data` with an optional `size` parameter.
	Error Comment(const char* data, size_t size = SIZE_MAX);

private:
	std::unique_ptr<void, FuncDeleter> PackToFuncImpl() const;

	_GEEK_IMPL
};

template <class Func>
std::unique_ptr<Func, Assembler::FuncDeleter> Assembler::PackCodeAsFunctor() const {
	auto p = PackToFuncImpl();
	auto ret = std::unique_ptr<Func, FuncDeleter>(reinterpret_cast<Func*>(p.get()));
	p.release();
	return ret;
}

template<class Func, class... Args>
decltype(auto) Assembler::InvokeCode(Args&&... args)
{
    auto func = PackCodeAsFunctor<Func>();
    return (*func)(std::forward<Args>(args)...);
}

// template <class Return, class ... Args>
// Return Assembler::InvokeCode(Args&&... args) {
//     auto func = PackCodeAsFunctor<Return(Args...)>();
//     if constexpr (std::is_void_v<Return>) {
//         (*func)(std::move(args)...);
//         return;
//     }
//     else {
//         return (*func)(std::move(args)...);
//     }
// }

class Assembler::Error {
public:
	Error(ErrorCode code);

	ErrorCode code() const { return code_; }
	std::string msg() const;

	bool IsSuccess() const;

private:
	ErrorCode code_;
};

//! AsmJit error codes.
enum Assembler::ErrorCode : uint32_t {
	//! No error (success).
	kErrorOk = 0,

	//! Out of memory.
	kErrorOutOfMemory,

	//! Invalid argument.
	kErrorInvalidArgument,

	//! Invalid state.
	//!
	//! If this error is returned it means that either you are doing something wrong or AsmJit caught itself by
	//! doing something wrong. This error should never be ignored.
	kErrorInvalidState,

	//! Invalid or incompatible architecture.
	kErrorInvalidArch,

	//! The object is not initialized.
	kErrorNotInitialized,
	//! The object is already initialized.
	kErrorAlreadyInitialized,

	//! Either a built-in feature was disabled at compile time and it's not available or the feature is not
	//! available on the target platform.
	//!
	//! For example trying to allocate large pages on unsupported platform would return this error.
	kErrorFeatureNotEnabled,

	//! Too many handles (Windows) or file descriptors (Unix/Posix).
	kErrorTooManyHandles,
	//! Code generated is larger than allowed.
	kErrorTooLarge,

	//! No code generated.
	//!
	//! Returned by runtime if the \ref CodeHolder contains no code.
	kErrorNoCodeGenerated,

	//! Invalid directive.
	kErrorInvalidDirective,
	//! Attempt to use uninitialized label.
	kErrorInvalidLabel,
	//! Label index overflow - a single \ref BaseAssembler instance can hold almost 2^32 (4 billion) labels. If
	//! there is an attempt to create more labels then this error is returned.
	kErrorTooManyLabels,
	//! Label is already bound.
	kErrorLabelAlreadyBound,
	//! Label is already defined (named labels).
	kErrorLabelAlreadyDefined,
	//! Label name is too long.
	kErrorLabelNameTooLong,
	//! Label must always be local if it's anonymous (without a name).
	kErrorInvalidLabelName,
	//! Parent id passed to \ref CodeHolder::newNamedLabelEntry() was either invalid or parent is not supported
	//! by the requested `LabelType`.
	kErrorInvalidParentLabel,

	//! Invalid section.
	kErrorInvalidSection,
	//! Too many sections (section index overflow).
	kErrorTooManySections,
	//! Invalid section name (most probably too long).
	kErrorInvalidSectionName,

	//! Relocation index overflow (too many relocations).
	kErrorTooManyRelocations,
	//! Invalid relocation entry.
	kErrorInvalidRelocEntry,
	//! Reloc entry contains address that is out of range (unencodable).
	kErrorRelocOffsetOutOfRange,

	//! Invalid assignment to a register, function argument, or function return value.
	kErrorInvalidAssignment,
	//! Invalid instruction.
	kErrorInvalidInstruction,
	//! Invalid register type.
	kErrorInvalidRegType,
	//! Invalid register group.
	kErrorInvalidRegGroup,
	//! Invalid physical register id.
	kErrorInvalidPhysId,
	//! Invalid virtual register id.
	kErrorInvalidVirtId,
	//! Invalid element index (ARM).
	kErrorInvalidElementIndex,
	//! Invalid prefix combination (X86|X64).
	kErrorInvalidPrefixCombination,
	//! Invalid LOCK prefix (X86|X64).
	kErrorInvalidLockPrefix,
	//! Invalid XACQUIRE prefix (X86|X64).
	kErrorInvalidXAcquirePrefix,
	//! Invalid XRELEASE prefix (X86|X64).
	kErrorInvalidXReleasePrefix,
	//! Invalid REP prefix (X86|X64).
	kErrorInvalidRepPrefix,
	//! Invalid REX prefix (X86|X64).
	kErrorInvalidRexPrefix,
	//! Invalid {...} register (X86|X64).
	kErrorInvalidExtraReg,
	//! Invalid {k} use (not supported by the instruction) (X86|X64).
	kErrorInvalidKMaskUse,
	//! Invalid {k}{z} use (not supported by the instruction) (X86|X64).
	kErrorInvalidKZeroUse,
	//! Invalid broadcast - Currently only related to invalid use of AVX-512 {1tox} (X86|X64).
	kErrorInvalidBroadcast,
	//! Invalid 'embedded-rounding' {er} or 'suppress-all-exceptions' {sae} (AVX-512) (X86|X64).
	kErrorInvalidEROrSAE,
	//! Invalid address used (not encodable).
	kErrorInvalidAddress,
	//! Invalid index register used in memory address (not encodable).
	kErrorInvalidAddressIndex,
	//! Invalid address scale (not encodable).
	kErrorInvalidAddressScale,
	//! Invalid use of 64-bit address.
	kErrorInvalidAddress64Bit,
	//! Invalid use of 64-bit address that require 32-bit zero-extension (X64).
	kErrorInvalidAddress64BitZeroExtension,
	//! Invalid displacement (not encodable).
	kErrorInvalidDisplacement,
	//! Invalid segment (X86).
	kErrorInvalidSegment,

	//! Invalid immediate (out of bounds on X86 and invalid pattern on ARM).
	kErrorInvalidImmediate,

	//! Invalid operand size.
	kErrorInvalidOperandSize,
	//! Ambiguous operand size (memory has zero size while it's required to determine the operation type.
	kErrorAmbiguousOperandSize,
	//! Mismatching operand size (size of multiple operands doesn't match the operation size).
	kErrorOperandSizeMismatch,

	//! Invalid option.
	kErrorInvalidOption,
	//! Option already defined.
	kErrorOptionAlreadyDefined,

	//! Invalid TypeId.
	kErrorInvalidTypeId,
	//! Invalid use of a 8-bit GPB-HIGH register.
	kErrorInvalidUseOfGpbHi,
	//! Invalid use of a 64-bit GPQ register in 32-bit mode.
	kErrorInvalidUseOfGpq,
	//! Invalid use of an 80-bit float (\ref TypeId::kFloat80).
	kErrorInvalidUseOfF80,
	//! Instruction requires the use of consecutive registers, but registers in operands weren't (AVX512, ASIMD load/store, etc...).
	kErrorNotConsecutiveRegs,
	//! Failed to allocate consecutive registers - allocable registers either too restricted or a bug in RW info.
	kErrorConsecutiveRegsAllocation,

	//! Illegal virtual register - reported by instruction validation.
	kErrorIllegalVirtReg,
	//! AsmJit cannot create more virtual registers.
	kErrorTooManyVirtRegs,

	//! AsmJit requires a physical register, but no one is available.
	kErrorNoMorePhysRegs,
	//! A variable has been assigned more than once to a function argument (BaseCompiler).
	kErrorOverlappedRegs,
	//! Invalid register to hold stack arguments offset.
	kErrorOverlappingStackRegWithRegArg,

	//! Unbound label cannot be evaluated by expression.
	kErrorExpressionLabelNotBound,
	//! Arithmetic overflow during expression evaluation.
	kErrorExpressionOverflow,

	//! Failed to open anonymous memory handle or file descriptor.
	kErrorFailedToOpenAnonymousMemory,

	//! Failed to open a file.
	//!
	//! \note This is a generic error that is used by internal filesystem API.
	kErrorFailedToOpenFile,

	//! Protection failure can be returned from a virtual memory allocator or when trying to change memory access
	//! permissions.
	kErrorProtectionFailure,

	// @EnumValuesEnd@

	//! Count of AsmJit error codes.
	kErrorCount
};
}
