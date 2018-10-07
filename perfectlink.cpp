//
//  perfectlink.cpp
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 02/10/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#include "perfectlink.h"
#include "common.h"
#include <pthread.h>
#include <iostream>
#include <mutex>
#include <cstring>

using namespace std;

unsigned const MSG_SIZE = 1000;
unsigned const BUF_SIZE = 1000;
unsigned const TIMEOUT = 5e6; //ms

mutex mtx;

struct thread_data {
    UDPSender* s;
    UDPReceiver* r;
    map<int, char*>* msgs;
};

void* receive(void *argthreads) {
    struct thread_data *args = (struct thread_data *)argthreads;
    UDPSender* s = args->s;
    UDPReceiver* r = args->r;
    map<int, char*>* msgs = args->msgs;
    
    // buffer for receiving messages
    char buf[BUF_SIZE];

    cout << "* Receiving thread is running" << endl;
    while(true){
        int len = r->receive(buf, MSG_SIZE);
        
        // receiving an ACK from a sent message
        if(len == 7 && memmem(buf + 4, 3, "ACK", 3))
        {
            int seqnumack = charsToInt32(buf);
            cout << "** Received ACK " << seqnumack << endl;
            
            //TODO re
            
            mtx.lock();
            msgs->erase(seqnumack);
            mtx.unlock();
           
        } // receiving content from another process => we send an ACK
        else {
            int tmp = charsToInt32(buf);
            cout << "** Received content " << tmp << endl;
            char* sdata = (char*) malloc(7);
            memcpy(sdata, buf, 4);
            memcpy(sdata + 4, "ACK", 3);
            cout << "** Sending ACK for " << tmp << endl;
            s->send(sdata, 7);
        }
        
    }
    pthread_exit(NULL);
}

PerfectLink::PerfectLink(UDPSender* s, UDPReceiver* r, int m)
{
    this->s = s;
    this->r = r;
    this->m = m;
    this->seqnum = 0;
    this->window = 10;
}

void PerfectLink::send()
{
    
    pthread_t threadid;
    struct thread_data threadargs;
    threadargs.s = this->s;
    threadargs.r = this->r;
    threadargs.msgs = &this->msgs; // note the pointer not to mess with multiple lists
    
    // Running the receiving side of a different thread. Passing threadargs as params
    // receive is not a class function
    if (pthread_create(&threadid, NULL, receive, (void *)&threadargs)) {
        cout << "Error: unable to create thread" << endl;
        exit(-1);
    }
    
    
    while(this->seqnum != this->m){
        // Craft WINDOW new messages if list is empty
        if (this->msgs.size() == 0){
            int lb = this->seqnum;
            int ub = this->seqnum + this->window;
            for(int i = lb; i < ub && i < this->m; i++){
                cout << "> Crafting message " << this->seqnum << endl;
                craftAndStoreMsg();
            }
        }
        // Send all messages if ACK missing
            map<int, char*>::iterator it;
            mtx.lock();
            for (it = this->msgs.begin(); it != this->msgs.end(); it++) {
                int tmp = (*it).first;
                char* sdata = (*it).second;
                this->s->send(sdata, MSG_SIZE + 4);
                cout << "> Sending " << tmp << endl;
            }
            mtx.unlock();

        waitForAcksOrTimeout();

    }
}


void PerfectLink::waitForAcksOrTimeout(){
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    while(this->msgs.size() != 0 ){
        long a = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - begin).count();
        if (a > TIMEOUT){break;}
    }
}

void PerfectLink::craftAndStoreMsg(){
    // allocating new memory
    char* data = (char*) malloc(MSG_SIZE + 4);
    
    // adding the sequence number
    int32ToChars(this->seqnum, data);

    // adding the message to the list
    mtx.lock();
    this->msgs[this->seqnum] = data;
    mtx.unlock();

    this->seqnum++;
}