#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <sys/time.h>

//#include "proj2.h"
#include "utilities.cpp"
#include <bitset>

using namespace std;

string keyword;
string file_string;
long count;


int main(int argc, char* argv[])
{
    // =============== SETUP =============== 
    int rc; // return code for pthread

    char* filename = NULL; // name of the input file
    // filename must be specified. parse_argv will check if filename is
    // given or not
    int threadcount = DEFAULT_THREAD_COUNT; // number of threads 
    chunksize = DEFAULT_CHUNK_SIZE;         // size of the chunk 
    verbose = DEFAULT_VERBOSE_FLAG;
    // default values are defined in proj2.h

    count = 0; // number of keyword found in the file

    struct timeval start_time, end_time, diff;
    // for measuring running time

    // parsing arguments
    if(parse_argv(argc, argv, threadcount, 
                  &filename, keyword, chunksize) == false) {
        cout << endl;
        help_message(argv);
        exit(EXIT_FAILURE);
    }
    // end of parsing arguments

    // read the content of the file and store it as file_string
    if(read_file(filename, file_string) == false) {
        cout << "Unable to read file. Something is wrong." << endl;
        exit(EXIT_FAILURE);
    }
    // end of reading file

    // Make a simple summary of the arguments gotten
    if(verbose) {
        cout << "Counting the occurrence of " << keyword << endl;
        cout << "Chunk size:                " << chunksize << endl;
        cout << "Number of threads:         " << threadcount << endl;
        cout << endl;
    }
    // =============== END OF SETUP =============== 

    gettimeofday(&start_time, 0); // Record the start time

    gettimeofday(&end_time, 0); // Record the end time

    timeval_diff(diff, start_time, end_time); // Get the difference of 
                                              // start and end time
    print_stats(diff, threadcount, chunksize, count, keyword);
}

void thread_create (unsigned long chunksiz )
{
  unsigned long start = global_pos;
  unsigned long limit = global_pos + chunksize;
  for (unsigned long offset = file_string.find(keyword, start);
      ((offset != std::string::npos) && (offset < limit));
      offset = file_string.find(keyword, offset + keyword.length()))
  {
    local_count ++;
  }

}
