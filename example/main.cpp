
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <sstream>

#include <geek/utils/searcher.h>
#include <geek/utils/file.h>
#include <geek/process/process.h>

const unsigned char hexData[240] = {
	0x4D, 0x5A, 0x90, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
	0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00,
	0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD, 0x21, 0xB8, 0x01, 0x4C, 0xCD, 0x21, 0x54, 0x68,
	0x69, 0x73, 0x20, 0x70, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x20, 0x63, 0x61, 0x6E, 0x6E, 0x6F,
	0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6E, 0x20, 0x69, 0x6E, 0x20, 0x44, 0x4F, 0x53, 0x20,
	0x6D, 0x6F, 0x64, 0x65, 0x2E, 0x0D, 0x0D, 0x0A, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1D, 0x11, 0x45, 0x14, 0x59, 0xDC, 0xCE, 0xC8, 0x59, 0xDC, 0xCE, 0xC8, 0x59, 0xDC, 0xCE, 0xC8,
	0x49, 0x58, 0xCF, 0xC9, 0x5A, 0xDC, 0xCE, 0xC8, 0x49, 0x58, 0xCD, 0xC9, 0x5A, 0xDC, 0xCE, 0xC8,
	0x49, 0x58, 0xCA, 0xC9, 0x53, 0xDC, 0xCE, 0xC8, 0x49, 0x58, 0xCB, 0xC9, 0x4E, 0xDC, 0xCE, 0xC8,
	0x12, 0xA4, 0xCF, 0xC9, 0x5D, 0xDC, 0xCE, 0xC8, 0x59, 0xDC, 0xCF, 0xC8, 0x1C, 0xDC, 0xCE, 0xC8,
	0x12, 0x59, 0xCB, 0xC9, 0x58, 0xDC, 0xCE, 0xC8, 0x12, 0x59, 0x31, 0xC8, 0x58, 0xDC, 0xCE, 0xC8,
	0x12, 0x59, 0xCC, 0xC9, 0x58, 0xDC, 0xCE, 0xC8, 0x52, 0x69, 0x63, 0x68, 0x59, 0xDC, 0xCE, 0xC8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x45, 0x00, 0x00, 0x64, 0x86, 0x08, 0x00
};

int main() {
	//TODO FindByModuleName������
	auto m = geek::ThisProc().Modules().FindByModuleName(L"example.exe");

	hexData[0];
	
	auto res = geek::ThisProc().SearchSig("11 45 14", m.DllBase(), m.SizeOfImage());
	
	for (auto o : res)
	{
		printf("%llx - %llx\n", o, *reinterpret_cast<const uint64_t*>(o));
	}
}

// auto dir = geek::File::GetAppDirectory();
// auto proc_opt = geek::Process::Open(L"C:\\Windows\\notepad.exe", PROCESS_ALL_ACCESS);
// if (!proc_opt.has_value())
// {
// 	auto opt = geek::Process::Create(L"C:\\Windows\\notepad.exe");
// 	if (!opt.has_value())
// 	{
// 		return -1;
// 	}
// 	proc_opt = { std::move(std::get<0>(opt.value())) };
// }
// auto proc = std::move(proc_opt.value());