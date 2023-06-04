#include "includes.hpp"

#pragma comment(lib, "vmm.lib")

namespace memory
{
	auto get_process_id(const char* process_name) -> DWORD;
	auto get_module_handle(const char* module_name) -> uint64_t;
}