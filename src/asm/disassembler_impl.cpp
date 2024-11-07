#include "disassembler_impl.h"

#include "utils/debug.h"

namespace geek {
namespace {
ZydisMachineMode ToZydis(DisAssembler::MachineMode m) {
	switch (m)
	{
	case DisAssembler::MachineMode::kLong64: return ZYDIS_MACHINE_MODE_LONG_64;
	case DisAssembler::MachineMode::kLongCompat32: return ZYDIS_MACHINE_MODE_LONG_COMPAT_32;
	case DisAssembler::MachineMode::kLongCompat16: return ZYDIS_MACHINE_MODE_LONG_COMPAT_16;
	case DisAssembler::MachineMode::kLegacy32: return ZYDIS_MACHINE_MODE_LEGACY_32;
	case DisAssembler::MachineMode::kLegacy16: return ZYDIS_MACHINE_MODE_LEGACY_16;
	case DisAssembler::MachineMode::kReal16: return ZYDIS_MACHINE_MODE_REAL_16;
	default:
		GEEK_ASSERT(false, "Unknow DisAssembler::MachineMode:", static_cast<uint8_t>(m));
		return ZYDIS_MACHINE_MODE_MAX_VALUE;
	}
}
ZydisStackWidth ToZydis(DisAssembler::StackWidth s) {
	switch (s)
	{
	case DisAssembler::StackWidth::k16: return ZYDIS_STACK_WIDTH_16;
	case DisAssembler::StackWidth::k32: return ZYDIS_STACK_WIDTH_32;
	case DisAssembler::StackWidth::k64: return ZYDIS_STACK_WIDTH_64;
	default:
		GEEK_ASSERT(false, "Unknow DisAssembler::StackWidth:", static_cast<uint8_t>(s));
		return ZYDIS_STACK_WIDTH_MAX_VALUE;
	}
}
ZydisFormatterStyle ToZydis(DisAssembler::FormatterStyle f) {
	switch (f)
	{
	case DisAssembler::FormatterStyle::kATT: return ZYDIS_FORMATTER_STYLE_ATT;
	case DisAssembler::FormatterStyle::kIntel: return ZYDIS_FORMATTER_STYLE_INTEL;
	case DisAssembler::FormatterStyle::kIntelMasm: return ZYDIS_FORMATTER_STYLE_INTEL_MASM;
	default:
		GEEK_ASSERT(false, "Unknow DisAssembler::FormatterStyle:", static_cast<uint8_t>(f));
		return ZYDIS_FORMATTER_STYLE_MAX_VALUE;
	}
}
}

DisAssembler::Impl::Impl(MachineMode machine_mode, StackWidth stack_width, FormatterStyle style)
{
	ZydisDecoderInit(&decoder_, ToZydis(machine_mode), ToZydis(stack_width));
	ZydisFormatterInit(&formatter_, ToZydis(style));
}
}