#include <stdio.h>
#include <dlfcn.h>

#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"

#if 0
int main()
{
	IplImage * img =cvLoadImage("dog.jpg",1);
	cvNamedWindow("Example", CV_WINDOW_AUTOSIZE);
	cvShowImage("Example", img);
	cvWaitKey(0);
	cvReleaseImage(&img);
	cvDestroyWindow("Example");
	return 0;
}

#else
typedef struct {
    int h;
    int w;
    int c;
    float *data;
} image;

typedef int (* MAIN_PTR) (int argc, char **argv);

typedef int (* OD_FUNC)(char *datacfg, char *cfgfile, char *weightfile, const image *imageptr, float thresh, float hier_thresh);
//void frame_OD(int camera_index,)


void * DL_open(char * sofile)
{
	void * pdlHandle=dlopen(sofile, RTLD_LAZY); // RTLD_LAZY 延迟加载
    char *pszErr = dlerror();
    if( !pdlHandle || pszErr )
    {
        printf("Load mylib failed!\n");
		printf("pdlHandle:%d,pszErr:%s\n",(int)pdlHandle,pszErr);
        return 1;
    }
	return pdlHandle;
}

void frame_OD_test()
{
	void *pdlHandle = DL_open("libdarknet.so");

	char * datacfg="";
	char * cfgfile="";
	char * weightfile="";
	//image im=load_image_cv("dog.jpg",3);
	IplImage* src=cvLoadImage("dog.jpg",1);
	float thresh=.24;
	float hier_thresh=.5;

	OD_FUNC od_func;
	od_func=(OD_FUNC)dlsym(pdlHandle,"test_detector_by_cvImage");
    if( !od_func )
    {
        char *pszErr = dlerror();
        printf("Find symbol failed!%s\n", pszErr);
        dlclose(pdlHandle);
        return 1;
    }
	else
	{
		printf("%p\n",od_func);
		od_func(datacfg,cfgfile,weightfile,src,thresh,hier_thresh);
	}
	dlclose(pdlHandle); // 系统动态链接库引用数减1
}

int testSO()
{
	void *pdlHandle = dlopen("libdarknet.so", RTLD_LAZY); // RTLD_LAZY 延迟加载
    char *pszErr = dlerror();
    if( !pdlHandle || pszErr )
    {
        printf("Load mylib failed!\n");
		printf("pdlHandle:%d,pszErr:%s\n",(int)pdlHandle,pszErr);
        return 1;
    }

	    
	MAIN_PTR  addr;
	char *argvv[] = {"./darknet","detector","demo","cfg/coco.data","cfg/yolo.cfg","yolo.weights","-c","2"};
	int argcc=8;

	addr = (MAIN_PTR)dlsym(pdlHandle, "main"); // 定位动态链接库中的函数
    if( !addr )
    {
        pszErr = dlerror();
        printf("Find symbol failed!%s\n", pszErr);
        dlclose(pdlHandle);
        return 1;
    }
	else
	{
		printf("%p\n",addr);
		addr(argcc,argvv);
	}

 	dlclose(pdlHandle); // 系统动态链接库引用数减1
	return 0;
}

int main(int argc,char ** argv)
{
	//testSO();
	frame_OD_test();
	return 0;
}

#endif
