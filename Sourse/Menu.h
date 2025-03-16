#pragma once
#include "Pch.h"
#include "Src/Application/main.h"
class Menu
{
protected:
	KdTexture* m_backTexture; //Pointer to back texture
	//Buttons
	std::vector<Button> m_buttons; 

	int m_buttonCnt = 0;
	std::unordered_map<std::string, MenuTemplate> m_stringToEnumMap =
	{
		{"MainMenu", MainMenu},
		{"Settings", Settings}
	};

	Button _newButton;
protected:
	bool IsValidMenuKeyword(std::string keyword, int* enumValue);

	void InitMainMenu();
public:
	Menu();
	bool SetBackTexture(KdTexture* texture);
	KdTexture* GetBackTexture();
	Menu(std::string keyword = "");
};

