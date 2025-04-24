/* DAC5.cpp
 * Students put your names here
 * Modified: December 26, 2024
 * 5-bit binary-weighted DAC connected to PB4-PB0
 */
#include <ti/devices/msp/msp.h>
#define PB0INDEX  11 // UART0_TX  SPI1_CS2  TIMA1_C0  TIMA0_C2
#define PB1INDEX  12 // UART0_RX  SPI1_CS3  TIMA1_C1  TIMA0_C2N
#define PB2INDEX  14 // UART3_TX  UART2_CTS I2C1_SCL  TIMA0_C3  UART1_CTS TIMG6_C0  TIMA1_C0
#define PB3INDEX  15 // UART3_RX  UART2_RTS I2C1_SDA  TIMA0_C3N UART1_RTS TIMG6_C1  TIMA1_C1
#define PB4INDEX  16 // UART1_TX  UART3_CTS TIMA1_C0  TIMA0_C2  TIMA1_C0N

// **************DAC5_Init*********************
// Initialize 5-bit DAC, called once
// Input: none
// Output: none
void DAC5_Init(void){
    // Configure PB4-PB0 as GPIO outputs
    IOMUX->SECCFG.PINCM[PB0INDEX] = 0x00000081; // GPIO output, no pull
    IOMUX->SECCFG.PINCM[PB1INDEX] = 0x00000081;
    IOMUX->SECCFG.PINCM[PB2INDEX] = 0x00000081;
    IOMUX->SECCFG.PINCM[PB3INDEX] = 0x00000081;
    IOMUX->SECCFG.PINCM[PB4INDEX] = 0x00000081;
    
    // Set PB4-PB0 as outputs
    GPIOB->DOE31_0 |= 0x0000001F; // Enable outputs for PB4-PB0
    
    // Initialize output to 0
    GPIOB->DOUT31_0 &= ~0x0000001F; // Clear PB4-PB0
}

// **************DAC5_Out*********************
// output to DAC5
// Input: 5-bit data, 0 to 31
// Input=n is converted to n*3.3V/31
// Output: none
// Note: this solution must be friendly
void DAC5_Out(uint32_t data){
    data &= 0x1F; // Ensure only 5 bits are used
    uint32_t current = GPIOB->DOUT31_0;
    current &= ~0x1F; // Clear bottom 5 bits
    current |= data;  // Set new value
    GPIOB->DOUT31_0 = current; // Atomic write
}
