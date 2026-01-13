# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

inno_space is a C++ command-line tool for direct access and analysis of MySQL InnoDB tablespace (.ibd) files. It can parse file headers, display index structures, extract records, fix corrupt pages, and update checksums. Targets MySQL 8.0+.

## Build Commands

```bash
# Build the project (produces executable named "inno")
make

# Clean build artifacts
make clean
```

Compiler: g++ with C++11 standard. Dependencies: RapidJSON (header-only, included in include/rapidjson/).

**macOS note:** The `include/my_config.h` was generated for Linux. If build fails with "undeclared identifier 'ulong'", change `#define HAVE_ULONG 1` to `/* #undef HAVE_ULONG */` in that file.

## Usage Examples

```bash
# Show tablespace header
./inno -f /path/to/file.ibd

# Show specific page
./inno -f /path/to/file.ibd -p <page_number>

# List all page types
./inno -f /path/to/file.ibd -c list-page-type

# Show index summary (B-tree structure)
./inno -f /path/to/file.ibd -c index-summary

# Dump all records (requires SDI file for column definitions)
./inno -f /path/to/file.ibd -s /path/to/sdi.json -c dump-all-records

# Update page checksum
./inno -f /path/to/file.ibd -p <page_number> -u

# Delete/mark corrupt page
./inno -f /path/to/file.ibd -p <page_number> -d
```

## Architecture

### Core Components

- **src/inno_space.cc** - Main application with CLI parsing and all major operations (ShowSpaceHeader, ShowIndexSummary, ShowRecord, DumpAllRecords, UpdateCheckSum, DeletePage)

- **Page/File Format Headers (include/):**
  - `fil0fil.h/cc` - File format definitions
  - `page0page.h/cc` - Page header management (16KB pages)
  - `fsp0fsp.h/cc` - File space header
  - `rec.h` - Record structure definitions
  - `mach_data.h/cc` - Binary data encoding/decoding utilities

- **src/crc32.cc** - CRC32 checksum implementation for page validation

### Key Design Patterns

- Code mirrors MySQL InnoDB internal structures
- Heavy use of bit manipulation and pointer arithmetic for binary format parsing
- Supports compact and dynamic record formats
- Uses RapidJSON to parse SDI (Serialized Dictionary Information) for column type definitions

### Global State

The application uses global variables for state: `fd` (file descriptor), `read_buf` (16KB page buffer), `dict_cols` (column definitions from SDI).
