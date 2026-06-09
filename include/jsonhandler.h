/**
 * Add JSON handling to SWF files. Allows easy-to-expand classes/traits, complex characters in descriptions
 *
 */
#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace JSONHandler
{
    class GetJSONData : public RE::GFxFunctionHandler
    {
    public:
        json LoadJSON(std::string filename);
        void Call(Params& params) override;
    };

    bool Install(RE::GFxMovieView*, RE::GFxValue*);
    void Register();
}