#ifndef WORLD_MAP_PANEL_H
#define WORLD_MAP_PANEL_H

#include <array>

#include "Button.h"
#include "Panel.h"
#include "../Math/Vector2.h"

class Renderer;

class WorldMapPanel : public Panel
{
private:
	Button<Game&> backToGameButton;
	Button<Game&, int> provinceButton;
	std::array<Int2, 9> provinceNameOffsets; // Yellow province name positions.
public:
	WorldMapPanel(Game &game);
	virtual ~WorldMapPanel();

	virtual std::pair<SDL_Texture*, CursorAlignment> getCurrentCursor() const override;
	virtual void handleEvent(const SDL_Event &e) override;
	virtual void render(Renderer &renderer) override;
};

#endif
