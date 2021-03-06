#include <cassert>
#include <string>

#include "CharacterClassGeneration.h"

CharacterClassGeneration::ClassData::ClassData()
{
	this->id = 0;
	this->isSpellcaster = false;
	this->hasCriticalHit = false;
	this->isThief = false;
}

CharacterClassGeneration::ChoiceData::ChoiceData()
{
	this->a = 0;
	this->b = 0;
	this->c = 0;
}

const int CharacterClassGeneration::ID_MASK = 0x1F;
const int CharacterClassGeneration::SPELLCASTER_MASK = 0x20;
const int CharacterClassGeneration::CRITICAL_HIT_MASK = 0x40;
const int CharacterClassGeneration::THIEF_MASK = 0x80;

const CharacterClassGeneration::ClassData &CharacterClassGeneration::getClassData(
	int a, int b, int c) const
{
	// A maximum of ten answers for any category is the limit.
	assert((a >= 0) && (a <= 10));
	assert((b >= 0) && (b <= 10));
	assert((c >= 0) && (c <= 10));

	// Find the index of the given A/B/C counts in the choices array.
	const int choiceID = [this, a, b, c]()
	{
		const int choicesSize = static_cast<int>(this->choices.size());

		// Search each choice for the matching A/B/C triplet.
		for (int i = 0; i < choicesSize; i++)
		{
			const CharacterClassGeneration::ChoiceData &choice = this->choices.at(i);

			if ((choice.a == a) && (choice.b == b) && (choice.c == c))
			{
				// The index was found, so return it.
				return i;
			}
		}

		// Otherwise, no associated mapping found.
		throw std::runtime_error("No class mapping found (a:" + std::to_string(a) + 
			", b:" + std::to_string(b) + ", c:" + std::to_string(c) + ").");
	}();

	// Calculate the class ID from the choice ID.
	const int classID = (choiceID < 48) ? (choiceID / 4) : (12 + ((choiceID - 48) / 3));

	// Get the class data associated with the class ID.
	return this->classes.at(classID);
}
