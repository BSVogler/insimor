//
//  main.cpp
//  insimou
//
//  Created by Benedikt privat on 27.12.20.
//

#ifdef INSIMOU_PYEXT
#include <Python.h>
#endif
#include <iostream>
#include <cstdlib>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <unistd.h>
#include "settings.h"
#include "numericBackend.hpp"


//this section is only for the extension
#ifdef INSIMOU_PYEXT
static PyObject *test(PyObject *self, PyObject *args){
    return (PyObject*)(1);
}
static PyMethodDef insimouEngineMethods[] = {
    {"start", test, METH_VARARGS, "Starts the engine"},
    {0,0,0,0}
};

static PyModuleDef insimouEngineModule = {
    PyModuleDef_HEAD_INIT, "insimou",
    "Perform fast RL-SNN calculations.", -1,
    insimouEngineMethods
};

PyMODINIT_FUNC PyInit_insimou(void) {
    return PyModule_Create(&insimouEngineModule);
}
#endif

//below c python
extern "C" {
using namespace std;

#define NUM_THREADS 1
long totalCycles = 0;
float duration=1;
std::thread* threads[NUM_THREADS];
bool running = false;
NumericBackend* backend = nullptr;
bool shared_exitflag = false;
int observation_dims = 4;
float feedback = 0;
std::vector<double> observations;
double neuronoutput[OUTPUTDIM] = {0};//todo initialize when back-end has returned firstoutput
double analogsignal = {0};
std::chrono::high_resolution_clock::time_point begin_time;
std::mutex mtx;
std::mutex newinputlock;
enum CompStatut{
    TOCOMP=0,
    DOING=1,
    DONE=2
};
short dirty=0;

struct thread_data {
    int  thread_id;
    std::string message;
};

void *PrintHello(void *threadarg) {
    //test/developer method?
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    
    //cout << "Thread ID : " << my_data->thread_id ;
    //cout << " Message : " << my_data->message << endl;
    
    pthread_exit(NULL);
}


//it is somehow nto possible to put this into the public namespace of the class
static NumericBackend* getNumericBackend(){
    //lazy constructor
    mtx.lock();
   if (backend == 0){
       //todo get values from python
       double arr[] = {-2.4,-3,-0.209,-4};
       // Initialize vector with a string array
       std::vector<double>  min(arr, arr + sizeof(arr)/sizeof(double));
       double arr_max[] = {2.4,3,0.209,4};
       std::vector<double>  max(arr_max, arr_max + sizeof(arr_max)/sizeof(double));
       double arr_num[] = {7,7,15,15};
       std::vector<int> num_neurons_dim(arr_num, arr_num + sizeof(arr_num)/sizeof(double));
       backend = new NumericBackend(min, max, num_neurons_dim);
   }
    mtx.unlock();
   return backend;
}


void *simulate_loop(){
    totalCycles = 0;
    auto backend = getNumericBackend();
    while(!shared_exitflag){
        ++totalCycles;
        if (dirty==TOCOMP) {
            newinputlock.try_lock();
            dirty=DOING;
        }
        backend->coreloop();
        if (dirty==DOING){//new input could overwrite this, we need a queue
            dirty=DONE;
            newinputlock.unlock();
        }
            
    }
    //why sleep a second for exiting?
    //std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    
    //exit, why is the mutex needed here?
    mtx.lock();
    duration = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - begin_time ).count();
    mtx.unlock();
    pthread_exit(NULL);
}

void mainthreadf(){
    //cout <<"Starting engine."<< endl;
    struct thread_data td[NUM_THREADS];
    //int rc;
    int i;
    
    begin_time = std::chrono::high_resolution_clock::now();
    for( i = 0; i < NUM_THREADS; ++i ) {
        //cout <<"main() : creating thread, " << i << endl;
        td[i].thread_id = i+1;
        td[i].message = "This is message";
        if (i==0) {
            threads[i] = new std::thread(simulate_loop);
        } else {
            
        }
    }
    running = true;
    //cout <<"Started engine."<< endl;
    //cout <<"Will sleep" << endl;
    //std::this_thread::sleep_for (std::chrono::milliseconds(200));
    //pthread_exit(NULL);
}

std::thread* mainthread = nullptr;
void start_async(){
    //async, will return instantly and spawn threads in background
    mainthread = new std::thread(mainthreadf);
}

//todo add parameter
void start_sync(){
    //synchronous, will spawn threads and after spawning return
    mainthreadf();
}

void printstats(){
    std::cout << totalCycles<<" cycles / "<<duration/1000<<" ms = "<< totalCycles/duration<<" cycles/us"<<endl;
    
    //print the observations. Test function for developers.
    cout << "Observations: ";
    for (int i=0; i < observations.size(); ++i) {
        cout << observations[i] << ", ";
    }
    cout <<endl;
    //    cout << "Outputs:"<<endl;
    //    for (int i=0; i < 5; ++i) {
    //        cout << neuronoutput[i] << ", ";
    //    }
    //    cout <<endl;
}

void stop(){
    if (running){
        shared_exitflag=true;//stops the threads
        for( int i = 0; i < NUM_THREADS; ++i ) {
            threads[i]->join();
        }
        if (mainthread != nullptr){
            mainthread->join();
        }
    } else {
        cout <<"Engine has not yet started." << endl;
    }
}

void setinput_async(double observation[], int lenobs){
    //will return after spawning background thread
    //std::thread thread = std::thread(setinput_thread, observation, lenobs);
    //do not wait
    //thread.detach();
}

void setinput(double observation[], int lenobs){
    //blocking
    observations.resize(lenobs);
    for (int i=0;i<lenobs;++i){
        observations[i] = observation[i];
    }
    observation_dims = lenobs;
    getNumericBackend()->setObservation(observation,lenobs);
    dirty = TOCOMP;
}

void setWeights(double weights[]){
    getNumericBackend()->setWeights(weights);
}

//this function sets the activation directly, when it is computed in python. Faster to set the input via setinput.
void setactivations(double activation[], int lenactivation){
    //blocking
    getNumericBackend()->setActivation(activation, lenactivation);
}

void give_reward(float reward){
    feedback = reward;
    if (NUM_THREADS <= 1){ //why only when limited to one thread?
        getNumericBackend()->setFeedback(feedback);
    }
}

float* getAction(){
    if (dirty!=DONE){
        newinputlock.lock();
    }
    auto actions = getNumericBackend()->getActions();
    newinputlock.unlock();
    return actions;

}

//std::array<float, INPUTDIM>
double* getWeights(){
    //cannot get the std::array object and get the pointer with data() here (local?)
    return getNumericBackend()->getWeights();
}

int main ( int argc, char *argv[] ) {
    //std::this_thread::sleep_for (std::chrono::milliseconds(500));
    double obs[4] = {3,100,4.4,12};
    setinput(obs, 4);
    start_sync();
    usleep(200);
    printstats();
    
    getAction();
    setinput(obs, 4);
    getAction();
    getAction();
    //    //std::this_thread::sleep_for (std::chrono::milliseconds(5));
    stop();
}
} //end extern C
