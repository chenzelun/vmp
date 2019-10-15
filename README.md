# vmp
vmp509

2019.10.15
ok:
    1.android 9 ok_1
    2.array of basic type is ok, eg. I (int)

error:
    1.out of memory
    2.android 4.5.6 local ref table is full over 

todo:
    1.encrypt and sign
    2.nodebug
    3.file encrypt
    4.config the function

extends:
    1. 'init' and 'clinit' could invoke by cdVM

function:
    1.android 4.5.6.8.9 ok_1
    2.arm v7.v8 ok_1
    3.dex write back
    4.so write back



2019.9.25
android4.5.6.8 ok_1
dex write back
so write back

android9 error: can't get the addr of dex by mmap
                need to know the sulotion of launch app

error: 
    1. out of memory: new large array
    2. can't found class of 'I' in array.




android4.4 5.1.1 6.0.1 8.1.0 OK_1
arm v7 v8 OK_1
android9 invoke system's functions run badly!!!
