#pragma once

#include "utils.h"

void RegisterString(RE::GFxValue* object, RE::GFxMovie* movie, std::string stringName, std::string stringContents)
{
	RE::GFxValue gfxString;
	movie->CreateString(&gfxString, stringContents.c_str());

	object->SetMember(stringName.c_str(), gfxString);
}

void AddBooleanMember(RE::GFxValue* entry, bool val, std::string_view name) {
	RE::GFxValue member(val);
	entry->SetMember(name.data(), member);
}

void AddStringMember(RE::GFxValue* entry, const char* val, const char* name) {
	RE::GFxValue member(val);
	entry->SetMember(name, member);
}

void AddIntMember(RE::GFxValue* entry, i32 val, std::string_view name) {
	RE::GFxValue member(val);
	entry->SetMember(name.data(), member);
}

void AddNumberMember(RE::GFxValue* entry, double val, std::string_view name) {
    RE::GFxValue member(val);
    entry->SetMember(name.data(), member);
}

void SendModEvent(std::string_view eventName, std::string_view strArg, float numArg, RE::TESForm* sender)
{
    auto* source = SKSE::GetModCallbackEventSource();
    if (!source) {
        return;
    }

    const std::string eventNameStr{ eventName };
    const std::string strArgStr{ strArg };

    SKSE::ModCallbackEvent ev{ RE::BSFixedString(eventNameStr.c_str()), RE::BSFixedString(strArgStr.c_str()), numArg, sender };

    source->SendEvent(&ev);
}

bool GetBooleanMember(RE::GFxValue& obj, const char* memberName) 
{
    if (!obj.HasMember(memberName)) {
        return false;
    }
    RE::GFxValue val;
    obj.GetMember(memberName, &val);
    if (val.GetType() == RE::GFxValue::ValueType::kBoolean) {
        return val.GetBool();
    }
    return false;
};

std::string_view GetStringMember(RE::GFxValue& obj, const char* memberName)
{
    if (!obj.HasMember(memberName)) {
        return "";
    }
    RE::GFxValue val;
    obj.GetMember(memberName, &val);
    if (val.GetType() == RE::GFxValue::ValueType::kString) {
        return val.GetString();
    }
    return "";
};

i32 GetIntMember(RE::GFxValue& obj, const char* memberName)
{
    if (obj.HasMember(memberName)) {
        RE::GFxValue val;
        obj.GetMember(memberName, &val);
        if (val.GetType() == RE::GFxValue::ValueType::kNumber) {
            return val.GetNumber();
        }
    }
    return 0;
};

bool IsClassOrTrait(RE::GFxValue entry)
{
    bool isClass = GetBooleanMember(entry, "isClass");
    bool isTrait = GetBooleanMember(entry, "isTrait");

    return (isClass || isTrait);
}
