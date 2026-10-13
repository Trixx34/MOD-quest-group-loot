// Reads the module's settings from the config entries the core sends (Module.quest-group-loot.<Key>, prefix removed).
// A bad value keeps the default and adds a warning. No SDK or core needed (tests/settings_test.cpp).
#pragma once

#include "share_policy.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace quest_share {

// Keys: RangeYards (0..200, default 0 = the core's default), NotifyLooter (0 or 1, default 1), ExcludedItemIds (comma-separated
// item entries, default none).
Policy ParseShareSettings(std::unordered_map<std::string, std::string> const& config, std::vector<std::string>& warnings);

} // namespace quest_share
