#include "includes.hpp"

namespace threads
{
	template<typename T>
	auto read(uint64_t address) -> T;

	template<typename T>
	auto write(uint64_t address, T value) -> void;

	auto memory_loop() -> void;
}