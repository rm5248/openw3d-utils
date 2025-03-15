// SPDX License:  	GPL-3.0-or-later

#ifndef MIXFILE_H
#define MIXFILE_H

#include <span>
#include <filesystem>
#include <vector>
#include <string>
#include <optional>
#include <stdint.h>
#include <map>

#include <boost/iostreams/device/mapped_file.hpp>

namespace openw3d {

/**
 * Represents a Westwood MIX file.
 */
class MIXFile
{
public:
    MIXFile();

    /**
     * Open the specified file as a MIX file.  If the file could be opened(and it is a valid MIX file),
     * return true.  Else, return false.
     *
     * @param mix_file
     * @param mode
     * @return
     */
    bool open(std::filesystem::path mix_file);

    /**
     * Close the current MIX file.
     */
    void close();

    /**
     * Get the filenames in a MIX file.
     *
     * @return
     */
    std::vector<std::string> filenames();

    /**
     * Get the bytes of the specified file in the MIX file.
     *
     * @param filename
     * @return
     */
    std::optional<std::span<const char>> get_file(const std::string& filename);

    /**
     * Get the size of the specified file.  Returns 0 if the file does not exist.
     *
     * @param filename
     * @return
     */
    uint32_t get_file_size(const std::string& filename);

    /**
     * Check if the given file exists in the MIX file.
     *
     * @param filename
     * @return
     */
    bool file_exists(const std::string& filename);

private:
    struct FileInfoStruct_Mix1 {
        bool operator== (const FileInfoStruct_Mix1 &src)	{ return false; }
        bool operator!= (const FileInfoStruct_Mix1 &src)	{ return true; }

        uint32_t CRC;				// CRC code for embedded file.
        uint32_t Offset;			// Offset from start of MIX file.
        uint32_t Size;				// Size of data subfile.
    };

    boost::iostreams::mapped_file m_mix_file;
    std::map<std::string,FileInfoStruct_Mix1> m_filename_to_info;
};

}

#endif // MIXFILE_H
