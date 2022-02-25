//author: Pınar Dilbaz 2243392
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "string.h"
#include "signal.h"
#include "unistd.h"
#include "fcntl.h"
#include <errno.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
/*First of all when running our program, we run it by adding -lpthread. 
In the first part, a matrix shift is done and we get how many threads will do this operation from the user.
Each thread matrix can shift as many rows and columns as size / number of threads.
First, threads are created for row operations and the ShihtRows function, which shifts a single row, is called, then in this function, the thread performs as many operations as it needs to do, one after another, and in this way, all threads complete the row operations they need to do, then the same operations are repeated to shift the column.
Also, if the matrix size is not divided by the number of threads, the last thread matrix needs to process adding more size%threadNumber.
Finally, the desired matrix is ​​taken from "matrix.txt".

In the second part, innovations were added to the dining philosophers problem and a solution was found by using mutex and condition variables of pthread.
First, we take how many philosophers will sit on the table from the user and we use mutex lock and unlock to solve any deadlocks or starving problems.
At the same time, the philosophers here need 3 forks to eat and we define an integer variable and hold a philosopher number / 3 forks here, if each philosopher picks fork, then forkInBox is decrement one, while each philosopher puts the fork back, the number of forks in the box increases.
At the end, the average hungry time and the maximum hungry period of the philosophers are calculated.
*/
//Part1 declarations
int size,k;
int matrix[100][100];
int newMatrix[100][100];
int row[100];
int column[100];
int count ;
int count2 ;
int counter ;
int counter2 ;
int control , control2;
int responsible,mod;
int s_value, d_value;
int m,n,d_value;

//Part2 declarations
int philosopherNumber;
pthread_mutex_t mutex;
pthread_cond_t cond[50];
int left;
int right;
int mostEat = 50;
enum { THINKING, HUNGRY, EATING } state[50];
int phil[50];
int forkInBox;
time_t starttime, endtime;
float finaltime;
float hungryTime[50];
int randTime=2;

//display matrix function
void displayMatrix(){
for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++){
      printf( "%d",matrix[i][j] );
      printf(" ");
    }
    printf("\n");
  }
}
//responsible for shifting rows function
void *shiftRows(void *x){
int tempe;
tempe = responsible;
//if we have last thread, we have to add mode size and thread number
if(control == d_value-1){
responsible = responsible+ mod;
}
for(int t = 0; t<responsible;t++){
for (int j = 0; j < size; j++){
  	row[j]=matrix[count2][j];
  	
   }
   count2++;
int temporary=row[size-1];
    for(int y=size-1;y>0;y--)
    {
        row[y]=row[y-1];
    }
    row[0]=temporary;
    
    for (int z = 0; z < size; z++){
    matrix[count][z] = row[z];
    }
  count++;
 }
 responsible = tempe;
 control++;
}

//responsible for shifting columns function
void *shiftColumns(void *x){
int tempe2;
tempe2 = responsible;
if(control2==d_value-1)
responsible = responsible+ mod;
for(int t = 0; t<responsible;t++){
for (int j = 0; j < size; j++){
  	column[j]=matrix[j][counter2];
 }
   counter2++;
int temporary1=column[0];
    for(int w=0;w<size-1;w++)
    {
        column[w]=column[w+1];
    }
    column[size-1]=temporary1;
    
    for (int z = 0; z < size; z++){
    matrix[z][counter] = column[z];
    }
  counter++;
  
 }
 responsible = tempe2;
 control2++;
}


void *matrixLeft(void *x){
// temporary array of size 
  int temp[size];
  int temp2[size];
  
  k = 1 % size;
 
  for (int i = 0; i < size; i++) {
  //left to right start
  // copy first size-k elements to temporary array
  for (int t = 0; t < size - k; t++)
      temp[t] = matrix[i][t];
 
  // copy the elements from k to end to starting
  
  for (int j = size - k; j < size; j++)
      matrix[i][j - size + k] = matrix[i][j];
 	//matrix[i][j] = matrix[i][j - size + k];
  // copy elements from temporary array to end
  for (int j = k; j < size; j++)
      matrix[i][j] = temp[j -k]; 
  
  }
}


void *matrixBottom(void *x){
int t= -1;
  for(int n = k; n<size ;n++){
  t = t +1;
  for(int m=0 ; m<size ; m++){
   newMatrix[t][m] = matrix[n][m];
  }
 }
 for(int n = 0; n<size-k ;n++){
  t = t +1;
  for(int m=0 ; m<size ; m++){
   newMatrix[t][m] = matrix[n][m];
  }
 }
 for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++){
      matrix[i][j] = newMatrix[i][j];
    }
  }
}

//Part2 Functions

void initialization_code(){
        for(int i = 0;i<philosopherNumber;i++){
        state[i] = THINKING;
        pthread_cond_init(&cond[i], NULL);
        }
    }
void test (int philNum){
        
    	if (state[philNum] == HUNGRY && state[left] != EATING && state[right] != EATING && forkInBox >=1 ) {
    	printf("Philosopher-%d is starting EATING.\n", philNum + 1);
    	state[philNum] = EATING;
    	sleep(rand() % 5 + 1);
    	time(&endtime);
    	hungryTime[philNum] += difftime(endtime, starttime);
    	pthread_cond_signal(&cond[philNum]);
  }
    }
void pickup (int philNum){
	pthread_mutex_lock(&mutex);
        printf("Philosopher-%d is HUNGRY.\n", philNum + 1);
  	state[philNum] = HUNGRY;
  	time(&starttime); 
  	test(philNum);
    	if(state[philNum] != EATING){
        pthread_cond_wait(&cond[philNum],&mutex);
    	}

  	pthread_mutex_unlock(&mutex);
  	forkInBox = forkInBox -1;
  	 
    }
void putdown (int philNum){
	
        pthread_mutex_lock(&mutex);
	left = (philNum + philosopherNumber - 1) % philosopherNumber;
        right = (philNum + 1) % philosopherNumber;
  	printf("Philosopher-%d puts down fork.\n", philNum + 1);
  	state[philNum] = THINKING;
  	test(left);
  	test(right);

  	pthread_mutex_unlock(&mutex);
  	forkInBox = forkInBox +1;
    }
    
void *philosopher(void *arg)
{
int *philNum = arg;
int loop=0;
while(loop<50) {
    printf("Now %d. Philosopher is starting to think.\n", *philNum + 1);
    if(forkInBox == 1)
    printf("Warning!! There is only 1 fork left at the box.\n");
    sleep(rand() % 5 + 1);
    pickup(*philNum);
    putdown(*philNum);
    loop++;
  }
  pthread_exit(&philNum);
}


// main part
int main() {

 FILE *fptr;
 int selection;
 char fileName[15] = "matrix.txt";
 printf("Welcome to Assignment-2 !!\n");
 printf("Please enter 1 for Part-1\n");
 printf("Please enter 2 for Part-2\n");
 printf("Please enter 3 for quit the system\n");
 printf("Enter your choice: ");
 scanf("%d",&selection);
 if(selection == 1){
 //take input d and s from the user
 printf("Please enter the s value: "); 
 scanf("%d",&s_value);  
 printf("Please enter the number of thread: "); 
 scanf("%d",&d_value);        
 //read matrix from file
 fptr = fopen(fileName, "r");
            if (fptr == NULL)
            {
                printf("Could not open file %s", fileName);
                return 0;
            }
           
            fscanf(fptr, "%d", &size);
            fscanf(fptr, "\n");
            matrix[size][size];
            newMatrix[size][size];
            //create thread maximum size is matrix size
            pthread_t tid[size];
            for (int i = 0; i < size; i++)
            {
            	for (int j = 0; j < size; j++) {
            	fscanf(fptr, "%d", &matrix[i][j]);
            	fscanf(fptr, "\n");
             }
           }
            fclose(fptr);
  //display the matrix function calling before shifting
  printf("Input Matrix\n");
  displayMatrix();
  
  responsible = size/d_value;
  mod = size%d_value;
  for(int c=0; c < s_value; c++){
  counter = 0;
  counter2 = 0;
  count = 0;
  count2 = 0;
  control = 0;
  control2 = 0;
  
  //create threads for rows
  for(m= 0; m< d_value;m++)
  pthread_create(&tid[m],NULL,shiftRows,(void *)(unsigned long long)(m));
  
  //wait threads
  for(n= 0; n< d_value;n++)
  pthread_join(tid[n],NULL);
 
  //create threads for columns
  for(m= 0; m< d_value;m++)
  pthread_create(&tid[m],NULL,shiftColumns,(void *)(unsigned long long)(m));
  //wait threads 
  for(n= 0; n< d_value;n++)
  pthread_join(tid[n],NULL);
  
 }
  // display final rotated matrix after shifted
  printf("Shifted Matrix\n");
  displayMatrix();
 }
 
 //Part2 start
 else if(selection == 2){
 
  srand(time(NULL));
  printf("Please enter the number of  philosopher threads: ");
  scanf("%d",&philosopherNumber);	
  initialization_code();
  
  pthread_t philTid[philosopherNumber];
  pthread_mutex_init(&mutex, NULL);
  
  forkInBox = philosopherNumber/3;
  hungryTime[philosopherNumber];

  for (int i = 0; i < philosopherNumber; i++) {
    pthread_create(&philTid[i], NULL, philosopher, &phil[i]);
    printf("Now %d. Philosopher is sitting.\n", i + 1);
    sleep(1);
  }
  
  for (int i = 0; i < philosopherNumber; i++)
    pthread_join(philTid[i], NULL);

    
  float average, sum = 0.0,maxHungry;
  for(int k = 0; k< philosopherNumber; k++ ){
  sum = sum + hungryTime[k];
  }
  average = sum / philosopherNumber;
  //maxHungry = largest(hungryTime,philosopherNumber);
  float max = hungryTime[0];
  int maxID = -1;
  //printf("philosopherNumber %d\n",philosopherNumber);
    for (int j = 1; j < philosopherNumber; j++){
        if (hungryTime[j] > max){
            max = hungryTime[j];
            }
            maxID = j;
  }
  printf("\nAfter all philosophers are done\n");
  printf("The average HUNGRY state duration is %f\n",average);
  printf("The largest HUNGRY state duration is %f and Philosopher ID is %d\n",max,maxID);
 
 }
 //Part2 end
 
 else if(selection == 3){
  
  return 0;
  
  }
}



