#include "lib.h"

namespace rs {
    Savefile::Savefile(const std::string& path) {
        const file_raw raw = file_raw::load_save_file(path);
        this->raw_source = raw;
    }

    void Savefile::save(std::string path) {
        save_file(this->raw_source);
    }
}