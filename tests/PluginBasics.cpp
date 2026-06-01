#include "helpers/test_helpers.h"
#include <PluginProcessor.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE ("Plugin instance", "[instance]")
{
    PluginProcessor testPlugin;

    SECTION ("name")
    {
        CHECK_THAT (testPlugin.getName().toStdString(),
            Catch::Matchers::Equals ("Harmonic Sequencer"));
    }

    SECTION ("midi effect")
    {
        CHECK (testPlugin.acceptsMidi());
        CHECK (testPlugin.producesMidi());
        CHECK (testPlugin.isMidiEffect());
    }
}
