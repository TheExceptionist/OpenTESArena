#ifndef CHOOSE_RACE_PANEL_H
#define CHOOSE_RACE_PANEL_H

#include <string>

#include "Button.h"
#include "Panel.h"
#include "../Entities/CharacterClass.h"
#include "../Math/Vector2.h"
#include "../Rendering/Texture.h"

class Renderer;

enum class GenderName;

class ChooseRacePanel : public Panel
{
private:
	// The mask ID for no selected province.
	static const int NO_ID;

	Button<Game&, const CharacterClass&, const std::string&> backToGenderButton;
	Button<Game&, const CharacterClass&, const std::string&, GenderName, int> acceptButton;
	CharacterClass charClass;
	GenderName gender;
	std::string name;

	// Gets the initial parchment pop-up.
	static std::unique_ptr<Panel> getInitialSubPanel(Game &game,
		const CharacterClass &charClass, const std::string &name);

	// Gets the mask ID associated with some pixel location, or "no ID" if none found.
	int getProvinceMaskID(const Int2 &position) const;

	void drawProvinceTooltip(int provinceID, Renderer &renderer);	
public:
	ChooseRacePanel(Game &game, const CharacterClass &charClass, 
		const std::string &name, GenderName gender);
	virtual ~ChooseRacePanel();

	virtual std::pair<SDL_Texture*, CursorAlignment> getCurrentCursor() const override;
	virtual void handleEvent(const SDL_Event &e) override;
	virtual void render(Renderer &renderer) override;
	virtual void renderSecondary(Renderer &renderer) override;
};

#endif
