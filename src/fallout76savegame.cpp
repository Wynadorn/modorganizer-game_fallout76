#include "fallout76savegame.h"

#include <Windows.h>

Fallout76SaveGame::Fallout76SaveGame(QString const &fileName, MOBase::IPluginGame const *game, bool const lightEnabled) :
  GamebryoSaveGame(fileName, game, lightEnabled)
{
  FileWrapper file(this, "FO76_SAVEGAME");
  file.skip<unsigned long>(); // header size
  file.skip<uint32_t>(); // header version
  file.read(saveNumber);

  file.read(playerName);

  unsigned long temp;
  file.read(temp);
  playerLevel = static_cast<unsigned short>(temp);
  file.read(playerLocation);

  QString ignore;
  file.read(ignore);   // playtime as ascii hh.mm.ss
  file.read(ignore);   // race name (i.e. BretonRace)

  file.skip<unsigned short>(); // Player gender (0 = male)
  file.skip<float>(2);         // experience gathered, experience required

  file.read(creationTime);
}

std::unique_ptr<GamebryoSaveGame::DataFields> Fallout4SaveGame::fetchDataFields() const
{
  FileWrapper file(getFilepath(), "FO4_SAVEGAME"); //10bytes

  {
    QString dummyName, dummyLocation;
    unsigned short dummyLevel;
    unsigned long dummySaveNumber;
    FILETIME dummyTime;

    fetchInformationFields(file, dummySaveNumber, dummyName, dummyLevel,
      dummyLocation, dummyTime);
  }

  QString ignore;
  std::unique_ptr<DataFields> fields = std::make_unique<DataFields>();

  fields->Screenshot = file.readImage(384, true);

  uint8_t saveGameVersion = file.readChar();
  file.read(ignore);     // game version
  file.skip<uint32_t>(); // plugin info size

  fields->Plugins = file.readPlugins();
  if (saveGameVersion >= 68) {
    fields->LightPlugins = file.readLightPlugins();
  }

  return fields;
}