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
#include <chrono>
#include <mutex>
#include <array>
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

#define NUM_THREADS 0
std::thread* threads[NUM_THREADS];
bool running = false;
NumericBackend* backend;
bool shared_exitflag = false;
int observationlength = 5;
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
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    
    //cout << "Thread ID : " << my_data->thread_id ;
    //cout << " Message : " << my_data->message << endl;
    
    pthread_exit(NULL);
}


void *simulate_loop(){
    int totalCycles = 0;
    while(!shared_exitflag){
        ++totalCycles;
        backend->coreloop();
    }
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    mtx.lock();
    cout << "Simulation: "<< totalCycles<<endl;
    float duration = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - begin_time ).count();
    std::cout << totalCycles<<"cycles /"<<duration<<"us ="<< totalCycles/duration<<"cycles/us"<<endl;
    mtx.unlock();
    pthread_exit(NULL);
}


void *out_loop(){
    int i = 0;
    int totalCycles = 0;
    while(!shared_exitflag){
        totalCycles++;
        i = totalCycles % 5;
        //this translates spikes to an analog signal
        analogsignal += 1;
    }
    mtx.lock();
    cout << "Out Loop: "<< totalCycles<<endl;
    float duration = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - begin_time ).count();
    std::cout << totalCycles<<"cycles /"<<duration<<"us ="<< totalCycles/duration<<"cycles/us"<<endl;
    mtx.unlock();
    pthread_exit(NULL);
}

void *feedback_loop(){
    cout <<"started thread feedback"<< endl;
    int i = 0;
    int totalCycles = 0;
    while(!shared_exitflag){
        backend->setFeedback(12);
        totalCycles++;
        i = totalCycles % 5;
    }
    mtx.lock();
    cout << "Out Loop: "<< totalCycles<<endl;
    float duration = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - begin_time ).count();
    std::cout << totalCycles<<"cycles /"<<duration<<"us ="<< totalCycles/duration<<"cycles/us"<<endl;
    mtx.unlock();
    pthread_exit(NULL);
}

void mainthreadf(){
    cout <<"Starting engine."<< endl;
    backend = new NumericBackend();
    struct thread_data td[NUM_THREADS];
    //int rc;
    int i;
    
    begin_time = std::chrono::high_resolution_clock::now();
    for( i = 0; i < NUM_THREADS; ++i ) {
        cout <<"main() : creating thread, " << i << endl;
        td[i].thread_id = i+1;
        td[i].message = "This is message";
        if (i==0) {
            threads[i] = new std::thread(simulate_loop);
        } else if(i==1){
            threads[i] = new std::thread(out_loop);
        } else if(i==2){
            threads[i] = new std::thread(feedback_loop);//, PrintHello, std::ref(td[i])
        } else {
            
        }
    }
    running = true;
    cout <<"Started engine."<< endl;
    //cout <<"Will sleep" << endl;
    //std::this_thread::sleep_for (std::chrono::milliseconds(200));
    //pthread_exit(NULL);
}

std::thread* mainthread = nullptr;
void start_async(){
    //async, will return instantly and spawn threads in background
    mainthread = new std::thread(mainthreadf);
}

void start_sync(){
    //synchronous, will spawn threads and after spawning return
    mainthreadf();
}

void printstats(){
    cout << "Observations: ";
    for (int i=0; i < observationlength; ++i) {
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
        cout <<"Will stop threads" << endl;
        shared_exitflag=true;//stops the threads
        for( int i = 0; i < NUM_THREADS; ++i ) {
            threads[i]->join();
        }
        printstats();
        if (mainthread != nullptr){
            mainthread->join();
        }
        cout << "Done";
    } else {
        cout <<"Engine has not yet started." << endl;
    }
}

void setinput_thread(float observation[], int lenobs){
    //this continuous loop select the neurons activation level based on the analog input signal
    //cout <<"Set input" << endl;
    //int lenobs = sizeof(observation)/sizeof(observation[0]);
    observationlength = lenobs;
    observations = std::vector<float>(lenobs);
    for (int i=0;i<lenobs;++i){
        observations[i] = observation[i];
        //cout <<observations[i] << ",";
    }
    //cout << endl;
    //todo enable when input=output
    //backend->setInput(observations, observationlength);
}

void setinput_async(float observation[], int lenobs){
    //will return after spawning background thread
    //std::thread thread = std::thread(setinput_thread, observation, lenobs);
    //do not wait
    //thread.detach();
}

void setinput(float observation[], int lenobs){
    //blocking
    setinput_thread(observation, lenobs);
}

void give_reward(float reward){
    feedback = reward;
}



float* getAction(){
//neuronoutput[0] = 7.0;
    return neuronoutput;
}

//std::array<float, INPUTDIM>
float* getWeights(){
    //cannot get the std::array object and get the pointer with data() here (local?)
    return backend->getWeights();
}

int main ( int argc, char *argv[] ) {
    //std::this_thread::sleep_for (std::chrono::milliseconds(500));
    float obs[3] = {3,100,4.4};
    setinput(obs,3);
    start_sync();
    printstats();
    getAction();
//  //  getAction();
////getAction();
//    //std::this_thread::sleep_for (std::chrono::milliseconds(5));
    stop();
}

}

