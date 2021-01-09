//
//  insimou.cpp
//  insimou
//
//  Created by Benedikt privat on 09.01.21.
//

#include <iostream>
#include "insimou.hpp"
#include "insimouPriv.hpp"

//#include <Python.h>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

#define NUM_THREADS 4
bool shared_exitflag = false;
float neuroninput[5] = {0,0,0,0,0};
float neuronoutput[5] = {0,0,0,0,0};
float weight[4] ={0,0,0,0};
float analogsignal = {0};
std::chrono::high_resolution_clock::time_point begin_time;
std::mutex mtx;

struct thread_data {
    int  thread_id;
    std::string message;
};

void *in_loop(){
    int i = 0;
    int totalCycles = 0;
    while(!shared_exitflag){
        i = (i+1) % 5;
        neuroninput[i] += 2;
        totalCycles++;
    }
    mtx.lock();
    cout << "inout cycle num: "<< totalCycles<<endl;
    float duration = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - begin_time ).count();
    std::cout << totalCycles/duration<<"/us"<<endl;
    mtx.unlock();
    pthread_exit(NULL);
}

void *simulate_loop(){
    int i = 0;
    int totalCycles = 0;
    while(!shared_exitflag){
        totalCycles++;
        i = totalCycles % 5;
        neuroninput[i] /= weight[i];
        neuronoutput[i] = neuroninput[i]*2;
    }
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    mtx.lock();
    cout << "Simulation: "<< totalCycles<<endl;
    float duration = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - begin_time ).count();
    std::cout << totalCycles/duration<<"/us"<<endl;
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
    std::cout << totalCycles/duration<<"/us"<<endl;
    mtx.unlock();
    pthread_exit(NULL);
}

void *feedback_loop(){
    int i = 0;
    int totalCycles = 0;
    while(!shared_exitflag){
        totalCycles++;
        i = totalCycles % 5;
        weight[i] = analogsignal;
    }
    mtx.lock();
    cout << "Out Loop: "<< totalCycles<<endl;
    float duration = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - begin_time ).count();
    std::cout << totalCycles/duration<<"/us"<<endl;
    mtx.unlock();
    pthread_exit(NULL);
}


void insimou::HelloWorld(const char * s)
{
    insimouPriv *theObj = new insimouPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void insimouPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

void insimou::start(){
    std::cout << "Hello world" << std::endl;
};

    
extern "C"
{
  void start(){
      std::thread* threads[NUM_THREADS];
      struct thread_data td[NUM_THREADS];
      //int rc;
      int i;
      
      begin_time = std::chrono::high_resolution_clock::now();
      for( i = 0; i < NUM_THREADS; i++ ) {
          cout <<"main() : creating thread, " << i << endl;
          td[i].thread_id = i;
          td[i].message = "This is message";
          if (i==0) {
              threads[i] = new std::thread(in_loop);
          } else if(i==1){
              threads[i] = new std::thread(simulate_loop);
          } else if(i==2){
              threads[i] = new std::thread(out_loop);
          } else {
              threads[i] = new std::thread(feedback_loop);//, PrintHello, std::ref(td[i])
          }
          
          //        if (rc) {
          //            cout << "Error:unable to create thread," << rc << endl;
          //            exit(-1);
          //        }
      }
      std::this_thread::sleep_for (std::chrono::milliseconds(200));
      shared_exitflag=true;//stops the threads
      for( i = 0; i < NUM_THREADS; i++ ) {
          threads[i]->join();
      }
      

      for (int i=0; i < 5; i++) {
          cout << neuroninput[i] << "  ";
      }
      cout <<endl;
      for (int i=0; i < 5; i++) {
          cout << neuronoutput[i] << "  ";
      }
      cout <<endl;

      pthread_exit(NULL);
  }
}
