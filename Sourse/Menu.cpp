#include "Pch.h"
#include "Menu.h"

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
