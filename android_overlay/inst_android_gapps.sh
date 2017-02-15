#
# original creator: susan.choi
# update: chadrick-kwaG
# this script will make populate directories needed to build the opengapps.
#



# this parameter is used to decie whether it will download from official github repo or from git.musio.com repo.
# downloading from official takes a lot of time. therefore the default will be downloading from git.msuio.com repo.

# !! but right now, the git.musio.com repo is not set properly. so for now, the default will be official download
DOWNLOAD_FROM_OFFICIAL=y

if [ $# -eq 0 ];then

fi

for opt in "$@"
do
	case $opt in
	--official)
	DOWNLOAD_FROM_OFFICIAL=y
	;;
esac
done


# inform the user of official repo download
if [ $DOWNLOAD_FROM_OFFICIAL == y ];then
	echo "opengapps sources will be downloaded from official repo"
fi


ROOTDIR=$(pwd)

mkdir -p vendor/google
cd vendor/google

# populate this directory with necessary files.
git clone https://github.com/opengapps/aosp_build.git

#after git download, the directory name is aosp_build. change this to 'build'
mv aosp_build build


# copy the 'opengapps-for-musio-2.mk' file to /vendor/google/build
cp $ROOTDIR/opengapps-for-musio2.mk build/


# we are curently in /vendor/google. go back to /vendor
cd ..

if [ $DOWNLOAD_FROM_OFFICIAL == y ];then
	git clone https://github.com/opengapps/opengapps.git
	cd opengapps

# now, download source of what we need.
# the --shallow option is added in order to reduce disk space usage and save time.
# since via board is ARM arch, the 'arm' parameter is used.
# this command will populate the '/source' directory with 'all' and 'arm'.
	./download_sources.sh --shallow arm


else
	git clone https://git.themusio.com/AKA-Intelligence/opengapps_arm_2016_jan_first.git
# since the new repo directory name will be nonstandard, change the directory name.
	mv opengapps_arm_2016_jan_first.git opengapps


## more job needs to be done here!
fi



#eof
