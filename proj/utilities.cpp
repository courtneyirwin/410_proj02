#include <string.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <deque>

using namespace std;

#define DEFAULT_THREAD_COUNT    10
#define DEFAULT_CHUNK_SIZE     200
#define DEFAULT_VERBOSE_FLAG false

unsigned long chunksize;
bool verbose;

/*
 * purpose: display to the users how to run this program
 *          also show users the available paramters
 * receive: the arguments
 * return:  none 
 */
void help_message(char *argv[]) {
    cout << "Usage " << argv[0] << " [parameters]" << endl;
    cout << "The following parameters are required:" << endl;
    cout << "      -f filename      Specify the input filename." << endl;
    cout << "      -k keyword       Specify the keyword to be counted." 
         << endl;
    cout << "The following parameters are optional:" << endl;
    cout << "      -t threadcount   Specify the total number of threads."
         << endl;
    cout << "                       Default value is " 
         << DEFAULT_THREAD_COUNT << "." << endl;
    cout << "      -c chunksize     Specify the chunk size."
         << endl
         << "                       Default value is " 
         << DEFAULT_CHUNK_SIZE << "." << endl;
    cout << "      -v               Verbose flag. Default value is false"
         << endl;
    cout << "      -h               Display help message." << endl;
}

/*
 * purpose: parse the parameters
 * receive: the arguments argv and argc
 * return:  threadcount - the number of threads
 *          filename - input filename
 *          chunksize - size of the chunk
 *          keyword - the keyword to be counted
 */
bool parse_argv(int argc, char* argv[], int& threadcount, 
                char** filename, string& keyword, unsigned long& chunksize) {
    char *endptr; // for strtol

    for(int i = 1; i < argc; i++) {
        if((!strncmp(argv[i], "-t", 2)) ||
           (!strncmp(argv[i], "-T", 2))) {
            if((i + 1) >= argc) {
                cerr << "Invalid thread count value. " 
                     << "There must be an interval vlaue after -t"
                     << endl;
                return false;
            }
            threadcount = strtol(argv[++i], &endptr, 0);
            if(*endptr || threadcount <= 0) {// Invalid interval value
                cerr << "Invalid thread count value. ";
                cerr << "Thread count must be a positive integer." << endl;
                return false;
            }
        }
        else if((!strncmp(argv[i], "-f", 2)) ||
                (!strncmp(argv[i], "-f", 2))) {
            if((i + 1) >= argc){
                cerr << "Invalid input filename." 
                     << "There must be an input filename after -f"
                     << endl;
                return false;
            }
            *filename = argv[++i];
        }
        else if((!strncmp(argv[i], "-k", 2)) ||
                (!strncmp(argv[i], "-k", 2))) {
            if((i + 1) >= argc) {
                cerr << "Invalid target keyword. " 
                     << "A keyword must be given after -k"
                     << endl;
                return false;
            }
            keyword = argv[++i];
        }
        else if((!strncmp(argv[i], "-c", 2)) ||
                (!strncmp(argv[i], "-C", 2))) {
            if((i + 1) >= argc){
                cerr << "Invalid chunk size." 
                     << "The chunk size must be specified after -c"
                     << endl;
                return false;
            }
            chunksize = strtol(argv[++i], &endptr, 0);
            if(*endptr || chunksize <= 0) { // Invalid chunk size
                cerr << "Invalid chunk size value. ";
                cerr << "chunk size must be a positive integer." << endl;
                return false;
            }
        }
        else if((!strncmp(argv[i], "-v", 2)) ||
                (!strncmp(argv[i], "-v", 2))) {
            verbose = true;
        }
        else if((!strncmp(argv[i], "-h", 2)) ||
                (!strncmp(argv[i], "-H", 2))) {
            return false;
        }
        else { // all other paramters are not allowed 
            cerr << "Invalid parameter: " << argv[i] << endl; 
            return false;
        }
    }
    // Make sure we get all we need.
    if(threadcount <= 0) {
        cerr << "Invalid thread count value. ";
        cerr << "Thread count must be a positive integer." << endl;
        return false;
    }
    if(*filename == NULL) {
        cerr << "Invalid input filename. " 
             << "An input filename must be specified using -f" << endl;
        return false;
    }
    if(keyword == "") {
        cerr << "Invalid keyword. "
             << "A keyword must be given to be counted using -k" << endl;
        return false;
    }
    return true;
}


/*
 * purpose: compute the difference of two time values 
 * receive: from, the starting time
 *          to, the ending time
 * returns: result: the difference between from and to
 */
void timeval_diff (struct timeval &result, 
                   const struct timeval &from, 
                   struct timeval &to) {
    /* Perform the carry for the later subtraction by updating y. */
    result.tv_usec = (to.tv_usec - from.tv_usec);
    result.tv_sec  = (to.tv_sec - from.tv_sec);
    if(result.tv_usec < 0) {
        result.tv_usec += 1000000;
        result.tv_sec -= 1;
    }
}

/*
 * purpose: display the time ot the console
 * receive: time - the time difference to be displayed
 *          threadcount - the number of threads
 *          chunksize - the size of the chunk
 *          count - the number of currence of keyword found
 *          keyword - the keyword to be searched
 * return:  none
 */
void print_stats(const timeval& time, const int& threadcount, const int& chunksize, const int& count, const string& keyword) {
    cout << "Found " << count << " \"" << keyword << "\"s\n";
    cout << "threadcount " << threadcount;
    cout << " chunksize " << chunksize;
    cout << " took ";
    cout << time.tv_sec << "." << setfill('0')
         << setw(6) << time.tv_usec << " s." << endl;
}



/*
 * purpose: read everything into memory, a string called file_string
 * receive: filename - input file name
 * return:  file_string - the content of the file
 *          true - if file is successfully read
 *          false - if failed to read the file
 */
bool read_file(const char* filename, string& file_string) {
    string line;
    ifstream myfile;
    myfile.open(filename, ios::in);
    if(myfile.is_open()) { // good
        if(verbose) {
            cout << filename << " is opened for reading." << endl;
        }
        while(getline(myfile, line)) {
            file_string += line;
        }
        return true;
    }   
    else {
        cerr << "Unable to open file: " << filename << endl;
        return false;
    }   
}
