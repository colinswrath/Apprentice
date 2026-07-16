#include "events.h"
#include "interface.h"
#include "jsonhandler.h"
#include "racemenu.h"
#include "settings.h"

void Listener(SKSE::MessagingInterface::Message* message) noexcept
{
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        if (auto settingsHandler{ Settings::GetSingleton() }) {
            settingsHandler->LoadSettings();
        }

        Events::RaceSexMenuWatcher::Register();
        JSONHandler::Register();
        RaceMenuHandler::RegisterGetTESGlobal();
    }

}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    REL::Module::reset();

    Init(skse);

    const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
    const auto name{ plugin->GetName() };
    const auto version{ plugin->GetVersion() };

    logger::init();

    logger::info("{} {} is loading...", name, version);

    if (const auto messaging{ SKSE::GetMessagingInterface() }; !messaging->RegisterListener(Listener)) {
        return false;
    }

    logger::info("{} has finished loading.", name);
    logger::info("");

    return true;
}