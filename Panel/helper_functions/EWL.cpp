#include "EWL.hpp"

EWL& EWL::Get()
{
    static EWL instance;
    return instance;
}

static EWL::Entry makeEntry(EWL::Type t, const std::string& msg)
{
    return {
        t,
        msg,
        std::chrono::system_clock::now()
    };
}

void EWL::AddError(const std::string& msg)
{
    timeline.push(makeEntry(Type::Error, msg));
}

void EWL::AddWarning(const std::string& msg)
{
    timeline.push(makeEntry(Type::Warning, msg));
}

void EWL::AddLog(const std::string& msg)
{
    timeline.push(makeEntry(Type::Log, msg));
}

const std::queue<EWL::Entry>& EWL::GetTimeline() const
{
    return timeline;
}