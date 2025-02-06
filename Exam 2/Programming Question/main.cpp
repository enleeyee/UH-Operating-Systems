#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <string>


struct arguments
{
    int pos;
    int size;
    int *array;
};


void swap(int &value1, int &value2)
{
    int temp = value1;
    value1 = value2;
    value2 = temp;
}

void *reverseIntArray(void *void_ptr)
{
	arguments *arg_ptr = (struct arguments *) void_ptr; // cast the void pointer to a struct arguments pointer
    int pos1 = arg_ptr->pos;
    int pos2 = arg_ptr->size - 1 - arg_ptr->pos;
    // call the swap function to change the values between array[pos1] and array[pos2]
    swap(arg_ptr->array[pos1], arg_ptr->array[pos2]);
    
    
    return nullptr;
}

int main()
{

    int size;
    std::cin >> size;
    int *array = new int[size];
    for (int i =0;i<size;i++)
        std::cin >> array[i];
    int nthreads = size / 2;
    pthread_t *tid = new pthread_t[nthreads];
    arguments * arg= new arguments[nthreads];
	for (int i = 0; i < nthreads; i++)
	{
        arg[i].pos = i;
        arg[i].size = size; 
        arg[i].array = array; // assign the address of the array of integers
        if ( pthread_create(&tid[i], NULL, reverseIntArray, &arg[i]) )
		{
			    fprintf(stderr, "Error creating thread\n");
			    return 1;
		}
	}
    // call pthread_join here
    for(int i = 0; i < nthreads; i++)
    {
        pthread_join(tid[i], NULL);
    }

    // DO NOT MODIFY THE LOOP BELOW
    for (int i =0; i< size;i++) 
        std::cout << array[i] << " ";
	delete [] tid;
    delete [] arg;
    delete [] array;
    return 0;
}