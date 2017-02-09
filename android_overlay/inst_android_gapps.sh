mkdir vendor
mkdir vendor/google
mkdir vendor/google/build
cd vendor/google/build
git clone https://github.com/opengapps/aosp_build.git
cd ../..
mkdir opengapps
mkdir opengapps/sources
mkdir opengapps/sources/all
cd opengapps/sources/all
git clone https://github.com/opengapps/all.git
mkdir ../arm
cd ../arm
unzip ../../../../open_gapps-arm-5.1-nano-20160105.zip
cd ../../../..
cp opengapps-for-musio-2.mk vendor/google/build

