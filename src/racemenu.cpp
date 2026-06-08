#include "racemenu.h"

#include <array>

#include "RE/F/FxDelegate.h"
#include "RE/F/FxResponseArgs.h"
#include "jsonhandler.h"
#include "utils.h"

i32 classFlag = 1u << 29;
i32 traitFlag = 1u << 30;

bool RaceMenu::Install()
{
    /*
    */
    if (const auto ui{ RE::UI::GetSingleton() }) {
        if (const auto menu{ ui->GetMenu(RE::RaceSexMenu::MENU_NAME) }) {
            if (const auto a_movie{ menu->uiMovie }) {
                if (const auto scaleformInterface{ SKSE::GetScaleformInterface() }) {
                    raceSexMovie = a_movie;

                    raceSexMovie->GetVariable(&racePanel, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel");
                    raceSexMovie->GetVariable(&raceSexPanelsInstance, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance");
                    raceSexMovie->GetVariable(&raceListEntryList, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel.itemList.entryList");
                    raceSexMovie->GetVariable(&categoryListEntryList, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel.slidingCategoryList.categoryList.entryList");

                    PopulateCategoryList();
                    PopulateRaceList();
                    ReplaceEntryPressHandler();
                    ReplaceSelectionChangeHandler();
                    CreateClassTraitUIElements();
                    return true;
                }
            }
        }
    }
    return false;
}

bool RaceMenu::PopulateRaceList()
{
    const u32    size        = raceListEntryList.GetArraySize();
    GetJSONData* getJSONData = nullptr;

    getJSONData = new GetJSONData;

    auto      classEntryJSON = getJSONData->LoadJSON("Data/SKSE/Plugins/app_classes.json");
    auto      traitEntryJSON = getJSONData->LoadJSON("Data/SKSE/Plugins/app_traits.json");
    const u32 newSize        = size + classEntryJSON.size() + traitEntryJSON.size();

    // Expand RaceList by numClassSize + numTraitSize
    raceListEntryList.SetArraySize(newSize);

    // Add Class entries
    for (auto i = 0; i < classEntryJSON.size(); i++) {
        RE::GFxValue classEntry;

        json el = classEntryJSON[i];

        raceSexMovie->CreateObject(&classEntry);

        // Populate the classEntry
        BuildListEntry(&classEntry, ENTRY_TYPE_RACE, el["Name"], classFlag, el["Description"], 0, i, el["UniqueKey"], true, false);

        // Set the classEntry in the entryList
        raceListEntryList.SetElement(size + i, classEntry);
    }

    // Add Trait entries
    for (auto i = 0; i < traitEntryJSON.size(); i++) {
        RE::GFxValue traitEntry;

        json el = traitEntryJSON[i];

        raceSexMovie->CreateObject(&traitEntry);

        // Populate the traitEntry
        BuildListEntry(&traitEntry, ENTRY_TYPE_RACE, el["Name"], traitFlag, el["Description"], 0, 0, el["UniqueKey"], false, true);

        // Set the traitEntry in the entryList
        raceListEntryList.SetElement(size + classEntryJSON.size() + i, traitEntry);
    }

    logger::info("Class and Trait entryList Injected");

    return true;
}

bool RaceMenu::PopulateCategoryList()
{
    const u32 size    = categoryListEntryList.GetArraySize();
    const u32 newSize = size + numNewCols;

    categoryListEntryList.SetArraySize(newSize);

    RE::GFxValue classEntry;
    RE::GFxValue traitEntry;

    raceSexMovie->CreateObject(&classEntry);
    raceSexMovie->CreateObject(&traitEntry);

    BuildCategoryEntry(&classEntry, 0, 1, "CLASS", classFlag, -970);
    BuildCategoryEntry(&traitEntry, 0, 1, "TRAIT", traitFlag, -965);

    categoryListEntryList.SetElement(newSize - 2, classEntry);
    categoryListEntryList.SetElement(newSize - 1, traitEntry);

    logger::info("CategoryList Injected");

    categoriesInjected = true;
    return true;
}

bool RaceMenu::ReplaceEntryPressHandler()
{
    if (onItemPressHandler == nullptr) {
        // Remove RaceMenu onItemPress listener
        RE::GFxValue result;
        RE::GFxValue argsRemove[3];
        argsRemove[0].SetString("itemPress");
        argsRemove[1] = raceSexPanelsInstance;
        argsRemove[2].SetString("onItemPress");
        raceSexPanelsInstance.Invoke("removeEventListener", &result, argsRemove, 3);
        logger::info("Removed Event Listener");

        // Stash the original
        RE::GFxValue _onItemPress;
        raceSexPanelsInstance.GetMember("onItemPress", &_onItemPress);
        raceSexPanelsInstance.SetMember("__onItemPress", _onItemPress);
        RE::GFxValue onItemPress = nullptr;

        onItemPressHandler = RE::make_gptr<OnItemPressHandler>(_onItemPress);

        raceSexMovie->CreateFunction(&onItemPress, onItemPressHandler.get());
        raceSexPanelsInstance.SetMember("onItemPress", onItemPress);

        // Replace with new onItemPress listener
        RE::GFxValue argsAdd[3];
        argsAdd[0].SetString("itemPress");
        argsAdd[1] = raceSexPanelsInstance;
        argsAdd[2].SetString("onItemPress");
        raceSexPanelsInstance.Invoke("addEventListener", &result, argsAdd, 3);
        logger::info("Readded new Event Listener");

        return true;
    }
    return false;
}

bool RaceMenu::ReplaceSelectionChangeHandler()
{
    if (onSelectionChangeHandler == nullptr) {
        // Remove RaceMenu onSelectionChangeHandler listener
        RE::GFxValue result;
        RE::GFxValue argsRemove[3];
        argsRemove[0].SetString("selectionChange");
        argsRemove[1] = raceSexPanelsInstance;
        argsRemove[2].SetString("onSelectionChange");
        raceSexPanelsInstance.Invoke("removeEventListener", &result, argsRemove, 3);
        logger::info("Removed Event Listener");

        // Stash the original
        RE::GFxValue _onSelectionChange;
        raceSexPanelsInstance.GetMember("onSelectionChange", &_onSelectionChange);
        raceSexPanelsInstance.SetMember("__onSelectionChange", _onSelectionChange);
        RE::GFxValue onSelectionChange = nullptr;

        onSelectionChangeHandler = RE::make_gptr<OnSelectionChangeHandler>(_onSelectionChange);

        raceSexMovie->CreateFunction(&onSelectionChange, onSelectionChangeHandler.get());
        raceSexPanelsInstance.SetMember("onSelectionChange", onSelectionChange);

        // Replace with new onSelectionChange listener
        RE::GFxValue argsAdd[3];
        argsAdd[0].SetString("selectionChange");
        argsAdd[1] = raceSexPanelsInstance;
        argsAdd[2].SetString("onSelectionChange");
        raceSexPanelsInstance.Invoke("addEventListener", &result, argsAdd, 3);
        logger::info("Readded new Event Listener");

        return true;
    }
    return false;
}

RE::GFxValue RaceMenu::BuildCategoryEntry(RE::GFxValue* a_entry, i32 typeInt, i32 filterInt, std::string textString, i32 flagInt, i32 priorityInt)
{
    AddBooleanMember(a_entry, false, "bDontHide");
    AddIntMember(a_entry, filterInt, "filterFlag");
    AddStringMember(a_entry, textString.data(), "text");
    AddIntMember(a_entry, flagInt, "flag");
    AddIntMember(a_entry, priorityInt, "priority");
    AddBooleanMember(a_entry, true, "enabled");

    return a_entry;
}

RE::GFxValue RaceMenu::BuildListEntry(RE::GFxValue* a_entry, int type, std::string text, int filterFlag, std::string raceDesc, int equipState, int raceID, std::string callbackName,
                                      bool isClass, bool isTrait)
{
    AddIntMember(a_entry, type, "type");
    AddStringMember(a_entry, text.data(), "text");
    AddIntMember(a_entry, filterFlag, "filterFlag");
    AddStringMember(a_entry, callbackName.data(), "callbackName");
    AddStringMember(a_entry, raceDesc.data(), "raceDescription");
    AddIntMember(a_entry, equipState, "equipState");
    AddIntMember(a_entry, raceID, "raceID");
    AddBooleanMember(a_entry, true, "enabled");
    AddBooleanMember(a_entry, isClass, "isClass");
    AddBooleanMember(a_entry, isTrait, "isTrait");

    return a_entry;
}

void OnItemPressHandler::Install()
{
    if (const auto ui{ RE::UI::GetSingleton() }) {
        if (const auto menu{ ui->GetMenu(RE::RaceSexMenu::MENU_NAME) }) {
            if (const auto a_movie{ menu->uiMovie }) {
                a_movie->GetVariable(&raceSexPanelsInstance, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance");
                a_movie->GetVariable(&itemListEntryList, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel.itemList.entryList");
                a_movie->GetVariable(&racePanel, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel");
                a_movie->GetVariable(&playerInfoMc, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.bottomBar.playerInfo");
                a_movie->GetVariable(&raceDescriptionMovie, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel.raceDescription");

                // Cache useful members
                racePanel.GetMember("itemList", &itemList);
                racePanel.GetMember("updateBottomBar", &_updateBottomBar);
                // raceDescriptionMovie.GetMember("textField", &raceDescriptionField);
                itemList.GetMember("requestUpdate", &_requestUpdate);
                isInstalled = true;

                playerInfoMc.GetMember("TraitValue", &traitValue);
                playerInfoMc.GetMember("ClassValue", &classValue);
            }
        }
    }
}

void OnItemPressHandler::Call(Params& a_params)
{
    assert(a_params.argCount >= 1);
    auto& event = a_params.args[0];

    if (!isInstalled) {
        Install();
        logger::info("OnItemPressHandler Installed");
    }

    RE::GFxValue listState;
    itemList.GetMember("listState", &listState);

    RE::GFxValue activeEntry;
    listState.GetMember("activeEntry", &activeEntry);

    RE::GFxValue indexVal;
    event.GetMember("index", &indexVal);
    i32 index = static_cast<i32>(indexVal.GetNumber());

    RE::GFxValue pressedEntry;
    itemListEntryList.GetElement(index, &pressedEntry);

    RE::GFxValue typeVal;
    pressedEntry.GetMember("type", &typeVal);
    i32 type = static_cast<i32>(typeVal.GetNumber());

    RE::GFxValue result;

    bool isClass = GetBooleanMember(pressedEntry, "isClass");
    bool isTrait = GetBooleanMember(pressedEntry, "isTrait");

    // Check if Class/Trait entry
    if (IsClassOrTrait(pressedEntry)) {
        // Store the callback for later mod event dispatch
        if (pressedEntry.HasMember("callbackName")) {
            RE::GFxValue callbackVal;
            pressedEntry.GetMember("callbackName", &callbackVal);
            std::string callback = callbackVal.GetString();
            logger::info("callback {}", callback);
            if (isClass) {
                selectedClassCallback = callback;
                logger::info("Selected Class callback: {}", callback);
            }
            else if (isTrait) {
                selectedTraitCallback = callback;
                logger::info("Selected Trait callback: {}", callback);
            }
        }

        // Update the UI display with the selected name
        RE::GFxValue nameVal;
        pressedEntry.GetMember("text", &nameVal);
        std::string selectedName = nameVal.GetString();

        if (isClass) {
            UpdateClassTraitDisplay(selectedName, "");
        }
        else if (isTrait) {
            UpdateClassTraitDisplay("", selectedName);
        }

        listState.SetMember("activeEntry", pressedEntry);

        itemList.Invoke("requestUpdate", &result, nullptr, 0);
        racePanel.Invoke("updateBottomBar", &result, nullptr, 0);

        RE::GFxValue sliderID;
        double       sliderIDVal = 0.0;
        if (pressedEntry.GetMember("sliderID", &sliderID)) {
            sliderIDVal = sliderID.GetNumber();
            logger::info("SliderID");
        }

        RE::GFxValue raceID;
        double       raceIDVal = 0.0;
        
        if (pressedEntry.GetMember("raceID", &raceID)) {
            raceIDVal = raceID.GetNumber();
            logger::info("RaceID");
        }


        RE::GFxValue result;
        RE::GFxValue argsShow[2];
        argsShow[0].SetNumber(raceIDVal);
        argsShow[1].SetNumber(sliderIDVal);
        argsShow[0].SetBoolean(true);

        raceSexPanelsInstance.Invoke("ChangeRace", &result, argsShow, 0);
        return;
    }

    raceSexPanelsInstance.Invoke("__onItemPress", &result, a_params.args, a_params.argCount);
    logger::info("Delegated to original onItemPress");
}

void OnItemPressHandler::SendClassTraitModEvents()
{
    // Send mod events for selected class and trait
    if (!selectedClassCallback.empty()) {
        SendModEvent("ClassMenu_Callback", selectedClassCallback, 1.0f);
        logger::info("Sent ClassMenu_Callback mod event with value: {}", selectedClassCallback);
    }
    if (!selectedTraitCallback.empty()) {
        SendModEvent("TraitMenu_Callback", selectedTraitCallback, 1.0f);
        logger::info("Sent TraitMenu_Callback mod event with value: {}", selectedTraitCallback);
    }
}

bool RaceMenu::CreateClassTraitUIElements()
{
    if (!raceSexMovie) {
        return false;
    }

    RE::GFxValue playerInfoMc;
    if (!raceSexMovie->GetVariable(&playerInfoMc, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.bottomBar.playerInfo")) {
        logger::warn("Could not get PlayerInfo_mc");
        return false;
    }

    // Pull existing BottomBar fields to duplicate from
    RE::GFxValue playerNameLabel;
    if (!raceSexMovie->GetVariable(&playerNameLabel, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.bottomBar.playerInfo.NameLabel")) {
        logger::warn("Could not get PlayerNameLabel TextField for reference");
        return false;
    }

    RE::GFxValue playerNameValue;
    if (!raceSexMovie->GetVariable(&playerNameValue, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.bottomBar.playerInfo.PlayerName")) {
        logger::warn("Could not get PlayerNameValue TextField for reference");
        return false;
    }

    RE::GFxValue xPos;
    RE::GFxValue yPos;
    RE::GFxValue width;
    RE::GFxValue height;
    RE::GFxValue depth;
    RE::GFxValue textFormat;

    // Create Trait Label
    playerNameLabel.GetMember("_x", &xPos);
    playerNameLabel.GetMember("_y", &yPos);
    playerNameLabel.GetMember("_width", &width);
    playerNameLabel.GetMember("_height", &height);
    playerNameLabel.Invoke("getTextFormat", &textFormat, nullptr, 0);
    playerInfoMc.Invoke("getNextHighestDepth", &depth, nullptr, 0);

    RE::GFxValue argsCreate[6];
    argsCreate[0].SetString("TraitLabel");
    argsCreate[1].SetNumber(depth.GetNumber() + 10);
    argsCreate[2].SetNumber(xPos.GetNumber() - 150);
    argsCreate[3].SetNumber(yPos.GetNumber());
    argsCreate[4].SetNumber(width.GetNumber());
    argsCreate[5].SetNumber(height.GetNumber());
    playerInfoMc.Invoke("createTextField", nullptr, argsCreate, 6);

    playerInfoMc.GetMember("TraitLabel", &traitLabel);
    AddStringMember(&traitLabel, "TRAIT", "text");

    RE::GFxValue fmtArgs[1];
    fmtArgs[0] = textFormat;
    traitLabel.Invoke("setTextFormat", nullptr, fmtArgs, 1);
    traitLabel.Invoke("setNewTextFormat", nullptr, fmtArgs, 1);

    // Create Trait Value
    playerNameValue.GetMember("_x", &xPos);
    playerNameValue.GetMember("_y", &yPos);
    playerNameValue.GetMember("_width", &width);
    playerNameValue.GetMember("_height", &height);
    playerNameValue.Invoke("getTextFormat", &textFormat, nullptr, 0);
    playerInfoMc.Invoke("getNextHighestDepth", &depth, nullptr, 0);

    argsCreate[0].SetString("TraitValue");
    argsCreate[1].SetNumber(depth.GetNumber() + 10);
    argsCreate[2].SetNumber(xPos.GetNumber() - 150);
    argsCreate[3].SetNumber(yPos.GetNumber());
    argsCreate[4].SetNumber(width.GetNumber());
    argsCreate[5].SetNumber(height.GetNumber());
    playerInfoMc.Invoke("createTextField", nullptr, argsCreate, 6);

    playerInfoMc.GetMember("TraitValue", &traitValue);
    AddStringMember(&traitValue, "", "text");

    fmtArgs[0] = textFormat;
    traitValue.Invoke("setTextFormat", nullptr, fmtArgs, 1);
    traitValue.Invoke("setNewTextFormat", nullptr, fmtArgs, 1);

    // Create Class Label
    playerNameLabel.GetMember("_x", &xPos);
    playerNameLabel.GetMember("_y", &yPos);
    playerNameLabel.GetMember("_width", &width);
    playerNameLabel.GetMember("_height", &height);
    playerNameLabel.Invoke("getTextFormat", &textFormat, nullptr, 0);
    playerInfoMc.Invoke("getNextHighestDepth", &depth, nullptr, 0);

    argsCreate[0].SetString("ClassLabel");
    argsCreate[1].SetNumber(depth.GetNumber() + 11);
    argsCreate[2].SetNumber(xPos.GetNumber() - 300);
    argsCreate[3].SetNumber(yPos.GetNumber());
    argsCreate[4].SetNumber(width.GetNumber());
    argsCreate[5].SetNumber(height.GetNumber());
    playerInfoMc.Invoke("createTextField", nullptr, argsCreate, 6);

    playerInfoMc.GetMember("ClassLabel", &classLabel);
    AddStringMember(&classLabel, "CLASS", "text");

    fmtArgs[0] = textFormat;
    classLabel.Invoke("setTextFormat", nullptr, fmtArgs, 1);
    classLabel.Invoke("setNewTextFormat", nullptr, fmtArgs, 1);

    // Create Class Value
    playerNameValue.GetMember("_x", &xPos);
    playerNameValue.GetMember("_y", &yPos);
    playerNameValue.GetMember("_width", &width);
    playerNameValue.GetMember("_height", &height);
    playerNameValue.Invoke("getTextFormat", &textFormat, nullptr, 0);
    playerInfoMc.Invoke("getNextHighestDepth", &depth, nullptr, 0);

    argsCreate[0].SetString("ClassValue");
    argsCreate[1].SetNumber(depth.GetNumber() + 10);
    argsCreate[2].SetNumber(xPos.GetNumber() - 300);
    argsCreate[3].SetNumber(yPos.GetNumber());
    argsCreate[4].SetNumber(width.GetNumber());
    argsCreate[5].SetNumber(height.GetNumber());
    playerInfoMc.Invoke("createTextField", nullptr, argsCreate, 6);

    playerInfoMc.GetMember("ClassValue", &classValue);
    AddStringMember(&classValue, "", "text");

    fmtArgs[0] = textFormat;
    classValue.Invoke("setTextFormat", nullptr, fmtArgs, 1);
    classValue.Invoke("setNewTextFormat", nullptr, fmtArgs, 1);

    uiElementsCreated = true;
    logger::info("Class/Trait UI elements created successfully");

    return true;
}

void OnItemPressHandler::UpdateClassTraitDisplay(const std::string& classText, const std::string& traitText)
{
    if (!classText.empty()) {
        AddStringMember(&classValue, classText.c_str(), "text");
        logger::info("Updated Class display to: {}", classText);
    }

    if (!traitText.empty()) {
        AddStringMember(&traitValue, traitText.c_str(), "text");
        logger::info("Updated Trait display to: {}", traitText);
    }
}

void OnSelectionChangeHandler::Install()
{
    if (const auto ui{ RE::UI::GetSingleton() }) {
        if (const auto menu{ ui->GetMenu(RE::RaceSexMenu::MENU_NAME) }) {
            if (const auto a_movie{ menu->uiMovie }) {
                a_movie->GetVariable(&raceSexPanelsInstance, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance");
                a_movie->GetVariable(&racePanel, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel");
                a_movie->GetVariable(&raceListEntryList, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel.itemList.entryList");
                a_movie->GetVariable(&raceDescription, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.raceDescription.textField");

                // Cache useful
                racePanel.GetMember("itemList", &itemList);
                
                isInstalled = true;
                logger::info("OnSelectionChangeHandler Installed");
            }
        }
    }
}

void OnSelectionChangeHandler::Call(Params& a_params)
{
    assert(a_params.argCount >= 1);
    auto& event = a_params.args[0];

    Install();

    RE::GFxValue result;
    raceSexPanelsInstance.Invoke("__onSelectionChange", &result, a_params.args, a_params.argCount);

    RE::GFxValue indexVal;
    event.GetMember("index", &indexVal);
    i32 index = static_cast<i32>(indexVal.GetNumber());

    RE::GFxValue listState;
    if (!itemList.GetMember("listState", &listState)) {
        logger::info("no listState");
        return;
    }
    logger::info("Got listState");

    //RE::GFxValue selectedEntry;
    //if (!listState.GetMember("selectedEntry", &selectedEntry)) {
    //    logger::info("no selectedEntry");
    //    return;
    //}

    RE::GFxValue selectedEntry;
    if (!raceListEntryList.GetElement(index, &selectedEntry)) {
        logger::info("no entry element");
        return;
    }

    logger::info("Got selectedEntry");

    RE::GFxValue catFlag;
    if (selectedEntry.GetMember("flag", &catFlag)) {
        logger::info("Category entry, continue");
        return;
    }

    RE::GFxValue filterFlag;
    if (!selectedEntry.GetMember("filterFlag", &filterFlag)) {
        logger::info("No filterFlag");
        return;
    }

    i32 filterFlagVal = GetIntMember(selectedEntry, "filterFlag");

    if (filterFlagVal > 0) {

        if ((filterFlagVal & classFlag) || (filterFlagVal & traitFlag)) {
            std::string_view descriptionVal = GetStringMember(selectedEntry, "raceDescription");
            raceDescription.SetText(descriptionVal.data());

            RE::GFxValue result;
            RE::GFxValue argsShow[1];
            argsShow[0].SetBoolean(true);
            raceSexPanelsInstance.Invoke("ShowRaceDescription", &result, argsShow, 1);
        }
        else {
            logger::info("No Class/Trait");
        }
    }
}
