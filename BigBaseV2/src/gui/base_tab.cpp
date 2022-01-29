#include "common.hpp"
#include "base_tab.h"
#include "imgui.h"
#include "script.hpp"
#include "fiber_pool.hpp"
#include "natives.hpp"
#include "gta_util.hpp"
#include "ImGuiBitfield.h"
static std::vector<CNetGamePlayer> playerlist;
void notification(const std::string& text)
{
	HUD::BEGIN_TEXT_COMMAND_THEFEED_POST("STRING");
	HUD::_THEFEED_SET_NEXT_POST_BACKGROUND_COLOR(184);
	HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(const_cast<char*>(text.c_str()));
	HUD::END_TEXT_COMMAND_THEFEED_POST_TICKER(false, false);
}
namespace big
{
	bool god_bool = false, invisible = false, policia_fake = false, policia_real = false;
	void base_tab::render_base_tab()
	{
		if (ImGui::BeginTabItem("Jugador"))
		{
			if (ImGui::Checkbox("Godmode ", &god_bool)) {
				if (god_bool) {
					ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::PLAYER_PED_ID(), true);
					notification("[INFO] Ahora eres inmortal");
				}
				else {
					ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::PLAYER_PED_ID(), false);
				}

			}
			/*if (ImGui::Checkbox("Bool", g_settings.options["test"].get<bool*>())) {
				notification("Works");
			}*/
			ImGui::SameLine();
			//if (ImGui::Button("Desactivar god mode")) {
			//	ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::PLAYER_PED_ID(), false);
			//}
			if (ImGui::Checkbox("Invisible", &invisible)) {
				if (invisible) {
					ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), false, 0);

				}
				else {
					ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), true, 0);
				}
			}
			if (ImGui::Checkbox("5 Estrellas fake ", &policia_fake)) {
				if (policia_fake) {
					MISC::SET_FAKE_WANTED_LEVEL(5);
				}
				else {
					MISC::SET_FAKE_WANTED_LEVEL(0);
				}
			}

			if (ImGui::Button("Policia real")) {

				PLAYER::SET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID(), 5, false);
				PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(PLAYER::PLAYER_ID(), false);
				notification("[INFO] 5 Estrellas puestas.");
			}
			ImGui::SameLine();
			if (ImGui::Button("Quitar policia")) {
				gta_util::get_local_playerinfo()->m_wanted_level = 0;
				notification("[INFO] Policia eliminada con exito");
			}


			//ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), -978.708496, -3001.840820, 13.317889, 0, 0, 0, 0);
			const char* items[] = { "Aeropuerto", "Los santos custom", "Eclipse Towers", "AmmoNation", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
			static int item_current = 0;
			ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));

			if (ImGui::Button("Teleport")) {
				switch (item_current) {
				case 0:
					ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), -978.708496, -3001.840820, 13.317889, 0, 0, 0, 0);
					break;
				case 1:
					ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), -1135.70727, -1987.154175, 12.976217, 0, 0, 0, 0);
					break;

				case 3:
					ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), -318.859039, 6074.433105, 30.614943, 0, 0, 0, 0);
				}


			}

			ImGui::Separator();

			if (ImGui::Button("Unload"))
			{
				g_running = false;
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Vehiculo")) {
			if (ImGui::Button("Spawn T20"))
			{
				notification("[INFO] T20 Spawneado con exito");
				QUEUE_JOB_BEGIN_CLAUSE()
				{
					constexpr auto hash = RAGE_JOAAT("T20");
					while (!STREAMING::HAS_MODEL_LOADED(hash))
					{
						STREAMING::REQUEST_MODEL(hash);
						script::get_current()->yield();
					}

					auto pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
					*(unsigned short*)g_pointers->m_model_spawn_bypass = 0x9090;
					Vehicle vehicle = VEHICLE::CREATE_VEHICLE(hash, pos.x, pos.y, pos.z, 0.f, TRUE, FALSE, FALSE);
					*(unsigned short*)g_pointers->m_model_spawn_bypass = 0x0574; //By writing the "old" bypass to the function, running CREATE_VEHICLE, then restoring it, the anti-cheat does not have enough time to catch the function in a dirty state.

					script::get_current()->yield();
					STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
					if (*g_pointers->m_is_session_started)
					{
						DECORATOR::DECOR_SET_INT(vehicle, "MPBitset", 0);
						ENTITY::_SET_ENTITY_SOMETHING(vehicle, TRUE); //True means it can be deleted by the engine when switching lobbies/missions/etc, false means the script is expected to clean it up.
						auto networkId = NETWORK::VEH_TO_NET(vehicle);
						if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(vehicle))
							NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(networkId, true);
						VEHICLE::SET_VEHICLE_IS_STOLEN(vehicle, FALSE);
					}
				}
				QUEUE_JOB_END_CLAUSE
			}

			if (ImGui::Button("Cambiar la matricula del coche")) {
				Ped playerPed = PLAYER::PLAYER_PED_ID();
				Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
				char* plateText = "BORJA";
				VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(veh, plateText);
				notification("Matricula cambiada");
			}

			if (ImGui::Button("Reparar vehiculo")) {
				Ped playerPed = PLAYER::PLAYER_PED_ID();
				Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
				VEHICLE::SET_VEHICLE_FIXED(veh);
				notification("Vehiculo reparado");
			}

			if (ImGui::Button("Explotar Vehiculo")) {
				Ped playerPed = PLAYER::PLAYER_PED_ID();
				Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
				VEHICLE::EXPLODE_VEHICLE(veh, true, false);
			}


			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Armas")) {

			if (ImGui::Button("Municion infinita")) {
				Ped playerPed = PLAYER::PLAYER_PED_ID();

				auto hash = WEAPON::GET_SELECTED_PED_WEAPON(playerPed);
				WEAPON::SET_PED_INFINITE_AMMO(playerPed, true, hash);
				notification("Municion infinita dada en el arma seleccionada");
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Extra")) {

			if (ImGui::Button("Hacer de dia")) {
				CLOCK::SET_CLOCK_TIME(12, 00, 00);
			}

			if (ImGui::Button("Tiempo soleado")) {
				MISC::SET_WEATHER_TYPE_NOW_PERSIST("EXTRASUNNY");
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Jugadores")) {
			if (ImGui::Button("hhahah")) {
				notification("jaj");
			}
			if (NETWORK::NETWORK_GET_NUM_CONNECTED_PLAYERS() != playerlist.size())
			{
				playerlist.clear();
				for (Player i = 0; i < gta::num_players; i++)
				{
					if (CNetGamePlayer* net_player = g_pointers->m_get_net_player(i))
					{
						auto cstr_name = net_player->get_name();
						std::string name = cstr_name;
						transform(name.begin(), name.end(), name.begin(), ::tolower);
						if (net_player->is_valid()) {
							
						}

					}
				}
			}

			ImGui::EndTabItem();
		}
	}
}