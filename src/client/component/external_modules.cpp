#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include <utils/hook.hpp>
#include <game/game.hpp>

namespace external_modules
{
	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			LoadLibraryA("ifs_hook.dll");
		}
	};
}

REGISTER_COMPONENT(external_modules::component)
