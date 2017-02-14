#
# this script will make populate directories needed to build the opengapps.
#

ROOTDIR=$(pwd)


mkdir vendor
mkdir vendor/google

cd vendor/google

# populate this directory with necessary files.
git clone https://github.com/opengapps/aosp_build.git

#after git download, the directory name is aosp_build. change this to 'build'
mv aosp_build build


# copy the 'opengapps-for-musio-2.mk' file to /vendor/google/build
cp $ROOTDIR/opengapps-for-musio2.mk build/


# we are curently in /vendor/google. go back to /vendor
cd ..

git clone https://github.com/opengapps/opengapps.git

#now there will be a directory named 'opengapps'. go into it.
cd opengapps

# now, download source of what we need.
# the --shallow option is added in order to reduce disk space usage and save time.
# since via board is ARM arch, the 'arm' parameter is used.
# this command will populate the '/source' directory with 'all' and 'arm'.
./download_sources.sh --shallow arm


