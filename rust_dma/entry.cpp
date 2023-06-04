#include "entry.hpp"
#include "memory.hpp"
#include "threads.hpp"
#include "globals.hpp"

auto main() -> void
{
	SetConsoleTitleA("geroin");

	printf("[ * ] loading...\n");

	vector<LPSTR> arguments = { (LPSTR)"", (LPSTR)"-device", (LPSTR)"fpga" };

	globals::vmm_handle = VMMDLL_Initialize(3, arguments.data());
	globals::game_pid = memory::get_process_id("RustClient.exe");

	printf("[ * ] game_pid : %i\n", globals::game_pid);

	globals::game_assembly = memory::get_module_handle("GameAssembly.dll");
	globals::unity_player = memory::get_module_handle("UnityPlayer.dll");

	printf("[ * ] creating threads...\n");

	CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(threads::memory_loop), 0, 0, 0);

	printf("[ * ] done\n");

	Sleep(-1);
}