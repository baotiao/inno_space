## Introduction

Inno_space is command tool to access InnoDB ibd file directly. It can parse the file give you detail information about the table. It can also fix corrupt page.

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
                -c list-page-type      -- show all page type
                -c index-summary       -- show indexes information
        -p page_num       -- show page information
                -c show-records        -- show all records information
        -u page_num       -- update page checksum
        -d page_num       -- delete page
Example:
====================================================
Show sbtest1.ibd all page type
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c list-page-type
Show sbtest1.ibd all indexes information
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c index-summary
Show specify page information
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -p 10
Delete specify page
./inno -f ~/git/primary/dbs2250/test/t1.ibd -d 2
Update specify page checksum
./inno -f ~/git/primary/dbs2250/test/t1.ibd -u 2

Example1:
Show basic file space information

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


Example 2:
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c index-summary
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
Btree hight: 2
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
Btree hight: 2
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

Example 3:
Show specify page information

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

Example 4:


Try to write some corrupt data to data file
printf 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb' | dd of=../primary/dbs2250/sbtest/sbtest1.ibd bs=1 seek=172032 count=100 conv=notrunc
The database will crash after visit the data
The log is:

2021-12-14T09:21:19.230754Z 9 [ERROR] [MY-030043] [InnoDB] InnoDB: Corrupt page resides in file: ./sbtest/sbtest1.ibd, offset: 163840, len: 16384
2021-12-14T09:21:19.230768Z 9 [ERROR] [MY-011906] [InnoDB] Database page corruption on disk or a failed file read of page [page id: space=15, page number=10]. You may have to recover from a backup.
2021-12-14T09:21:19.230775Z 9 [Note] [MY-011876] [InnoDB] Page dump in ascii and hex (16384 bytes):

Delete specify page
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -d 10

Update specify page checksum
./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -u 10

Start mysqld, the database can be started success

```

## Contact Us

Author: baotiao@gmail.com
