// quest-group-loot: when a party member loots a quest item, the other party members who still need it receive a copy.
//
// Usage: quest-group-loot <tier1-endpoint>. The core passes the endpoint when it spawns the module. The module only notices the
// loot and asks; the core finds the party members in range with a quest that needs the item and gives it to them.

#include "share_policy.h"
#include "share_settings.h"

#include "forever_module/module_client.h"

#include <iostream>
#include <unordered_map>
#include <vector>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: quest-group-loot <tier1-endpoint>\n";
        return 1;
    }

    forever_module::ModuleManifest manifest;
    manifest.module_name = "quest-group-loot";
    manifest.module_version = "0.1.0";
    manifest.tier1_hooks = {"PlayerScript.OnLootItem"};

    forever_module::ModuleClient client(std::move(manifest));
    if (!client.Connect(argv[1]) || !client.Handshake())
    {
        std::cerr << "quest-group-loot: could not connect to the core: " << client.rejection_reason() << "\n";
        return 1;
    }

    // Config arrives right after the handshake. A later re-delivery (on the Run() thread) applies to the next loot.
    quest_share::Policy policy;
    client.OnConfig([&policy](std::unordered_map<std::string, std::string> const& config)
    {
        std::vector<std::string> warnings;
        policy = quest_share::ParseShareSettings(config, warnings);
        for (std::string const& warning : warnings)
            std::cerr << "quest-group-loot: config: " << warning << "\n";
    });

    client.OnNotify("PlayerScript.OnLootItem", [&client, &policy](forever_module::EventNotify const& notify)
    {
        forever_module::PlayerItemLootedEvent const& loot = notify.player_item_looted();

        quest_share::LootedItem item;
        item.ItemEntry = loot.item_entry();
        item.ItemClass = loot.item_class();
        item.Count = loot.count();
        if (!quest_share::ShouldShare(item, policy))
            return;

        std::uint64_t const playerGuid = loot.player_guid();
        std::string const itemName = loot.item_name();
        bool const notifyLooter = policy.NotifyLooter;
        client.ShareQuestItem(playerGuid, loot.item_entry(), loot.count(), policy.RangeYards,
                              [&client, playerGuid, itemName, notifyLooter](forever_module::ActionReply const& reply)
        {
            if (!reply.success())
            {
                // A player on their own is the common case, not a fault: only log the others.
                if (reply.reason() != "player is not in a group")
                    std::cerr << "quest-group-loot: could not share " << itemName << ": " << reply.reason() << "\n";
                return;
            }
            if (notifyLooter && reply.recipients() > 0)
                client.SendPlayerMessage(playerGuid, quest_share::FormatShareMessage(itemName, reply.recipients()));
        });
    });

    client.Run();
    return 0;
}
