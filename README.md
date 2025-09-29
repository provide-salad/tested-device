# tested device
## linux chainload rootfs but worse

synopsis
---
a worse version of busybox switch_root made from scratch

spsoinys
---
tdl;r this thing loads a different linux distro. this does not load a different kernel. you cannot load window. tool to load another kernel coming February 30
- it's experimemtel
- what it does
- what it doesn't
- use a VM
- **you still cannot load window**

how it's work
---
*tested device* is a very buggy rootfs chainloader made be me. also it only works in linux. do not try to use window. if you use window brick your device hahahahahahahahhaha

**YOU MUST NOT INSTALL THIS AS YOUR INITRAMFS UNLESS YOU KNOW WHAT YOU'RE ARE DOING**

it assumes that /dev will be mounted, and on initramfs it's not unless you mount it manually before calling the program.

if you want to install it in the initramfs, you can use the shell script and install it in /sbin/init and also you need busybox:

```shell
#!/bin/busybox sh
export PATH=/bin:/sbin:/usr/bin:/usr/sbin
busybox --install /bin || :
test -e /dev || mkdir /dev || exit
mount -t devtmpfs devtmpfs /dev || exit
exec /sbin/tested-device
read
```

It does these things:
1. enumerate files in /dev that start with "sd"
3. for each file:
4. check for ext2 magic number. if it's not there, skip it
5. compare ext2 uuid against the one that was given to the make script. if it's doesn't match, skip it
6. if it's not skipped:
7. make directory '/newroot'. the program assumes '/newroot' does not exists.
8. mount the file on '/newroot'.
9. check for PID 1. if it's not PID 1, crash here.
10. recursively delete everything unless it's a different filesystem.
11. something like busybox switch_root
12. profit

If it fails, it will not crash the system because it will make it hang so that you can read the errors message.

how to install free download legit free 2025 no cap fr fr
---

1. first, you need utility `cut` and `echo` or shell builtin
2. 2th, you need GNU toolchain and `make`
3. 3th, you need utility `tail` and `xxd`
4. shell command and replace data:
```shell
tail -c+1129 "path/to/newroot.ext2" | xxd -ps -l 16
```
  - you could maybe use uuid from utility `debugfs`
1. shell command and replace data:
```shell
make CONFIG_UUID="sample text"
```
  - if you cross compile you can add to the end `CC="path/to/sysroot/whatever-linux-whatever-gcc"` and replace data
  - if you use musl you can get tiny file < 100KB
1. create target skeleton:
```shell
mkdir -p root/dev root/sbin
cp tested-device root/sbin
```
1. generate other root image. example:
```shell
mke2fs -d root root.ext2 256K
```
1. load result into saladBox or real machine or whatever
```shell
saladBox tested-device "path/to/vmlinuz"
saladBox tested-device root.ext2 00-rootfs
saladBox tested-device "path/to/newroot.ext2" 01-newroot
saladBox tested-device
```

note: if it fails you get to start over

**WE ARE NOT RESPOSNIBLE FOR YOU BRICK YOUR DEVICE BECAUSE OF STUPID**

**WE ARE NOT RESPONSIBLE FOR YOU BRICK YOUR DEVICE FOR ANY REASON, INCLUDING REASON NOT RELATED TO STUPID**
