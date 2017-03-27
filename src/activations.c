#include "activations.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_activation_string(ACTIVATION a)
{
    switch(a){
        case LOGISTIC:
            return "logistic";
        case LOGGY:
            return "loggy";
        case RELU:
            return "relu";
        case ELU:
            return "elu";
        case RELIE:
            return "relie";
        case RAMP:
            return "ramp";
        case LINEAR:
            return "linear";
        case TANH:
            return "tanh";
        case PLSE:
            return "plse";
        case LEAKY:
            return "leaky";
        case STAIR:
            return "stair";
        case HARDTAN:
            return "hardtan";
        case LHTAN:
            return "lhtan";
        default:
            break;
    }
    return "relu";
}

ACTIVATION get_activation(char *s)
{
    if (strcmp(s, "logistic")==0) return LOGISTIC;
    if (strcmp(s, "loggy")==0) return LOGGY;
    if (strcmp(s, "relu")==0) return RELU;
    if (strcmp(s, "elu")==0) return ELU;
    if (strcmp(s, "relie")==0) return RELIE;
    if (strcmp(s, "plse")==0) return PLSE;
    if (strcmp(s, "hardtan")==0) return HARDTAN;
    if (strcmp(s, "lhtan")==0) return LHTAN;
    if (strcmp(s, "linear")==0) return LINEAR;
    if (strcmp(s, "ramp")==0) return RAMP;
    if (strcmp(s, "leaky")==0) return LEAKY;
    if (strcmp(s, "tanh")==0) return TANH;
    if (strcmp(s, "stair")==0) return STAIR;
    fprintf(stderr, "Couldn't find activation function %s, going with ReLU\n", s);
    return RELU;
}

float activate(float x, ACTIVATION a)
{
    switch(a){
        case LINEAR:
            return linear_activate(x);
        case LOGISTIC:
            return logistic_activate(x);
        case LOGGY:
            return loggy_activate(x);
        case RELU:
            return relu_activate(x);
        case ELU:
            return elu_activate(x);
        case RELIE:
            return relie_activate(x);
        case RAMP:
            return ramp_activate(x);
        case LEAKY:
            return leaky_activate(x);
        case TANH:
            return tanh_activate(x);
        case PLSE:
            return plse_activate(x);
        case STAIR:
            return stair_activate(x);
        case HARDTAN:
            return hardtan_activate(x);
        case LHTAN:
            return lhtan_activate(x);
    }
    return 0;
}

#if 1
void activate_array(float *x, int n, ACTIVATION a)
{
    int i;
    switch(a){
        case LINEAR:
#pragma omp parallel for num_threads(4)
            for(i = 0; i < n; ++i){
                x[i] = linear_activate(x[i]);
            }
            break;
        case LOGISTIC:
#pragma omp parallel for num_threads(4)
            for(i = 0; i < n; ++i){
                x[i] = logistic_activate(x[i]);
            }
            break;
        case LOGGY:
#pragma omp parallel for num_threads(4)
            for(i = 0; i < n; ++i){
                x[i] = loggy_activate(x[i]);
            }
            break;
        case RELU:
#pragma omp parallel for num_threads(4)
            for(i = 0; i < n; ++i){
                x[i] = relu_activate(x[i]);
            }
            break;
        case ELU:
#pragma omp parallel for num_threads(4)
            for(i = 0; i < n; ++i){
                x[i] = elu_activate(x[i]);
            }
            break;
        case RELIE:
#pragma omp parallel for num_threads(4)
            for(i = 0; i < n; ++i){
                x[i] = relie_activate(x[i]);
            }
            break;
        case RAMP:
#pragma omp parallel for num_threads(4)
            for(i = 0; i < n; ++i){
                x[i] = ramp_activate(x[i]);
            }
            break;
        case LEAKY:
#pragma omp parallel for num_threads(4)
//#pragma omp simd
            for(i = 0; i < n; ++i){
                x[i] = leaky_activate(x[i]);
            }
            break;
        case TANH:
#pragma omp parallel for num_threads(4)
            for(i = 0; i < n; ++i){
                x[i] = tanh_activate(x[i]);
            }
            break;
        case PLSE:
#pragma omp parallel for num_threads(4)
            for(i = 0; i < n; ++i){
                x[i] = plse_activate(x[i]);
            }
            break;
        case STAIR:
#pragma omp parallel for num_threads(4)
            for(i = 0; i < n; ++i){
                x[i] = stair_activate(x[i]);
            }
            break;
        case HARDTAN:
#pragma omp parallel for num_threads(4)
            for(i = 0; i < n; ++i){
                x[i] = hardtan_activate(x[i]);
            }
            break;
        case LHTAN:
#pragma omp parallel for num_threads(4)
            for(i = 0; i < n; ++i){
                x[i] = lhtan_activate(x[i]);
            }
            break;
    }
}

#else
void activate_array(float *x, const int n, const ACTIVATION a)
{
    int i;
#pragma omp parallel for num_threads(4)
#pragma omp simd
    for(i = 0; i < n; ++i){
        x[i] = activate(x[i], a);
    }
}
#endif

float gradient(float x, ACTIVATION a)
{
    switch(a){
        case LINEAR:
            return linear_gradient(x);
        case LOGISTIC:
            return logistic_gradient(x);
        case LOGGY:
            return loggy_gradient(x);
        case RELU:
            return relu_gradient(x);
        case ELU:
            return elu_gradient(x);
        case RELIE:
            return relie_gradient(x);
        case RAMP:
            return ramp_gradient(x);
        case LEAKY:
            return leaky_gradient(x);
        case TANH:
            return tanh_gradient(x);
        case PLSE:
            return plse_gradient(x);
        case STAIR:
            return stair_gradient(x);
        case HARDTAN:
            return hardtan_gradient(x);
        case LHTAN:
            return lhtan_gradient(x);
    }
    return 0;
}

void gradient_array(const float *x, const int n, const ACTIVATION a, float *delta)
{
    int i;
    for(i = 0; i < n; ++i){
        delta[i] *= gradient(x[i], a);
    }
} 

