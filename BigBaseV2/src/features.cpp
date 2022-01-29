#include "common.hpp"
#include "features.hpp"
#include "logger.hpp"
#include "natives.hpp"
#include "script.hpp"

namespace big
{
	void features::run_tick()
	{
		/*if (NETWORK::NETWORK_GET_NUM_CONNECTED_PLAYERS() != playerlist.size())
		{
			for (Player i = 0; i < gta::num_players; i++)
			{
				if (CNetGamePlayer* net_player = g_pointers->m_get_net_player(i))
				{
					auto cstr_name = net_player->get_name();
					std::string name = cstr_name;
					transform(name.begin(), name.end(), name.begin(), ::tolower);
					playerlist[name] = { cstr_name, i };

				}
			}
		}*/
	}

	void features::script_func()
	{
		while (true)
		{
			TRY_CLAUSE
			{

			}
			EXCEPT_CLAUSE
			script::get_current()->yield();
		}
	}

}
