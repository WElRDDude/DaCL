#include "overlay.hpp"
#include <cstdlib>
#include <sstream>

void Overlay::overlay_text(const std::string& video_file, const std::string& text, const std::string& output_file) {
    // Use ffmpeg to overlay text as a demonstration
    std::ostringstream cmd;
    cmd << "ffmpeg -i " << video_file
        << " -vf \"drawtext=text='" << text << "':x=10:y=10:fontsize=24:fontcolor=white\""
        << " -codec:a copy " << output_file << " -y";
    std::system(cmd.str().c_str());
}