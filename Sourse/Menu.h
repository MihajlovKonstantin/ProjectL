#pragma once
class Menu
{
private:
	KdTexture* m_backTexture; //Pointer to back texture
public:
	Menu();
	bool SetBackTexture(KdTexture* texture);
	KdTexture* GetBackTexture();
};

