#include <geek/process/process.h>

#include <regex>
#include <algorithm>
#include <array>
#include <mutex>
#include <cstddef>
#include <psapi.h>
#include <geek/process/ntinc.h>
#include <geek/utils/strutil.h>
#include <geek/utils/searcher.h>

#include "errordefs.h"
#include "utils/debug.h"

namespace geek {
namespace {
// template<typename IMAGE_THUNK_DATA_T>
// bool RepairImportAddressTableFromModule(Process& proc, Image* image, _IMAGE_IMPORT_DESCRIPTOR* import_descriptor, uint64_t import_image_base, bool skip_not_loaded) {
// 	IMAGE_THUNK_DATA_T* import_name_table = (IMAGE_THUNK_DATA_T*)image->RvaToPoint(import_descriptor->OriginalFirstThunk);
// 	IMAGE_THUNK_DATA_T* import_address_table = (IMAGE_THUNK_DATA_T*)image->RvaToPoint(import_descriptor->FirstThunk);
// 	Image import_image;
// 	if (import_image_base) {
// 		auto import_image_res = proc.LoadImageFromImageBase(import_image_base);
// 		if (!import_image_res) {
// 			return false;
// 		}
// 		import_image = std::move(*import_image_res);
// 	}
// 	else if (!skip_not_loaded) {
// 		return false;
// 	}
// 	for (; import_name_table->u1.ForwarderString; import_name_table++, import_address_table++) {
// 		if (!import_image_base) {
// 			import_address_table->u1.Function = import_address_table->u1.Function = 0x1234567887654321;
// 			continue;
// 		}
// 		uint32_t export_rva;
// 		if (import_name_table->u1.Ordinal >> (sizeof(import_name_table->u1.Ordinal) * 8 - 1) == 1) {
// 			auto export_addr = proc.GetExportProcAddress(&import_image, (char*)((import_name_table->u1.Ordinal << 1) >> 1));
// 			if (!export_addr) return false;
// 			import_address_table->u1.Function = export_addr.value();
// 		}
// 		else {
// 			IMAGE_IMPORT_BY_NAME* func_name = (IMAGE_IMPORT_BY_NAME*)image->RvaToPoint(import_name_table->u1.AddressOfData);
// 			auto export_addr = proc.GetExportProcAddress(&import_image, (char*)func_name->Name);
// 			if (!export_addr) return false;
// 			import_address_table->u1.Function = export_addr.value();
// 		}
// 		//import_address_table->u1.Function = import_module_base + export_rva;
// 	}
// 	return true;
// }

class CallPageX86 {
public:
	CallPageX86(Process* process, bool sync)
		: process_(process) {
		auto res = process->AllocMemory(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (res) {
			exec_page_ = *res;
			process->CallGenerateCodeX86(exec_page_, sync);
		}
	}

	~CallPageX86() {
		Close();
	}

	CallPageX86(const CallPageX86&) = delete;
	void operator=(const CallPageX86&) = delete;

	CallPageX86(CallPageX86&& rv) {
		Close();
		process_ = rv.process_;
		exec_page_ = rv.exec_page_;
		rv.exec_page_ = 0;
	}

	uint64_t exec_page() const { return exec_page_; }


private:
	void Close() {
		if (exec_page_) {
			process_->FreeMemory(exec_page_);
			exec_page_ = 0;
		}
	}
private:
	Process* process_;
	uint64_t exec_page_ = 0;
};
struct ExecPageHeaderX86 {
	uint32_t call_addr;
	uint32_t context_addr;
	uint32_t stack_count;
	uint32_t stack_addr;
};

class CallPageX64 {
public:
	CallPageX64(Process* process, bool sync)
		: process_(process) {
		auto res = process->AllocMemory(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (res) {
			exec_page_ = *res;
			process->CallGenerateCodeX64(exec_page_, sync);
		}
	}

	~CallPageX64() {
		Close();
	}

	CallPageX64(const CallPageX64&) = delete;
	void operator=(const CallPageX64&) = delete;

	CallPageX64(CallPageX64&& rv) {
		Close();
		process_ = rv.process_;
		exec_page_ = rv.exec_page_;
		rv.exec_page_ = 0;
	}

	uint64_t exec_page() const { return exec_page_; }


private:
	void Close() {
		if (exec_page_) {
			process_->FreeMemory(exec_page_);
			exec_page_ = 0;
		}
	}
private:
	Process* process_;
	uint64_t exec_page_ = 0;
};

struct ExecPageHeaderX64 {
	uint64_t call_addr;
	uint64_t context_addr;
	uint64_t stack_count;
	uint64_t stack_addr;
};


typedef VOID(NTAPI* PIMAGE_TLS_CALLBACK32)(uint32_t DllHandle, DWORD Reason, PVOID Reserved);
typedef VOID(NTAPI* PIMAGE_TLS_CALLBACK64)(uint64_t DllHandle, DWORD Reason, PVOID Reserved);
typedef BOOL(WINAPI* DllEntryProc32)(uint32_t hinstDLL, DWORD fdwReason, uint32_t lpReserved);
typedef BOOL(WINAPI* DllEntryProc64)(uint64_t hinstDLL, DWORD fdwReason, uint64_t lpReserved);
typedef int (WINAPI* ExeEntryProc)(void);

auto NtdllModule = GetModuleHandleW(L"ntdll.dll");
auto NtQueryInformationProcess = (pfnNtQueryInformationProcess)GetProcAddress(NtdllModule, "NtQueryInformationProcess");
auto NtWow64QueryInformationProcess64 = (pfnNtWow64QueryInformationProcess64)GetProcAddress(NtdllModule, "NtWow64QueryInformationProcess64");
auto NtWow64ReadVirtualMemory64 = (pfnNtWow64ReadVirtualMemory64)::GetProcAddress(NtdllModule, "NtWow64ReadVirtualMemory64");
}


NewlyCreatedProcessInfo::NewlyCreatedProcessInfo(const PROCESS_INFORMATION& info)
	: info_(info)
{
}

Process NewlyCreatedProcessInfo::NewlyProcess() const
{
	return Process(UniqueHandle(info_.hProcess));
}

Thread NewlyCreatedProcessInfo::NewlyThread() const
{
	return Thread(UniqueHandle(info_.hThread));
}

std::optional<Process> Process::Open(DWORD pid, DWORD desiredAccess)
{
	auto hProcess = OpenProcess(desiredAccess, FALSE, pid);
	if (hProcess == NULL) {
		return {};
	}
	return Process{ UniqueHandle(hProcess) };
}

std::optional<Process> Process::Open(std::wstring_view process_name, DWORD desiredAccess)
{
	auto p = CachedProcessList().FindFirstByProcName(process_name);
	if (p.IsEnd()) {
		return std::nullopt;
	}
	return Open(p.ProcessId(), desiredAccess);
}

std::optional<NewlyCreatedProcessInfo> Process::Create(std::wstring_view command, BOOL inheritHandles,
	DWORD creationFlags)
{
	std::wstring command_ = command.data();
	STARTUPINFOW startupInfo{ sizeof(startupInfo) };
	PROCESS_INFORMATION processInformation{};
	if (!CreateProcessW(NULL, (LPWSTR)command_.c_str(), NULL, NULL, inheritHandles, creationFlags, NULL, NULL, &startupInfo, &processInformation)) {
		return {};
	}
	return processInformation;
}

std::optional<NewlyCreatedProcessInfo> Process::CreateByToken(std::wstring_view tokenProcessName,
	std::wstring_view command, BOOL inheritHandles, DWORD creationFlags, STARTUPINFOW* si, PROCESS_INFORMATION* pi)
{
	HANDLE hToken_ = NULL;
	auto p = CachedProcessList().FindFirstByProcName(tokenProcessName);
	if (p.IsEnd())
		return std::nullopt;

	UniqueHandle hProcess{ OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, p.ProcessId()) };
	OpenProcessToken(*hProcess, TOKEN_ALL_ACCESS, &hToken_);
	if (hToken_ == NULL) {
		return {};
	}
	UniqueHandle hToken{ hToken_ };

	if (!si) {
		STARTUPINFOW tempSi{};
		si = &tempSi;
	}
	if (!pi) {
		PROCESS_INFORMATION tempPi{ };
		pi = &tempPi;
	}
	si->cb = sizeof(STARTUPINFO);
	// si->lpDesktop = L"winsta0\\default";
	si->dwFlags |= STARTF_USESHOWWINDOW;
	si->wShowWindow |= SW_HIDE;
	std::wstring command_copy = command.data();
	BOOL ret = CreateProcessAsUserW(*hToken, NULL, (LPWSTR)command_copy.c_str(), NULL, NULL, inheritHandles, creationFlags | NORMAL_PRIORITY_CLASS, NULL, NULL, si, pi);
	if (!ret) {
		return {};
	}
	return *pi;
}

std::vector<uint64_t> Process::SearchSig(std::string_view hex_string, uint64_t start_address, size_t size) const
{
	if (auto opt = ReadMemory(start_address, size); opt)
	{
		std::vector<uint64_t> total{};
		auto m = std::move(*opt);
		auto res = Searcher::SearchHex(hex_string, reinterpret_cast<const char*>(m.data()), m.size());
		if (res.empty())
			return {};

		for (auto i : res)
			total.push_back(start_address + i);
		return total;
	}
	return {};
}

bool Process::Terminate(uint32_t exitCode)
{
	bool ret = ::TerminateProcess(Handle(), exitCode);
	process_handle_.Reset();
	return ret;
}

bool Process::SetDebugPrivilege(bool IsEnable) const
{
	DWORD LastError = 0;
	HANDLE TokenHandle = 0;

	if (!OpenProcessToken(Handle(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TokenHandle)) {
		LastError = GetLastError();
		if (TokenHandle) {
			CloseHandle(TokenHandle);
		}
		return LastError;
	}
	TOKEN_PRIVILEGES TokenPrivileges;
	memset(&TokenPrivileges, 0, sizeof(TOKEN_PRIVILEGES));
	LUID v1;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &v1)) {
		LastError = GetLastError();
		CloseHandle(TokenHandle);
		return LastError;
	}
	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Luid = v1;
	if (IsEnable) {
		TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else {
		TokenPrivileges.Privileges[0].Attributes = 0;
	}
	AdjustTokenPrivileges(TokenHandle, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	LastError = GetLastError();
	CloseHandle(TokenHandle);
	return LastError;
}

HANDLE Process::Handle() const noexcept
{
	if (this == nullptr) {
		return ThisProc().Handle();
	}
	return *process_handle_;
}

DWORD Process::ProcId() const noexcept
{
	return GetProcessId(Handle());
}

bool Process::IsX32() const noexcept
{
	auto handle = Handle();

	::BOOL IsWow64;
	if (!::IsWow64Process(handle, &IsWow64)) {
		return false;
	}

	if (IsWow64) {
		return true;
	}

	::SYSTEM_INFO SystemInfo = { 0 };
	::GetNativeSystemInfo(&SystemInfo);
	if (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
		return false;
	}
	else if (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) {
		return true;
	}
	return false;

}

bool Process::IsCurrent() const
{
	return *process_handle_ == ThisProc().Handle();
}

std::optional<uint64_t> Process::AllocMemory(uint64_t addr, size_t len, DWORD type, DWORD protect) const
{
	if (ms_wow64.Wow64Operation(Handle())) {
		auto ptr = ms_wow64.VirtualAllocEx64(Handle(), (DWORD64)addr, len, type, protect);
		if (ptr == 0) {
			return {};
		}
		return static_cast<uint64_t>(ptr);
	}
	auto ptr = VirtualAllocEx(Handle(), (LPVOID)addr, len, type, protect);
	if (ptr == NULL) {
		return {};
	}
	return reinterpret_cast<uint64_t>(ptr);
}

std::optional<uint64_t> Process::AllocMemory(size_t len, DWORD type, DWORD protect) const
{
	return AllocMemory(NULL, len, type, protect);
}

bool Process::FreeMemory(uint64_t addr, size_t size, DWORD type) const
{
	if (ms_wow64.Wow64Operation(Handle())) {
		return ms_wow64.VirtualFreeEx64(Handle(), (DWORD64)addr, size, type);
	}
	return VirtualFreeEx(Handle(), (LPVOID)addr, size, type);
}

bool Process::ReadMemory(uint64_t addr, void* buf, size_t len) const
{
	SIZE_T readByte;
	if (IsCurrent()) {
		memcpy(buf, (void*)addr, len);
		return true;
	}
	if (ms_wow64.Wow64Operation(Handle())) {
		auto e = NtWow64ReadVirtualMemory64(Handle(), addr, buf, len, NULL);
		GEEK_UPDATE_NT_ERROR(e);
		if (!NT_SUCCESS(e)) {
			return false;
		}
	}
	else {
		if (!::ReadProcessMemory(Handle(), (void*)addr, buf, len, &readByte)) {
			GEEK_UPDATE_WIN_ERROR();
			// throw ProcessException(ProcessException::Type::kReadProcessMemoryError);
			return false;
		}
	}
	return true;
}

std::optional<std::vector<uint8_t>> Process::ReadMemory(uint64_t addr, size_t len) const
{
	std::vector<uint8_t> buf;
	buf.resize(len, 0);
	if (!ReadMemory(addr, buf.data(), len)) {
		return {};
	}
	return buf;
}

bool Process::WriteMemory(uint64_t addr, const void* buf, size_t len, bool force) const
{
	DWORD oldProtect;
	if (force) {
		if (!SetMemoryProtect(addr, len, PAGE_EXECUTE_READWRITE, &oldProtect)) {
			GEEK_UPDATE_WIN_ERROR();
			return false;
		}
	}
	SIZE_T readByte;
	bool success = true;
	if (ms_wow64.Wow64Operation(Handle())) {
		HMODULE NtdllModule = GetModuleHandleW(L"ntdll.dll");
		pfnNtWow64QueryInformationProcess64 NtWow64QueryInformationProcess64 = (pfnNtWow64QueryInformationProcess64)GetProcAddress(NtdllModule, "NtWow64QueryInformationProcess64");
		pfnNtWow64WriteVirtualMemory64 NtWow64WriteVirtualMemory64 = (pfnNtWow64WriteVirtualMemory64)GetProcAddress(NtdllModule, "NtWow64WriteVirtualMemory64");
		if (!NT_SUCCESS(NtWow64WriteVirtualMemory64(Handle(), addr, (PVOID)buf, len, NULL))) {
			GEEK_UPDATE_WIN_ERROR();
			success = false;
		}
	}
	else {
		if (IsCurrent()) {
			memcpy((void*)addr, buf, len);
		}
		else if (!::WriteProcessMemory(Handle(), (void*)addr, buf, len, &readByte)) {
			GEEK_UPDATE_WIN_ERROR();
			success = false;
		}
	}
	if (force) {
		SetMemoryProtect(addr, len, oldProtect, &oldProtect);
	}
	return success;
}

std::optional<uint64_t> Process::WriteMemoryWithAlloc(const void* buf, size_t len, DWORD protect) const
{
	auto mem = AllocMemory(len, (DWORD)MEM_COMMIT, protect);
	if (!mem) {
		return {};
	}
	WriteMemory(mem.value(), buf, len);
	return mem;
}

bool Process::SetMemoryProtect(uint64_t addr, size_t len, DWORD newProtect, DWORD* oldProtect) const
{
	bool success = false;
	if (ms_wow64.Wow64Operation(Handle())) {
		success = ms_wow64.VirtualProtectEx64(Handle(), (DWORD64)addr, len, newProtect, oldProtect);
	}
	else {
		success = ::VirtualProtectEx(Handle(), (LPVOID)addr, len, newProtect, oldProtect);
	}
	return success;
}

std::optional<MemoryProtectScope> Process::GetMemoryProtectScope(uint64_t address, size_t length, DWORD protect) const {
    auto scope = MemoryProtectScope(this, address, length, protect);

    DWORD old_protect;
    if (scope.ChangeProtect(protect, &old_protect)) {
        scope.old_protect_ = old_protect;
        return { std::move(scope) };
    }
    return std::nullopt;
}

Address Process::At(uint64_t addr) const
{
	return { const_cast<Process*>(this), addr };
}

std::wstring Process::DebugName() const
{
	std::wstring p = IsX32() ? L"-x32" : L"-x64";
	if (auto n = BaseName())
	{
		return *n + p;
	}
	return L"<unknow>" + p;
}

std::optional<std::wstring> Process::BaseName() const
{
	wchar_t tmp[MAX_PATH]{};
	if (!GetModuleBaseNameW(Handle(), NULL, tmp, sizeof(tmp) / sizeof(wchar_t)))
	{
		GEEK_UPDATE_WIN_ERROR();
		return std::nullopt;
	}
	return std::wstring(tmp);
}

// std::optional<MemoryInfo> Process::GetMemoryInfo(uint64_t addr) const
// {
// 	uint64_t size;
// 	MEMORY_BASIC_INFORMATION    memInfo = { 0 };
// 	MEMORY_BASIC_INFORMATION64    memInfo64 = { 0 };
// 	if (ms_wow64.Wow64Operation(Handle())) {
// 		size = geek::Wow64::VirtualQueryEx64(Handle(), addr, &memInfo64, sizeof(memInfo64));
// 		if (size != sizeof(memInfo64)) { return {}; }
// 		return MemoryInfo(memInfo64);
// 	}
// 	else {
// 		size_t size = ::VirtualQueryEx(Handle(), (PVOID)addr, &memInfo, sizeof(memInfo));
// 		if (size != sizeof(memInfo)) { return {}; }
// 		if (IsX86()) {
// 			return MemoryInfo(*(MEMORY_BASIC_INFORMATION32*)&memInfo);
// 		}
// 		else {
// 			return MemoryInfo(*(MEMORY_BASIC_INFORMATION64*)&memInfo);
// 		}
// 	}
// }
//
// std::optional<std::vector<MemoryInfo>> Process::GetMemoryInfoList() const
// {
// 	std::vector<MemoryInfo> memory_block_list;
//
// 	memory_block_list.reserve(200);
// 	/*
//         typedef struct _SYSTEM_INFO {
//         union {
//         DWORD dwOemId;
//         struct {
//         WORD wProcessorArchitecture;
//         WORD wReserved;
//         } DUMMYSTRUCTNAME;
//         } DUMMYUNIONNAME;
//         DWORD     dwPageSize;
//         LPVOID    lpMinimumApplicationAddress;
//         LPVOID    lpMaximumApplicationAddress;
//         DWORD_PTR dwActiveProcessorMask;
//         DWORD     dwNumberOfProcessors;
//         DWORD     dwProcessorType;
//         DWORD     dwAllocationGranularity;
//         WORD        wProcessorLevel;
//         WORD        wProcessorRevision;
//         } SYSTEM_INFO, *LPSYSTEM_INFO;
//         */
//
// 	uint64_t p = 0;
// 	MEMORY_BASIC_INFORMATION mem_info = { 0 };
// 	MEMORY_BASIC_INFORMATION64 mem_info64 = { 0 };
// 	while (true) {
// 		uint64_t size;
// 		if (ms_wow64.Wow64Operation(Handle())) {
// 			size = geek::Wow64::VirtualQueryEx64(Handle(), p, &mem_info64, sizeof(mem_info64));
// 			if (size != sizeof(mem_info64)) { break; }
// 			memory_block_list.push_back(MemoryInfo{ mem_info64 });
// 			p += mem_info64.RegionSize;
// 		}
// 		else {
// 			size_t size = ::VirtualQueryEx(Handle(), (PVOID)p, &mem_info, sizeof(mem_info));
// 			if (size != sizeof(mem_info)) { break; }
// 			if (IsX86()) {
// 				memory_block_list.push_back(MemoryInfo{ *(MEMORY_BASIC_INFORMATION32*)&mem_info });
// 			}
// 			else {
// 				memory_block_list.push_back(MemoryInfo{ *(MEMORY_BASIC_INFORMATION64*)&mem_info });
// 			}
// 			p += mem_info.RegionSize;
// 		}
//             
// 	}
// 	return memory_block_list;
// }
//
// bool Process::ScanMemoryInfoList(const std::function<bool(uint64_t raw_addr, uint8_t* addr, size_t size)>& callback,
// 	bool include_module) const
// {
// 	bool success = false;
// 	do {
// 		auto module_list_res = GetModuleInfoList();
// 		if (!module_list_res) {
// 			return false;
// 		}
// 		auto& module_list = module_list_res.value();
// 		auto vec_res = GetMemoryInfoList();
// 		if (!vec_res) {
// 			return false;
// 		}
// 		auto& vec = vec_res.value();
// 		size_t sizeSum = 0;
//
// 		for (int i = 0; i < vec.size(); i++) {
// 			if (vec[i].protect & PAGE_NOACCESS || !vec[i].protect) {
// 				continue;
// 			}
//
// 			if (include_module == false) {
// 				bool is_module = false;
// 				for (int j = 0; j < module_list.size(); j++) {
// 					if (vec[i].base >= module_list[j].base && vec[i].base < module_list[j].base + module_list[j].base) {
// 						is_module = true;
// 						break;
// 					}
// 				}
// 				if (!(!is_module && vec[i].protect & PAGE_READWRITE && vec[i].state & MEM_COMMIT)) {
// 					continue;
// 				}
// 			}
//
// 			auto temp_buff = ReadMemory(vec[i].base, vec[i].size);
// 			if (!temp_buff) {
// 				continue;
// 			}
//                 
// 			if (callback(vec[i].base, temp_buff.value().data(), temp_buff.value().size())) {
// 				break;
// 			}
// 			sizeSum += vec[i].size;
// 		}
// 		success = true;
// 	} while (false);
// 	return success;
// }

std::optional<std::wstring> Process::GetCommandLineStr() const
{
	typedef NTSTATUS(NTAPI* _NtQueryInformationProcess)(
		HANDLE ProcessHandle,
		DWORD ProcessInformationClass,
		PVOID ProcessInformation,
		DWORD ProcessInformationLength,
		PDWORD ReturnLength
	);
        
	if (IsX32()) {
		UNICODE_STRING32 commandLine;
		_NtQueryInformationProcess NtQuery = (_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
		if (!NtQuery) {
			return {};
		}

		PROCESS_BASIC_INFORMATION32 pbi;
		NTSTATUS isok = NtQuery(Handle(), ProcessBasicInformation, &pbi, sizeof(RTL_USER_PROCESS_PARAMETERS32), NULL);
		if (!NT_SUCCESS(isok)) {
			return {};
		}

		PEB32 peb;
		RTL_USER_PROCESS_PARAMETERS32 upps;
		PRTL_USER_PROCESS_PARAMETERS32 rtlUserProcParamsAddress;
		if (!ReadMemory((uint64_t)&(((PEB32*)(pbi.PebBaseAddress))->ProcessParameters), &rtlUserProcParamsAddress, sizeof(rtlUserProcParamsAddress))) {
			return {};
		}

		if (!ReadMemory((uint64_t)&(rtlUserProcParamsAddress->CommandLine), &commandLine, sizeof(commandLine))) {
			return {};
		}

		std::wstring buf(commandLine.Length, L' ');
		if (!ReadMemory((uint64_t)commandLine.Buffer,
		                (void*)buf.data(), commandLine.Length)) {
			return {};
		}
		return buf;
	}
	else {

		UNICODE_STRING64 commandLine;
		PROCESS_BASIC_INFORMATION64 pbi;
		HMODULE NtdllModule = GetModuleHandleA("ntdll.dll");
		if (ms_wow64.Wow64Operation(Handle())) {
			pfnNtWow64QueryInformationProcess64 NtWow64QueryInformationProcess64 = (pfnNtWow64QueryInformationProcess64)GetProcAddress(NtdllModule, "NtWow64QueryInformationProcess64");
			if (!NT_SUCCESS(NtWow64QueryInformationProcess64(Handle(), ProcessBasicInformation, &pbi, sizeof(pbi), NULL))) {
				return {};
			}
		}
		else {
			pfnNtQueryInformationProcess NtQueryInformationProcess = (pfnNtQueryInformationProcess)GetProcAddress(NtdllModule, "NtQueryInformationProcess");
			if (!NT_SUCCESS(NtQueryInformationProcess(Handle(), ProcessBasicInformation, &pbi, sizeof(pbi), NULL))) {
				return {};
			}
		}

		PEB64 peb;
		RTL_USER_PROCESS_PARAMETERS64 upps;
		PRTL_USER_PROCESS_PARAMETERS64 rtlUserProcParamsAddress;
		if (!ReadMemory((uint64_t) & (((PEB64*)(pbi.PebBaseAddress))->ProcessParameters), &rtlUserProcParamsAddress, sizeof(rtlUserProcParamsAddress))) {
			return {};
		}

		if (!ReadMemory((uint64_t) & (rtlUserProcParamsAddress->CommandLine), &commandLine, sizeof(commandLine))) {
			return {};
		}

		std::wstring buf(commandLine.Length, L' ');
		if (!ReadMemory((uint64_t)commandLine.Buffer,
		                (void*)buf.data(), commandLine.Length)) {
			return {};
		}
		return buf;
	}
}

std::optional<uint16_t> Process::LockAddress(uint64_t addr) const
{
	uint16_t instr;
	if (!ReadMemory(addr, &instr, 2)) {
		return {};
	}
	unsigned char jmpSelf[] = { 0xeb, 0xfe };
	if (!WriteMemory(addr, jmpSelf, 2, true)) {
		return {};
	}
	return instr;
}

bool Process::UnlockAddress(uint64_t addr, uint16_t instr) const
{
	return WriteMemory(addr, &instr, 2, true);
}

std::optional<Thread> Process::CreateThread(uint64_t start_routine, uint64_t parameter, DWORD dwCreationFlags) const
{
	DWORD thread_id = 0;
	HANDLE thread_handle = NULL;
	if (IsCurrent()) {
		thread_handle = ::CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(start_routine), reinterpret_cast<PVOID64>(parameter), dwCreationFlags, &thread_id);
	}
	else {
		if (ms_wow64.Wow64Operation(Handle())) {
			auto ntdll64 = ms_wow64.GetNTDLL64();
			auto RtlCreateUserThread = ms_wow64.GetProcAddress64(ntdll64, "RtlCreateUserThread");
			auto ntdll_RtlExitThread = ms_wow64.GetProcAddress64(ntdll64, "RtlExitUserThread");

			unsigned char shell_code[] = {
				0x48, 0x89, 0x4c, 0x24, 0x08,                               // mov       qword ptr [rsp+8],rcx 
				0x57,                                                       // push      rdi
				0x48, 0x83, 0xec, 0x20,                                     // sub       rsp,20h
				0x48, 0x8b, 0xfc,                                           // mov       rdi,rsp
				0xb9, 0x08, 0x00, 0x00, 0x00,                               // mov       ecx,8
				0xb8, 0xcc, 0xcc, 0xcc, 0xcc,                               // mov       eac,0CCCCCCCCh
				0xf3, 0xab,                                                 // rep stos  dword ptr [rdi]
				0x48, 0x8b, 0x4c, 0x24, 0x30,                               // mov       rcx,qword ptr [__formal]
				0x48, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rcx,   parameter
				0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rax,0 
				0xff, 0xd0,                                                 // call      rax    start_routine
				0x48, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rcx,0
				0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov       rax,0
				0xff, 0xd0                                                  // call      rax
                    
			};

			auto buf_addr = AllocMemory(size_t{ 4096 }, DWORD{ MEM_RESERVE | MEM_COMMIT }, PAGE_EXECUTE_READWRITE);
			if (!buf_addr) {
				return {};
			}

			//r8
			memcpy(shell_code + 32, &parameter, sizeof(parameter));

			memcpy(shell_code + 42, &start_routine, sizeof(start_routine));

			//RtlExitUserThread
			memcpy(shell_code + 64, &ntdll_RtlExitThread, sizeof(DWORD64));
			size_t write_size = 0;

			if (!WriteMemory(*buf_addr, shell_code, sizeof(shell_code))) {
				FreeMemory(*buf_addr);
				return {};
			}

			struct {
				DWORD64 UniqueProcess;
				DWORD64 UniqueThread;
			} client_id { 0 };

			auto error = ms_wow64.X64Call(RtlCreateUserThread, 10,
			                              reinterpret_cast<DWORD64>(Handle()), 
			                              static_cast<DWORD64>(NULL), static_cast<DWORD64>(FALSE),
			                              static_cast<DWORD64>(0), static_cast<DWORD64>(NULL), static_cast<DWORD64>(NULL),
			                              static_cast<DWORD64>(*buf_addr), static_cast<DWORD64>(0),
			                              reinterpret_cast<DWORD64>(&thread_handle),
			                              reinterpret_cast<DWORD64>(&client_id));
                
			if (thread_handle) {
				::WaitForSingleObject(thread_handle, INFINITE);
			}

			FreeMemory(*buf_addr);
			if (!NT_SUCCESS(error)) {
				return {};
			}
		}
		else {
			thread_handle = ::CreateRemoteThread(Handle(), NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(start_routine), reinterpret_cast<PVOID64>(parameter), dwCreationFlags, &thread_id);
		}
	}
	if (thread_handle == NULL) {
		return {};
	}
	return Thread{ thread_handle };
}

std::optional<uint16_t> Process::BlockThread(Thread* thread) const
{
	if (!thread->Suspend()) {
		return {};
	}
	unsigned char jmpSelf[] = { 0xeb, 0xfe };
	uint64_t ip;
	if (IsX32()) {
		_CONTEXT32 context;
		GetThreadContext(thread, context);
		ip = context.Eip;
	}
	else {
		_CONTEXT64 context;
		GetThreadContext(thread, context);
		ip = context.Rip;
	}
	auto old_instr = LockAddress(ip);
	thread->Resume();
	return old_instr;
}

bool Process::ResumeBlockedThread(Thread* thread, uint16_t instr) const
{
	if (!thread->Suspend()) {
		return false;
	}
	uint16_t oldInstr;
	uint64_t ip;
	if (IsX32()) {
		_CONTEXT32 context;
		GetThreadContext(thread, context);
		ip = context.Eip;
	}
	else {
		_CONTEXT64 context;
		GetThreadContext(thread, context);
		ip = context.Rip;
	}
	auto success = UnlockAddress(ip, instr);
	thread->Resume();
	return success;
}

bool Process::IsTheOwningThread(Thread* thread) const
{
	return GetProcessIdOfThread(thread) == ProcId();
}

bool Process::GetThreadContext(Thread* thread, _CONTEXT32& context, DWORD flags) const
{
	if (IsX32()) {
		return false;
	}
	bool success;
	context.ContextFlags = flags;
	if (!ThisProc().IsX32()) {
		success = ::Wow64GetThreadContext(thread->handle(), &context);
	}
	else {
		success = ::GetThreadContext(thread->handle(), reinterpret_cast<CONTEXT*>(&context));
	}
	return success;
}

bool Process::GetThreadContext(Thread* thread, _CONTEXT64& context, DWORD flags) const
{
	if (IsX32()) {
		return false;
	}
	bool success;
	context.ContextFlags = flags;
	if (ms_wow64.Wow64Operation(Handle())) {
		success = ms_wow64.GetThreadContext64(thread->handle(), &context);
	}
	else {
		success = ::GetThreadContext(thread->handle(), reinterpret_cast<CONTEXT*>(&context));
	}
	return success;
}

bool Process::SetThreadContext(Thread* thread, _CONTEXT32& context, DWORD flags) const
{
	if (!IsX32()) {
		return false;
	}
	bool success; 
	context.ContextFlags = flags;
	if (!ThisProc().IsX32()) {
		success = ::Wow64SetThreadContext(thread->handle(), &context);
	}
	else {
		success = ::SetThreadContext(thread->handle(), reinterpret_cast<CONTEXT*>(&context));
	}
	return success;
}

bool Process::SetThreadContext(Thread* thread, _CONTEXT64& context, DWORD flags) const
{
	if (!IsX32()) {
		return false;
	}
	bool success;
	context.ContextFlags = flags;
	if (ms_wow64.Wow64Operation(Handle())) {
		success = ms_wow64.SetThreadContext64(thread->handle(), &context);
	}
	else {
		success = ::SetThreadContext(thread->handle(), reinterpret_cast<CONTEXT*>(&context));
	}
	return success;
}

bool Process::WaitExit(DWORD dwMilliseconds) const
{
	if (IsCurrent()) {
		return false;
	}
	return WaitForSingleObject(Handle(), dwMilliseconds) == WAIT_OBJECT_0;
}

std::optional<DWORD> Process::GetExitCode() const
{
	DWORD code;
	if (!GetExitCodeProcess(Handle(), &code)) {
		return {};
	}
	return code;
}

// std::optional<uint64_t> Process::LoadLibraryFromImage(Image* image, bool exec_tls_callback, bool call_dll_entry,
// 	uint64_t init_parameter, bool skip_not_loaded, bool zero_pe_header, bool entry_call_sync)
// {
// 	if (IsX86() != image->IsPE32()) {
// 		return 0;
// 	}
// 	auto image_base_res = AllocMemory(image->GetImageSize(), (DWORD)MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
// 	if (!image_base_res) return {};
// 	auto& image_base = *image_base_res;
// 	bool success = false;
// 	do {
// 		if (!image->RepairRepositionTable(image_base)) {
// 			break;
// 		}
// 		if (!RepairImportAddressTable(image, skip_not_loaded)) {
// 			break;
// 		}
// 		auto image_buf = image->SaveToImageBuf(image_base, zero_pe_header);
// 		if (!WriteMemory(image_base, image_buf.data(), image_buf.size())) {
// 			break;
// 		}
// 		/*
//             * tls的调用必须同步，否则出现并发执行的问题
//             */
// 		if (exec_tls_callback) {
// 			ExecuteTls(image, image_base);
// 		}
// 		if (call_dll_entry) {
// 			CallEntryPoint(image, image_base, init_parameter, entry_call_sync);
// 		}
// 		success = true;
// 	} while (false);
// 	if (success == false && image_base) {
// 		FreeMemory(image_base);
// 		image_base = 0;
// 	}
// 	image->SetMemoryImageBase(image_base);
// 	return image_base;
// }

std::optional<Image> Process::LoadImageFromImageBase(uint64_t base) const
{
	if (IsCurrent()) {
		return Image::LoadFromImageBuf((void*)base, base);
	}
	else {
		auto m = Modules().FindByModuleBase(base);
		if (!m.IsValid()) return std::nullopt;

		auto buf = ReadMemory(base, m.SizeOfImage());
		if (!buf) {
			return {};
		}
		return Image::LoadFromImageBuf(buf->data(), base);
	}
}

bool Process::FreeLibraryFromImage(Image* image, bool call_dll_entry) const
{
	if (call_dll_entry) {
		//if (!CallEntryPoint(image, image->GetMemoryImageBase(), DLL_PROCESS_DETACH)) {
		//    return false;
		//}
	}
	return FreeMemory(image->GetMemoryImageBase());
}

std::optional<uint64_t> Process::LoadLibraryByFileName(std::wstring_view lib_name, bool sync)
{
	if (IsCurrent()) {
		auto addr = ::LoadLibraryW(lib_name.data());
		if (!addr) {
			return {};
		}
		return reinterpret_cast<uint64_t>(addr);
	}

	auto m = Modules().FindByModuleName(lib_name);
	if (m.IsValid()) {
		return m.DllBase();
	}

	uint64_t addr = NULL;
        
	if (ms_wow64.Wow64Operation(Handle())) {
		auto ntdll64 = ms_wow64.GetNTDLL64();
		auto LdrLoadDll = ms_wow64.GetProcAddress64(ntdll64, "LdrLoadDll");
		UNICODE_STRING64 us64;
		auto str_len = lib_name.size() * 2;
		if (str_len % 8 != 0) {
			str_len += 8 - str_len % 8;
		}
		auto len = 0x1000 + str_len + sizeof(UNICODE_STRING64) + sizeof(DWORD64);
		auto lib_name_buf_res = AllocMemory(NULL, len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!lib_name_buf_res) {
			return {};
		}
		auto lib_name_buf = *lib_name_buf_res;
		lib_name_buf += 0x1000;
		do {
			if (!WriteMemory(lib_name_buf, lib_name.data(), len)) {
				break;
			}
			auto unicode_str_addr = lib_name_buf + str_len;
               
			auto raw_str_len = lib_name.size() * 2;
			if (!WriteMemory(uint64_t{ unicode_str_addr + reinterpret_cast<uint64_t>(&((UNICODE_STRING64*)0)->Length) }, &raw_str_len, 2)) {
				break;
			}
			if (!WriteMemory(uint64_t{ unicode_str_addr + reinterpret_cast<uint64_t>(&((UNICODE_STRING64*)0)->MaximumLength) }, &raw_str_len, 2)) {
				break;
			}
			if (!WriteMemory(uint64_t{ unicode_str_addr + reinterpret_cast<uint64_t>(&((UNICODE_STRING64*)0)->Buffer) }, &lib_name_buf, 8)) {
				break;
			}

			Call(lib_name_buf - 0x1000, LdrLoadDll, { 0, 0, unicode_str_addr, unicode_str_addr + sizeof(UNICODE_STRING64) }, &addr, Process::CallConvention::kStdCall, sync);
		} while (false);
		if (sync && lib_name_buf) {
			FreeMemory(lib_name_buf);
		}

	}
	else {
        auto lib_name_len = (lib_name.size() + 1) * sizeof(wchar_t);
		auto len = 0x1000 + lib_name_len;
		auto buf_opt = AllocMemory(NULL, len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!buf_opt) {
			return {};
		}
		auto buf = *buf_opt;
		buf += 0x1000;
		do {
			if (!WriteMemory(buf, lib_name.data(), lib_name_len)) {
				break;
			}
			Call(buf - 0x1000, (uint64_t)::LoadLibraryW, { buf }, &addr, Process::CallConvention::kStdCall, sync);
		} while (false);
		if (sync && buf) {
			FreeMemory(buf);
		}
	}
        
	return addr;
}

bool Process::FreeLibrary(uint64_t module_base) const
{
	if (IsCurrent()) {
		return ::FreeLibrary((HMODULE)module_base);
	}
	do {
		auto thread = CreateThread((uint64_t)::FreeLibrary, module_base);
		if (!thread) {
			return false;
		}
		thread.value().WaitExit(INFINITE);
	} while (false);
	return false;
}
// std::optional<uint64_t> Process::GetExportProcAddress(Image* image, const char* func_name)
// {
// 	uint32_t export_rva;
// 	if (reinterpret_cast<uintptr_t>(func_name) <= 0xffff) {
// 		export_rva = image->GetExportRvaByOrdinal(reinterpret_cast<uint16_t>(func_name));
// 	}
// 	else {
// 		export_rva = image->GetExportRvaByName(func_name);
// 	}
// 	// 可能返回一个字符串，需要二次加载
// 	// 对应.def文件的EXPORTS后加上 MsgBox = user32.MessageBoxA 的情况
// 	uint64_t va = (uint64_t)image->GetMemoryImageBase() + export_rva;
// 	auto export_directory = (uint64_t)image->GetMemoryImageBase() + image->GetDataDirectory()[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
// 	auto export_directory_size = image->GetDataDirectory()[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
// 	// 还在导出表范围内，是这样子的字符串：NTDLL.RtlAllocateHeap
// 	if (va > export_directory && va < export_directory + export_directory_size) {
// 		std::string full_name = (char*)image->RvaToPoint(export_rva);
// 		auto offset = full_name.find(".");
// 		auto dll_name = full_name.substr(0, offset);
// 		auto func_name = full_name.substr(offset + 1);
// 		if (!dll_name.empty() && !func_name.empty()) {
// 			auto image_base = LoadLibrary(geek::Convert::AnsiToUtf16le(dll_name).c_str());
// 			if (image_base == 0) return {};
// 			auto import_image = LoadImageFromImageBase(image_base.value());
// 			if (!import_image) return {};
// 			auto va_res = GetExportProcAddress(&import_image.value(), func_name.c_str());
// 			if (!va_res) return {};
// 			return va_res.value();
// 		}
// 	}
// 	return va;
// }

bool Process::Call(uint64_t exec_page, uint64_t call_addr, const std::vector<uint64_t>& par_list, uint64_t* ret_value,
	CallConvention call_convention, bool sync, bool init_exec_page)
{

	bool success = false;
	if (IsX32()) {
		if (call_convention == CallConvention::kStdCall) {
			std::vector<uint32_t> converted_values;
			converted_values.reserve(par_list.size());  // 预先分配足够的空间

			// 遍历 input，将每个 uint64_t 值转换为 uint32_t 并存入 result
			std::transform(par_list.begin(), par_list.end(), std::back_inserter(converted_values),
			               [](uint64_t value) {
				               return static_cast<uint32_t>(value);  // 显式转换
			               });

			auto context = CallContextX86{};
			if (par_list.size() > 0) {
				auto list = std::initializer_list<uint32_t>(&*converted_values.begin(), &*(converted_values.end() - 1) + 1);
				context.stack = list;
			}
			success = Call(exec_page, call_addr, &context, sync, init_exec_page);
			if (ret_value && !sync) {
				*ret_value = context.eax;
			}
		}
	}
	else {
		auto context = CallContextX64{};
		if (par_list.size() >= 5) {
			auto list = std::initializer_list<uint64_t>(&par_list[4], &*(par_list.end() - 1) + 1);
			context.stack = list;
		}
		if (par_list.size() >= 1) {
			context.rcx = par_list[0];
		}
		if (par_list.size() >= 2) {
			context.rcx = par_list[1];
		}
		if (par_list.size() >= 3) {
			context.r8 = par_list[2];
		}
		if (par_list.size() >= 4) {
			context.r9 = par_list[3];
		}
		success = Call(exec_page, call_addr, &context, sync, init_exec_page);
		if (ret_value && !sync) {
			*ret_value = context.rax;
		}
	}
	return success;
}

bool Process::Call(uint64_t call_addr, const std::vector<uint64_t>& par_list, uint64_t* ret_value,
	CallConvention call_convention)
{
	auto exec_page  = AllocMemory(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!exec_page) {
		return false;
	}
	bool success = Call(*exec_page, call_addr, par_list, ret_value, call_convention, true, true);
	FreeMemory(*exec_page);
	return success;
}

bool Process::CallGenerateCodeX86(uint64_t exec_page, bool sync) const
{
	constexpr int32_t exec_offset = 0x100;

	std::array<uint8_t, 0x1000> temp_data = { 0 };
	uint8_t* temp = temp_data.data();
	if (IsCurrent()) {
		temp = reinterpret_cast<uint8_t*>(exec_page);
	}

	int32_t i = exec_offset;

	// 保存非易变寄存器
	// push ebp
	temp[i++] = 0x55;
	// mov ebp, esp
	temp[i++] = 0x89;
	temp[i++] = 0xe5;

	// 3个局部变量
	// sub esp, 0xc
	temp[i++] = 0x83;
	temp[i++] = 0xec;
	temp[i++] = 0x0c;

	// push ebx
	temp[i++] = 0x53;
	// push esi
	temp[i++] = 0x56;
	// push edi
	temp[i++] = 0x57;

	// 获取ExecPageHeaderX86*
	// mov eax, [ebp + 8]
	temp[i++] = 0x8b;
	temp[i++] = 0x45;
	temp[i++] = 0x08;

	// copy stack
	// mov ecx, [ExecPageHeaderX86.stack_count]
	temp[i++] = 0x8b;
	temp[i++] = 0x48;
	temp[i++] = offsetof(ExecPageHeaderX86, stack_count);

	// mov esi, [ExecPageHeaderX86.stack_addr]
	temp[i++] = 0x8b;
	temp[i++] = 0x70;
	temp[i++] = offsetof(ExecPageHeaderX86, stack_addr);

	// mov eax, 4
	temp[i++] = 0xb8;
	*(uint32_t*)&temp[i] = 4;
	i += 4;

	// mul ecx
	temp[i++] = 0xf7;
	temp[i++] = 0xe1;

	// sub esp, eax
	temp[i++] = 0x29;
	temp[i++] = 0xc4;

	// mov edi, esp
	temp[i++] = 0x89;
	temp[i++] = 0xe7;

	// cld
	temp[i++] = 0xfc;

	// rep movsd
	temp[i++] = 0xf3;
	temp[i++] = 0xa5;

	// 获取ExecPageHeaderX86*
	// mov eax, [ebp + 8]
	temp[i++] = 0x8b;
	temp[i++] = 0x45;
	temp[i++] = 0x08;

	// mov ecx, [ExecPageHeaderX86.context_addr]
	temp[i++] = 0x8b;
	temp[i++] = 0x48;
	temp[i++] = offsetof(ExecPageHeaderX86, context_addr);

	// mov eax, [ExecPageHeaderX86.call_addr]
	temp[i++] = 0x36;
	temp[i++] = 0x8b;
	temp[i++] = 0x40;
	temp[i++] = offsetof(ExecPageHeaderX86, call_addr);

	// 调用地址保存到局部变量1
	// mov [ebp - 4], eax
	temp[i++] = 0x89;
	temp[i++] = 0x45;
	temp[i++] = -0x04;

	// mov eax, [context.eax]
	temp[i++] = 0x8b;
	temp[i++] = 0x41;
	temp[i++] = offsetof(CallContextX86, eax);

	// mov edx, [context.edx]
	temp[i++] = 0x8b;
	temp[i++] = 0x51;
	temp[i++] = offsetof(CallContextX86, edx);

	// mov ebx, [context.ebx]
	temp[i++] = 0x8b;
	temp[i++] = 0x59;
	temp[i++] = offsetof(CallContextX86, ebx);

	// mov esi, [context.esi]
	temp[i++] = 0x8b;
	temp[i++] = 0x71;
	temp[i++] = offsetof(CallContextX86, esi);

	// mov edi, [context.edi]
	temp[i++] = 0x8b;
	temp[i++] = 0x79;
	temp[i++] = offsetof(CallContextX86, edi);

	// mov ecx, [context.ecx]
	temp[i++] = 0x8b;
	temp[i++] = 0x49;
	temp[i++] = offsetof(CallContextX86, ecx);

	// call [ebp - 4]
	temp[i++] = 0xff;
	temp[i++] = 0x55;
	temp[i++] = -0x04;

	// 两个寄存器先保存到局部变量
	// mov [ebp - 8], ecx
	temp[i++] = 0x89;
	temp[i++] = 0x4d;
	temp[i++] = -0x08;

	// mov [ebp - 0xc], eax
	temp[i++] = 0x89;
	temp[i++] = 0x45;
	temp[i++] = -0x0c;

	// mov eax, [ebp + 8]
	temp[i++] = 0x8b;
	temp[i++] = 0x45;
	temp[i++] = 0x08;

	// mov ecx, [ExecPageHeaderX86.context_addr]
	temp[i++] = 0x8b;
	temp[i++] = 0x48;
	temp[i++] = offsetof(ExecPageHeaderX86, context_addr);

	// 暂时不做栈拷贝
	// add esp, [context.balanced_esp]
	temp[i++] = 0x03;
	temp[i++] = 0x61;
	temp[i++] = offsetof(CallContextX86, balanced_esp);

	// mov eax, [ebp - 0xc]
	temp[i++] = 0x8b;
	temp[i++] = 0x45;
	temp[i++] = -0x0c;

	// mov [context.eax], eax
	temp[i++] = 0x89;
	temp[i++] = 0x41;
	temp[i++] = offsetof(CallContextX86, eax);

	// mov [context.edx], edx
	temp[i++] = 0x89;
	temp[i++] = 0x51;
	temp[i++] = offsetof(CallContextX86, edx);

	// mov [context.ebx], ebx
	temp[i++] = 0x89;
	temp[i++] = 0x59;
	temp[i++] = offsetof(CallContextX86, ebx);

	// mov [context.esi], esi
	temp[i++] = 0x89;
	temp[i++] = 0x71;
	temp[i++] = offsetof(CallContextX86, esi);

	// mov [context.edi], edi
	temp[i++] = 0x89;
	temp[i++] = 0x79;
	temp[i++] = offsetof(CallContextX86, edi);

	// mov eax(context), ecx
	temp[i++] = 0x89;
	temp[i++] = 0xC8;

	// mov ecx, [ebp - 8]
	temp[i++] = 0x8b;
	temp[i++] = 0x4d;
	temp[i++] = -0x08;

	// mov [context.ecx], ecx
	temp[i++] = 0x89;
	temp[i++] = 0x48;
	temp[i++] = offsetof(CallContextX86, ecx);

	// pop edi
	temp[i++] = 0x5f;
	// pop esi
	temp[i++] = 0x5e;
	// pop ebx
	temp[i++] = 0x5b;

	// mov esp, ebp
	temp[i++] = 0x89;
	temp[i++] = 0xec;

	// pop ebp
	temp[i++] = 0x5d;

	if (sync && IsCurrent()) {
		temp[i++] = 0xc3;
	}
	else {
		// 创建线程需要平栈
		temp[i++] = 0xc2;        // ret 4
		*(uint16_t*)&temp[i] = 4;
		i += 2;
	}

	if (!IsCurrent()) {
		if (!WriteMemory(exec_page, temp, 0x1000)) {
			return false;
		}
	}
	return true;
}

bool Process::Call(uint64_t exec_page, uint64_t call_addr, CallContextX86* context, bool sync, bool init_exec_page) const
{
	constexpr int32_t header_offset = 0x0;
	constexpr int32_t context_offset = 0x40;
	constexpr int32_t stack_offset = 0x80; // 0x80 = 128 / 4 = 32个参数

	constexpr int32_t exec_offset = 0x100;

	if (init_exec_page) {
		if (!CallGenerateCodeX86(exec_page, sync)) {
			return false;
		}
	}

	bool success = false;
	do {
		if (sync && IsCurrent()) {
			ExecPageHeaderX86 header;
			header.call_addr = static_cast<uint32_t>(call_addr);
			header.context_addr = reinterpret_cast<uint32_t>(context);
			header.stack_count = static_cast<uint32_t>(context->stack.size());
			header.stack_addr = reinterpret_cast<uint32_t>(context->stack.begin());
			using Func = void(*)(ExecPageHeaderX86*);
			Func func = reinterpret_cast<Func>(exec_page + exec_offset);
			func(&header);
		}
		else {
			if (!WriteMemory(exec_page + context_offset, context, offsetof(CallContextX86, stack))) {
				return false;
			}
			if (!WriteMemory(exec_page + stack_offset, context->stack.begin(), context->stack.size() * sizeof(uint32_t))) {
				return false;
			}

			ExecPageHeaderX86 header;
			header.call_addr = static_cast<uint32_t>(call_addr);
			header.context_addr = static_cast<uint32_t>(exec_page + context_offset);
			header.stack_count = static_cast<uint32_t>(context->stack.size());
			header.stack_addr = static_cast<uint32_t>(exec_page + stack_offset);
			if (!WriteMemory(exec_page + header_offset, &header, sizeof(header))) {
				return false;
			}

			auto thread = CreateThread(exec_page + exec_offset, exec_page + header_offset);
			if (!thread) {
				break;
			}

			if (sync) {
				if (!thread.value().WaitExit()) {
					break;
				}
				ReadMemory(exec_page + context_offset, context, offsetof(CallContextX86, stack));
			}
		}
		success = true;
	} while (false);
	return success;
}

bool Process::Call(uint64_t call_addr, CallContextX86* context, bool sync) const
{
	uint64_t exec_page = 0;
	bool init_exec_page = true;
	bool success = false;
	if (sync && IsCurrent()) {
		static CallPageX86 call_page(nullptr, true);
		exec_page = call_page.exec_page();
		if (!exec_page) {
			return false;
		}
		success = Call(exec_page, call_addr, context, sync, false);
	}
	else {
		auto res = AllocMemory(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (res) {
			exec_page = *res;
			CallGenerateCodeX86(exec_page, sync);
		}
		if (!exec_page) {
			return false;
		}
		success = Call(exec_page, call_addr, context, sync, false);
		if (sync) {
			FreeMemory(exec_page);
		}
	}
	return success;
}

bool Process::CallGenerateCodeX64(uint64_t exec_page, bool sync) const
{
	constexpr int32_t exec_offset = 0x800;

	std::array<uint8_t, 0x1000> temp_data = { 0 };
	uint8_t* temp = temp_data.data();
	if (IsCurrent()) {
		temp = reinterpret_cast<uint8_t*>(exec_page);
	}

	int32_t i = exec_offset;

	// 保存参数
	// mov [rsp+8], rcx // ExecPageHeaderX64*
	//temp[i++] = 0x48;
	//temp[i++] = 0x89;
	//temp[i++] = 0x4c;
	//temp[i++] = 0x24;
	//temp[i++] = 0x08;

	// 保存非易变寄存器
	// push rbx
	temp[i++] = 0x53;

	// push rbp
	temp[i++] = 0x55;

	// push rsi
	temp[i++] = 0x56;

	// push rdi
	temp[i++] = 0x57;

	// push r12
	temp[i++] = 0x41;
	temp[i++] = 0x54;

	// push r13
	temp[i++] = 0x41;
	temp[i++] = 0x55;

	// push r14
	temp[i++] = 0x41;
	temp[i++] = 0x56;

	// push r15
	temp[i++] = 0x41;
	temp[i++] = 0x57;

	// 预分配栈，直接分一块足够大的空间，0x400给参数，0x20给局部变量，0x8是对齐
	// sub rsp, 0x428
	temp[i++] = 0x48;
	temp[i++] = 0x81;
	temp[i++] = 0xec;
	*(uint32_t*)&temp[i] = 0x428;
	i += 4;

	// mov rax, [ExecPageHeaderX64.call_addr]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x41;
	temp[i++] = offsetof(ExecPageHeaderX64, call_addr);
        
	// 调用地址放到第一个局部变量
	// mov [rsp+0x400], rax
	temp[i++] = 0x48;
	temp[i++] = 0x89;
	temp[i++] = 0x84;
	temp[i++] = 0x24;
	*(uint32_t*)&temp[i] = 0x400;
	i += 4;

	// mov rax, [ExecPageHeaderX64.context_addr]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x41;
	temp[i++] = offsetof(ExecPageHeaderX64, context_addr);
	// context放到第二个局部变量
        
	// mov [rsp+0x400+0x8], rax
	temp[i++] = 0x48;
	temp[i++] = 0x89;
	temp[i++] = 0x84;
	temp[i++] = 0x24;
	*(uint32_t*)&temp[i] = 0x400 + 0x8;
	i += 4;

	// copy stack
	// mov rsi, [ExecPageHeaderX64.stack_addr]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x71;
	temp[i++] = offsetof(ExecPageHeaderX64, stack_addr);

	// mov rcx, [ExecPageHeaderX64.stack_count]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x49;
	temp[i++] = offsetof(ExecPageHeaderX64, stack_count);

	// 从rsp+0x20开始复制
	// mov rdi, rsp
	temp[i++] = 0x48;
	temp[i++] = 0x89;
	temp[i++] = 0xe7;

	// add rdi, 0x20
	temp[i++] = 0x48;
	temp[i++] = 0x83;
	temp[i++] = 0xc7;
	temp[i++] = 0x20;

	// cld
	temp[i++] = 0xfc;

	// rep movsq
	temp[i++] = 0xf3;
	temp[i++] = 0x48;
	temp[i++] = 0xa5;


	// 拿到context_addr
	// mov rcx, [rsp + 0x400 + 0x8]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x8c;
	temp[i++] = 0x24;
	*(uint32_t*)&temp[i] = 0x400 + 0x8;
	i += 4;

	// mov rax, [context.rax]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x41;
	temp[i++] = offsetof(CallContextX64, rax);

	// mov rdx, [context.rdx]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x51;
	temp[i++] = offsetof(CallContextX64, rdx);

	// mov rbx, [context.rbx]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x59;
	temp[i++] = offsetof(CallContextX64, rbx);

	// mov rbp, [context.rbp]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x69;
	temp[i++] = offsetof(CallContextX64, rbp);

	// mov rsi, [context.rsi]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x71;
	temp[i++] = offsetof(CallContextX64, rsi);

	// mov rdi, [context.rdi]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x79;
	temp[i++] = offsetof(CallContextX64, rdi);

	// mov r8, [context.r8]
	temp[i++] = 0x4c;
	temp[i++] = 0x8b;
	temp[i++] = 0x41;
	temp[i++] = offsetof(CallContextX64, r8);

	// mov r9, [context.r9]
	temp[i++] = 0x4c;
	temp[i++] = 0x8b;
	temp[i++] = 0x49;
	temp[i++] = offsetof(CallContextX64, r9);

	// mov r10, [context.r10]
	temp[i++] = 0x4c;
	temp[i++] = 0x8b;
	temp[i++] = 0x51;
	temp[i++] = offsetof(CallContextX64, r10);

	// mov r11, [context.r11]
	temp[i++] = 0x4c;
	temp[i++] = 0x8b;
	temp[i++] = 0x59;
	temp[i++] = offsetof(CallContextX64, r11);

	// mov r12, [context.r12]
	temp[i++] = 0x4c;
	temp[i++] = 0x8b;
	temp[i++] = 0x61;
	temp[i++] = offsetof(CallContextX64, r12);

	// mov r13, [context.r13]
	temp[i++] = 0x4c;
	temp[i++] = 0x8b;
	temp[i++] = 0x69;
	temp[i++] = offsetof(CallContextX64, r13);

	// mov r14, [context.r14]
	temp[i++] = 0x4c;
	temp[i++] = 0x8b;
	temp[i++] = 0x71;
	temp[i++] = offsetof(CallContextX64, r14);

	// mov r15, [context.r15]
	temp[i++] = 0x4c;
	temp[i++] = 0x8b;
	temp[i++] = 0x79;
	temp[i++] = offsetof(CallContextX64, r15);

	// mov rcx, [context.rcx]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x49;
	temp[i++] = offsetof(CallContextX64, rcx);

	// call [rsp + 0x400]
	temp[i++] = 0xff;
	temp[i++] = 0x94;
	temp[i++] = 0x24;
	*(uint32_t*)&temp[i] = 0x400;
	i += 4;
        
	// 局部变量保存下rcx
	// mov [rsp + 0x400 + 0x10], rcx
	temp[i++] = 0x48;
	temp[i++] = 0x89;
	temp[i++] = 0x8c;
	temp[i++] = 0x24;
	*(uint32_t*)&temp[i] = 0x400 + 0x10;
	i += 4;

	// 拿到context_addr
	// mov rcx, [rsp + 0x400 + 0x8]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x8c;
	temp[i++] = 0x24;
	*(uint32_t*)&temp[i] = 0x400 + 0x8;
	i += 4;


	// mov [context.rax], rax
	temp[i++] = 0x48;
	temp[i++] = 0x89;
	temp[i++] = 0x41;
	temp[i++] = offsetof(CallContextX64, rax);

	// mov [context.rdx], rdx
	temp[i++] = 0x48;
	temp[i++] = 0x89;
	temp[i++] = 0x51;
	temp[i++] = offsetof(CallContextX64, rdx);

	// mov [context.rbx], rbx
	temp[i++] = 0x48;
	temp[i++] = 0x89;
	temp[i++] = 0x59;
	temp[i++] = offsetof(CallContextX64, rbx);

	// mov [context.rbp], rbp
	temp[i++] = 0x48;
	temp[i++] = 0x89;
	temp[i++] = 0x69;
	temp[i++] = offsetof(CallContextX64, rbp);

	// mov [context.rsi], rsi
	temp[i++] = 0x48;
	temp[i++] = 0x89;
	temp[i++] = 0x71;
	temp[i++] = offsetof(CallContextX64, rsi);

	// mov [context.rdi], rdi
	temp[i++] = 0x48;
	temp[i++] = 0x89;
	temp[i++] = 0x79;
	temp[i++] = offsetof(CallContextX64, rdi);

	// mov [context.r8], r8
	temp[i++] = 0x4c;
	temp[i++] = 0x89;
	temp[i++] = 0x41;
	temp[i++] = offsetof(CallContextX64, r8);

	// mov [context.r9], r9
	temp[i++] = 0x4c;
	temp[i++] = 0x89;
	temp[i++] = 0x49;
	temp[i++] = offsetof(CallContextX64, r9);

	// mov [context.r10], r10
	temp[i++] = 0x4c;
	temp[i++] = 0x89;
	temp[i++] = 0x51;
	temp[i++] = offsetof(CallContextX64, r10);

	// mov [context.r11], r11
	temp[i++] = 0x4c;
	temp[i++] = 0x89;
	temp[i++] = 0x59;
	temp[i++] = offsetof(CallContextX64, r11);

	// mov [context.r12], r12
	temp[i++] = 0x4c;
	temp[i++] = 0x89;
	temp[i++] = 0x61;
	temp[i++] = offsetof(CallContextX64, r12);

	// mov [context.r13], r13
	temp[i++] = 0x4c;
	temp[i++] = 0x89;
	temp[i++] = 0x69;
	temp[i++] = offsetof(CallContextX64, r13);

	// mov [context.r14], r14
	temp[i++] = 0x4c;
	temp[i++] = 0x89;
	temp[i++] = 0x71;
	temp[i++] = offsetof(CallContextX64, r14);

	// mov [context.r15], r15
	temp[i++] = 0x4c;
	temp[i++] = 0x89;
	temp[i++] = 0x79;
	temp[i++] = offsetof(CallContextX64, r15);


	// mov rax(context), rcx
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0xC1;

	// mov rcx, [rsp + 0x400 + 0x10]
	temp[i++] = 0x48;
	temp[i++] = 0x8b;
	temp[i++] = 0x8c;
	temp[i++] = 0x24;
	*(uint32_t*)&temp[i] = 0x400 + 0x10;
	i += 4;

	// mov [context.rcx], rcx
	temp[i++] = 0x48;
	temp[i++] = 0x89;
	temp[i++] = 0x48;
	temp[i++] = offsetof(CallContextX64, rcx);


	// add rsp, 0x428
	temp[i++] = 0x48;
	temp[i++] = 0x81;
	temp[i++] = 0xc4;
	*(uint32_t*)&temp[i] = 0x428;
	i += 4;

	// pop r15
	temp[i++] = 0x41;
	temp[i++] = 0x5f;

	// pop r14
	temp[i++] = 0x41;
	temp[i++] = 0x5e;

	// pop r13
	temp[i++] = 0x41;
	temp[i++] = 0x5d;
        
	// pop r12
	temp[i++] = 0x41;
	temp[i++] = 0x5c;

	// pop rdi
	temp[i++] = 0x5f;

	// pop rsi
	temp[i++] = 0x5e;

	// pop rbp
	temp[i++] = 0x5d;

	// pop rbx
	temp[i++] = 0x5b;

	// ret
	temp[i++] = 0xc3;

	if (!IsCurrent()) {
		if (!WriteMemory(exec_page, temp, 0x1000)) {
			return false;
		}
	}
	return true;
}

bool Process::Call(uint64_t exec_page, uint64_t call_addr, CallContextX64* context, bool sync, bool init_exec_page) const
{
	constexpr int32_t header_offset = 0x0;
	constexpr int32_t context_offset = 0x100;
	constexpr int32_t stack_offset = 0x400; // 0x400 = 128 / 8 = 128个参数

	constexpr int32_t exec_offset = 0x800;

	if (init_exec_page) {
		if (!CallGenerateCodeX64(exec_page, sync)) {
			return false;
		}
	}

	bool success = false;
	do {
		if (sync && IsCurrent()) {
			ExecPageHeaderX64 header;
			header.call_addr = call_addr;
			header.context_addr = reinterpret_cast<uint64_t>(context);
			header.stack_count = context->stack.size();
			header.stack_addr = reinterpret_cast<uint64_t>(context->stack.begin());
			using Func = void(*)(ExecPageHeaderX64*);
			Func func = reinterpret_cast<Func>(exec_page + exec_offset);
			func(&header);
		}
		else {
			if (!WriteMemory(exec_page + context_offset, context, offsetof(CallContextX86, stack))) {
				return false;
			}
			if (!WriteMemory(exec_page + stack_offset, context->stack.begin(), context->stack.size() * sizeof(uint32_t))) {
				return false;
			}

			ExecPageHeaderX64 header;
			header.call_addr = call_addr;
			header.context_addr = exec_page + context_offset;
			header.stack_count = context->stack.size();
			header.stack_addr = exec_page + stack_offset;
			if (!WriteMemory(exec_page + header_offset, &header, sizeof(header))) {
				return false;
			}

			auto thread = CreateThread(exec_page + exec_offset, exec_page + header_offset);
			if (!thread) {
				break;
			}

			if (sync) {
				if (!thread.value().WaitExit()) {
					break;
				}
				ReadMemory(exec_page + context_offset, context, offsetof(CallContextX64, stack));
			}
		}
		success = true;
	} while (false);
	return success;
}

bool Process::Call(uint64_t call_addr, CallContextX64* context, bool sync) const
{
	uint64_t exec_page = 0;
	bool init_exec_page = true;
	bool success = false;
	if (sync && IsCurrent()) {
		static CallPageX64 call_page(nullptr, true);
		exec_page = call_page.exec_page();
		if (!exec_page) {
			return false;
		}
		success = Call(exec_page, call_addr, context, sync, false);
	}
	else {
		auto res = AllocMemory(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (res) {
			exec_page = *res;
			CallGenerateCodeX64(exec_page, sync);
		}
		if (!exec_page) {
			return false;
		}
		success = Call(exec_page, call_addr, context, sync, false);
		if (sync) {
			FreeMemory(exec_page);
		}
	}
	return success;
}

const ModuleList& Process::Modules() const
{
	if (!modules_)
	{
		modules_ = { const_cast<Process*>(this) };
	}
	return *modules_;
}

// bool Process::RepairImportAddressTable(Image* image, bool skip_not_loaded)
// {
// 	auto import_descriptor = (_IMAGE_IMPORT_DESCRIPTOR*)image->RvaToPoint(image->GetDataDirectory()[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
// 	if (import_descriptor == nullptr) {
// 		return false;
// 	}
// 	for (; import_descriptor->FirstThunk; import_descriptor++) {
// 		if(import_descriptor->OriginalFirstThunk == NULL) import_descriptor->OriginalFirstThunk = import_descriptor->FirstThunk;
// 		char* import_module_name = (char*)image->RvaToPoint(import_descriptor->Name);
// 		auto import_module_base_res = LoadLibrary(geek::Convert::AnsiToUtf16le(import_module_name).c_str());
// 		if (!import_module_base_res) return false;
// 		if (image->IsPE32()) {
// 			if (!RepairImportAddressTableFromModule<IMAGE_THUNK_DATA32>(*this, image, import_descriptor, import_module_base_res.value(), skip_not_loaded)) {
// 				return false;
// 			}
// 		}
// 		else {
// 			if (!RepairImportAddressTableFromModule<IMAGE_THUNK_DATA64>(*this, image, import_descriptor, import_module_base_res.value(), skip_not_loaded)) {
// 				return false;
// 			}
// 		}
// 	}
// 	return true;
// }
//
// bool Process::ExecuteTls(Image* image, uint64_t image_base)
// {
// 	auto tls_dir = (IMAGE_TLS_DIRECTORY*)image->RvaToPoint(image->GetDataDirectory()[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
// 	if (tls_dir == nullptr) {
// 		return false;
// 	}
// 	PIMAGE_TLS_CALLBACK* callback = (PIMAGE_TLS_CALLBACK*)tls_dir->AddressOfCallBacks;
// 	if (callback) {
// 		while (true) {
// 			if (IsCur()) {
// 				if (!*callback) {
// 					break;
// 				}
// 				if (image->IsPE32()) {
// 					PIMAGE_TLS_CALLBACK32 callback32 = *(PIMAGE_TLS_CALLBACK32*)callback;
// 					callback32((uint32_t)image_base, DLL_PROCESS_ATTACH, NULL);
// 				}
// 				else {
// 					PIMAGE_TLS_CALLBACK64 callback64 = *(PIMAGE_TLS_CALLBACK64*)callback;
// 					callback64(image_base, DLL_PROCESS_ATTACH, NULL);
// 				}
// 			}
// 			else {
// 				if (image->IsPE32()) {
// 					PIMAGE_TLS_CALLBACK32 callback32;
// 					if (!ReadMemory((uint64_t)callback, &callback32, sizeof(PIMAGE_TLS_CALLBACK32))) {
// 						return false;
// 					}
// 					Call(image_base, (uint64_t)callback32, { image_base, DLL_PROCESS_ATTACH , NULL });
// 				}
// 				else {
// 					PIMAGE_TLS_CALLBACK64 callback64;
// 					if (!ReadMemory((uint64_t)callback, &callback64, sizeof(PIMAGE_TLS_CALLBACK64))) {
// 						return false;
// 					}
// 					Call(image_base, (uint64_t)callback64, { image_base, DLL_PROCESS_ATTACH , NULL });
// 				}
// 			}
// 			callback++;
// 		}
// 	}
// 	return true;
// }

bool Process::CallEntryPoint(Image* image, uint64_t image_base, uint64_t init_parameter, bool sync)
{
	if (IsCurrent()) {
		uint32_t rva = image->NtHeader().OptionalHeader().AddressOfEntryPoint();
		if (image->IsDll()) {
			if (image->IsPE32()) {
				DllEntryProc32 DllEntry = (DllEntryProc32)(image_base + rva);
				DllEntry((uint32_t)image_base, DLL_PROCESS_ATTACH, (uint32_t)init_parameter);
			}
			else {
				DllEntryProc64 DllEntry = (DllEntryProc64)(image_base + rva);
				DllEntry(image_base, DLL_PROCESS_ATTACH, init_parameter);
			}
		}
		else {
			ExeEntryProc ExeEntry = (ExeEntryProc)(LPVOID)(image_base + rva);
			// exe不执行
		}
	}
	else {
		uint64_t entry_point = (uint64_t)image_base + image->NtHeader().OptionalHeader().AddressOfEntryPoint();
		if (!Call(image_base, entry_point, { image_base, DLL_PROCESS_ATTACH , init_parameter }, nullptr, CallConvention::kStdCall, sync)) {
			return false;
		}
	}
	return true;
}

std::optional<PROCESS_BASIC_INFORMATION32> Process::Pbi32() const
{
	GEEK_ASSERT(IsX32());
	PROCESS_BASIC_INFORMATION32 pbi32{};
	if (auto e = NtQueryInformationProcess(Handle(), ProcessBasicInformation, &pbi32, sizeof(pbi32), NULL);
		!NT_SUCCESS(e))
	{
		GEEK_UPDATE_NT_ERROR(e);
		return std::nullopt;
	}
	return pbi32;
}

std::optional<PROCESS_BASIC_INFORMATION64> Process::Pbi64() const
{
	PROCESS_BASIC_INFORMATION64 pbi64{};
	if (ms_wow64.Wow64Operation(Handle())) {
		if (auto e = NtWow64QueryInformationProcess64(Handle(), ProcessBasicInformation, &pbi64, sizeof(pbi64), NULL);
			!NT_SUCCESS(e))
		{
			GEEK_UPDATE_NT_ERROR(e);
			return std::nullopt;
		}
	}
	else {
		if (auto e = NtQueryInformationProcess(Handle(), ProcessBasicInformation, &pbi64, sizeof(pbi64), NULL);
			!NT_SUCCESS(e))
		{
			GEEK_UPDATE_NT_ERROR(e);
			return std::nullopt;
		}
	}
	return pbi64;
}

std::optional<PEB32> Process::Peb32() const
{
	GEEK_ASSERT(IsX32());

	auto pbi = Pbi32();
	if (!pbi)
		return std::nullopt;
	return ReadMemoryToValue<PEB32>(pbi->PebBaseAddress);
}

std::optional<PEB64> Process::Peb64() const
{
	GEEK_ASSERT(!IsX32());

	auto pbi = Pbi64();
	if (!pbi)
		return std::nullopt;
	return ReadMemoryToValue<PEB64>(pbi->PebBaseAddress);
}

ProcessList& Process::CachedProcessList()
{
	static ProcessList cache{};
	return cache;
}

std::optional<std::vector<uint8_t>> Process::GetResource(HMODULE hModule, DWORD ResourceID, LPCWSTR type)
{
	bool success = false;
	std::vector<uint8_t> res;
	HRSRC hResID = NULL;
	HRSRC hRes = NULL;
	HANDLE hResFile = INVALID_HANDLE_VALUE;
	do {
		HRSRC hResID = FindResourceW(hModule, MAKEINTRESOURCEW(ResourceID), type);
		if (!hResID) {
			break;
		}

		HGLOBAL hRes = LoadResource(hModule, hResID);
		if (!hRes) {
			break;
		}

		LPVOID pRes = LockResource(hRes);
		if (pRes == NULL) {
			break;
		}

		unsigned long dwResSize = SizeofResource(hModule, hResID);
		res.resize(dwResSize);
		memcpy(&res[0], pRes, dwResSize);
		success = true;
	} while (false);

	if (hResFile != INVALID_HANDLE_VALUE) {
            
		hResFile = INVALID_HANDLE_VALUE;
	}
	if (hRes) {
		UnlockResource(hRes);
		FreeResource(hRes);
		hRes = NULL;
	}
	if (!success) {
		return {};
	}
	return res;
}

bool Process::SaveFileFromResource(HMODULE hModule, DWORD ResourceID, LPCWSTR type, LPCWSTR saveFilePath)
{
	auto resource = GetResource(hModule, ResourceID, type);
	if (!resource) {
		return false;
	}
	return geek::File::WriteFile(saveFilePath, resource->data(), resource->size());
}

MemoryProtectScope::MemoryProtectScope(MemoryProtectScope&& right) noexcept {
    owning_process_ = right.owning_process_;
    address_ = right.address_;
    length_ = right.length_;
    current_protect_ = right.current_protect_;
    old_protect_ = right.old_protect_;

    right.owning_process_ = nullptr;
    right.address_ = 0;
    right.length_ = 0;
    right.current_protect_ = 0;
    right.old_protect_ = 0;
}

MemoryProtectScope::MemoryProtectScope(const Process* owning_process, uint64_t address, size_t length, DWORD protect)
    : owning_process_(owning_process),
    address_(address),
    length_(length),
    current_protect_(protect),
    old_protect_(0)
{
}

MemoryProtectScope::~MemoryProtectScope() {
    if (old_protect_) {
        DWORD tmp;
        owning_process_->SetMemoryProtect(address_, length_, old_protect_, &tmp);
    }
}

bool MemoryProtectScope::ChangeProtect(DWORD protect, DWORD* old_protect) {
    DWORD tmp;
    bool suc = owning_process_->SetMemoryProtect(address_, length_, protect, &tmp);
    if (!suc)
        return false;

    if (old_protect) {
        *old_protect = tmp;
    }
    current_protect_ = protect;

    return true;
}

Process::Process(UniqueHandle process_handle) noexcept:
	process_handle_ { std::move(process_handle) }
{
}

DWORD Process::GetProcessIdFromThread(Thread* thread)
{
	return ::GetProcessIdOfThread(thread->handle());
}

bool Process::Terminate(std::wstring_view processName)
{
	auto process = Open(processName);
	if (!process) return false;
	return process.value().Terminate(0);
}

Process& ThisProc()
{
	static Process proc{ reinterpret_cast<HANDLE>(-1) };
	return proc;
}
}
