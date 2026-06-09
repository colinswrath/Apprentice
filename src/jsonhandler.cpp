#pragma once

#include "utils.h"
#include "jsonhandler.h"
#include <filesystem>

namespace JSONHandler
{
    /* Core JSON loader, using nlohmann::json */
    json GetJSONData::LoadJSON(std::string filename)
    {
        std::ifstream file(filename);

        if (!file.is_open()) {
            logger::info("Failed to finFd {}", filename);
            return json::array();
        }

        json jf = json::parse(file);
        return jf;
    }

    /* Scaleform Callback */
    void GetJSONData::Call(Params& params)
    {
        assert(params.argCount >= 1);
        const auto filename = params.args[0].GetString();

        auto jsonArray = LoadJSON(filename);
        logger::info("Loaded {} with {} elements", filename, jsonArray.size());

        RE::GFxValue* retVal = params.retVal;
        RE::GFxMovie* movie  = params.movie;

        movie->CreateArray(retVal);
        for (auto& element : jsonArray) {
            RE::GFxValue elementObj;
            movie->CreateObject(&elementObj);

            Utils::RegisterString(&elementObj, params.movie, "Name", element["Name"].get<std::string>());
            Utils::RegisterString(&elementObj, params.movie, "Description", element["Description"].get<std::string>());
            Utils::RegisterString(&elementObj, params.movie, "UniqueKey", element["UniqueKey"].get<std::string>());

            retVal->PushBack(elementObj);
        }
    }

    /* Install Scaleform Callback*/
    bool Install(RE::GFxMovieView* a_view, RE::GFxValue*)
    {
        RE::GFxValue globals;
        std::string  swfName = a_view->GetMovieDef()->GetFileURL();

        bool result = a_view->GetVariable(&globals, "_global");
        if (result && swfName == "Interface/RaceSex_menu.swf") {
            RE::GFxValue LAM;
            a_view->CreateObject(&LAM);

            RE::GFxValue fnValue;

            static GetJSONData* getJSONData;

            a_view->CreateFunction(&fnValue, getJSONData);
            LAM.SetMember("GetJSONData", fnValue);

            globals.SetMember("LAM", LAM);
            logger::info("JSON Getter installed.");
            return true;
        }
        return false;
    }

    /* Register loader in Scaleform interface*/
    void Register()
    {
        if (const auto scaleform{ SKSE::GetScaleformInterface() }) {
            scaleform->Register(Install, "LAM");
        }
    }

} // namespace JSONHandler
