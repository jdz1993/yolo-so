#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include <pwd.h>

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
typedef struct{
    float x, y, w, h;
} box;

struct object_info
{
	int ob_class;
	float ob_prob;
	box ob_box;
	struct object_info * next;
};

void print_object_info(struct object_info * oi)
{
	while(oi!=NULL)
	{
		printf("class:%d,prob:%f,center(%f,%f),width:%f,height:%f\n",oi->ob_class,oi->ob_prob,
			   oi->ob_box.x,oi->ob_box.y,oi->ob_box.w,oi->ob_box.h);
		oi=oi->next;
	}
}

typedef int (* MAIN_PTR) (int argc, char **argv);

typedef int (* OD_FUNC)(const IplImage *imageptr);

typedef int (* OD_FUNC_BY_FNAME)(const char *fname);

void * DL_open(char * sofile)
{
	void * pdlHandle=dlopen(sofile, RTLD_LAZY); // RTLD_LAZY 延迟加载
    char *pszErr = dlerror();
    if( !pdlHandle || pszErr )
    {
        printf("Load mylib failed!\n");
		printf("pdlHandle:%p,pszErr:%s\n",(int *)pdlHandle,pszErr);
        return (void *)0;
    }
	return pdlHandle;
}

void * DLSYM(void * pdlHandle,const char * funcname)
{
	void * ret=(void *)dlsym(pdlHandle,funcname);
    if( !ret )
    {
        char *pszErr = dlerror();
        printf("Find symbol failed!%s\n", pszErr);
        dlclose(pdlHandle);
    }
	return ret;
}

void frame_OD_test(IplImage* src)
{
	void *pdlHandle = DL_open("libdarknet.so");


	char * datacfg="cfg/coco.data";
	char * cfgfile="cfg/yolo.cfg";
	char * weightfile="yolo.weights";

	float thresh=.24;
	float hier_thresh=.5;

	//OD_FUNC od_func=(OD_FUNC)DLSYM(pdlHandle,"test_detector_by_cvImage");
	//struct object_info * oi=od_func(datacfg,cfgfile,weightfile,src,thresh,hier_thresh);
	//struct object_info * oi=od_func(src);
	//printf("ob_num: %d\n",oi->ob_num);

/*	for(int i=0;i<oi->ob_num;i++)
	{
		if(oi->ob_prob[i]>0.02)
			printf("%d\t%f\n",oi->ob_class[i],oi->ob_prob[i]);
	}*/

	dlclose(pdlHandle); // 系统动态链接库引用数减1
}
//void delete_object_info(struct object_info * oi)
//{
//	delete_float_info(oi->ob_prob);
//	delete_int_info(oi->ob_class);
//	//free(oi);
//	//oi=NULL;
//}



void frame_OD_test_by_fname(const char * fname)
{
//    char cwd[1024];
//    if (getcwd(cwd, sizeof(cwd)) != NULL)
//        fprintf(stdout, "Current working dir: %s\n", cwd);
//    else
//        perror("getcwd() error");
//    char path[200]="/opt/intel/mklml_2017.0.1/lib/:";
//    char ros[100]="/opt/ros/kinetic/lib";
//    strncat(path,cwd,strlen(cwd));
//    strncat(path,"/",1);
//    strncat(path,":",1);
//    strncat(path,ros,strlen(ros));
//    printf("path:%s\tsizeofpath:%d\n",path,strlen(path));
//    setenv("LD_LIBRARY_PATH",path,1);
//    printf("LD_LIBRARY_PATH: %s\n", getenv("LD_LIBRARY_PATH"));


    printf("set env succeed\n");

	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		fprintf(stdout, "Current working dir: %s\n", cwd);
	else
		perror("getcwd() error");
	strncat(cwd,"/",1);
	strncat(cwd,"libdarknet.so",13);
	void *pdlHandle = DL_open(cwd);

	char * datacfg="cfg/coco.data";
	char * cfgfile="cfg/yolo.cfg";
	char * weightfile="yolo.weights";

	float thresh=.24;
	float hier_thresh=.5;
        int i;
	OD_FUNC_BY_FNAME od_func=(OD_FUNC_BY_FNAME)DLSYM(pdlHandle,"test_detector_by_filename");
	//struct object_info * oi=od_func(datacfg,cfgfile,weightfile,src,thresh,hier_thresh);

    for(i = 0; i < 10; i++) {
        printf("%d----\n", i);

		struct object_info * oi=(struct object_info * )od_func(fname);

		print_object_info(oi);

		// delete oi, I didn't write it

	}


	dlclose(pdlHandle); // 系统动态链接库引用数减1
}



int main(int argc,char ** argv)
{
	//testSO();
#if 0
	IplImage* src=cvLoadImage("dog.jpg",1);
	frame_OD_test(src);
#else
//    const char * env=getenv("LD_LIBRARY_PATH");
//    printf("formal env:LD_LIBRARY_PATH: %s\n", env);


//    uid_t uid = geteuid();
//    struct passwd *pw = getpwuid(uid);
//    if (pw)
//    {
//    printf("%s\n", pw->pw_name);
//    }

    /* mkl, $PWD */



	frame_OD_test_by_fname("dog.jpg");

        printf("LD_LIBRARY_PATH: %s\n", getenv("LD_LIBRARY_PATH"));
#endif
	return 0;
}



int testSO()
{
	void *pdlHandle = dlopen("libdarknet.so", RTLD_LAZY); // RTLD_LAZY 延迟加载
    char *pszErr = dlerror();
    if( !pdlHandle || pszErr )
    {
        printf("Load mylib failed!\n");
		printf("pdlHandle:%p,pszErr:%s\n",(int *)pdlHandle,pszErr);
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

#endif
