#include "broadcast.h"

void Broadcast::onMessage(unsigned source, char *buffer, unsigned length)
{
    // do nothing
}

void Broadcast::broadcast(char *message, unsigned length, unsigned source)
{
    // do nothing
}

Broadcast::Broadcast(unsigned this_process, vector<PerfectLink *> links) : Receiver(this_process)
{
    // saving perfect links
    this->links = links;
}

void Broadcast::broadcastPublic(char *message, unsigned length)
{
    // send message from this process
    broadcast(message, length, this_process);
}
