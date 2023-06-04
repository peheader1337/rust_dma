#include "threads.hpp"
#include "globals.hpp"
#include "memory.hpp"

template<typename T>
auto threads::read(uint64_t address) -> T
{
	T buffer;
	VMMDLL_MemRead(globals::vmm_handle, globals::game_pid, address, PBYTE(&buffer), sizeof(T));
	return buffer;
}

template<typename T>
auto threads::write(uint64_t address, T value) -> void
{
	VMMDLL_MemWrite(globals::vmm_handle, globals::game_pid, address, PBYTE(&value), sizeof(T));
}

namespace rust
{
	namespace offsets
	{
		auto local_player_c = 0x2FD1080; // LocalPlayer_c*

		namespace base_player
		{
			auto player_flags = 0x740; // BasePlayer - BasePlayer.PlayerFlags playerFlags
			auto base_movement = 0x550; // BasePlayer - BaseMovement movement
			auto base_collision = 0x558; // BasePlayer - BaseCollision collision
			auto player_input = 0x548; // BasePlayer - PlayerInput input
			auto visible_player_list = 0x8; // BasePlayer - ListDictionary<ulong, BasePlayer> visiblePlayerList
			auto player_eyes = 0x748; // BasePlayer - PlayerEyes eyes
			auto player_inventory = 0x750; // BasePlayer - PlayerInventory inventory
			auto active_item_id = 0x668; // BasePlayer - ItemId clActiveItem
		}

		namespace player_inventory
		{
			auto container_belt = 0x28; // PlayerInventory - ItemContainer containerBelt
		}

		namespace item_container
		{
			auto available_slots = 0x38; // ItemContainer - List<int> availableSlots
		}

		namespace item
		{
			auto uid = 0x20;
			auto held_entity = 0xA8; // Item - EntityRef heldEntity
		}

		namespace base_projectile
		{
			auto recoil = 0x2E0; // BaseProjectile - RecoilProperties recoil
		}

		namespace recoil_properties
		{
			auto override = 0x78; // RecoilProperties - RecoilProperties newRecoilOverride
			auto yaw_min = 0x18; // RecoilProperties - float recoilYawMin
			auto yaw_max = 0x1C; // RecoilProperties - float recoilYawMin
			auto pitch_min = 0x20; // RecoilProperties - float recoilYawMin
			auto pitch_max = 0x24; // RecoilProperties - float recoilYawMin
		}

		namespace walk_movement
		{
			auto ground_angle = 0xC4; // PlayerWalkMovement - float groundAngle
			auto ground_angle_new = 0xC8; // PlayerWalkMovement - float groundAngleNew
		}
	}

	class local_player
	{
	public:
		auto get_entity() -> uint64_t
		{
			auto static_fields = threads::read<uint64_t>(reinterpret_cast<uint64_t>(this) + 0xB8);
			return threads::read<uint64_t>(static_fields + 0x0);
		}
	};

	class player_inventory
	{
	public:
		auto get_belt_container() -> uint64_t
		{
			return threads::read<uint64_t>(reinterpret_cast<uint64_t>(this) + offsets::player_inventory::container_belt);
		}
	};

	class item_container
	{
	public:
		auto get_item_list() -> uint64_t
		{
			return threads::read<uint64_t>(reinterpret_cast<uint64_t>(this) + offsets::item_container::available_slots);
		}
	};

	class item
	{
	public:
		auto get_id() -> int
		{
			return threads::read<int>(reinterpret_cast<uint64_t>(this) + offsets::item::uid);
		}

		auto get_projectile() -> uint64_t
		{
			return threads::read<uint64_t>(reinterpret_cast<uint64_t>(this) + offsets::item::held_entity);
		}
	};

	class base_player
	{
	public:
		auto get_player_flags() -> int
		{
			return threads::read<int>(reinterpret_cast<uint64_t>(this) + offsets::base_player::player_flags);
		}

		auto set_player_flags(int flag) -> void
		{
			threads::write<int>(reinterpret_cast<uint64_t>(this) + offsets::base_player::player_flags, flag);
		}

		auto get_movement() -> uint64_t
		{
			return threads::read<int>(reinterpret_cast<uint64_t>(this) + offsets::base_player::base_movement);
		}

		auto get_input() -> uint64_t
		{
			return threads::read<int>(reinterpret_cast<uint64_t>(this) + offsets::base_player::player_input);
		}

		auto get_eyes() -> uint64_t
		{
			return threads::read<int>(reinterpret_cast<uint64_t>(this) + offsets::base_player::player_eyes);
		}

		auto get_inventory() -> uint64_t
		{
			return threads::read<int>(reinterpret_cast<uint64_t>(this) + offsets::base_player::player_inventory);
		}

		auto get_player_list() -> uint64_t
		{
			auto static_fields = threads::read<uint64_t>(reinterpret_cast<uint64_t>(this) + 0xB8);
			return threads::read<uint64_t>(static_fields + offsets::base_player::visible_player_list);
		}

		auto get_weapon_by_slot(int slot) -> uint64_t
		{
			auto inventory = (player_inventory*)get_inventory();
			auto belt = (item_container*)inventory->get_belt_container();
			auto item_list = belt->get_item_list();
			auto buffer = threads::read<uint64_t>(item_list + 0x20 + (slot * 0x8));
			return buffer;
		}

		auto get_active_weapon() -> uint64_t
		{
			auto active_id = threads::read<int>(reinterpret_cast<uint64_t>(this) + offsets::base_player::active_item_id);
			if (!active_id) return 0;

			for (auto slot = 0; slot < 6; slot++)
			{
				auto weapon = (item*)this->get_weapon_by_slot(slot);

				if (active_id == weapon->get_id())
					return (uint64_t)weapon;
			}
		}
	};

	class base_projectile
	{
	public:
		auto get_recoil_properties() -> uint64_t
		{
			auto recoil_properties = threads::read<int>(reinterpret_cast<uint64_t>(this) + offsets::item::held_entity);
			if (!recoil_properties) return 0;

			auto override = threads::read<uint64_t>(recoil_properties + offsets::recoil_properties::override);
			if (override)
				return override;

			return recoil_properties;
		}
	};

	class recoil_properties
	{
	public:
		auto set_recoil_yaw(float min, float max) -> void
		{
			threads::write(reinterpret_cast<uint64_t>(this) + offsets::recoil_properties::yaw_min, min);
			threads::write(reinterpret_cast<uint64_t>(this) + offsets::recoil_properties::yaw_max, max);
		}

		auto set_recoil_pitch(float min, float max) -> void
		{
			threads::write(reinterpret_cast<uint64_t>(this) + offsets::recoil_properties::pitch_min, min);
			threads::write(reinterpret_cast<uint64_t>(this) + offsets::recoil_properties::pitch_max, max);
		}

		auto remove_recoil() -> void
		{
			set_recoil_yaw(0.0f, 0.0f);
			set_recoil_pitch(0.0f, 0.0f);
		}
	};
	
	class walk_movement
	{
	public:
		auto get_ground_angle() -> float
		{
			return threads::read<float>(reinterpret_cast<uint64_t>(this) + offsets::walk_movement::ground_angle);
		}

		auto set_ground_angle(float angle) -> void
		{
			threads::write<float>(reinterpret_cast<uint64_t>(this) + offsets::walk_movement::ground_angle, angle);
			threads::write<float>(reinterpret_cast<uint64_t>(this) + offsets::walk_movement::ground_angle_new, angle);
		}

		auto remove_fall() -> void
		{
			this->set_ground_angle(0.0f);
		}
	};
}

auto threads::memory_loop() -> void
{
	printf("[ * ] game_assembly : %p\n", globals::game_assembly);
	printf("[ * ] unity_player : %p\n", globals::unity_player);

	auto local_player = (rust::local_player*)read<uint64_t>(globals::game_assembly + rust::offsets::local_player_c);
	
	while (1)
	{
		auto entity = (rust::base_player*)local_player->get_entity();

		if (entity != 0)
		{
			auto movement = (rust::walk_movement*)entity->get_movement();
			auto weapon = (rust::item*)entity->get_active_weapon();

			if (weapon != 0)
			{
				auto projectile = (rust::base_projectile*)weapon->get_projectile();
				auto recoil = (rust::recoil_properties*)projectile->get_recoil_properties();

				recoil->remove_recoil();
			}

			if (movement != 0)
			{
				movement->remove_fall();

				auto current_flag = entity->get_player_flags();
				entity->set_player_flags(current_flag | 4);

				Sleep(1);
			}
		}
	}
}