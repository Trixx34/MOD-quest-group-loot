// SPDX-License-Identifier: GPL-2.0-or-later
// Decides which looted items are shared with the party, and formats the message to the looter. No SDK or core
// needed, so it is unit-tested (tests/policy_test.cpp).
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace quest_share {

// Item class 12 is Quest.
constexpr std::uint32_t kQuestItemClass = 12;

struct Policy
{
    // How far from the looter a party member may be to get a copy. 0 leaves it to the core's default (100 yards); the
    // core allows at most 200.
    std::uint32_t RangeYards = 0;
    // Tell the looter how many party members were given the item.
    bool NotifyLooter = true;
    // Item entries that are never shared.
    std::vector<std::uint32_t> ExcludedItemIds;
};

struct LootedItem
{
    std::uint32_t ItemEntry = 0;
    std::uint32_t ItemClass = 0;
    std::uint32_t Count = 0;
};

// True for a quest item, with a count, that is not excluded. The core decides who gets it (party, range, quest need).
bool ShouldShare(LootedItem const& item, Policy const& policy);

// "Shared [Item Name] with 2 party members."
std::string FormatShareMessage(std::string const& itemName, std::uint32_t recipients);

} // namespace quest_share
