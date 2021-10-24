// Parallel_Accumulate_2_implementation.cpp : This file contains the 'main' function. Program execution begins and ends there.
// 
// How to implement parallel version of accumulate algorithm, 
// 
// if the dataset is large enough, we will divide it into different chucks, 
// size of these is block_size, we are going to accumulate each of these data chunks in different rates
// and the results of accumulation called from each thread will be sored in a smaller area,
// then we will acumulate the result this from calling thread
// 
// Also notice: in initial accumulation, we will acumulate the final datablock, calling thread as well
// 
// But be careful that input dataset is large enough to divide, otherwise overhead of spawning new threads will extend execution time
// 
// 
// we will define minimum number of block size and spawn threads only to run number of elements more than
// 
// Ex: if minimum block size is 1000, and if dataset has only 800 elements, we wil not spawn any new threads,
// instead we will perform accumulation in calling thread, if the dataset has 5000 elements, we will spawn 
// 4 new threads, adn with the calling thread we have only 5 threads to perform 
// 
// 
//

#include <iostream>
#include <thread>
#include<algorithm>
#include<vector>
#include<numeric>
#include<functional>
//2. define the inimum block size
#define MIN_BLOCK_SIZE 1000


//10. have another function called accumulate with same template parameters and same argument list
template<typename iterator, typename T>
void accumulate(iterator start, iterator end, T& ref)
{
    //10a. in this case, notice the reference is not used as an initial value but as a data transfer machanism
    //10b. assign the value returned from std::accumulate() call to the reference we pass 
    ref = std::accumulate(start, end, 0);

    //10c. return to for loop
}


//1. create the paralel accumulator template
template<typename iterator, typename T>
T parallel_accumulate(iterator start, iterator end, T& ref)
{
    //3. find out the number of elements in input dataset
    int input_size = std::distance(start, end);

    //4. divide input size by minimum block size, store in int allowed_threads, to get the ammount of datablocks we need depending on the size
    int allowed_threads_by_elements = (input_size)/MIN_BLOCK_SIZE;

    //4a. Number of elements in the input is not the only factor affecting the number of data blocks we have.
    // we have to consider the number pf paralel threads allowed by OS as well.
    // ex: we may have set with 1,000,000 elements so if we calculate number of threads, depending only on our min block size,
    // we would have 1000 threads running in paralel, but the system may have 12 cores, and can run 12 threads in paralel,
    // so having more thread than that will force aplication to execute using ?tag searching? which will reduce
    // the execution efficiency.
    // so we need to take the hardware concurrency into account as well
    // 
    // 
    //

    //5. take the value of hardware_concurrency() and store it in the variable...

    int allowed_threads_by_hardware = std::thread::hardware_concurrency(); //in my case this number is 12 cores, but yours may be different.

    //6. the numebr of threads we have is the minimum value returned by hardware_concurrency(), and value we calculate using min_BLOCK_SIZE
    int num_threads = std::min(allowed_threads_by_elements, allowed_threads_by_hardware);

    //6a. this will eliminate the problem of spawning large amoutn of threads in case wa have a large dataset

    //7. After calculating the actual number of threads we will have, we need to calculate the data block size we 
    // are going to pass to each thread based on the number of threads
    //7a. divide (input_size + 1) by num_threads
    int block_size = (input_size + 1) / num_threads;

    //8. Now we need 2 vectors to store the threads we are going to launch, and the result calculated by each of those
    // threads.
    //8a. define the two vectors

    std::vector<T> results(num_threads);

    std::vector<std::thread> threads(num_threads - 1);// 8b. threads vector size is num_threads minus one, as we are 
                                                      // going to accumulate the final data block in the calling thread



    //9. now we have to launch the threads with the coresponding data blocks

    iterator last;  //9a. this is going to be the last element of each data block

    //9b. create a for loop
    for (int i = 0; i < num_threads - 1; i++)
    {   //in this loop we have to launch threads with correct data blocks

        //for first datablock , beginning will be start iterator, and we have to calculate the last iterator

        last = start;
        std::advance(last, block_size); //this increments the 'last' iterators position by block size ammount
        
        //9c. now we can launch a new thread with the start and alst iterators. inside each of the threads call 
        // std::accumulate() for given data block. 

        //9d. we have not learned how to transfer the return value from one thread to another, but we DO know
        // that if we pass reference to a new thread, any update will be recievable to the calling thread as well.

        //10. wrap the std::accumulate() function using the function which takes reference, and assign the value
        // calculated by the std::accumulate() function with the given data block to that reference


        //10d. we can lauch each thread with e accumulate function with correct data iterators.
        // we are going to sotre the result calculated in each thread in the result vector,
        // so we can pass the reference to the vector element with current index to accumulate function
        threads[i] = std::thread(accumulate<iterator, T>, start, last,
            std::ref(results[i]));

        // also we are going to store the newly created thread in the threads vector as well, so we can call join function afterwards

        //11for the next iteration, start ptr should be the end pointer for current iteration, so we can
        //assign last to start
        start = last;

    }


    //12. call accumulate() on last data block in this thread
    results[num_threads - 1] = std::accumulate(start, end, 0);

    //13. now we call join() function on all newly launched threads
    //13a. iterate through the threads vector and call the join function
    std::for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

    //all the accumulated results of each data block is now stored in the results vector
    //14. call accumulate on the result array to get the final result

    return std::accumulate(results.begin(), results.end(), ref);

}


int main()
{

    //15. in this function initialize random int array with 8000 elements

    const int size = 8000;
    int allowed_threads_by_hardware = std::thread::hardware_concurrency();
    //int num_threads = std::min(allowed_threads_by_elements, allowed_threads_by_hardware);


    int* my_array = new int[size];
    int ref = 0;

    int allowed_threads_by_elements = size / MIN_BLOCK_SIZE;
    int num_threads = std::min(allowed_threads_by_elements, allowed_threads_by_hardware);
    int block_size = (size + 1) / num_threads;


    


    //srand(0);

    //confirm the size of the elements.
    int i = 0;
    printf("Array elements: \n ");
    while (i <= size)
    {
        my_array[i] = i;
        std::cout << my_array[i]<< " \n ";
        i++;
    }

    printf("\n \n");

    //for (size_t i = 0; i < size; i++)
    //{
    //    //my_array[i] = rand();
    //    std::cout << my_array[i] <<"-";
    //    i++;

    //}

    //15. call parallel accumulate implementation with the array
    int ret_val = parallel_accumulate<int*, int>(my_array, my_array + size, ref);


    

    printf("- Size of elements based on const int size - %d\n", size);
    printf("- Number of available cores in our system - %d\n", allowed_threads_by_hardware);
    printf("- Number allowed threads - %d\n", allowed_threads_by_hardware);
    printf("- the numebr of threads we have OR minimum value returned by hardware_concurrency() OR accumulated value - %d\n", num_threads);
    printf("- Block size - %d\n\n", block_size);

    printf("- Accumulated value : %d\n \n", ret_val);

}

