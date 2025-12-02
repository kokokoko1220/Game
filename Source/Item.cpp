#include "Item.h"
#include "ItemManager.h"

void Item::Destroy()
{
	ItemManager::Instance().Remove(this);
}