// Unit tests for ParseShareSettings. No core needed.
#include "share_settings.h"

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

void Defaults()
{
    std::vector<std::string> warnings;
    auto const p = quest_share::ParseShareSettings({}, warnings);
    CHECK(p.RangeYards == 0 && p.NotifyLooter && p.ExcludedItemIds.empty());
    CHECK(warnings.empty());
}

void ReadsValues()
{
    std::vector<std::string> warnings;
    auto const p = quest_share::ParseShareSettings(
        { { "RangeYards", "60" }, { "NotifyLooter", "0" }, { "ExcludedItemIds", "3000, 3001 ,3002" } }, warnings);
    CHECK(p.RangeYards == 60);
    CHECK(!p.NotifyLooter);
    CHECK(p.ExcludedItemIds.size() == 3 && p.ExcludedItemIds[0] == 3000 && p.ExcludedItemIds[2] == 3002);
    CHECK(warnings.empty());
}

void BadValuesWarnAndKeepDefaults()
{
    std::vector<std::string> warnings;
    auto const p = quest_share::ParseShareSettings(
        { { "RangeYards", "250" }, { "NotifyLooter", "yes" }, { "ExcludedItemIds", "3000,abc" } }, warnings);
    CHECK(p.RangeYards == 0);                              // above 200: the default
    CHECK(p.NotifyLooter);                                 // not 0 or 1: the default
    CHECK(p.ExcludedItemIds.size() == 1 && p.ExcludedItemIds[0] == 3000); // the good entry is kept
    CHECK(warnings.size() == 3);
}

void RangeBoundsAreInclusive()
{
    std::vector<std::string> warnings;
    CHECK(quest_share::ParseShareSettings({ { "RangeYards", "200" } }, warnings).RangeYards == 200);
    CHECK(quest_share::ParseShareSettings({ { "RangeYards", "0" } }, warnings).RangeYards == 0);
    CHECK(warnings.empty());
}

} // namespace

int main()
{
    Defaults();
    ReadsValues();
    BadValuesWarnAndKeepDefaults();
    RangeBoundsAreInclusive();

    if (failures == 0)
        std::printf("settings_test: all checks passed\n");
    return failures == 0 ? 0 : 1;
}
