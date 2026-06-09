/**
 * Add RaceMenu support via DLL injection.
 * Class/Trait tabs are added as new categories with an associated filterFlag that corresponds with new entries in the Race list
 * Classes/Traits entries are added to the Race list with a filterFlag corresponding with the above category flag.
 * This lets the system know that Category X with filterFlag Y should only show entries from the Race list with filterFlag Y.
 */
#pragma once

namespace RaceMenuHandler
{
    class OnItemPressHandler;
    class OnSelectionChangeHandler;

    constexpr int ENTRY_TYPE_CAT    = 0;
    constexpr int ENTRY_TYPE_RACE   = 1;
    constexpr int ENTRY_TYPE_SLIDER = 2;

    class RaceMenu : public Singleton<RaceMenu>
    {
    public:
        RE::GFxValue raceSexPanelsInstance;
        RE::GFxValue racePanel;
        RE::GFxValue categoryList;
        RE::GFxValue itemList;

        RE::GFxValue slidingCategoryList;
        RE::GFxValue raceListEntryList;
        RE::GFxValue categoryListEntryList;

        RE::GPtr<RE::GFxMovieView>         raceSexMovie = nullptr;
        RE::GPtr<OnItemPressHandler>       onItemPressHandler = nullptr;
        RE::GPtr<OnSelectionChangeHandler> onSelectionChangeHandler = nullptr;

        RE::GFxValue traitLabel;
        RE::GFxValue traitValue;
        RE::GFxValue classLabel;
        RE::GFxValue classValue;

        i32  numNewCols         = 2;
        bool categoriesInjected = false;
        bool uiElementsCreated  = false;

        bool Install();

        bool PopulateRaceList();
        bool PopulateCategoryList();

        bool ReplaceEntryPressHandler();
        bool ReplaceSelectionChangeHandler();

        RE::GFxValue BuildCategoryEntry(RE::GFxValue* a_entry, i32 typeInt, i32 filterInt, std::string textString, i32 flagInt, i32 priorityInt);
        RE::GFxValue BuildListEntry(RE::GFxValue* a_entry, int type, std::string text, int filterFlag, std::string raceDesc, int equipState, int raceID, std::string callbackName,
                                    bool isClass, bool isTrait);
        bool         CreateClassTraitUIElements();

    private:
    };

    class OnItemPressHandler : public RE::GFxFunctionHandler
    {
    public:
        RE::GFxValue raceSexPanelsInstance;
        RE::GFxValue racePanel;
        RE::GFxValue itemList;
        RE::GFxValue itemListEntryList;
        RE::GFxValue playerInfoMc;
        RE::GFxValue raceDescriptionMovie;
        RE::GFxValue raceDescriptionField;

        RE::GFxValue _onItemPress;
        RE::GFxValue _updateBottomBar;
        RE::GFxValue _requestUpdate;
        RE::GFxValue loadingIcon;
        bool         isInstalled = false;

        RE::GFxValue traitValue;
        RE::GFxValue classValue;

        // Track selected class and trait callbacks for mod events
        std::string selectedClassCallback;
        std::string selectedTraitCallback;

        bool uiElementsCreated = false;

        OnItemPressHandler(const RE::GFxValue& a_orig) : _onItemPress(a_orig) {}

        void Install();
        void Call(Params& a_params) override;
        void SendClassTraitModEvents();
        void UpdateClassTraitDisplay(const std::string& classText, const std::string& traitText);
    };

    class OnSelectionChangeHandler : public RE::GFxFunctionHandler
    {
    public:
        RE::GFxValue raceSexPanelsInstance;
        RE::GFxValue racePanel;
        RE::GFxValue raceListEntryList;
        RE::GFxValue raceDescription;
        RE::GFxValue itemList;

        RE::GFxValue _showRaceDescription;
        RE::GFxValue _onSelectionChange;
        bool         isInstalled = false;

        OnSelectionChangeHandler(const RE::GFxValue& a_orig) : _onSelectionChange(a_orig) {}

        void Install();
        void Call(Params& a_params) override;
    };
} // namespace RaceMenuHandler
