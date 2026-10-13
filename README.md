# MOD-quest-group-loot

When a party member loots a quest item, each other party member who still needs it receives a copy.

The module listens for `PlayerScript.OnLootItem`, and for a quest item (item class 12) it asks the core to share it
(`ShareQuestItem`, a Tier 1 action). The core does the checks, so the module cannot give an item to anyone it likes:

- the looter must be in a party, and the item must be a quest item;
- a member gets a copy only on the looter's map, within range (default 100 yards, at most 200), with a quest that still needs
  the item (`Player::HasQuestForItem`) and room in the bags;
- the looter keeps their own copy, and a copy given this way does not fire the loot event again, so there is no loop.

If anyone was given the item, the looter gets a message: `Shared [Item Name] with 2 party members.`

## Needs

The SDK with `ShareQuestItem` (branch `m7-quest-share-sdk`) and a core with the share action (branch `m7-quest-share-core`).

## Configuration

See `config/modules.d/quest-group-loot.conf.example`: `RangeYards` (0 = core default), `NotifyLooter` (default 1), and
`ExcludedItemIds` (default none). A bad value keeps the default and writes a line to the module's stderr.

## Build

    cmake -S . -B build -DFOREVER_MODULE_SDK_SOURCE=<path to forever-module-sdk>
    cmake --build build
    ctest --test-dir build
