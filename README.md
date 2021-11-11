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
./inno -f ~/git/primary/dbs2250/test/t1.ibd -p 2
./inno -f ~/git/primary/dbs2250/test/t1.ibd -d 2
```

## Contact Us

Author: baotiao@gmail.com
