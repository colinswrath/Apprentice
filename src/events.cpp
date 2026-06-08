#include "events.h"
#include "interface.h"
#include "racemenu.h"

namespace Events
{
    RE::BSEventNotifyControl VanillaMenuWatcher::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		if (!a_event)
			return RE::BSEventNotifyControl::kContinue;

		if (!bInitDataCalled && a_event->opening) {
			if (auto ui{ RE::UI::GetSingleton() }) {
				if (auto menu{ ui->GetMenu(RE::RaceSexMenu::MENU_NAME) }) {
					if (auto movie{ menu->uiMovie.get() }) {
						const auto scaleform = SKSE::GetScaleformInterface();
						movie->Invoke("_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.InitData", nullptr, nullptr, 0);
						bInitDataCalled = true;
						logger::info("InitData called for RaceSexMenu");
						//bGetJSONDataRegistered = true;
					}
				}
			}
		}

		// Send class/trait mod events when menu is closing
		if (!a_event->opening) {
			if (const auto raceMenuInjector{ RaceMenu::GetSingleton() }) {
				if (raceMenuInjector->onItemPressHandler) {
                    raceMenuInjector->onItemPressHandler->SendClassTraitModEvents();
				}
			}
			bInitDataCalled = false;  // Reset for next time
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	void VanillaMenuWatcher::Register()
	{
		if (auto ui{ RE::UI::GetSingleton() }) {
			if ( auto dataHandler { RE::TESDataHandler::GetSingleton()}) {
				
				// Separate out RaceMenu support
				if (dataHandler->LookupLoadedModByName("RaceMenu.esp")) {
					static RaceMenuWatcher watcher;
					ui->AddEventSink(&watcher);

				}
				else {
					static VanillaMenuWatcher watcher;
					ui->AddEventSink(&watcher);
				}
			}
		}
	}

	RE::BSEventNotifyControl RaceMenuWatcher::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		if (!a_event || a_event->menuName != RE::RaceSexMenu::MENU_NAME)
			return RE::BSEventNotifyControl::kContinue;

		const auto ui = RE::UI::GetSingleton();
		if (!ui)
			return RE::BSEventNotifyControl::kContinue;

		const auto raceMenuInjector = RaceMenu::GetSingleton();
        if (!raceMenuInjector) {
            return RE::BSEventNotifyControl::kContinue;
		}

		if (a_event->opening) {
			if (!bInitDataCalled) {
				if (auto raceSexMenu{ ui->GetMenu(RE::RaceSexMenu::MENU_NAME) }) {
					if (auto movie{ raceSexMenu->uiMovie.get() }) {
						movie->Invoke("_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.InitData", nullptr, nullptr, 0);
						bInitDataCalled = true;
						logger::info("InitData called for RaceMenu");
					}
				}
			}

			if (!raceMenuInjector->categoriesInjected) {
                if (!raceMenuInjector->Install()) {
                    logger::info("Failed to install RaceMenu integration");
				};
			}
		} else {
			// Send class/trait mod events when menu is closing
            if (raceMenuInjector->onItemPressHandler) {
                raceMenuInjector->onItemPressHandler->SendClassTraitModEvents();
                logger::info("Sent RaceMenuEvent");
			}
			bInitDataCalled = false;  // Reset for next time
		}
		
		return RE::BSEventNotifyControl::kContinue;

	}
}