#pragma once
#include <string>

class Overlay {
public:
    static void overlay_text(const std::string& video_file, const std::string& text, const std::string& output_file);
};