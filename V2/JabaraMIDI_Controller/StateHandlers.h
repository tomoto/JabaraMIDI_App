#pragma once
#include <Arduino.h>
#include "Data.h"
#include "UIState.h"
#include "Saver.h"
#include "RemoteConfigurator.h"

struct StateHandler {
  typedef std::function<void(int)> OnChange;
  typedef std::function<void()> OnClick;

  OnChange onChange;
  OnClick onClick;
  OnClick onLongPress;
};

class StateHandlers {
private:
  Data& data;
  UIState& uiState;
  Saver& saver;
  RemoteConfigurator& remoteConfigurator;

  StateHandler playHandler {
    [this](int delta){
      // nothing to do
    },
    [this](){
      data.mute = !data.mute;
      commitData();
    },
    [this]() {
      enterMenu();
    }
  };

  StateHandler menuHandler {
    [this](int delta){
      int newValue = uiState.menuItem + delta;
      while (newValue < 0) {
        newValue += uiState.menuItemCount;
      }
      uiState.menuItem = newValue % uiState.menuItemCount;
    },
    [this](){
      enterMenuItem();
    },
    [this]() {
      leaveMenu();
    }
  };

  std::array<StateHandler, 4> menuItemHandlers {
    commitOnExit([this](int delta){ changeDataValue(data.controlChangeNumber, delta, 0, 127); }),
    commitOnChange([this](int delta){ changeDataValue(data.minValue, delta, 0, 127); }),
    commitOnChange([this](int delta){ changeDataValue(data.force, delta * 50, 100, 900); }),
    commitOnChange([this](int delta){ changeDataValue(data.curve, delta * 0.05f, 0.1f, 2.0f); }),
  };

public:
  StateHandlers(Data& data, UIState& uiState, Saver& saver, RemoteConfigurator& remoteConfigurator) :
    data(data), uiState(uiState), saver(saver), remoteConfigurator(remoteConfigurator) {}

  StateHandler& getHandler() {
    switch (uiState.mode) {
    case UIState::Mode::PLAY:
      return playHandler;
    case UIState::Mode::MENU:
      return menuHandler;
    case UIState::Mode::MENUITEM:
      return menuItemHandlers[uiState.menuItem];
    }
  }

  void commitData() {
    remoteConfigurator.send();
  }

private:
  template<typename T>
  inline void changeDataValue(T& value, T delta, T minValue, T maxValue) {
    value = min(maxValue, max(minValue, value + delta));
  }

  void enterMenuItem() {
    uiState.mode = UIState::Mode::MENUITEM;
  }

  void leaveMenuItem() {
    uiState.mode = UIState::Mode::MENU;
  }

  void enterMenu() {
    uiState.mode = UIState::Mode::MENU;
  }

  void leaveMenu() {
    saver.save(data);
    uiState.mode = UIState::Mode::PLAY;
  }

  StateHandler commitOnExit(StateHandler::OnChange onChange) {
    return StateHandler {
      onChange,
      [this](){
        leaveMenuItem();
        commitData();
      },
      [this]() {
        leaveMenu();
        commitData();
      }
    };
  }

  StateHandler commitOnChange(StateHandler::OnChange onChange) {
    return StateHandler {
      [this, onChange](int delta) {
        onChange(delta);
        commitData();
      },
      [this]() {
        leaveMenuItem();
      },
      [this]() {
        leaveMenu();
      }
    };
  }
};