#ifndef inno_space_ibd2sdi_h
#define inno_space_ibd2sdi_h

#include <string>

// Extract SDI (Serialized Dictionary Information) from an InnoDB tablespace file
// Parameters:
//   file_fd: file descriptor of the .ibd file
//   json_output: string to receive the extracted JSON
// Returns: 0 on success, non-zero on error
int extract_sdi_from_ibd(int file_fd, std::string& json_output);

// Parse SDI JSON and populate global offset and column dictionaries
// Parameters:
//   json_str: JSON string to parse
// Returns: 0 on success, non-zero on error
int parse_sdi_json(const std::string& json_str);

#endif
