GPU=0
CUDNN=0
OPENCV=0
DEBUG=0

ARCH= -gencode arch=compute_20,code=[sm_20,sm_21] \
      -gencode arch=compute_30,code=sm_30 \
      -gencode arch=compute_35,code=sm_35 \
      -gencode arch=compute_50,code=[sm_50,compute_50] \
      -gencode arch=compute_52,code=[sm_52,compute_52]

# This is what I use, uncomment if you know your arch and want to specify
# ARCH=  -gencode arch=compute_52,code=compute_52

VPATH=./src/
DYNLIB=./lib/libdarknet.so
EXEC=darknet
OBJDIR=./obj/

#CC=gcc -fopenmp
CC=/home/intel/Tool/icc16.0/bin/intel64/icc -fopenmp 
NVCC=nvcc 
OPTS=-Ofast
LDFLAGS= -lm -pthread 
COMMON= 
CFLAGS=-Wall -Wfatal-errors 

SHARE= -fPIC -nostartfiles

ifeq ($(DEBUG), 1) 
OPTS=-O0 -g
endif

CFLAGS+=$(OPTS)

ifeq ($(OPENCV), 1) 
COMMON+= -DOPENCV
CFLAGS+= -DOPENCV
LDFLAGS+= -L/opt/ros/kinetic/lib -lopencv_calib3d3 -lopencv_core3 -lopencv_features2d3 -lopencv_flann3 -lopencv_highgui3 -lopencv_imgproc3 -lopencv_ml3 -lopencv_objdetect3 -lopencv_photo3 -lopencv_stitching3 -lopencv_superres3 -lopencv_video3 -lopencv_videostab3 -L/usr/local/lib -lrt -lpthread -lm -ldl 
COMMON+= -I/opt/ros/kinetic/include/opencv-3.2.0/opencv -I/opt/ros/kinetic/include/opencv-3.2.0
endif

ifeq ($(GPU), 1) 
COMMON+= -DGPU -I/usr/local/cuda/include/
CFLAGS+= -DGPU
LDFLAGS+= -L/usr/local/cuda/lib64 -lcuda -lcudart -lcublas -lcurand
endif

ifeq ($(CUDNN), 1) 
COMMON+= -DCUDNN 
CFLAGS+= -DCUDNN
LDFLAGS+= -lcudnn
endif

CFLAGS+= -I/opt/intel/mklml_2017.0.1/include
LDFLAGS+= -L/opt/intel/mklml_2017.0.1/lib -lmklml_intel -liomp5
OBJ=gemm.o utils.o cuda.o convolutional_layer.o list.o image.o activations.o im2col.o col2im.o blas.o crop_layer.o dropout_layer.o maxpool_layer.o softmax_layer.o data.o matrix.o network.o connected_layer.o cost_layer.o parser.o option_list.o darknet.o detection_layer.o captcha.o route_layer.o writing.o box.o nightmare.o normalization_layer.o avgpool_layer.o coco.o dice.o yolo.o detector.o layer.o compare.o classifier.o local_layer.o swag.o shortcut_layer.o activation_layer.o rnn_layer.o gru_layer.o rnn.o rnn_vid.o crnn_layer.o demo.o tag.o cifar.o go.o batchnorm_layer.o art.o region_layer.o reorg_layer.o super.o voxel.o tree.o
ifeq ($(GPU), 1) 
LDFLAGS+= -lstdc++ 
OBJ+=convolutional_kernels.o activation_kernels.o im2col_kernels.o col2im_kernels.o blas_kernels.o crop_layer_kernels.o dropout_layer_kernels.o maxpool_layer_kernels.o network_kernels.o avgpool_layer_kernels.o
endif

OBJS = $(addprefix $(OBJDIR), $(OBJ))
DEPS = $(wildcard src/*.h) Makefile

lib: $(OBJS)
	rm -rf lib
	mkdir -p lib
	$(CC) $(COMMON) $(CFLAGS) -shared -o $(DYNLIB) $^ $(LDFLAGS)
	cp lib/libdarknet.so depoly/

$(OBJDIR)%.o: %.c $(DEPS)
	mkdir -p obj
	$(CC) $(COMMON) $(CFLAGS) -fPIC -c $< -o $@

#$(OBJDIR)%.o: %.cu $(DEPS)
#	$(NVCC) $(ARCH) $(COMMON) --compiler-options "$(CFLAGS)" -fPIC -c $< -o $@


.PHONY: clean

clean:
	rm -rf $(OBJS) $(EXEC) lib

