#pragma once

#include "Animations/AnimationFilm.h"
#include <list>
#include <string>

namespace parsers {

// FullParser-compatible function for AnimationFilmHolder::Load()
// Signature matches: bool(std::list<AnimationFilm::Data>& output, const std::string& input)
bool ParseFilmsFromJSON(
    std::list<anim::AnimationFilm::Data>& output,
    const std::string& jsonContent
);

// Convenience function: loads JSON file and populates AnimationFilmHolder
bool LoadFilmsFromFile(const std::string& filePath);

}
