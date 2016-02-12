// main.c
// Runs on LM4F120/TM4C123
// UART runs at 115,200 baud rate 
// Daniel Valvano
// May 23, 2014

/* This example accompanies the books
  "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
  ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

"Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
 
 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF4       (*((volatile uint32_t *)0x40025040))
#define SWITCHES  (*((volatile uint32_t *)0x40025044))
#define SW2       0x10                      // on the left side of the Launchpad board
#define SW1       0x01                      // on the right side of the Launchpad board
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
#include <stdio.h>
#include <stdint.h> // C99 variable types
#include "inc/tm4c123gh6pm.h"
void Output_Init(void);
void PortF_Init(void){ volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x00000020;  // 1) activate clock for Port F
  delay = SYSCTL_RCGCGPIO_R;        // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
}


uint32_t PortF_Input(void){     
  return (GPIO_PORTF_DATA_R&0x11);  // read PF4,PF0 inputs
}

void PortF_Output(uint32_t data){ // write PF3-PF1 outputs
  GPIO_PORTF_DATA_R = data;      
}
uint32_t PortD_Input(void){     
  return (GPIO_PORTD_DATA_R&0x0F);  // read PF4,PF0 inputs
}

void PortD_Output(uint32_t data){ // write PF3-PF1 outputs
  GPIO_PORTD_DATA_R = data;      
}

void BookExamples(void){ // examples from the book
  int8_t cc = 0x56; // (‘V’)
  int32_t xx = 100;
  int16_t yy = -100;
  float zz = 3.14159265;
  printf("cc = %c %d %#x\n",cc,cc,cc);	//cc = V 86 0x56
  printf("xx = %c %d %#x\n",xx,xx,xx);	//xx = d 100 0x64
  printf("yy = %d %#x\n",yy,yy);	//yy = -100 0xffffff9c
  printf("zz = %e %E %f %g %3.2f\n",zz,zz,zz,zz,zz);	//zz = 3.14159 3.14
}
void GPIO_Init(void){  
  SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate port D
  while((SYSCTL_PRGPIO_R&0x08)==0){};   // allow time for clock to stabilize
                                    // 2) no need to unlock PD3-0
  GPIO_PORTD_AMSEL_R &= ~0x0F;      // 3) disable analog functionality on PD3-0
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // 4) GPIO
  GPIO_PORTD_DIR_R |= 0x00;         // 5) make PD3-0 out ISAAC ROBERT 
  GPIO_PORTD_AFSEL_R &= ~0x0F;      // 6) regular port function 
  GPIO_PORTD_DEN_R |= 0x0F;         // 7) enable digital I/O on PD3-0
}
void delay(){
	int x=1000000;
	while (x>0){
		x--;
	}
}

void portD_Listener(uint32_t *data) {
	for( ; ; ){
		if (SW1 == PortF_Input()) {
			*data=PortD_Input();
			delay();
			break;
		} else continue;
	}
}

char op_check(){
	char operation;
	while (1){
		if (SW1 == PortF_Input()) {
			operation='1';
			delay();
			break;
		}	else if (SW2 == PortF_Input()) {
			operation = '2';
			delay();
			break;
		}	else operation = '0';
	}	
	return operation;
}	

int main(void){
  Output_Init();              // initialize output device
  PortF_Init();
	GPIO_Init();
	char operand;
	int op1, op2, zOut;
	uint32_t swIn, sw2In;
	printf("\nInput first number:\n");
	PortF_Output(RED);
	//Replaced by UART I/O
	//scanf("%c", &x);
	
	portD_Listener(&swIn);
	op1 = (int)swIn;
	if (op1>9) op1=9;
	printf("%d\n", op1);
	printf("Input second number:\n");
	//scanf("%c", &y);
	portD_Listener(&sw2In);
	op2 = (int)sw2In;
	if (op2>9) op2=9;
	printf("%d\n", op2);
	printf("Left button to add\nRight button to subtract");
	operand =op_check();
	
	//scanf("%c", &add);
	PortF_Output(GREEN);
	switch(operand){
		case '1':
			zOut = op1 + op2;
			printf("\n %d + %d = %d",op1,op2, zOut);
			break;
		case '2':
			zOut = op1 - op2;
			printf("\n %d - %d = %d",op1,op2, zOut);
			break;
	}	
	
}
