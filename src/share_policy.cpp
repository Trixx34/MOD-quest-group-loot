// SPDX-License-Identifier: GPL-2.0-or-later
#include "share_policy.h"

#include <algorithm>

namespace quest_share {

bool ShouldShare(LootedItem const& item, Policy const& policy)
{
    if (item.ItemClass != kQuestItemClass || item.Count == 0)
        return false;
    return std::find(policy.ExcludedItemIds.begin(), policy.ExcludedItemIds.end(), item.ItemEntry) == policy.ExcludedItemIds.end();
}

std::string FormatShareMessage(std::string const& itemName, std::uint32_t recipients)
{
    return "Shared [" + itemName + "] with " + std::to_string(recipients) + (recipients == 1 ? " party member." : " party members.");
}

} // namespace quest_share
