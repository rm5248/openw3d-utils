# Westwood MIX File format

The MIX file format is very simple.  It is just a collection of files that are
stored in a single large file for access.

The basic structure is as follows:
```
     MIX File Format                                                                        
  ┌────────────────────────────────────────────────────────────────────────────────────────┐
  │ File Header                                                                            │
  │    Signature - MIX1                                                                    │
  │    header_offset - Where to find information on sub-files                              │
  │    names_offset - Where to find the file names of the sub-files                        │
  │                                                                                        │
  │ header_offset                                                                          │
  │    uint32_t - how many FileInfoStruct                                                  │
  │    0...N - FileInfoStruct                                                              │
  │                                                                                        │
  │ names_offset                                                                           │
  │    uint32_t - how many files exist                                                     │
  │    0...N - filename strings.  String starts with uint8_t len, includes NULL terminator │
  │      uint8_t - string len                                                              │
  └────────────────────────────────────────────────────────────────────────────────────────┘
```

See the following sections for more details

## MIX File Header

```
struct MIXFile_Header
{
    char	signature[4];
    uint32_t	header_offset;
    uint32_t	names_offset;

};
```

Signature - Identifies this as a MIX file.  Characters are `MIX1`.
header_offset - Identifies a location in the file that identifies information
about the files in the MIX
names_offset - where in the file the names of files are stored

## File Information(header_offset)

At the `header_offset` location in the MIX file, there is a 4-byte array
length specifying how many entries there are in the file.  The array of
`FileInfoStruct` is immediately following this length value.

```
struct FileInfoStruct {
    uint32_t CRC;      // CRC code for embedded file.
    uint32_t Offset;   // Offset from start of MIX file.
    uint32_t Size;     // Size of data subfile.
};
```

## File name information

The file name information is an array of variable sized char giving the name
of the file.  Each entry starts with a single byte giving the length of the
string.  Note that this length also includes the terminating NULL character.

## Known Limitations of the MIX format

* Because the offsets are all `uint32_t` in size, MIX files are effectively
capped at 4GB.
