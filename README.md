![Darknet Logo](http://pjreddie.com/media/files/darknet-black-small.png)

# Build Darknet lib
Use Command 

	make lib -j8

this will build Darknet to libdarknet.so, and copy it to ./depoly
 - if "dlopen is undefined", gcc with -ldl

# Port darknet to new machine

### Build from Source Environment settings

 - mkl
 - icc ( or default gcc )
 - opencv ( if needed )
If you need mkl and icc, you can contact 1072425571@qq.com and I can send you a copy.
The path/to/library should be configed in Makefile or using export LD_LIBRARY_PATH, more specific information is below.

### Config path
change Makefile library path

if libmkl.so not found

 - export LD_LIBRARY_PATH=/path/to/library/mklml_2017.0.1/lib/:$LD_LIBRARY_PATH
 - For example: export LD_LIBRARY_PATH=/opt/intel/mklml_2017.0.1/lib/:$LD_LIBRARY_PATH

if libdarknet.so libimf.so libintlc.so.5 libirng.so libsvml.so not found

 - export LD_LIBRARY_PATH=$PWD/:$LD_LIBRARY_PATH

