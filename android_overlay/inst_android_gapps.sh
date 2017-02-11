mkdir vendor
mkdir vendor/google
mkdir vendor/google/build
cd vendor/google/build
git clone https://github.com/opengapps/aosp_build.git
cd ../..
mkdir opengapps
mkdir opengapps/sources
#mkdir opengapps/sources/all
cd opengapps/sources
git clone https://github.com/opengapps/all.git
#mkdir ../arm
#cd ../arm
git clone https://git.themusio.com/AKA-Intelligence/opengapps_arm_2016_jan_first.git
cd ../../../..
cp opengapps-for-musio-2.mk vendor/google/build

