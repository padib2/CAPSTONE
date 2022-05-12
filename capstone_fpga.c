#include "mkl25z4.h"
#define MASK(x)			(1UL << x)


//port b
#define to_send_bin (3)

//port d
#define receive_pin (7)

#define send_bin (5)
#define result_bin (0)
#define arduino_ready_bin (2)
#define fpga_ready_bin (3)


volatile int index = 0;
int to_write[4] = {4, 8, 7, 2};
float sample_matrix[16][3] = {{4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43},
                              {4.23, 3.23, 2.43}};

float input_vector[3] = {2.34, 5.43, 2.82};
float result_vector[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

float temp;
float multiplier = 4.23;
volatile int all_sent = 0;


unsigned long start2;
unsigned long ending2;
volatile unsigned long counter;

unsigned long millis(void);
void SysTick_Handler();
void initialize();
void fpga_done();



int main(){


	while(1){
all_sent = 0;
PTD->PDOR &= ~(send_bin);

if(PTD->PDIR & MASK(fpga_ready_bin)){
    start2 = millis();
    index = 0;
    PTB->PDOR = (PTB->PDOR & 0b110000) | (to_write[index] & 0b1111);
    index++;
    PTD->PDOR |= send_bin;

    while(index < 12);
    ending2 = millis();



	}
}

}

void initialize(){
	//SYSTICK
	SysTick->LOAD = (20971520u/100000u)-1;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;

	__disable_irq();
	//clock gate ports D and B

	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK|SIM_SCGC5_PORTE_MASK;

	//set up 3 pins as GPIO --> configure mux to connect pins as GPIO

	//send_bin
	PORTD->PCR[send_bin ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[send_bin ] |= PORT_PCR_MUX(1);
	PTD->PDDR |= MASK(send_bin ) ; //set as output


	//receive_pin
	PORTD->PCR[receive_pin ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[receive_pin ] |= PORT_PCR_MUX(1);

	PTD->PDDR &= ~MASK(receive_pin ) ;  //clear to make input

	PORTD->PCR[receive_pin] |=PORT_PCR_PE_MASK; //configure pullup

	PORTD->PCR[receive_pin] &=~PORT_PCR_IRQC_MASK; //clear the fields
	PORTD->PCR[receive_pin]  |= PORT_PCR_IRQC(9); //set for rising edge only
	//clear previous spurious interrupts on this port <<********************
	PORTD->ISFR |= MASK(receive_pin);		//w1c

	//result_bin
	PORTD->PCR[result_bin ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[result_bin ] |= PORT_PCR_MUX(1);

	//arduino_ready_bin
	PORTD->PCR[arduino_ready_bin ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[arduino_ready_bin ] |= PORT_PCR_MUX(1);

	//fpga_ready_bin
	PORTD->PCR[fpga_ready_bin ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[fpga_ready_bin ] |= PORT_PCR_MUX(1);

	//PORT B
	//to_send_bin
	PORTB->PCR[to_send_bin ] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[to_send_bin ] |= PORT_PCR_MUX(1);
	PTB->PDDR |= MASK(to_send_bin ) ; //set as output


	NVIC->ISER[0] |= MASK(PORTD_IRQn); //0x80000000	or PORTD_IRQn	;//set up bit 31 (to enable interrupts on port D (int31)



	__enable_irq(); //enable all IRQs
}

void PORTD_IRQHandler(void){
	if(PORTD->ISFR & MASK(receive_pin)) { //check if flag  activated
		fpga_done();
		PORTD->ISFR |= MASK(receive_pin);
	}

}

void fpga_done(){

  if (PTD->PDIR & MASK(fpga_ready_bin) && index < 12){
    PTD->PDOR &= ~(send_bin);
    PTB->PDOR = (PTB->PDOR & 0b110000) | (to_write[index] & 0b1111);
    PTD->PDOR |= send_bin;
    index++;
  }
}

void SysTick_Handler(){
  counter++;
}

unsigned long millis(void){
  return (unsigned long) counter;
}
