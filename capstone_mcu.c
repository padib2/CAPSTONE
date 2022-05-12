#include "mkl25z4.h"
#include <stdio.h>

#define MASK(X) (1UL << X)

void initialise(void);
unsigned long millis(void);
void SysTick_Handler();
void  matrix_operation();

volatile unsigned long counter;

int sum = 0;
int sample_matrix[16][2] = {
    {10, 11},
    {10, 11},
	{10, 11},
	{10, 11},
	{10, 11},
	{10, 11},
	{10, 11},
	{10, 11},
	{10, 11},
	{10, 11},
	{10, 11},
	{10, 11},
	{10, 11},
	{10, 11},
	{10, 11},
	{10, 11}
};


int input_vector[2][1] = {
		{1},
		{1}
};

int result_vector[16][1];

int main(){
  initialise();

  while(1){
    unsigned long a = millis();
    printf("start time is %d\n", a);
    matrix_operation();
    unsigned long b = millis();
    printf("end time is %d\n", b);

    unsigned long time = b-a;
    printf("time taken is %d\n", time);


  }


return 0;
}

void initialise(){
  SysTick->LOAD = (20971520u/100000u)-1;
  SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

void SysTick_Handler(){
  counter++;
}

unsigned long millis(void){
  return (unsigned long) counter;
}

void  matrix_operation(void){
	for (int i=0; i<16; i++){
		for (int j=0; j<2; j++){
			result_vector[i][1] += sample_matrix[i][j] * input_vector[j][1];

		}
		//result_vector[i][1]= sum;
		//sum = 0;
	}
}
