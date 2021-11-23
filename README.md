## Introduction

Inno_space is a parser for InnoDB file formats.

It parse the .ibd file to human readable format. The origin idea come from Jeremy Cole's inno_ruby. However, inno_ruby don't support MySQL8.0, and it is written in ruby, which hard to make the environment. 

Inno_space is written in c++ and it doesn't rely on any library. You only need get the source code and Type `make` then everything get done. You don't need make the boring ruby environment.

Another interesting feature in Inno_space is it can by pass some corrupt page, if some pages in database corrupt, it can't startup. Inno_space can delete the corrupt page and then the database can startup. The database only loose one page, the other data can still retrieve.

## Feature

* Support read all block in ibd file 
* Support read the specify block in the ibd file
* Support remove corrupt page in ibd file
* Support update page's check some


## Usage

```
Inno space
usage: inno [-h] [-f test/t.ibd] [-p page_num]
        -h                -- show this help
        -f test/t.ibd     -- ibd file
                -c space-page-type     -- show all page type
                -c space-indexes       -- show indexes information
        -p page_num       -- show page information
        -u page_num       -- update page checksum
        -d page_num       -- delete page
Example:
====================================================
Show sbtest1.ibd all page type
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c space-page-type
Show sbtest1.ibd all indexes information
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c space-indexes
Delete specify page
./inno -f ~/git/primary/dbs2250/test/t1.ibd -d 2

Example1:
Show basic file space information

└─[$] ./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c space-page-type
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


Example 2:
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c space-page-type
File path /home/zongzhi.czz/git/primary/dbs2250/sbtest/sbtest1.ibd path, page num 0
==========================Space Header==========================
Space ID: 15
Highest Page number: 158976
Free limit Page Number: 152256
FREE_FRAG page number: 24
Next Seg ID: 7
File size 2604662784
Root page space_id 15 page_no 4
Leaf page segment
SEGMENT id 4 space 15; inode page no 2; reserved page 137056 used 137003; full ext 2140; fragm pages 32; free extents 0; not full extents 1: pages 11
non-Leaf page segment
SEGMENT id 3 space 15; inode page no 2; reserved page 160 used 116; full ext 1; fragm pages 32; free extents 0; not full extents 1: pages 20
Root page space_id 15 page_no 31940
Leaf page segment
SEGMENT id 6 space 15; inode page no 2; reserved page 14465 used 12160; full ext 7; fragm pages 1; free extents 0; not full extents 219: pages 11711
non-Leaf page segment
SEGMENT id 5 space 15; inode page no 2; reserved page 19 used 19; full ext 0; fragm pages 19; free extents 0; not full extents 0: pages 0


Example 3:
Show specify page information

└─[$] ./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c space-page-type -p 10
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

Example 4:
Delete specify page
./inno -f ~/git/primary/dbs2250/test/t1.ibd -d 2

```

## Contact Us

Author: baotiao@gmail.com
