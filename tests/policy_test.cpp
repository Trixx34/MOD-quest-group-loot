// SPDX-License-Identifier: GPL-2.0-or-later
// Unit tests for the share policy. No core needed.
#include "share_policy.h"

#include <cstdio>

namespace {

int failures = 0;

#define CHECK(cond)                                                    \
    do {                                                               \
        if (!(cond)) {                                                 \
            ++failures;                                                \
            std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        }                                                              \
    } while (0)

quest_share::LootedItem Quest(std::uint32_t entry, std::uint32_t count = 1)
{
    quest_share::LootedItem item;
    item.ItemEntry = entry;
    item.ItemClass = quest_share::kQuestItemClass;
    item.Count = count;
    return item;
}

void SharesQuestItems()
{
    quest_share::Policy policy;
    CHECK(quest_share::ShouldShare(Quest(3000), policy));
    CHECK(quest_share::ShouldShare(Quest(3000, 5), policy));
}

void IgnoresOtherItems()
{
    quest_share::Policy policy;
    quest_share::LootedItem grey = Quest(2589);
    grey.ItemClass = 7; // trade goods
    CHECK(!quest_share::ShouldShare(grey, policy));
    CHECK(!quest_share::ShouldShare(Quest(3000, 0), policy)); // nothing looted
}

void ExcludedItemsAreNotShared()
{
    quest_share::Policy policy;
    policy.ExcludedItemIds = { 3000, 3001 };
    CHECK(!quest_share::ShouldShare(Quest(3000), policy));
    CHECK(!quest_share::ShouldShare(Quest(3001), policy));
    CHECK(quest_share::ShouldShare(Quest(3002), policy));
}

void MessageNamesTheItemAndTheCount()
{
    CHECK(quest_share::FormatShareMessage("Wolf Pelt", 2) == "Shared [Wolf Pelt] with 2 party members.");
    CHECK(quest_share::FormatShareMessage("Wolf Pelt", 1) == "Shared [Wolf Pelt] with 1 party member.");
}

} // namespace

int main()
{
    SharesQuestItems();
    IgnoresOtherItems();
    ExcludedItemsAreNotShared();
    MessageNamesTheItemAndTheCount();

    if (failures == 0)
        std::printf("policy_test: all checks passed\n");
    return failures == 0 ? 0 : 1;
}
