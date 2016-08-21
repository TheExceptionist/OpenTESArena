#include <cassert>

#include "Button.h"

#include "../Game/GameState.h"
#include "../Math/Int2.h"

Button::Button(int32_t x, int32_t y, int32_t width, int32_t height, 
	const std::function<void(GameState*)> &function)
	: Surface(x, y, width, height)
{
	this->function = function;
}

Button::Button(const Int2 &center, int32_t width, int32_t height, 
	const std::function<void(GameState*)> &function)
	: Button(center.getX() - (width / 2), center.getY() - (height / 2), 
		width, height, function) { }

Button::Button(const std::function<void(GameState*)> &function)
	: Button(0, 0, 1, 1, function) { }

Button::~Button()
{

}

void Button::click(GameState *gameState)
{
	this->function(gameState);
}
