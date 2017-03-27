#include <stdio.h>
#include <dlfcn.h>

typedef int (* MAIN_PTR) (int argc, char **argv);



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
	testSO();
	
	return 0;
}
