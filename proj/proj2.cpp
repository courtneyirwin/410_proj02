#include <iostream>
#include <string>
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
string sub, str;

using namespace std;

string keyword;
string file_string;
unsigned long global_pos = 0;
long count;
pthread_mutex_t mutexGlobal = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCount = PTHREAD_MUTEX_INITIALIZER;

void *countSubstring(void *tid) {
  // Create local count
  size_t offset;
  int local_count = 0;

  // While 
  while(global_pos < file_string.length()) {
    unsigned start = global_pos;
    unsigned limit = global_pos + chunksize;

    pthread_mutex_lock(&mutexGlobal);
    global_pos = limit; 
    pthread_mutex_unlock(&mutexGlobal);

    if (keyword.length() == 0) return 0;
    for (offset = file_string.find(keyword, start);
        ((offset != std::string::npos) && (offset < limit));
        offset = file_string.find(keyword, offset + keyword.length()))
    {
      cout << "FOUND " << endl;
      ++local_count;
    }

    //mutex lock
    pthread_mutex_lock(&mutexCount);
    count += local_count;
    //unlock
    pthread_mutex_unlock(&mutexCount);
  }
  pthread_exit(NULL);
}

void *printHello(void *tid) {
  cout << "Hello world" << endl;
  pthread_exit(NULL);
}



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

  pthread_t threads[threadcount];

  /* Create independent threads each of which will execute function */
  int status, i;

  for(i=0; i < threadcount; i++) {
    status = pthread_create(&threads[i], NULL, countSubstring, (void *)i);

    if (status != 0) {
      cout << "ERROR in thread creation" << endl;
      exit(-1);
    }
  }  

  // Loop through threads, set rc to pthreads joining
  for (int i = 0; i < threadcount; i++) {
    rc = pthread_join(threads[i], NULL);
  }

  gettimeofday(&end_time, 0); // Record the end time

  timeval_diff(diff, start_time, end_time); // Get the difference of 
  // start and end time
  print_stats(diff, threadcount, chunksize, count, keyword);
}
