#include "Utilities/FilmParser.h"
#include "Animations/AnimationFilmHolder.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>

namespace parsers {

bool ParseFilmsFromJSON(
    std::list<anim::AnimationFilm::Data>& output,
    const std::string& jsonContent)
{
    try
    {
        auto json = nlohmann::json::parse(jsonContent);

        if (!json.contains("films") || !json["films"].is_array())
            return false;

        for (const auto& film : json["films"])
        {
            anim::AnimationFilm::Data data;
            data.id = film["id"].get<std::string>();

            // Prepend ASSETS path to make it a full path
            data.path = std::string(ASSETS) + "/" + film["path"].get<std::string>();

            for (const auto& frame : film["frames"])
            {
                anim::AnimationFilm::FrameData fd;
                fd.rect.x = frame["x"].get<int>();
                fd.rect.y = frame["y"].get<int>();
                fd.rect.w = frame["w"].get<int>();
                fd.rect.h = frame["h"].get<int>();

                // Parse optional display offset for centering
                if (frame.contains("ox"))
                    fd.offset.x = frame["ox"].get<int>();
                if (frame.contains("oy"))
                    fd.offset.y = frame["oy"].get<int>();

                data.frames.push_back(fd);
            }

            // Parse optional color key for transparency
            if (film.contains("colorKey") && film["colorKey"].is_object())
            {
                const auto& ck = film["colorKey"];
                data.colorKey.r = static_cast<gfx::RGBValue>(ck["r"].get<int>());
                data.colorKey.g = static_cast<gfx::RGBValue>(ck["g"].get<int>());
                data.colorKey.b = static_cast<gfx::RGBValue>(ck["b"].get<int>());
                data.colorKey.enabled = true;
            }

            output.push_back(data);
        }

        return true;
    }
    catch (const nlohmann::json::exception&)
    {
        return false;
    }
}

bool LoadFilmsFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
        return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    anim::AnimationFilmHolder::Get().Load(buffer.str(), ParseFilmsFromJSON);

    return true;
}

}
