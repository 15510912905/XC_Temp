#!/bin/sh
#cd /root/app
i=`cat pingpang.ini`
ulimit -c unlimited
case $i in
"mainzone= zone1" )
{
    #cd /root/app/zone1
    cd 'zone1'
    ./xc_app
}
;;
"mainzone= zone2" )
{
    #cd /root/app/zone2
    cd 'zone2'
    ./xc_app
}
;;
* )
{
    echo "zone error"
}
;;
esac
