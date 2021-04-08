#!/bin/sh
basepath=$(cd `dirname $0`; pwd)
cp xc_app /home/public/linux_release/compile_fs/buildroot-at91/output/target/root/app/zone1/
cp xc_app /home/public/linux_release/compile_fs/buildroot-at91/output/target/root/app/zone2/
cp alarm.cfg /home/public/linux_release/compile_fs/buildroot-at91/output/target/root/app
cd '/home/public/linux_release/compile_fs/buildroot-at91/'
make
cd '/home/public/linux_release/compile_fs/buildroot-at91/output/images/'
cp rootfs.ubi $basepath

