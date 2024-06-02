#include "SubMenu.h"

SubMenu::SubMenu():
	isActive_(false),
	camera_(new Camera{})
{}

void SubMenu::ActiveUpdate() {
	if (isActive_) {
		Update();
	}
}

void SubMenu::ActiveDraw()
{
	if (isActive_) {
		Draw();
	}
}
