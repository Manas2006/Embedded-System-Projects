/* ECE319K_Lab4main.c
 * Traffic light FSM
 * ECE319H students must use pointers for next state
 * ECE319K students can use indices or pointers for next state
 * Put your names here or look silly
  */


#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/UART.h"
#include "../inc/Timer.h"
#include "../inc/Dump.h"  // student's Lab 3
#include "ti/devices/msp/m0p/mspm0g350x.h"
#include <stdio.h>
#include <string.h>
// put both EIDs in the next two lines
const char EID1[] = "SK62288"; //  ;replace abc123 with your EID
const char EID2[] = "MAP7848"; //  ;replace abc123 with your EID
// Hint implement Traffic_Out before creating the struct, make struct match your Traffic_Out



// initialize all 6 LED outputs and 3 switch inputs
// assumes LaunchPad_Init resets and powers A and B
void Traffic_Init(void){ // assumes LaunchPad_Init resets and powers A and B
    //first initialize the LEDs
    IOMUX->SECCFG.PINCM[PB0INDEX] |= 0x81;
    IOMUX->SECCFG.PINCM[PB1INDEX] |= 0x81;
    IOMUX->SECCFG.PINCM[PB2INDEX] |= 0x81;

    IOMUX->SECCFG.PINCM[PB6INDEX] |= 0x81;
    IOMUX->SECCFG.PINCM[PB7INDEX] |= 0x81;
    IOMUX->SECCFG.PINCM[PB8INDEX] |= 0x81;

    IOMUX->SECCFG.PINCM[PB22INDEX] |= 0x81;
    IOMUX->SECCFG.PINCM[PB27INDEX] |= 0x81;
    IOMUX->SECCFG.PINCM[PB26INDEX] |= 0x81;

    IOMUX->SECCFG.PINCM[PB17INDEX] |= 0x40081;
    IOMUX->SECCFG.PINCM[PB16INDEX] |= 0x40081;
    IOMUX->SECCFG.PINCM[PB15INDEX] |= 0x40081;

    GPIOB->DOE31_0 |= (1<<0);
    GPIOB->DOE31_0 |= (1<<1);
    GPIOB->DOE31_0 |= (1<<2);
    GPIOB->DOE31_0 |= (1<<6);
    GPIOB->DOE31_0 |= (1<<7);
    GPIOB->DOE31_0 |= (1<<8);
    GPIOB->DOE31_0 |= (1<<22);
    GPIOB->DOE31_0 |= (1<<27);
    GPIOB->DOE31_0 |= (1<<26);
 
  
}
/* Activate LEDs
* Inputs: west is 3-bit value to three east/west LEDs
*         south is 3-bit value to three north/south LEDs
*         walk is 3-bit value to 3-color positive logic LED on PB22,PB26,PB27
* Output: none
* - west =1 sets west green
* - west =2 sets west yellow
* - west =4 sets west red
* - south =1 sets south green
* - south =2 sets south yellow
* - south =4 sets south red
* - walk=0 to turn off LED
* - walk bit 22 sets blue color
* - walk bit 26 sets red color
* - walk bit 27 sets green color
* Feel free to change this. But, if you change the way it works, change the test programs too
* Be friendly*/
void Traffic_Out(uint32_t west, uint32_t south, uint32_t walk){


  GPIOB->DOUT31_0 = (GPIOB->DOUT31_0 & (0xF3BFFE38));

  GPIOB->DOUT31_0 |= south;
  GPIOB->DOUT31_0 |= (west<<6);


  uint32_t walkb1 = walk & 1;
  uint32_t walkb2 = walk & 2;
  uint32_t walkb3 = walk & 4;

  GPIOB->DOUT31_0 |= (walkb3<<25);
  GPIOB->DOUT31_0 |= (walkb2<<25);
  GPIOB->DOUT31_0 |= (walkb1<<22);
  
   

    
}
/* Read sensors
 * Input: none
 * Output: sensor values
 * - bit 0 is west car sensor
 * - bit 1 is south car sensor
 * - bit 2 is walk people sensor
* Feel free to change this. But, if you change the way it works, change the test programs too
 */
uint32_t Traffic_In(void){

    return ((GPIOB->DIN31_0 & 0x38000)>>15);

    //return 0; // write this
}

uint32_t westcopy;
uint32_t southcopy;
uint32_t walkcopy;


struct state
{
  uint32_t west;
  uint32_t south;
  uint32_t walk;
  uint32_t delay;
  const struct state *Next[8];
};
typedef const struct state State_t;

#define SouthGreen &FSM[0]
#define SouthYellow &FSM[1]
#define AllRed1 &FSM[2]
#define Walk &FSM[3]
#define Red1 &FSM[4]
#define Off1 &FSM[5]
#define Red2 &FSM[6]
#define Off2 &FSM[7]
#define AllRed2 &FSM[8]
#define WestGreen &FSM[9]
#define WestYellow &FSM[10]
#define AllRed3 &FSM[11]

State_t FSM[13]={
  {4, 1, 2, 300,{SouthGreen, SouthYellow, SouthGreen, SouthYellow, SouthYellow, SouthYellow, SouthYellow, SouthYellow}},  // South Green (3 sec)
  {4, 2, 2, 50,{AllRed1, AllRed1, AllRed1, AllRed1, AllRed1, AllRed1, AllRed1, AllRed1}},  // South Yellow (0.5 sec)
  {4, 4, 2, 30,{Walk, Walk, SouthGreen, Walk, WestGreen, Walk, WestGreen, Walk}},  // All Red 1 (0.3 sec)
  {4, 4, 7, 500,{Red1, Red1, Red1, Red1, Red1, Red1, Red1, Red1}},  // Walk (5 sec)
  {4, 4, 2, 100,{Off1, Off1, Off1, Off1, Off1, Off1, Off1, Off1}},  // Red 1 (1 sec)
  {4, 4, 0, 25,{Red2, Red2, Red2, Red2, Red2, Red2, Red2, Red2}},  // Off 1 (0.25 sec)
  {4, 4, 2, 30,{Off2, Off2, Off2, Off2, Off2, Off2, Off2, Off2}},  // Red 2 (0.3 sec)
  {4, 4, 0, 30,{AllRed2, AllRed2, AllRed2, AllRed2, AllRed2, AllRed2, AllRed2, AllRed2}},  // Off 2 (0.3 sec)
  {4, 4, 2, 30,{WestGreen, Walk, SouthGreen, SouthGreen, WestGreen, WestGreen, WestGreen, WestGreen}},  // All Red 2 (0.3 sec)
  {1, 4, 2, 300,{WestYellow, WestYellow, WestYellow, WestYellow, WestYellow, WestYellow, WestYellow, WestYellow}},  // West Green (3 sec)
  {2, 4, 2, 100,{AllRed3, AllRed3, AllRed3, AllRed3, AllRed3, AllRed3, AllRed3, AllRed3}},  // West Yellow (1 sec)
  {4, 4, 2, 100,{SouthGreen, Walk, SouthGreen, SouthGreen, WestGreen, Walk, SouthGreen, SouthGreen}},  // All Red 3 (1 sec)
};




  State_t *pointer = SouthGreen;
   // state pointer
  uint32_t input; // next state




// use main1 to determine Lab4 assignment
void Lab4Grader(int mode);
void Grader_Init(void);
int main1(void){ // main1
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Lab4Grader(0); // print assignment, no grading
  while(1){
  }
}
// use main2 to debug LED outputs
// at this point in ECE319K you need to be writing your own test functions
// modify this program so it tests your Traffic_Out  function
int main2(void){ // main2
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Grader_Init(); // execute this line before your code
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization
  if((GPIOB->DOE31_0 & 0x20)==0){
    UART_OutString("access to GPIOB->DOE31_0 should be friendly.\n\r");
  }
  UART_Init();
  UART_OutString("Lab 4, Spring 2025, Step 1. Debug LEDs\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");
  while(1){
   
    //this is for testing south first
    Traffic_Out(0, 1, 0);
    Debug_Dump(GPIOB->DOUT31_0);
    Traffic_Out(0, 2, 0);
    Debug_Dump(GPIOB->DOUT31_0);
    Traffic_Out(0, 4, 0);
    Debug_Dump(GPIOB->DOUT31_0);


    //testing for west
    Traffic_Out(1, 0, 0);
    Debug_Dump(GPIOB->DOUT31_0);
    Traffic_Out(2, 0, 0);
    Debug_Dump(GPIOB->DOUT31_0);
    Traffic_Out(4, 0, 0);
    Debug_Dump(GPIOB->DOUT31_0);



    //this is testing for red & white
    Traffic_Out(0, 0, 2);
    Debug_Dump(GPIOB->DOUT31_0);
    Traffic_Out(0, 0, 7);
    Debug_Dump(GPIOB->DOUT31_0);
    

    if((GPIOB->DOUT31_0&0x20) == 0){
      UART_OutString("DOUT not friendly\n\r");
    }
  }
}
// use main3 to debug the three input switches
// at this point in ECE319K you need to be writing your own test functions
// modify this program so it tests your Traffic_In  function
int main3(void){ // main3
  uint32_t last=0,now;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Traffic_Init(); // your Lab 4 initialization
  Debug_Init();   // Lab 3 debugging
  UART_Init();
  __enable_irq(); // UART uses interrupts
  UART_OutString("Lab 4, Spring 2025, Step 2. Debug switches\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");
  while(1){
    now = Traffic_In(); // Your Lab4 input
    if(now != last){ // change
      UART_OutString("Switch= 0x"); UART_OutUHex(now); UART_OutString("\n\r");
      Debug_Dump(now);
    }
    last = now;
    Clock_Delay(800000); // 10ms, to debounce switch
  }
}
// use main4 to debug using your dump
// proving your machine cycles through all states
int main4(void){// main4
uint32_t input;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization
 // set initial state
  Debug_Init();   // Lab 3 debugging
  UART_Init();
  __enable_irq(); // UART uses interrupts
  UART_OutString("Lab 4, Spring 2025, Step 3. Debug FSM cycle\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");
  uint32_t comboin;
  State_t *nextState;
  uint32_t j;
// initialize your FSM
  SysTick_Init();   // Initialize SysTick for software waits

  while(1){
        Traffic_Out(pointer->west, pointer->south, pointer->walk);
        SysTick_Wait10ms(pointer->delay);
        
        input = 7; // Hardcoded as per lab instructions
        

        // Get the state index
        j = (pointer - FSM)+1;
        

        comboin = (pointer->west | (pointer->south << 9) | (pointer->walk << 16) | (j << 24));
        Debug_Dump(comboin);

        pointer = pointer->Next[input]; // Move to next state
      // 1) output depending on state using Traffic_Out
      // call your Debug_Dump logging your state number and output
      // 2) wait depending on state
      // 3) hard code this so input always shows all switches pressed
      // 4) next depends on state and input
  }
}
// use main5 to grade
int main(void){// main5
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Grader_Init(); // execute this line before your code
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization
// initialize your FSM
  SysTick_Init();   // Initialize SysTick for software waits
  // initialize your FSM
  Lab4Grader(1); // activate UART, grader and interrupts
  while(1){
    Traffic_Out(pointer->west, pointer->south, pointer->walk); // call output
    SysTick_Wait10ms(pointer->delay);// delay
    input = Traffic_In();
    pointer = pointer->Next[input];
      // 1) output depending on state using Traffic_Out
      // call your Debug_Dump logging your state number and output
      // 2) wait depending on state
      // 3) input from switches
      // 4) next depends on state and input
  }
}

