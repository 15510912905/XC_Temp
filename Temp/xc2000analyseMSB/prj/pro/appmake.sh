BUILD_CMD=$1

#make clean

case $1 in
	arm)
        make arm
        ;;
	*)
		make linux
		cp xc_app zone1
		cp xc_app zone2
		;;
esac
		

