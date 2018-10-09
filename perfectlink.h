//
//  perfectlink.h
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 02/10/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#ifndef perfectlink_h
#define perfectlink_h
#include "sender.h"
#include "receiver.h"
#include <stdio.h>
#include <list>
#include <mutex>
#include <map>

using std::mutex;
using std::map;
using std::pair;

/** @class This class implements the Perfect Link */
class PerfectLink : public Sender, public Receiver, public Target
{
private:
    map<int, pair<int, char*> > msgs;

    /** @brief Object for sending data to other host */
    Sender* s;

    /** @brief Object for receiving data from the other host */
    Receiver* r;

    /** @brief Current sequence number? Receive or send? */
    int seqnum;

    /** @brief ??? */
    int window;

    /** @brief Add message to send loop */
    void craftAndStoreMsg(char *buffer, int length);

    /** @brief Wait for an ACK */
    void waitForAcksOrTimeout();

    /** @brief Timeout for 1 message in microseconds (1e-6 sec) */
    unsigned const TIMEOUT = 5000000;

    /** @brief A mutex to be used by the PerfectLink class */
    mutex mtx;

    void onMessage(unsigned source, char* buffer, unsigned length);
public:
    /**
     * @brief Establish a perfect link
     * @param s A UDPSender targeted at a host
     * @param r A UDPReceiver obtaining data from the same host
     */
    PerfectLink(Sender *s, Receiver *r, Target *target = nullptr);

    /** @brief Send data */
    void send(char *buffer, int length);
};

#endif /* perfectlink_h */
