//
//  test.cpp
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 20/10/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#include "test.h"
#include <string>
#include <iostream>
#include <cassert>
#include <unistd.h>
#include "common.h"
#define private public
#include "perfectlink.h"
#include "udpsender.h"
#include "udpreceiver.h"
#include "membership.h"

using std::cout;
using std::endl;

//test if msgs is empty at the beginning
//and if sending empty buffers or nullptr
//is ignored
//void testPerfectLinkSend1(PerfectLink* l)
//{
//    assert(l->msgs.size() == 0);
//
//    char buffer[MAXLEN];
//    l->send(buffer, 0);
//    l->send(nullptr, 0);
//
//    assert(l->msgs.size() == 0 && l->seqnum == 0);
//
//}

//Sending some messages through a perfect link
//to be tested in log file afterwards
void runPerfectLink(PerfectLink* l, int nbr){
    
    char buffer[16];
    for (unsigned i = 0; i < nbr; i++){
        l->send(buffer, 16);
    }
}


void testPerfectLink(unsigned n, unsigned m, vector<PerfectLink*> links)
{
        
    int nMessages = m;

    vector<PerfectLink*>::iterator it;
    for(it = links.begin(); it != links.end(); it++)
    {
        runPerfectLink(*it, nMessages);
    }

}

void testUDP(unsigned n, vector<UDPSender *> senders, UDPReceiver *receiver)
{
    vector<UDPSender*>::iterator sender_;
    for(sender_ = senders.begin(); sender_ != senders.end(); sender_++)
    {
        UDPSender* sender = *sender_;
        char buf[MAXLEN - 10];
        buf[0] = 0x01;
        for(unsigned i = 0; i < 10000000; i++)
        {
            int32ToChars(i, buf + 1);
            sender->send(buf, MAXLEN - 10);
        }
    }
}

// number of threads for log test
const int n_threads = 10;

// number of messages per thread
const int n_msg = 100000;

// sleep us between messages
const int sleep_us = 10;

void* logger_thread(void* arg)
{
    // current thread number
    int threadid = *(int*) arg;

    // logs a lot of messages to the log
    char buf[100];

    // for 100 000 000 messages...
    for(int i = 0; i < n_msg; i++)
    {
      snprintf(buf, 99, "thread %d message %d", threadid, i);
      memorylog->log(buf);
      usleep(sleep_us);
    }

    return(nullptr);
}

void testLOG()
{
    // allocating memory
    pthread_t* threads = new pthread_t[n_threads];
    int* args = new int[n_threads];

    // sanity check
    assert(threads);
    assert(args);

    // saving number of threads/messages
    char buf[100];
    snprintf(buf, 99, "%d %d", n_threads, n_msg);
    memorylog->log(buf);

    printf("Will use %d threads each with %d messages, sleep %d us between messages\n", n_threads, n_msg, sleep_us);
    printf("Total log throughput: %d messages / sec\n", n_threads * 1000000 / sleep_us);
    printf("Expected time: %d seconds\n", sleep_us * n_msg / 1000000);

    // spawning threads
    for(int i = 0; i < n_threads; i++)
    {
        args[i] = i;
        pthread_create(&threads[i], nullptr, logger_thread, &args[i]);
    }

    // waiting for them to finish
    for(int i = 0; i < n_threads; i++)
        pthread_join(threads[i], nullptr);
    delete[] threads;
    delete[] args;
}
