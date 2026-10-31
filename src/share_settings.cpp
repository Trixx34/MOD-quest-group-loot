// SPDX-License-Identifier: GPL-2.0-or-later
#include "share_settings.h"

#include <charconv>
#include <system_error>

namespace quest_share {

namespace {

constexpr std::uint32_t kMaxRangeYards = 200;

// Parses a whole decimal number. Returns false if the text is not one.
bool ParseNumber(std::string const& text, std::uint32_t& value)
{
    std::uint32_t parsed = 0;
    auto const [end, error] = std::from_chars(text.data(), text.data() + text.size(), parsed);
    if (error != std::errc() || end != text.data() + text.size() || text.empty())
        return false;
    value = parsed;
    return true;
}

} // namespace

Policy ParseShareSettings(std::unordered_map<std::string, std::string> const& config, std::vector<std::string>& warnings)
{
    Policy policy;

    if (auto const it = config.find("RangeYards"); it != config.end())
    {
        std::uint32_t value = 0;
        if (ParseNumber(it->second, value) && value <= kMaxRangeYards)
            policy.RangeYards = value;
        else
            warnings.push_back("Module.quest-group-loot.RangeYards: '" + it->second + "' is not a whole number from 0 to 200; using 0.");
    }

    if (auto const it = config.find("NotifyLooter"); it != config.end())
    {
        if (it->second == "0" || it->second == "1")
            policy.NotifyLooter = it->second == "1";
        else
            warnings.push_back("Module.quest-group-loot.NotifyLooter: '" + it->second + "' is not 0 or 1; using 1.");
    }

    if (auto const it = config.find("ExcludedItemIds"); it != config.end())
    {
        // Split on commas; spaces around entries are allowed, empty entries are ignored.
        std::string const& list = it->second;
        std::size_t start = 0;
        while (start <= list.size())
        {
            std::size_t comma = list.find(',', start);
            if (comma == std::string::npos)
                comma = list.size();
            std::string entry = list.substr(start, comma - start);
            entry.erase(0, entry.find_first_not_of(' '));
            entry.erase(entry.find_last_not_of(' ') + 1);
            if (!entry.empty())
            {
                std::uint32_t id = 0;
                if (ParseNumber(entry, id))
                    policy.ExcludedItemIds.push_back(id);
                else
                    warnings.push_back("Module.quest-group-loot.ExcludedItemIds: '" + entry + "' is not an item entry; ignored.");
            }
            start = comma + 1;
        }
    }
    return policy;
}

} // namespace quest_share
