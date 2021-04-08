
make clean
make linux
cp xc_app zone1
cp xc_app zone2
mv xc_app ./version/linux_app

make clean
make arm
./compile_fs.sh
mv xc_app ./version/arm_app
mv rootfs.ubi ./version/armrootfs.ubi
