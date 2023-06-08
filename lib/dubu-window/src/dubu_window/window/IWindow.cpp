#include "IWindow.h"

namespace dubu::window {

IWindow::~IWindow() {}

IWindow::IWindow() {}

void IWindow::SimulateEventKey(const EventKey& e) {
	Emit(e);

	switch (e.action) {
	case Action::Press:
		Emit(EventKeyPress{
		    .key = e.key, .scancode = e.scancode, .mods = e.mods});
		break;
	case Action::Release:
		Emit(EventKeyRelease{
		    .key = e.key, .scancode = e.scancode, .mods = e.mods});
		break;
	case Action::Repeat:
		Emit(EventKeyRepeat{
		    .key = e.key, .scancode = e.scancode, .mods = e.mods});
		break;
	}
}

void IWindow::SimulateEventChar(const EventChar& e) {
	Emit(e);
}

void IWindow::SimulateEventCursorPos(const EventCursorPos& e) {
	Emit(e);
}

void IWindow::SimulateEventMouseButton(const EventMouseButton& e) {
	Emit(e);

	switch (e.action) {
	case Action::Press:
		Emit(EventMouseButtonPress{.button = e.button, .mods = e.mods});
		break;
	case Action::Release:
		Emit(EventMouseButtonRelease{.button = e.button, .mods = e.mods});
		break;
	default:
		break;
	}
}

bool IWindow::IsGamepadConnected(int) {
	return false;
}

std::optional<GamepadState> IWindow::GetGamepadState(int) {
	return std::nullopt;
}

}  // namespace dubu::window