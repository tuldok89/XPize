#include <archive.h>
#include <archive_entry.h>
#include <memory>
#include <stdexcept>
#include <filesystem>

#include "ArchiveExtractor.h"

// Custom Deleters for std::unique_ptr
// These ensure archive_read_free and archive_write_free are called automatically.
struct ArchiveReadDeleter {
    void operator()(struct archive* a) const {
        if (a) {
            archive_read_close(a);
            archive_read_free(a);
        }
    }
};

struct ArchiveWriteDeleter {
    void operator()(struct archive* a) const {
        if (a) {
            archive_write_close(a);
            archive_write_free(a);
        }
    }
};

// Type aliases for cleaner code
using ArchiveReaderPtr = std::unique_ptr<struct archive, ArchiveReadDeleter>;
using ArchiveWriterPtr = std::unique_ptr<struct archive, ArchiveWriteDeleter>;
namespace fs = std::filesystem;

size_t ArchiveExtractor::extract(const wxString& fileName, const wxString& outputDir)
{
    int r;
    size_t count = 0;

    fs::create_directories(outputDir.ToUTF8().data());

    ArchiveReaderPtr a(archive_read_new());
    if (!a) throw std::runtime_error("Failed to create archive reader.");
    archive_read_support_filter_all(a.get());
    archive_read_support_format_all(a.get());

    ArchiveWriterPtr ext(archive_write_disk_new());
    if (!ext) throw std::runtime_error("Failed to create archive writer.");

    int flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_SECURE_NODOTDOT;
    archive_write_disk_set_options(ext.get(), flags);

    r = archive_read_open_filename_w(a.get(), fileName.wc_str(), 10240);
    if (r != ARCHIVE_OK) {
        throw std::runtime_error("Failed to open: " + std::string(archive_error_string(a.get())));
    }

    struct archive_entry* entry;
    while (archive_read_next_header(a.get(), &entry) == ARCHIVE_OK) {
        fs::path fullOutputPath = fs::path(outputDir.wc_str()) / archive_entry_pathname_w(entry);
        archive_entry_set_pathname_utf8(entry, reinterpret_cast<const char*>(fullOutputPath.u8string().c_str()));

        if (archive_write_header(ext.get(), entry) == ARCHIVE_OK) {
            copyData(a.get(), ext.get());
            archive_write_finish_entry(ext.get());
            count++;
        }
    }
    return count;
}

void ArchiveExtractor::copyData(archive* ar, archive* aw)
{
    int r;
    const void* buff;
    size_t size;
    int64_t offset;

    while (true) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF) return;
        if (r < ARCHIVE_OK) throw std::runtime_error(archive_error_string(ar));

        r = archive_write_data_block(aw, buff, size, offset);
        if (r < ARCHIVE_OK) throw std::runtime_error(archive_error_string(aw));
    }
}
