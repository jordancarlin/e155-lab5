// lab5_jc
// Jordan Carlin
// jcarlin@hmc.edu
// October 2024

#include "lab5_jc.h"

#define CW 0
#define CCW 1

volatile int pulses = 0;
volatile int direction = 0;
volatile bool currentAChannel = 0, currentBChannel = 0;

int main(void) {

    configureFlash();
    configureClock();

    // Enable inputs for encoder
    gpioEnable(GPIO_PORT_A);
    gpioEnable(GPIO_PORT_B);
    pinMode(ENCODER_A, GPIO_INPUT);
    pinMode(ENCODER_B, GPIO_INPUT);

    // Initialize timer
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    initTIM(DELAY_TIM);

    // Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // Configure EXTICR for the encoder interrupts
    SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR2_EXTI6, 0b000); // Select PA6
    SYSCFG->EXTICR[2] |= _VAL2FLD(SYSCFG_EXTICR3_EXTI8, 0b000); // Select PA8

    // Enable interrupts globally
    __enable_irq();

    EXTI->IMR1 |= (1 << gpioPinOffset(ENCODER_A)); // Configure the mask bit
    EXTI->RTSR1 |= (1 << gpioPinOffset(ENCODER_A));// Enable rising edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(ENCODER_A));// Enable falling edge trigger

    EXTI->IMR1 |= (1 << gpioPinOffset(ENCODER_B)); // Configure the mask bit
    EXTI->RTSR1 |= (1 << gpioPinOffset(ENCODER_B));// Enable rising edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(ENCODER_B));// Enable falling edge trigger

    NVIC->ISER[0] |= (1 << EXTI9_5_IRQn); // Turn on EXTI interrupt in NVIC_ISER

    float speed = 0.0;

    while(1){
        delay_millis(DELAY_TIM, 1000); // wait one second
        printf("Pulses: %d: ", pulses); // Debugging statement to check pulses value
        speed = (float)pulses/480.0; // 120 pulses * 2 (two sensors) * 2 (rising/falling)
        printf("Current speed: %.3f rotations/s", speed);
        if (direction) {
            printf(" in CCW direction\n");
        } else {
            printf(" in CW direction\n");
        }
        pulses = 0;
    }

}

void checkDirection(bool newAChannel, bool newBChannel){
    // printf("current channels: %d %d\n", currentAChannel, currentBChannel);
    // printf("new channels: %d %d\n", newAChannel, newBChannel);
    if (!currentAChannel & !currentBChannel) {
        if (newAChannel & !newBChannel) direction = CW;
        else if (!newAChannel & newBChannel) direction = CCW;
    } else if (!currentAChannel & currentBChannel) {
        if (!newAChannel & !newBChannel) direction = CW;
        else if (newAChannel & newBChannel) direction = CCW;
    }
    else if (currentAChannel & !currentBChannel) {
        if (newAChannel & newBChannel) direction = CW;
        else if (!newAChannel & !newBChannel) direction = CCW;
    }
    else if (currentAChannel & currentBChannel) {
        if (!newAChannel & newBChannel) direction = CW;
        else if (newAChannel & !newBChannel) direction = CCW;
    }
    currentAChannel = newAChannel;
    currentBChannel = newBChannel;
}

void EXTI9_5_IRQHandler(void){
    bool newAChannel = 0, newBChannel = 0;
    // Check that encoder A was what triggered the interrupt
    if (EXTI->PR1 & (1 << ENCODER_A_NUM)){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << ENCODER_A_NUM);

        // rising edge
        if (digitalRead(ENCODER_A)){
            pulses ++;
            newAChannel = 1;
            // printf("A rising\n");
        }
        else { // falling edge
            pulses ++;
            newAChannel = 0;
            // printf("A falling\n");
        }
        checkDirection(newAChannel, newBChannel);
    } else if (EXTI->PR1 & (1 << ENCODER_B_NUM)) {
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << ENCODER_B_NUM);

        // rising edge
        if (digitalRead(ENCODER_B)){
            pulses ++;
            newBChannel = 1;
            // printf("B rising\n");
        }
        else { // falling edge
            pulses ++;
            newBChannel = 0;
            // printf("B falling\n");
        }
        checkDirection(newAChannel, newBChannel);
    }
}
