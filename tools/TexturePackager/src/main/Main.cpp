#include <cstdio>

#ifdef _WIN32
#include <filesystem>
namespace fs = std::filesystem;
#elif __APPLE__
#include <experimental/filesystem>
#include <Atlas.h>

namespace fs = std::experimental::filesystem;
#endif

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Usage: <command> <target-folder>\n");
        return 0;
    }

    std::string targetFolder = std::string(argv[1]);
    while (targetFolder.find_last_of(fs::path::preferred_separator) == (targetFolder.size() - 1)) {
        targetFolder = targetFolder.substr(0, targetFolder.size() - 1);
    }
    targetFolder += fs::path::preferred_separator;

    fs::path target = targetFolder;
    if (!fs::is_directory(target)) {
        printf("Given path is not a directory: '%s'\n", targetFolder.c_str());
        return 0;
    }

    Atlas atlas(targetFolder, targetFolder.substr(0, targetFolder.size() - 1));

    printf("Finding all '.png's in: '%s'\n", targetFolder.c_str());
    bool found = false;
    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(targetFolder)) {
        if (fs::is_regular_file(entry)) {
            if (entry.path().extension() == ".png") {
                std::string file = entry.path().string();
                std::string ref = file.substr(0, file.size() - 4);
                ref = ref.substr(targetFolder.size());
                printf("    %s\n", ref.c_str());
                atlas.addTexture(ref, file);
                found = true;
            }
        }
    }
    if (!found) {
        printf("    None found\n");
        return 0;
    }
    if (!atlas.loadInfo()) {
        printf("Failed: Could not load info\n");
        return 0;
    }
    if (!atlas.packRectangles(4096, 4096)) {
        printf("Failed: Could not pack textures\n");
        return 0;
    }
    if (!atlas.build()) {
        printf("Failed: Could not create output images\n");
        return 0;
    }
    if (!atlas.writeData()) {
        printf("Failed: Could not create output info\n");
        return 0;
    }

    return 0;
}