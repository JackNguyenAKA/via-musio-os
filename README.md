#VIA ported MUSIO OS

this repo mainly holds the overlay files that should be added to port MUSIO OS to VIA version board.

# bspinst directory

this directory holds stock images provided from VIA. These are Android 5.1.1 build which is not MUSIO OS. Stored in this repo to make it easier to gain access to these files, which are frequently used to compare the performance with MUSIO OS and ensure features are working in stock Android images.

# setup

* unzip via bsp (not includd in this repo. get it from hwcell)
* copy `android_overlay` contents to android build root directory of the unziped VIA bsp.
```
$ cp -Ra android_overlay/* {android_root}
```
* move to android root directory.
```
$ cd {android_root}
```

* run inst_android_gapps.sh
```
$ ./inst_android_gapps.sh
```

this process may take a long time due to downloading opengapps sources. Beaware.

* if it is your first time to unzip the VIA BSP, then make sure that {android_root}/build/envsetup.sh is executable.
```
$ chmod u+x /build/envsetup.sh
```
* run `/build/envsetup.sh`
```
$ ./build/envsetup.sh
```
* build it. to do this there are several ways. you can use lunch and select sabresd_6dq variants. Or you can directly trigger make with something like this:
```
$ make PRODUCT-sabresd_6dq-user -j8
```


