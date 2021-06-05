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
std::thread* threads[NUM_THREADS];
bool running = false;
NumericBackend* backend = nullptr;
bool shared_exitflag = false;
int observation_dims = 4;
float feedback = 0;
std::vector<float> observations;
float neuronoutput[OUTPUTDIM] = {0};//todo initialize when back-end has returned firstoutput
float analogsignal = {0};
std::chrono::high_resolution_clock::time_point begin_time;
std::mutex mtx;

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
   if (backend == 0){
       //todo get values from python
       float arr[] = {-2.4,-3,-0.209,-4};
       // Initialize vector with a string array
       std::vector<float>  min(arr, arr + sizeof(arr)/sizeof(float));
       float arr_max[] = {2.4,3,0.209,4};
       std::vector<float>  max(arr_max, arr_max + sizeof(arr_max)/sizeof(float));
       float arr_num[] = {7,7,15,15};
       std::vector<int> num_neurons_dim(arr_num, arr_num + sizeof(arr_num)/sizeof(float));
       backend = new NumericBackend(min, max, num_neurons_dim);
   }
   return backend;
}


void *simulate_loop(){
    long totalCycles = 0;
    auto backend = getNumericBackend();
    while(!shared_exitflag){
        ++totalCycles;
        backend->coreloop();
    }
    //why sleep a second for exiting?
    //std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    
    //exit
    mtx.lock();
    float duration = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - begin_time ).count();
    std::cout << totalCycles<<" cycles / "<<duration/1000<<" ms = "<< totalCycles/duration<<" cycles/us"<<endl;
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
        printstats();
        if (mainthread != nullptr){
            mainthread->join();
        }
    } else {
        cout <<"Engine has not yet started." << endl;
    }
}

void setinput_async(float observation[], int lenobs){
    //will return after spawning background thread
    //std::thread thread = std::thread(setinput_thread, observation, lenobs);
    //do not wait
    //thread.detach();
}

void setinput(float observation[], int lenobs){
    //blocking
    observation_dims = lenobs;
    observations = std::vector<float>(lenobs);
    for (int i=0;i<lenobs;++i){
        observations[i] = observation[i];
    }
    getNumericBackend()->setObservation(observation,lenobs);
}

void give_reward(float reward){
    feedback = reward;
    if (NUM_THREADS <= 1){
        getNumericBackend()->setFeedback(feedback);
    }
}

float* getAction(){
    return getNumericBackend()->getActions();
}

//std::array<float, INPUTDIM>
float* getWeights(){
    std::cout<< "GETWEIGHTS" <<std::endl;
    //cannot get the std::array object and get the pointer with data() here (local?)
    return getNumericBackend()->getWeights();
}

int main ( int argc, char *argv[] ) {
    //std::this_thread::sleep_for (std::chrono::milliseconds(500));
    float obs[4] = {3,100,4.4,12};
    setinput(obs, 4);
    start_sync();
    usleep(200);
    printstats();
    
    getAction();
    getAction();
    getAction();
    //    //std::this_thread::sleep_for (std::chrono::milliseconds(5));
    stop();
}
} //end extern C
