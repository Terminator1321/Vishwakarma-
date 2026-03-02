#ifndef EWL_HPP
#define EWL_HPP

#include <queue>
#include <string>
#include <chrono>

class EWL
{
public:
    enum class Type
    {
        Error,
        Warning,
        Log
    };

    struct Entry
    {
        Type type;
        std::string message;
        std::chrono::system_clock::time_point timestamp;
    };

private:
    std::queue<Entry> timeline;

    EWL() = default;

public:
    EWL(const EWL&) = delete;
    EWL& operator=(const EWL&) = delete;

    static EWL& Get();

    void AddError(const std::string& msg);
    void AddWarning(const std::string& msg);
    void AddLog(const std::string& msg);

    const std::queue<Entry>& GetTimeline() const;
};

#endif