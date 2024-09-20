#include "main.h"
#include <random>
#include <Windows.h>
//#include <chrono>
//test
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
//test contraste texte
#include "C:\Users\bacus\Documents\me\OKVM\apex_guest\Client\Client\imgui\imgui.h"

typedef struct player
{
	float dist = 0;
	int entity_team = 0;
	float boxMiddle = 0;
	float h_y = 0;
	float width = 0;
	float height = 0;
	float b_x = 0;
	float b_y = 0;
	bool knocked = false;
	bool visible = false;
	int health = 0;
	int shield = 0;
	int maxshield = 0;
	int armortype = 0;
	int xp_level = 0;
	char name[33] = { 0 };
}player;

typedef struct spectator {
	bool is_spec = false;
	char name[33] = { 0 };
}spectator;

uint32_t check = 0xABCD;

int aim_key = VK_LBUTTON;
int aim_key2 = VK_RBUTTON;
//int bhop_key = VK_SPACE;
//int rapidfire_key = VK_XBUTTON1;
bool use_nvidia = false;
bool active = true;
bool ready = false;
extern visuals v;
int aim = 2; //read
bool esp = false; //read
//bool item_glow = false;
bool player_glow = false;
bool aim_no_recoil = true;
bool aiming = false; //read
uint64_t g_Base = 0; //write
float max_dist = 110.0f * 40.0f;
//float esp_distance = 300.0f * 40.0f;
float smooth = 130.00f;
float max_fov = 3.80f;
int bone = 2;
//int SuperKey = VK_LSHIFT;
//static bool startSg = false;

//float esp_distance = 300.0f; // Units in meters

float DDS = 80.0f * 40.0f; //need test 25 before for closets targets but seem to be wrong
//float EBD = 300.0f * 40.0f; //distance for seer esp and box esp

bool firing_range = false;
bool shooting = false; //read

//bool bone_auto = true;
//int shoot_key = VK_LBUTTON;
//1v1
bool onevone = false;

//items
//bool medbackpack = true;
//bool heavybackpack = true;
//bool goldbackpack = true;
//Shield upgrades
//bool shieldupgrade2 = true;  //blue
//bool shieldupgrade3 = true;  //purple
//bool shieldupgrade4 = true;  //gold
//bool shieldupgrade5 = true;  //red
//bool shieldupgradehead3 = true;
//bool shieldupgradehead4 = true;
//bool shielddown3 = true;
//bool shielddown4 = true;
//Optics
//bool optic1xhcog = true;
//bool optic2xhcog = true;
//bool opticthreat = true;
//Nades
//bool grenade_arc_star = true;
//bool grenade_thermite = true;
//Shotguns
//bool weapon_eva8 = true;
//Energy weapons
//bool weapon_lstar = true;
//bool weapon_nemesis = true;
//bool weapon_havoc = true;
//bool weapon_prowler = false;
//bool weapon_volt = true;
//Heavy Weapons
//bool weapon_flatline = true;
//bool weapon_hemlock = true;
//bool weapon_3030_repeater = true;
//bool weapon_rampage = true;
//bool weapon_car_smg = true;
//Light weapons
//bool weapon_re45 = true;
//bool weapon_alternator = true;
//bool weapon_r99 = true;

///test contraste texte
ImU32 GetContrastColor(ImU32 backgroundColor) {
	// Calculer le contraste entre la couleur de fond et le blanc
	int r = (backgroundColor >> 16) & 0xFF;
	int g = (backgroundColor >> 8) & 0xFF;
	int b = backgroundColor & 0xFF;
	float luminance = (0.299f * r + 0.587f * g + 0.114f * b) / 255.0f;

	return luminance > 0.5f ? IM_COL32(0, 0, 0, 255) : IM_COL32(255, 255, 255, 255);
}

// Obtenir dynamiquement la couleur de fond
ImU32 GetImU32Color(const ImVec4& color) {
	return IM_COL32(
		static_cast<int>(color.x * 255.0f),
		static_cast<int>(color.y * 255.0f),
		static_cast<int>(color.z * 255.0f),
		static_cast<int>(color.w * 255.0f)
	);
}
///end test

static bool showing = false;
static bool k_del = 0;
float cfsize = max_fov;

int spectators = 0; //write
int allied_spectators = 0; //write

//Player Glow Color and Brightness
float glowr = 100.0f; //Red Value
float glowg = 0.0f; //Green Value
float glowb = 0.0f; //Blue Value
float glowcolor[3] = { 000.0f, 000.0f, 000.0f };
//more glow stuff
//glow visable
float glowrviz = 0.0f;
float glowgviz = 100.0f;
float glowbviz = 0.0f;
float glowcolorviz[3] = { 000.0f, 000.0f, 000.0f };
//knocked
float glowrknocked = 100.0f;
float glowgknocked = 100.0f;
float glowbknocked = 100.0f;
float glowcolorknocked[3] = { 000.0f, 000.0f, 000.0f };

bool valid = false; //write
bool next = false; //read write

int index = 0;

uint64_t add[29];

bool k_f1 = 0;
bool k_f2 = 0;
bool k_f3 = 0;

bool k_f5 = 0;
bool k_f6 = 0;
bool k_f7 = 0;
bool k_f8 = 0;
bool k_f9 = 0;
bool k_f10 = 0;
bool k_f11 = 0;
//bool k_lshift = 0;

bool IsKeyDown(int vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

player players[100];

void randomBone()
{
	int boneArray[3] = { 0, 1, 2 };

	// Seed the random number generator with the current time
	srand((unsigned int)time(NULL));

	int randVal = rand() % 3;
	bone = boneArray[randVal];
	Sleep(500);
}

spectator spectator_list[100];

void Overlay::RenderSpectator() {
    ImGui::SetNextWindowPos(ImVec2(490, 0));
    ImGui::SetNextWindowSize(ImVec2(190, 130));
    ImGui::Begin(XorStr("##spectator_list"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	ImGui::TextColored(RED, "%d", spectators);
	ImGui::SameLine();
	ImGui::Text("-");
	ImGui::SameLine();
	ImGui::TextColored(GREEN, "%d", allied_spectators);

    int text_index = 0;
    for (const auto& spectator : spectator_list) {
        if (spectator.is_spec) {
			ImGui::TextColored(WHITE, "%d", text_index + 1);
            ImGui::SameLine(25);
			ImGui::TextColored(ORANGE, "%s", spectator.name);
            text_index++;
			//TEST// Convert char array to std::wstring
			//std::wstring wideStr;
			//for (int i = 0; spectator.name[i] != '\0'; ++i)
			//	wideStr += static_cast<wchar_t>(spectator.name[i]);

			// Pass the wide string to ImGui::TextColored
			//ImGui::TextColored(ORANGE, "%ls", wideStr.c_str());

			//text_index++;
			//
        }
    }

    ImGui::End();
    //memset(spectator_list, 0, sizeof(spectator_list));
}

void Overlay::RenderEsp()
{
	next = false;
	if (g_Base != 0 && esp)
	{
		memset(players, 0, sizeof(players));
		while (!next && esp)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		if (next && valid)
		{

			// Add a log file for debugging
			//std::ofstream logFile("client_debug.log", std::ios::app);

			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)getWidth(), (float)getHeight()));
			ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

			for (int i = 0; i < 100; i++)
			{
				if (players[i].health > 0)
				{
					std::string distance = std::to_string(players[i].dist / 39.62);
					distance = distance.substr(0, distance.find('.')) + "m(" + std::to_string(players[i].entity_team) + ")";
					if (v.box)
					{
						if (players[i].visible)
						{
							if (players[i].dist < 9000.0f) //before 9000.0f
								DrawBox(GREEN, players[i].boxMiddle, players[i].h_y, players[i].width, players[i].height); //BOX
							else
								DrawBox(WHITE, players[i].boxMiddle, players[i].h_y, players[i].width, players[i].height); //BOX
						}
						else
						{
							DrawBox(RED, players[i].boxMiddle, players[i].h_y, players[i].width, players[i].height); //red if player not visible
						}
					}

					// Assuming players[i].dist represents the distance of the player

					// Define the maximum distance you want to consider (previously referred to as max_distance)
					//float max_dist = 80.0f * 40.0f; // Adjust this value as per your requirement

					// Calculate a scaling factor based on the player's distance
					float distanceFactor = 1.0f - (players[i].dist / max_dist);

					// Define the minimum and maximum values for max_fov, cfsize, and smooth
					const float min_max_fov = 3.80f;
					const float max_max_fov = 12.00f;
					const float min_cfsize = min_max_fov;
					const float max_cfsize = max_max_fov;
					const float min_smooth = 90.00f;
					const float max_smooth = 130.00f;

					// Smoothly interpolate the adjusted values based on the distance factor
					float adjusted_max_fov = min_max_fov + distanceFactor * (max_max_fov - min_max_fov);
					float adjusted_cfsize = min_cfsize + distanceFactor * (max_cfsize - min_cfsize);
					float adjusted_smooth = min_smooth + distanceFactor * (max_smooth - min_smooth);

					// Check the distance condition and update the values accordingly
					if (players[i].dist < DDS) // DDS at top as before
					{
						max_fov = adjusted_max_fov;
						cfsize = adjusted_cfsize;
						aim_key = true; // You can set this based on your needs
						aim_key2 = true;
						smooth = adjusted_smooth;
					}
					else
					{
						max_fov = 3.80f; //adjusted_max_fov;
						cfsize = max_fov;
						aim_key = true; // You can set this based on your needs
						aim_key2 = false;
						smooth = 130.00f; //adjusted_smooth;
					}

					//if(v.line)
					//	DrawLine(ImVec2((float)(getWidth() / 2), (float)getHeight()), ImVec2(players[i].b_x, players[i].b_y), BLUE, 1); //LINE FROM MIDDLE SCREEN

					if (v.distance)
					{
						std::string distance = std::to_string(players[i].dist / 39.62);
						distance = distance.substr(0, distance.find('.')) + "m (" + std::to_string(players[i].entity_team) + ")";
						distance += " |" + std::to_string(players[i].xp_level) + "|";

						// Display the distance just below the name
						if (players[i].knocked)
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 25)), RED, distance.c_str());  // DISTANCE
						else
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 25)), GREEN, distance.c_str());  // DISTANCE
					}

					// SEER STYLE health bar logic
					if (v.healthbar)
					{
						if (players[i].dist < 9000.0f)
						{
							// Divide the values by 2 to reduce the size of the health bar
							float barX = players[i].b_x - (players[i].width / 4.0f) + 5;  // Halve the width
							float barY = players[i].b_y - (players[i].height / 2.0f) - 10;  // Halve the height

							DrawSeerLikeHealth(barX, barY, players[i].shield, players[i].maxshield, players[i].armortype, players[i].health);  // Health bar
						}
					}

					// Display the name just above the distance
					if (v.name)
					{
						if (players[i].knocked)
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 15)), RED, players[i].name);  // NAME in RED if knocked
						else
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 15)), GREEN, players[i].name);  // NAME in GREEN if not knocked
					}

				}
			}

			ImGui::End();
		}
	}
}

int main(int argc, char** argv)
{
	add[0] = (uintptr_t)&check;
	add[1] = (uintptr_t)&aim;
	add[2] = (uintptr_t)&esp;
	add[3] = (uintptr_t)&aiming;
	add[4] = (uintptr_t)&g_Base;
	add[5] = (uintptr_t)&next;
	add[6] = (uintptr_t)&players[0];
	add[7] = (uintptr_t)&valid;
	add[8] = (uintptr_t)&max_dist;
	//add[9] = (uintptr_t)&item_glow;
	add[9] = (uintptr_t)&player_glow;
	add[10] = (uintptr_t)&aim_no_recoil;
	add[11] = (uintptr_t)&smooth;
	add[12] = (uintptr_t)&max_fov;
	add[13] = (uintptr_t)&bone;
	add[14] = (uintptr_t)&spectators;
	add[15] = (uintptr_t)&allied_spectators;

	//glow notvisable
	add[16] = (uintptr_t)&glowr;
	add[17] = (uintptr_t)&glowg;
	add[18] = (uintptr_t)&glowb;
	//glow visable
	add[19] = (uintptr_t)&glowrviz;
	add[20] = (uintptr_t)&glowgviz;
	add[21] = (uintptr_t)&glowbviz;
	//knocked
	add[22] = (uintptr_t)&glowrknocked;
	add[23] = (uintptr_t)&glowgknocked;
	add[24] = (uintptr_t)&glowbknocked;
	add[25] = (uintptr_t)&firing_range;
	add[26] = (uintptr_t)&shooting;
	//items
	//add[28] = (uintptr_t)&medbackpack;
	//add[29] = (uintptr_t)&heavybackpack;
	//add[30] = (uintptr_t)&goldbackpack;
	//add[31] = (uintptr_t)&shieldupgrade2;
	//add[32] = (uintptr_t)&shieldupgrade3;
	//add[33] = (uintptr_t)&shieldupgrade4;
	//add[34] = (uintptr_t)&shieldupgrade5;
	//add[35] = (uintptr_t)&shieldupgradehead3;
	//add[36] = (uintptr_t)&shieldupgradehead4;
	//add[37] = (uintptr_t)&shielddown3;
	//add[38] = (uintptr_t)&shielddown4;
	//add[39] = (uintptr_t)&optic1xhcog;
	//add[40] = (uintptr_t)&optic2xhcog;
	//add[41] = (uintptr_t)&opticthreat;
	//add[42] = (uintptr_t)&grenade_arc_star;
	//add[43] = (uintptr_t)&grenade_thermite;
	//add[44] = (uintptr_t)&weapon_eva8;
	//add[45] = (uintptr_t)&weapon_lstar;
	//add[46] = (uintptr_t)&weapon_nemesis;
	//add[47] = (uintptr_t)&weapon_havoc;
	//add[48] = (uintptr_t)&weapon_prowler;
	//add[49] = (uintptr_t)&weapon_volt;
	//add[50] = (uintptr_t)&weapon_flatline;
	//add[51] = (uintptr_t)&weapon_hemlock;
	//add[52] = (uintptr_t)&weapon_3030_repeater;
	//add[53] = (uintptr_t)&weapon_rampage;
	//add[54] = (uintptr_t)&weapon_car_smg;
	//add[55] = (uintptr_t)&weapon_re45;
	//add[56] = (uintptr_t)&weapon_alternator;
	//add[57] = (uintptr_t)&weapon_r99;
	add[27] = (uintptr_t)&onevone;
	add[28] = (uintptr_t)&spectator_list;
	//add[29] = (uintptr_t)&esp_distance;


	printf(XorStr("add offset: 0x%I64x\n"), (uint64_t)&add[0] - (uint64_t)GetModuleHandle(NULL));

	Overlay ov1 = Overlay();
	ov1.Start();
	printf(XorStr("Waiting for host process...\n"));
	while (check == 0xABCD)
	{
		if (IsKeyDown(VK_F4))
		{
			active = false;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if (active)
	{
		ready = true;
		printf(XorStr("Ready\n"));
	}
		
	while (active)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (IsKeyDown(VK_F4))
		{
			active = false;
		}

		//Load at start for saved settings to take effect. Need to save once to make the file. 

		for (static bool once = true; once; once = false) {
			std::ifstream config("Settings.txt");
			if (config.is_open())
			{
				//config >> std::boolalpha >> firing_range;
				config >> aim;
				config >> std::boolalpha >> esp;
				//config >> std::boolalpha >> item_glow;
				config >> std::boolalpha >> player_glow;
				//config >> std::boolalpha >> aim_no_recoil;
				config >> max_dist;
				//config >> smooth;
				//config >> max_fov;
				//config >> bone;
				config >> v.healthbar;
				config >> v.shieldbar;
				config >> v.distance;
				//config >> v.player_level;
				config >> v.line;
				//config >> cfsize;
				//config >> DDS;
				//config >> aiming;
				config >> glowr;
				config >> glowg;
				config >> glowb;
				config >> glowcolor[0];
				config >> glowcolor[1];
				config >> glowcolor[2];
				//glow visable
				config >> glowrviz;
				config >> glowgviz;
				config >> glowbviz;
				config >> glowcolorviz[0];
				config >> glowcolorviz[1];
				config >> glowcolorviz[2];
				//glow knocked
				config >> glowrknocked;
				config >> glowgknocked;
				config >> glowbknocked;
				config >> glowcolorknocked[0];
				config >> glowcolorknocked[1];
				config >> glowcolorknocked[2];
				config >> firing_range;
				//config >> onevone;
				config.close();
			}
		}

		memset(spectator_list, 0, sizeof(spectator_list));

		if (IsKeyDown(VK_F1) && k_f1 == 0)
		{
			k_f1 = 1;
			esp = !esp;
			// aim = 2;
			player_glow = !player_glow;
			k_f6 = 1;
			//item_glow = !item_glow;
		}
		else if (!IsKeyDown(VK_F1) && k_f1 == 1)
		{
			k_f1 = 0;
			k_f6 = 0;
		}

		if (IsKeyDown(VK_F5) && k_f5 == 0)
		{
			k_f5 = 1;
			esp = !esp;
		}
		else if (!IsKeyDown(VK_F5) && k_f5 == 1)
		{
			k_f5 = 0;
		}

		if (IsKeyDown(VK_F6) && k_f6 == 0)
		{
			k_f6 = 1;
			//switch (aim)
			//{
			//case 0:
			//	aim = 1;
			//	break;
			//case 1:
			aim = 2;
			//	break;
			//case 2:
			//	aim = 0;
			//	break;
			//default:
			//	break;
			//}
		}
		else if (!IsKeyDown(VK_F6) && k_f6 == 1)
		{
			k_f6 = 0;
		}

		if (IsKeyDown(VK_F7) && k_f7 == 0)
		{
			k_f7 = 1;
			player_glow = !player_glow;
		}
		else if (!IsKeyDown(VK_F7) && k_f7 == 1)
		{
			k_f7 = 0;
		}

		if (IsKeyDown(VK_F8) && k_f8 == 0)
		{
			k_f8 = 1;
			//item_glow = !item_glow;
		}
		else if (!IsKeyDown(VK_F8) && k_f8 == 1)
		{
			k_f8 = 0;
		}

		if (IsKeyDown(VK_F9) && k_f9 == 0)
		{
			k_f9 = 1;
			onevone = !onevone;
		}
		else if (!IsKeyDown(VK_F9) && k_f9 == 1)
		{
			k_f9 = 0;
		}

		if (IsKeyDown(VK_F10) && k_f10 == 0)
		{
			k_f10 = 1;
			firing_range = !firing_range;
		}
		else if (!IsKeyDown(VK_F10) && k_f10 == 1)
		{
			k_f10 = 0;
		}

		if (IsKeyDown(VK_LEFT))
		{
			if (max_dist > 100.0f * 40.0f)
				max_dist -= 50.0f * 40.0f;
			std::this_thread::sleep_for(std::chrono::milliseconds(130));
		}

		if (IsKeyDown(VK_RIGHT))
		{
			if (max_dist < 800.0f * 40.0f)
				max_dist += 50.0f * 40.0f;
			std::this_thread::sleep_for(std::chrono::milliseconds(130));
		}

		if (IsKeyDown(VK_DELETE) && k_del == 0)
		{
			k_del = 1;
			showing = !showing;
		}
		else if (!IsKeyDown(VK_DELETE) && k_del == 1)
		{
			k_del = 0;
		}
		
		////////////////////////////////////NORMAL AIM & BUTTON///////////////////////////////////////
		if (IsKeyDown(aim_key) || IsKeyDown(aim_key2))
		{
			aiming = true;
			//randomBone();//RANDOMIZE BONE WHEN SHOOT
		}
		else
		{
			aiming = false;
		}
		
	}
	ready = false;
	ov1.Clear();
	if(!use_nvidia)
		system(XorStr("taskkill /F /T /IM Overlay.exe")); //custom overlay process name
	return 0;
}
