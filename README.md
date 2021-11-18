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
usage: inno [-hv] [-f test/t.ibd]
        -h                -- show this help
        -f test/t.ibd     -- ibd file
        -p page_num       -- show page information
        -u page_num       -- update page checksum
        -d page_num       -- delete page
Example:
====================================================

Show basic file space information

└─[$] ./inno -f ~/git/primary/dbs2250/sbtest/sbtest1.ibd -c space-page-type
File path /home/zongzhi.czz/git/primary/dbs2250/sbtest/sbtest1.ibd path, page num 0
page num 0
==========================Space Header==========================
Space ID: 3
Highest Page number: 9216
Free limit Page Number: 8832
FREE_FRAG page number: 61
Next Seg ID: 9
File size 150994944
Find root page space_id 3 page_no 4
Find root page space_id 3 page_no 7
Find root page space_id 3 page_no 588

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

Delete specify page
./inno -f ~/git/primary/dbs2250/test/t1.ibd -d 2

```

## Contact Us

Author: baotiao@gmail.com
