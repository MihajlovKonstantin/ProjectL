#include "Pch.h"
#include "Menu.h"

bool Menu::IsValidMenuKeyword(std::string keyword, int* enumValue)
{
	bool _result = false;

	auto _finder = m_stringToEnumMap.find(keyword);
	if (_finder != m_stringToEnumMap.end())
	{
		*enumValue = _finder->second;
		_result = true;
	}

	return _result;
}

void Menu::InitMainMenu()
{
	_newButton;
}

Menu::Menu()
{
}

bool Menu::SetBackTexture(KdTexture* texture)
{
	m_backTexture = texture;

	return (m_backTexture) ? true: false;

}

KdTexture* Menu::GetBackTexture()
{
	return m_backTexture;
}

Menu::Menu(std::string keyword)
{
	int _seed = 0;
	bool _keywordValidator = IsValidMenuKeyword(keyword,&_seed);

	if (_keywordValidator)
	{
		switch (_seed)
		{
		case MainMenu:
			MessageBoxA(APP.m_window.GetWndHandle(), "MainMenuì¬", "MM", MB_OK);
			break;
		case Settings:
			MessageBoxA(APP.m_window.GetWndHandle(), "Settingsì¬", "Set", MB_OK);
			break;
		}
	}
	else{
		MessageBoxA(APP.m_window.GetWndHandle(), "MenuKeyWord‚ªŠÔˆá‚¤", "Error", MB_OK);
	}

}
