#include "memory.hpp"
#include "globals.hpp"

#pragma comment(lib, "vmm.lib")

auto memory::get_process_id(const char* process_name) -> DWORD
{
	DWORD buffer;
	bool ret = VMMDLL_PidGetFromName(globals::vmm_handle, (LPSTR)process_name, &buffer);

	if (ret)
	{
		return buffer;
	}		

	return -1;
}

auto memory::get_module_handle(const char* module_name) -> uint64_t
{
	uint64_t address = VMMDLL_ProcessGetModuleBaseU(globals::vmm_handle, globals::game_pid, (LPSTR)module_name);
	return address;
}