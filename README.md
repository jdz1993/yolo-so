This is yolo accelerated with mkl, and export to libdarknet.so in ./lib/.

# Build Darknet lib
Use Command 

	make lib -j8

this will build Darknet to libdarknet.so, and copy it to ./depoly
 - if "dlopen is undefined", gcc with -ldl

# Port darknet to new machine
### Deploy Environment settings
 - mkl
 - opencv ( if needed )

### Run from source Environment settings

 - mkl
 - icc
 - opencv ( if needed )

### config path
change Makefile library path

	CC=/home/intel/Tool/icc16.0/bin/intel64/icc -fopenmp

change to

	CC=/home/username/Tool/icc16.0/bin/intel64/icc -fopenmp

mkl library is preferred to put in directory:/opt/intel/mklml_2017.0.1/lib/

if libmkl.so not found

 - export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/intel/mklml_2017.0.1/lib/

you can also put this line in ~/.bashrc, then in shell:

	source ~./bashrc

if libdarknet.so not found, jump in /path/to/libdarknet.so, in shell:

	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD

# yolo.weights
Weights file and other details about origin darknet can be found in [Darknet project website](http://pjreddie.com/darknet).
And in this project, yolo.weights should be under ./depoly/ directory.



