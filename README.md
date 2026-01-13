## Introduction

Inno_space is a command-line tool designed for direct access to InnoDB (.ibd) files. It offers the capability to parse these files, providing detailed table information, and it can also assist in fixing corrupt pages. Inno_space converts .ibd files into human-readable formats, inspired by Jeremy Cole's inno_ruby. Unlike inno_ruby, Inno_space supports MySQL 8.0 and is implemented in C++, eliminating the need to set up a Ruby environment.

An intriguing feature of Inno_space is its ability to bypass corrupt pages. In cases where a database won't start due to corruption in some pages, Inno_space can delete the corrupt page, allowing the database to start. This results in the loss of only one page, with other data remaining retrievable.

## Features

* Supports reading all blocks in .ibd files.
* Allows reading a specific block within the .ibd file.
* Provides the capability to remove corrupt pages in .ibd files.
* Supports updating page checksums.
* **Supports dumping records from .ibd files.**

## Build

```bash
# Build the project (produces executable named "inno")
make

# Clean build artifacts
make clean
```

**macOS:** If the build fails with `undeclared identifier 'ulong'`, edit `include/my_config.h` and change `#define HAVE_ULONG 1` to `/* #undef HAVE_ULONG */`.

## Usage

```shell
Inno_space
usage: inno [-h] [-f test/t.ibd] [-p page_num]
        -h                -- show this help
        -f test/t.ibd     -- ibd file
                -c list-page-type      -- show all page types
                -c index-summary       -- show indexes information
                -c show-undo-file      -- show undo log detail
        -p page_num       -- show page information
                -c show-records        -- show all records information
        -u page_num       -- update page checksum
        -d page_num       -- delete page

Example:
====================================================
Show sbtest1.ibd all page types
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c list-page-type
Show sbtest1.ibd all indexes information
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c index-summary
Show undo_001 all rseg information
./inno -f ~/git/primary/dbs2250/log/undo_001 -c show-undo-file
Show specified page information
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -p 10
Delete specified page
./inno -f ~/git/primary/dbs2250/test/t1.ibd -d 2
Update specified page checksum
./inno -f ~/git/primary/dbs2250/test/t1.ibd -u 2
Show records in specified page
./inno -f ~/git/db8r/dbs2250/sbtest/sbtest1.ibd -p 100 -c show-records -s ./tool/sbtest1.json
Dump all records in .ibd file
./inno -f ~/git/db8r/dbs2250/sbtest/sbtest1.ibd -c dump-all-records -s ./tool/sbtest1.json

```

**Example1:**
Show basic file space information

```shell
└─[$] ./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c list-page-type
File path /home/zongzhi.czz/git/primary/dbs2250/sbtest/sbtest1.ibd path, page num 0
page num 0
==========================space page type==========================
File size 2604662784
start           end             count           type
0               0               1               FSP HDR
1               1               1               INSERT BUFFER BITMAP
2               2               1               INDEX NODE PAGE
3               3               1               SDI INDEX PAGE
4               16383           16380           INDEX PAGE
16384           16384           1               XDES
16385           16385           1               INSERT BUFFER BITMAP
16386           31990           15605           INDEX PAGE
31991           31999           9               FRESHLY ALLOCATED PAGE
32000           32767           768             INDEX PAGE
32768           32768           1               XDES
32769           32769           1               INSERT BUFFER BITMAP
32770           49151           16382           INDEX PAGE
49152           49152           1               XDES
49153           49153           1               INSERT BUFFER BITMAP
49154           65535           16382           INDEX PAGE
65536           65536           1               XDES
65537           65537           1               INSERT BUFFER BITMAP
65538           81919           16382           INDEX PAGE
81920           81920           1               XDES

```

**Example 2:**
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c index-summary

```shell
File path /home/zongzhi.czz/git/primary/dbs2250/sbtest/sbtest1.ibd path, page num 0
==========================Space Header==========================
Space ID: 15
Highest Page number: 158976
Free limit Page Number: 152256
FREE_FRAG page number: 24
Next Seg ID: 7
File size 2604662784
========Primary index========
Primary index root page space_id 15 page_no 4
Btree height: 2
<<<Leaf page segment>>>
SEGMENT id 4, space id 15
Extents information:
FULL extent list size 2140
FREE extent list size 0
PARTIALLY FREE extent list size 1
Pages information:
Reserved page num: 137056
Used page num: 137003
Free page num: 53

<<<Non-Leaf page segment>>>
SEGMENT id 3, space id 15
Extents information:
FULL extent list size 1
FREE extent list size 0
PARTIALLY FREE extent list size 1
Pages information:
Reserved page num: 160
Used page num: 116
Free page num: 44

========Secondary index========
Secondary index root page space_id 15 page_no 31940
Btree height: 2
<<<Leaf page segment>>>
SEGMENT id 6, space id 15
Extents information:
FULL extent list size 7
FREE extent list size 0
PARTIALLY FREE extent list size 219
Pages information:
Reserved page num: 14465
Used page num: 12160
Free page num: 2305

<<<Non-Leaf page segment>>>
SEGMENT id 5, space id 15
Extents information:
FULL extent list size 0
FREE extent list size 0
PARTIALLY FREE extent list size 0
Pages information:
Reserved page num: 19
Used page num: 19
Free page num: 0

**Suggestion**
File size 2604662784, reserved but not used space 39354368, percentage 1.51%
Optimize table will get new fie size 2565308416
```

**Example 3:**
Show specific page information

```shell
└─[$] ./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -p 10
File path /home/zongzhi.czz/git/primary/dbs2250/sbtest/sbtest1.ibd path, page num 10
==========================block==========================
FIL Header:
CheckSum: 1187106543
crc 1187106543
Page number: 10
Previous Page: 9
Next Page: 12
Page LSN: 35174063
Page Type: 17855
Flush LSN: 0
Index Header:
Number of Directory Slots: 18
Garbage Space: 0
Number of Records: 73
Max Trx id: 0
Page level: 0
Index ID: 142
```

Example 4:

```shell
Try to write some corrupt data to data file
printf 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb' | dd of=../primary/dbs2250/sbtest/sbtest1.ibd bs=1 seek=172032 count=100 conv=notrunc
The database will crash after visiting the data
The log is:

2021-12-14T09:21:19.230754Z 9 [ERROR] [MY-030043] [InnoDB] InnoDB: Corrupt page resides in file: ./sbtest/sbtest1.ibd, offset: 163840, len: 16384
2021-12-14T09:21:19.230768Z 9 [ERROR] [MY-011906] [InnoDB] Database page corruption on disk or a failed file read of page [page id: space=15, page number=10]. You may have to recover from a backup.
2021-12-14T09:21:19.230775Z 9 [Note] [MY-011876] [InnoDB] Page dump in ASCII and hex (16384 bytes):

Delete specified page
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -d 10

Update specified page checksum
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -u 10
```

Start mysqld, and the database can be started successfully

**Example 5:**

Try to show records on a specified page number

```shell
./inno -f /home/zongzhi.czz/git/db8r/dbs2250/sbtest/sbtest1.ibd -c show-records -p 10  -s ./tool/sbtest1.json

File path /home/zongzhi.czz/git/db8r/dbs2250/sbtest/sbtest1.ibd path, page num 10
=========================10's block==========================
FIL Header:
CheckSum: 613295053
Page number: 10
Previous Page: 9
Next Page: 11
Page LSN: 92471792635
Page Type: 17855
Flush LSN: 0

Index Header:
Number of Directory Slots: 19
Garbage Space: 0
Number of Records: 73
Max Trx id: 0
Page level: 0
Index ID: 359

offset from the previous record 26
offset inside the page 125
heap no 2
record status 0
Info Flags: is_deleted 0, is_min_record 0
id: 329
k: 67746
c: 25087106756-05358861945-28639810730-70293660170-58309876130-54681200436-23663683142-92420314539-18642450369-82863665005
pad: 39869284856-57048363201-54479788494-88842253993-52056631753
... 
then all the records on page num 10

```





Read more about InnoDB file_space:

https://blog.jcole.us/innodb/

http://baotiao.github.io/2021/11/29/inno-space.html

### Contact Us
If you have any questions or need assistance, feel free to contact the author at baotiao@gmail.com
