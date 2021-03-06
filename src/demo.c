#include "network.h"
#include "detection_layer.h"
#include "region_layer.h"
#include "cost_layer.h"
#include "utils.h"
#include "parser.h"
#include "box.h"
#include "image.h"
#include "demo.h"
#include <sys/time.h>


#define FRAMES 1

#ifdef OPENCV
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/videoio/videoio_c.h"
image get_image_from_stream(CvCapture *cap);

static char **demo_names;
static image **demo_alphabet;
static int demo_classes;

static float **probs;
static box *boxes;
static network net;
static image in   ;
static image in_s ;
static image det  ;
static image det_s;
static image disp = {0};
static CvCapture * cap;
static float fps = 0;
static float demo_thresh = 0;
static float demo_hier_thresh = .5;

static float *predictions[FRAMES];
static int demo_index = 0;
static image images[FRAMES];
static float *avg;

//static int detect_thread_run=0;
//static int detect_thread_finished=0;
//static pthread_mutex_t lock_t;

void *fetch_in_thread(void *ptr)
{
    in = get_image_from_stream(cap);
    if(!in.data){
        error("Stream closed.");
    }
    in_s = resize_image(in, net.w, net.h);
    return 0;
}

void *detect_in_thread(void *ptr)
{
    //detect_thread_run=1;
    float nms = .4;

    layer l = net.layers[net.n-1];
    float *X = det_s.data;
    float *prediction = network_predict(net, X);

    memcpy(predictions[demo_index], prediction, l.outputs*sizeof(float));
    mean_arrays(predictions, FRAMES, l.outputs, avg);
    l.output = avg;

    free_image(det_s);
    if(l.type == DETECTION){
        get_detection_boxes(l, 1, 1, demo_thresh, probs, boxes, 0);
    } else if (l.type == REGION){
        get_region_boxes(l, 1, 1, demo_thresh, probs, boxes, 0, 0, demo_hier_thresh);
    } else {
        error("Last layer must produce detections\n");
    }
    if (nms > 0) do_nms(boxes, probs, l.w*l.h*l.n, l.classes, nms);
    printf("\033[2J");
    printf("\033[1;1H");
    printf("\nFPS:%.1f\n",fps);
    printf("Objects:\n\n");

    images[demo_index] = det;
    det = images[(demo_index + FRAMES/2 + 1)%FRAMES];
    demo_index = (demo_index + 1)%FRAMES;

    
    draw_detections(det, l.w*l.h*l.n, demo_thresh, boxes, probs, demo_names, demo_alphabet, demo_classes);

    //pthread_mutex_lock(&lock_t);
    //detect_thread_finished=1;
    //pthread_mutex_unlock(&lock_t);

    //detect_thread_run=0;
    return 0;
}

double get_wall_time()
{
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

void demo(char *cfgfile, char *weightfile, float thresh, int cam_index, const char *filename, char **names, int classes, int frame_skip, char *prefix, float hier_thresh)
{
    //skip = frame_skip;
    image **alphabet = load_alphabet();
    int delay = frame_skip;
    demo_names = names;
    demo_alphabet = alphabet;
    demo_classes = classes;
    demo_thresh = thresh;
    demo_hier_thresh = hier_thresh;
    printf("Demo\n");
    net = parse_network_cfg(cfgfile);
    if(weightfile){
        load_weights(&net, weightfile);
    }
    set_batch_network(&net, 1);

    srand(2222222);

    if(filename){
        printf("video file: %s\n", filename);
        cap = cvCaptureFromFile(filename);
    }else{
        cap = cvCaptureFromCAM(cam_index);
    }

    if(!cap) error("Couldn't connect to webcam.\n");

    layer l = net.layers[net.n-1];
    int j;

    avg = (float *) calloc(l.outputs, sizeof(float));
    for(j = 0; j < FRAMES; ++j) predictions[j] = (float *) calloc(l.outputs, sizeof(float));
    for(j = 0; j < FRAMES; ++j) images[j] = make_image(1,1,3);

    boxes = (box *)calloc(l.w*l.h*l.n, sizeof(box));
    probs = (float **)calloc(l.w*l.h*l.n, sizeof(float *));
    for(j = 0; j < l.w*l.h*l.n; ++j) probs[j] = (float *)calloc(l.classes, sizeof(float));

    pthread_t fetch_thread;
    pthread_t detect_thread;

//pthread_t fetch_thread2;

    fetch_in_thread(0);
    det = in;
    det_s = in_s;

    fetch_in_thread(0);
    detect_in_thread(0);
    disp = det;
    det = in;
    det_s = in_s;

    for(j = 0; j < FRAMES/2; ++j){
        fetch_in_thread(0);
        detect_in_thread(0);
        disp = det;
        det = in;
        det_s = in_s;
    }

    int count = 0;

    if(!prefix){
        cvNamedWindow("Demo", CV_WINDOW_NORMAL); 
        cvMoveWindow("Demo", 0, 0);
        cvResizeWindow("Demo", 1352, 1013);
    }

    double before = get_wall_time();

    //pthread_mutex_init(&lock_t,NULL);
    static IplImage *src;
    double niddle=get_wall_time();

    while(1){
//printf("count:%d\n",count);
        ++count;

        if(1){
            if(pthread_create(&fetch_thread, 0, fetch_in_thread, 0)) error("Thread creation failed");
            if(pthread_create(&detect_thread, 0, detect_in_thread, 0)) error("Thread creation failed");

            if(!prefix){
               // show_image(disp, "Demo");
                int c = cvWaitKey(100);
                if (c == 10){
                    if(frame_skip == 0) frame_skip = 60;
                    else if(frame_skip == 4) frame_skip = 0;
                    else if(frame_skip == 60) frame_skip = 4;   
                    else frame_skip = 0;
                }
            }else{
                char buff[256];
                sprintf(buff, "%s_%08d", prefix, count);
                save_image(disp, buff);
            }

            pthread_join(fetch_thread, 0);
            pthread_join(detect_thread, 0);

            double after = get_wall_time();
            //float curr = 1./(after - before);
            float curr = 1./(after - before);
            fps = curr;
            before=get_wall_time();


            if(delay == 0){
                free_image(disp);
                disp  = det;
            }
            show_image(disp, "Demo");
                cvWaitKey(10);

            det   = in;
            det_s = in_s;
        }
        else if (0){

        niddle=get_wall_time();
        src=NULL;
        src=cvQueryFrame(cap);
        cvNamedWindow("src", CV_WINDOW_NORMAL); 
        cvShowImage("src", src);
        cvWaitKey(1);
        printf("cvQueryFrame:%f\n",get_wall_time()-niddle);

        niddle=get_wall_time();

                //fetch_in_thread(0);
        if (!src) 
        {
            printf("src is null\n");
           in= make_empty_image(0,0,0);
        }
        image im;
//printf("%d", ipl_to_image((IplImage *)src));
       // im=ipl_to_image((IplImage *)src);
#if 0
        image im = ipl_to_image(src);

        rgbgr_image(im);

        in = im;
    if(!in.data){
        error("Stream closed.");
    }
#endif
    in_s = resize_image(in, net.w, net.h);

                det   = in;
                det_s = in_s;
printf("fetch_in_thread:%f\n",get_wall_time()-niddle);

niddle=get_wall_time();
show_image(in, "in");
cvWaitKey(1);
printf("show_image:%f\n",get_wall_time()-niddle);

niddle=get_wall_time();
                detect_in_thread(0);
printf("detect_in_thread:%f\n",get_wall_time()-niddle);
                
                double after = get_wall_time();
                float curr = 1./(after - before);
                fps = curr;
                before=get_wall_time();

                if(delay == 0) {
                    free_image(disp);
                    disp = det;
                }
                show_image(disp, "Demo");
                cvWaitKey(10);
        }
        else if(0)
        {
fetch_in_thread(0);
det   = in;
                det_s = in_s;
detect_in_thread(0);
double after = get_wall_time();
                float curr = 1./(after - before);
                fps = curr;
                before=get_wall_time();

                if(delay == 0) {
                    free_image(disp);
                    disp = det;
                }
                show_image(disp, "Demo");
                cvWaitKey(1);
        }

#if 0
        else
        {
            fetch_in_thread(0);
            det   = in;
            det_s = in_s;
            if(detect_thread_run==0)
            {
                pthread_join(detect_thread, 0);
                pthread_create(&detect_thread, 0, detect_in_thread, 0);
                
                
            }
             show_image(det, "Det");
             cvWaitKey(1);
                
            printf("detect_thread_finished:%d\n",detect_thread_finished);


            if(detect_thread_finished==1)
            {
                double after = get_wall_time();
                float curr = 1./(after - before);
                fps = curr;
                before=get_wall_time();

                free_image(disp);
                disp = det;
               
                show_image(disp, "Demo");
                cvWaitKey(1);

                pthread_mutex_lock(&lock_t);
                detect_thread_finished=0;
                pthread_mutex_unlock(&lock_t);
                
            }

        }
#endif
    }
    
}
#else
void demo(char *cfgfile, char *weightfile, float thresh, int cam_index, const char *filename, char **names, int classes, int frame_skip, char *prefix, float hier_thresh)
{
    fprintf(stderr, "Demo needs OpenCV for webcam images.\n");
}
#endif

