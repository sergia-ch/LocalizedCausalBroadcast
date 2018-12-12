/** @file inmemorylog.h
 * @author Sergei Volodin, EPFL
 * @date October 2018
 * @brief This file implements the In Memory Log
 */

#ifndef INMEMORYLOG_H
#define INMEMORYLOG_H

class ImMemoryLog;

#include <string>
#include <fstream>
#include <mutex>

using std::string;
using std::ofstream;
using std::mutex;

/** @class This class implements the In Memory Log */
class InMemoryLog
{
private:

    /// Output file
    ofstream file;

    /// Output file with timestamps
    ofstream file_ts;

    /// Output file (immediate)
    ofstream file_immediate;

    /// Maximal number of messages
    static const int MAX_MESSAGES = 10000000;

    /// Current number of messages in the buffer
    volatile int messages = 0;

    /// Is function log() working now?
    volatile bool active;

    /// Buffer for data
    string *buffer;

    /// Buffer for data
    uint64_t *timestamps;

    /// Mutex for buffer access
    mutex m;

    /// @brief Process ID
    unsigned n;
public:
    /**
     * @brief InMemoryLog initializer
     * @param destination_filename The file to write
     */
    InMemoryLog(unsigned n, string destination_filename);

    /**
     * @brief log Logs a string
     * @param content The string to log
     */
    void log(string content);

    /**
     * @brief Dump all data to file from memory
     * Call from ONE thread only!
     */
    void dump();
};

#endif // INMEMORYLOG_H
