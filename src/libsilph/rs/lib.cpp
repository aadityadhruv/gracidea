#include "lib.h"
#include "core.h"

namespace rs {
    Savefile::Savefile(const std::string& path) {
        const file_raw raw = file_raw::load_save_file(path);
        this->raw_source = raw;
    }

    void Savefile::save(std::string path) {
        this->raw_source.save_file();
    }
}
