#pragma once
#include <string>
#include <vector>
#include <utility>

namespace BPPresets
{
    // A preset is a name, a genre category, and a set of parameter overrides
    // (real, un-normalized values). Applying a preset resets all parameters to
    // their defaults, then applies these overrides — so any parameter not
    // listed takes its default value.
    struct Preset
    {
        std::string name;
        std::string category;
        std::string description;
        std::vector<std::pair<std::string, float>> params;
    };

    // The full ordered list of 200 genre presets (generated in Presets.cpp).
    const std::vector<Preset>& getAll();
}
