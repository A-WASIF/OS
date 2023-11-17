#include <iostream>
#include <list>
#include <functional>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <chrono>
#include <tuple>
#include <vector>
using namespace std;
//Function prototypes

//implementation of a simple parrele for loop for a 1D range
void* thread_function(void* arg);
// Implementaion for a parrelel for loop for a 2D range
void* thread_function_2D(void* arg);

// Parrelel for loop implementation for a 1D range 
void parallel_for(int low, int high, std::function<void(int)> lambda, int numThreads);

//Parrelel for loop implementation for a 2D range 
void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)> lambda, int numThreads);

//implementation of a simple parrele for loop for a 1D range
void* thread_function(void* arg)
{
    auto params = *static_cast<std::tuple<int, int, std::function<void(int)>>*>(arg);
    int start = std::get<0>(params);
    int end = std::get<1>(params);
    auto lambda = std::get<2>(params);

    for (int i = start; i <= end; ++i)
    {
       
        lambda(i);
    }
    pthread_exit(nullptr);
}
// Parrelel for loop implementation for a 1D range 
void parallel_for(int low, int high, std::function<void(int)> lambda, int numThreads)
{
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<pthread_t> threads(numThreads);

    // Create threads and assign tasks
    for (int i = 0; i < numThreads; ++i)
    {
        int chunkSize = (high - low + 1) / numThreads;
        int start = low + i * chunkSize;
        int end = (i == numThreads - 1) ? high : start + chunkSize - 1;
         if(i==numThreads-1){
            end=high;
            
        }

        auto params = new std::tuple<int, int, std::function<void(int)>>(start, end, lambda);
        int result=pthread_create(&threads[i], nullptr, &thread_function, params);
        if (result != 0) {
            std::cerr << "Error during making thread: " << strerror(result) << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // Join threads
    for (auto& thread : threads)
    {
        pthread_join(thread, nullptr);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Total execution time: " << elapsed.count() << " seconds\n";
}
// Implementaion for a parrelel for loop for a 2D range
void* thread_function_2D(void* arg)
{
    auto params = *static_cast<std::tuple<int, int, int, int, std::function<void(int, int)>>*>(arg);
    int start1 = std::get<0>(params);
    int end1 = std::get<1>(params);
    int low2 = std::get<2>(params);
    int high2 = std::get<3>(params);
    auto lambda = std::get<4>(params);
    // cout<<start1<<end1<<low2<<high2<<endl;
    // cout<<start1<<" : "<<end1<<endl;
    //  std::cout <<"start1: " <<start1 << ", " << end1<<endl;

    for (int i = start1; i < end1; i++)
    {
       
        for (int j = low2; j < high2; j++)
        {
            //std::cout <<"i:"<<i<<"j" <<j << ",high2 " << high2<<endl;
            
            lambda(i, j);
        }
    }
    pthread_exit(NULL);
}
//// Parrelel for loop implementation for a 2D range 
void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)> lambda, int numThreads)
{
    //std::cout << low1<< ", " << high1<<"\n";
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<pthread_t> threads(numThreads);
    //cout<<numThreads<<endl;

    for (int i = 0; i < numThreads; ++i)
    {
        int chunkSize1 = (high1 - low1) / numThreads;
        //std::cout << "chunck"<<chunkSize1<<"\n";
        int start1 =i*chunkSize1;
       
        int end1 =(i+1)*chunkSize1;
         if(i==numThreads-1){
            end1=high1;
            
        }
        //std::cout << start1 << ", " << end1<<"\n";
        

        auto params = new std::tuple<int, int, int, int, std::function<void(int, int)>>(start1, end1, low2, high2, lambda);

        int result=pthread_create(&threads[i], NULL, &thread_function_2D, params);
        if (result != 0) {
            std::cerr << "Error during making thread: " << strerror(result) << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    // cout<<"created threads"<<endl;

    // Join threads
    for (auto& thread : threads)
    {

        pthread_join(thread, nullptr);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Total execution time: " << elapsed.count() << " seconds\n";
}
//Entry point for the user's main function. 
int user_main(int argc, char **argv);

/* Demonstration on how to pass lambda as parameter.
 * "&&" means r-value reference. You may read about it online.
 */
void demonstration(std::function<void()> && lambda) {
  lambda();
}

//Main function
int main(int argc, char **argv) {
  /* 
   * Declaration of a sample C++ lambda function
   * that captures variable 'x' by value and 'y'
   * by reference. Global variables are by default
   * captured by reference and are not to be supplied
   * in the capture list. Only local variables must be 
   * explicity captured if they are used inside lambda.
   */
  int x=5,y=1;
  // Declaring a lambda expression that accepts void type parameter
  auto /*name*/ lambda1 = /*capture list*/[/*by value*/ x, /*by reference*/ &y](void) {
    /* Any changes to 'x' will throw compilation error as x is captured by value */
    y = 5;
    std::cout<<"====== Welcome to Assignment-"<<y<<" of the CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  // Executing the lambda function
  demonstration(lambda1); // the value of x is still 5, but the value of y is now 5

  int rc = user_main(argc, argv);
 
  auto /*name*/ lambda2 = [/*nothing captured*/]() {
    std::cout<<"====== Hope you enjoyed CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  demonstration(lambda2);
  return rc;
}

#define main user_main