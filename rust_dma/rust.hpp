#include "includes.hpp"

namespace rust
{
	class local_player
	{
	public:
		auto get_entity() -> uint64_t
		{
			auto static_fields = memory::read_ptr(reinterpret_cast<uint64_t>(this) + 0xB8);
			return memory::read_ptr(static_fields + 0x0);
		}
	};

	class base_player
	{
	public:
		auto get_player_flag() -> int
		{
			return memory::read_int(reinterpret_cast<uint64_t>(this) + 0x740);
		}

		auto set_player_flag(int flag) -> void
		{
			memory::write_int(reinterpret_cast<uint64_t>(this) + 0x740, flag);
		}
	};
}