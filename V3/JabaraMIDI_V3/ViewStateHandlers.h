#pragma once

#include "Saver.h"
#include "ViewData.h"
#include "ViewState.h"

struct ViewStateHandler {
  typedef std::function<void(int)> OnChange;
  typedef std::function<void()> OnClick;

  OnChange onChange;
  OnClick onClick;
  OnClick onLongPress;
};

class ViewStateHandlers {
 public:
  typedef std::function<void(const ViewData&)> OnCommit;

 private:
  ViewData& data;
  ViewState& state;
  Saver& saver;
  OnCommit onCommit;

  ViewStateHandler playHandler{[this](int delta) {
                                 // nothing to do
                               },
                               [this]() {
                                 data.mute = !data.mute;
                                 commitData();
                               },
                               [this]() { enterMenu(); }};

  ViewStateHandler menuHandler{
      [this](int delta) {
        int newValue = state.menuItem + delta;
        while (newValue < 0) {
          newValue += state.menuItemCount;
        }
        state.menuItem = newValue % state.menuItemCount;
      },
      [this]() { enterMenuItem(); }, [this]() { leaveMenu(); }};

  std::array<ViewStateHandler, 4> menuItemHandlers{
      commitOnExit([this](int delta) {
        changeDataValue(data.controlChangeNumber, delta, 0, 127);
      }),
      commitOnChange(
          [this](int delta) { changeDataValue(data.minValue, delta, 0, 127); }),
      commitOnChange([this](int delta) {
        changeDataValue(data.force, delta * 50, 100, 900);
      }),
      commitOnChange([this](int delta) {
        changeDataValue(data.curve, delta * 0.05f, 0.1f, 2.0f);
      }),
  };

 public:
  ViewStateHandlers(ViewData& data, ViewState& state, Saver& saver,
                    OnCommit onCommit)
      : data(data), state(state), saver(saver), onCommit(onCommit) {}

  ViewStateHandler& getHandler() {
    switch (state.mode) {
      case ViewState::Mode::PLAY:
        return playHandler;
      case ViewState::Mode::MENU:
        return menuHandler;
      case ViewState::Mode::MENUITEM:
        return menuItemHandlers[state.menuItem];
    }
  }

  void commitData() { onCommit(data); }

 private:
  template <typename T>
  inline void changeDataValue(T& value, T delta, T minValue, T maxValue) {
    value = min(maxValue, max(minValue, value + delta));
  }

  void enterMenuItem() { state.mode = ViewState::Mode::MENUITEM; }

  void leaveMenuItem() { state.mode = ViewState::Mode::MENU; }

  void enterMenu() { state.mode = ViewState::Mode::MENU; }

  void leaveMenu() {
    saver.save(data);
    state.mode = ViewState::Mode::PLAY;
  }

  ViewStateHandler commitOnExit(ViewStateHandler::OnChange onChange) {
    return ViewStateHandler{onChange,
                            [this]() {
                              leaveMenuItem();
                              commitData();
                            },
                            [this]() {
                              leaveMenu();
                              commitData();
                            }};
  }

  ViewStateHandler commitOnChange(ViewStateHandler::OnChange onChange) {
    return ViewStateHandler{[this, onChange](int delta) {
                              onChange(delta);
                              commitData();
                            },
                            [this]() { leaveMenuItem(); },
                            [this]() { leaveMenu(); }};
  }
};