#pragma once
#include <EEPROM.h>
#include "Data.h"

struct Saver {
  struct SavedData {
    int magic = 0x185a75c;
    Data data;
  };

  void begin() {
    EEPROM.begin(sizeof(SavedData));
  }

  void commit() {
    EEPROM.commit();
  }

  void load(Data& data) {
    SavedData defaultSavedData;
    SavedData loadedData;
    EEPROM.get(0, loadedData);
    if (loadedData.magic == defaultSavedData.magic) {
      data = loadedData.data;
    }
  }

  void save(Data& data) {
    SavedData savedData;
    savedData.data = data;
    EEPROM.put(0, savedData);
    EEPROM.commit();
  }
};
