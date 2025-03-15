// SPDX License:  	GPL-3.0-or-later

#include "mixfile.h"

using openw3d::MIXFile;

struct MIXFile_Header1
{
    char	signature[4];
    uint32_t	header_offset;
    uint32_t	names_offset;

};

struct MIXFile_DataHeader1
{
    uint32_t	file_count;
};

MIXFile::MIXFile()
{

}

bool MIXFile::open(const std::filesystem::path mix_file){
    if( m_mix_file.is_open() ){
        m_mix_file.close();
    }

    m_mix_file.open( mix_file, boost::iostreams::mapped_file::mapmode::readonly );
    if( !m_mix_file.is_open() ){
        return false;
    }

    if( m_mix_file.size() < sizeof(struct MIXFile_Header1) ){
        m_mix_file.close();
        return false;
    }

    if( m_mix_file.const_data() == nullptr ){
        // ??
        return false;
    }

    const struct MIXFile_Header1* head1 = reinterpret_cast<const struct MIXFile_Header1*>(m_mix_file.const_data());
    if( memcmp(head1->signature, "MIX1", sizeof(head1->signature)) ){
        // This is not a MIX file
        m_mix_file.close();
        return false;
    }

    // Validate that we can seek to the header_offset and names_offset
    if( m_mix_file.size() < head1->header_offset || m_mix_file.size() < head1->names_offset ){
        m_mix_file.close();
        return false;
    }

    // Seek to the data start
    const struct MIXFile_DataHeader1* data_head = reinterpret_cast<const struct MIXFile_DataHeader1*>(m_mix_file.const_data() + head1->header_offset);
    const struct MIXFile_DataHeader1* filenames_head = reinterpret_cast<const struct MIXFile_DataHeader1*>(m_mix_file.const_data() + head1->names_offset);

    if( data_head->file_count != filenames_head->file_count ){
        // The number of files vs. the number of filenames don't match.  This is bad.
        m_mix_file.close();
        return false;
    }

    const uint32_t MAX_FILES_IN_MIX = 50000; // seems like a reasonable number to me
    uint32_t max_files = std::min(MAX_FILES_IN_MIX, data_head->file_count);
    const char* filename_ptr = m_mix_file.const_data() + head1->names_offset + sizeof(struct MIXFile_DataHeader1);
    for(uint32_t x = 0; x < max_files; x++){
        // Grab the filename first.  The filename is stored as a pascal string: the first
        // byte is the length of the string, followed by the number of characters(including the terminating NULL)
        std::string filename;
        filename.reserve(255);
        uint8_t len = *filename_ptr;
        filename_ptr++;
        while(len){
            if(*filename_ptr != 0){
                filename.push_back(*filename_ptr);
            }
            len--;
            filename_ptr++;
        }

        // Now let's shove this into our map.  The FileInfoStruct is a simple offset calculation.
        const struct FileInfoStruct_Mix1* info_header = reinterpret_cast<const struct FileInfoStruct_Mix1*>(m_mix_file.const_data() + head1->header_offset + sizeof(struct MIXFile_DataHeader1));
        m_filename_to_info[filename] = info_header[x];
    }

    return true;
}

std::vector<std::string> MIXFile::filenames(){
    std::vector<std::string> ret;

    for(auto const& val : m_filename_to_info){
        ret.emplace_back(val.first);
    }

    return ret;
}

std::optional<std::span<const char>> MIXFile::get_file(const std::string& filename){
    auto it = m_filename_to_info.find(filename);
    if(it == m_filename_to_info.end()){
        return {};
    }

    return std::span<const char>(m_mix_file.const_data() + it->second.Offset,
                                    it->second.Size);
}

uint32_t MIXFile::get_file_size(const std::string& filename){
    auto it = m_filename_to_info.find(filename);
    if(it == m_filename_to_info.end()){
        return 0;
    }

    return it->second.Size;
}

bool MIXFile::file_exists(const std::string& filename){
    auto it = m_filename_to_info.find(filename);
    if(it == m_filename_to_info.end()){
        return false;
    }

    return true;
}

std::optional<openw3d::FileInfoStruct_Mix1> MIXFile::file_info(const std::string& filename){
    auto it = m_filename_to_info.find(filename);
    if(it == m_filename_to_info.end()){
        return {};
    }

    return it->second;
}
