#include <cassert>

#include "SDL.h"

#include "ClimateType.h"
#include "LocationType.h"
#include "WeatherType.h"
#include "WorldData.h"
#include "WorldType.h"
#include "../Assets/CityDataFile.h"
#include "../Assets/INFFile.h"
#include "../Assets/MIFFile.h"
#include "../Assets/RMDFile.h"
#include "../Media/TextureManager.h"
#include "../Rendering/Renderer.h"
#include "../Utilities/Debug.h"
#include "../Utilities/String.h"

WorldData::WorldData()
{
	// Partially initialized until constructed through one of the static load methods.
	this->worldType = WorldType::City;
	this->currentLevel = -1;
}

WorldData::~WorldData()
{

}

std::string WorldData::generateCityInfName(ClimateType climateType, WeatherType weatherType)
{
	const std::string climateLetter = [climateType]()
	{
		if (climateType == ClimateType::Temperate)
		{
			return "T";
		}
		else if (climateType == ClimateType::Desert)
		{
			return "D";
		}
		else
		{
			return "M";
		}
	}();

	// City/town/village letter. Wilderness is "W".
	const std::string locationLetter = "C";

	const std::string weatherLetter = [weatherType]()
	{
		if ((weatherType == WeatherType::Clear) ||
			(weatherType == WeatherType::Overcast))
		{
			return "N";
		}
		else if (weatherType == WeatherType::Rain)
		{
			return "R";
		}
		else if (weatherType == WeatherType::Snow)
		{
			return "S";
		}
		else
		{
			// Not sure what this letter represents.
			return "W";
		}
	}();

	return climateLetter + locationLetter + weatherLetter + ".INF";
}

std::string WorldData::generateWildernessInfName(ClimateType climateType, WeatherType weatherType)
{
	const std::string climateLetter = [climateType]()
	{
		if (climateType == ClimateType::Temperate)
		{
			return "T";
		}
		else if (climateType == ClimateType::Desert)
		{
			return "D";
		}
		else
		{
			return "M";
		}
	}();

	// Wilderness is "W".
	const std::string locationLetter = "W";

	const std::string weatherLetter = [weatherType]()
	{
		if ((weatherType == WeatherType::Clear) ||
			(weatherType == WeatherType::Overcast))
		{
			return "N";
		}
		else if (weatherType == WeatherType::Rain)
		{
			return "R";
		}
		else if (weatherType == WeatherType::Snow)
		{
			return "S";
		}
		else
		{
			// Not sure what this letter represents.
			return "W";
		}
	}();

	return climateLetter + locationLetter + weatherLetter + ".INF";
}

LocationType WorldData::getLocationTypeFromID(int cityID)
{
	// Local IDs can be between 0 and 31.
	const auto &idPair = CityDataFile::getLocalAndProvinceID(cityID);
	const int localID = idPair.first;

	if (localID < 8)
	{
		return LocationType::CityState;
	}
	else if (localID < 16)
	{
		return LocationType::Town;
	}
	else if (localID < 32)
	{
		return LocationType::Village;
	}
	else
	{
		throw std::runtime_error("Bad city ID \"" + std::to_string(cityID) + "\".");
	}
}

WorldData WorldData::loadInterior(const MIFFile &mif)
{
	WorldData worldData;

	// Generate levels.
	for (const auto &level : mif.getLevels())
	{
		worldData.levels.push_back(LevelData::loadInterior(
			level, mif.getDepth(), mif.getWidth()));
	}

	// Convert start points from the old coordinate system to the new one.
	for (const auto &point : mif.getStartPoints())
	{
		worldData.startPoints.push_back(VoxelGrid::getTransformedCoordinate(
			point, mif.getDepth(), mif.getWidth()));
	}

	worldData.currentLevel = mif.getStartingLevelIndex();
	worldData.worldType = WorldType::Interior;
	worldData.mifName = mif.getName();

	return worldData;
}

WorldData WorldData::loadPremadeCity(const MIFFile &mif, ClimateType climateType,
	WeatherType weatherType)
{
	WorldData worldData;
	
	// Generate level.
	const auto &level = mif.getLevels().front();
	const std::string infName = WorldData::generateCityInfName(climateType, weatherType);
	const INFFile inf(infName);
	worldData.levels.push_back(LevelData::loadPremadeCity(
		level, inf, mif.getDepth(), mif.getWidth()));

	// Convert start points from the old coordinate system to the new one.
	for (const auto &point : mif.getStartPoints())
	{
		worldData.startPoints.push_back(VoxelGrid::getTransformedCoordinate(
			point, mif.getDepth(), mif.getWidth()));
	}

	worldData.currentLevel = mif.getStartingLevelIndex();
	worldData.worldType = WorldType::City;
	worldData.mifName = mif.getName();

	return worldData;
}

WorldData WorldData::loadCity(int cityID, const MIFFile &mif, int cityX, int cityY,
	int cityDim, const std::vector<uint8_t> &reservedBlocks, const Int2 &startPosition,
	LocationType locationType, WeatherType weatherType)
{
	WorldData worldData;

	// Generate level.
	const auto &level = mif.getLevels().front();

	// To do: obtain climate from city data.
	const ClimateType climateType = ClimateType::Temperate;

	const std::string infName = WorldData::generateCityInfName(climateType, weatherType);
	const INFFile inf(infName);
	worldData.levels.push_back(LevelData::loadCity(level, cityX, cityY, cityDim,
		reservedBlocks, startPosition, inf, mif.getDepth(), mif.getWidth()));

	// Convert start points from the old coordinate system to the new one.
	for (const auto &point : mif.getStartPoints())
	{
		worldData.startPoints.push_back(VoxelGrid::getTransformedCoordinate(
			point, mif.getDepth(), mif.getWidth()));
	}

	worldData.currentLevel = mif.getStartingLevelIndex();
	worldData.worldType = WorldType::City;
	worldData.mifName = mif.getName();

	return worldData;
}

WorldData WorldData::loadWilderness(int rmdTR, int rmdTL, int rmdBR, int rmdBL,
	ClimateType climateType, WeatherType weatherType)
{
	WorldData worldData;

	const std::string infName = WorldData::generateWildernessInfName(climateType, weatherType);
	const INFFile inf(infName);

	// Load wilderness data (128x128 blank slate with four chunks. No starting points to load).
	worldData.levels.push_back(LevelData::loadWilderness(rmdTR, rmdTL, rmdBR, rmdBL, inf));

	worldData.currentLevel = 0;
	worldData.worldType = WorldType::Wilderness;
	worldData.mifName = "WILD.MIF";

	return worldData;
}

int WorldData::getCurrentLevel() const
{
	return this->currentLevel;
}

WorldType WorldData::getWorldType() const
{
	return this->worldType;
}

const std::string &WorldData::getMifName() const
{
	return this->mifName;
}

EntityManager &WorldData::getEntityManager()
{
	return this->entityManager;
}

const EntityManager &WorldData::getEntityManager() const
{
	return this->entityManager;
}

const std::vector<Double2> &WorldData::getStartPoints() const
{
	return this->startPoints;
}

std::vector<LevelData> &WorldData::getLevels()
{
	return this->levels;
}

const std::vector<LevelData> &WorldData::getLevels() const
{
	return this->levels;
}

void WorldData::setLevelActive(int levelIndex, TextureManager &textureManager,
	Renderer &renderer)
{
	assert(levelIndex < this->levels.size());
	this->currentLevel = levelIndex;

	// Clear all entities.
	for (const auto *entity : this->entityManager.getAllEntities())
	{
		renderer.removeFlat(entity->getID());
		this->entityManager.remove(entity->getID());
	}

	// Clear software renderer textures.
	renderer.clearTextures();

	// Get the level being switched to.
	const auto &level = this->levels.at(this->currentLevel);

	// If the world is an interior, change the sky palette. The outdoor dungeons have a gray
	// sky while their lower levels have a black sky.
	if (this->worldType == WorldType::Interior)
	{
		const uint32_t skyColor = level.getInteriorSkyColor();
		renderer.setSkyPalette(&skyColor, 1);
	}

	// Load the .INF file associated with the level.
	// - To do: not sure if LevelData should store its own INFFile or just the name.
	const INFFile inf(level.getInfName());

	// Load .INF voxel textures into the renderer. Assume all voxel textures are 64x64.
	const int voxelTextureCount = static_cast<int>(inf.getVoxelTextures().size());
	for (int i = 0; i < voxelTextureCount; i++)
	{
		const auto &textureData = inf.getVoxelTextures().at(i);

		const std::string textureName = String::toUppercase(textureData.filename);
		const std::string extension = String::getExtension(textureName);

		const bool isIMG = extension == ".IMG";
		const bool isSET = extension == ".SET";
		const bool noExtension = extension.size() == 0;

		if (isSET)
		{
			// Use the texture data's .SET index to obtain the correct surface.
			const auto &surfaces = textureManager.getSurfaces(textureName);
			const SDL_Surface *surface = surfaces.at(textureData.setIndex);
			renderer.setVoxelTexture(i, static_cast<const uint32_t*>(surface->pixels));
		}
		else if (isIMG)
		{
			const SDL_Surface *surface = textureManager.getSurface(textureName);
			renderer.setVoxelTexture(i, static_cast<const uint32_t*>(surface->pixels));
		}
		else if (noExtension)
		{
			// Ignore texture names with no extension. They appear to be lore-related names
			// that were used at one point in Arena's development.
			static_cast<void>(textureData);
		}
		else
		{
			DebugCrash("Unrecognized voxel texture extension \"" + extension + "\".");
		}
	}

	// Load .INF flat textures into the renderer.
	// - To do: maybe turn this into a while loop, so the index variable can be incremented
	//   by the size of each .DFA. It's incorrect as-is.
	/*const int flatTextureCount = static_cast<int>(inf.getFlatTextures().size());
	for (int i = 0; i < flatTextureCount; i++)
	{
		const auto &textureData = inf.getFlatTextures().at(i);

		const std::string textureName = String::toUppercase(textureData.filename);
		const std::string extension = String::getExtension(textureName);

		const bool isDFA = extension == ".DFA";
		const bool isIMG = extension == ".IMG";
		const bool noExtension = extension.size() == 0;

		if (isDFA)
		{
			// To do: creatures don't have .DFA files (although they're referenced in the .INF
			// files), so I think the extension needs to be .CFA instead for them.
			//const auto &surfaces = textureManager.getSurfaces(textureName);
			//for (const auto *surface : surfaces)
			//{
			//renderer.addTexture(static_cast<const uint32_t*>(surface->pixels),
			//surface->w, surface->h);
			//}
		}
		else if (isIMG)
		{
			const SDL_Surface *surface = textureManager.getSurface(textureName);
			renderer.setFlatTexture(i, static_cast<const uint32_t*>(surface->pixels),
				surface->w, surface->h);
		}
		else if (noExtension)
		{
			// Ignore texture names with no extension. They appear to be lore-related names
			// that were used at one point in Arena's development.
			static_cast<void>(textureData);
		}
		else
		{
			DebugCrash("Unrecognized flat texture extension \"" + extension + "\".");
		}
	}*/
}
