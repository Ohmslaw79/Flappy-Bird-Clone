#include "stm32f0xx.h"
#include "lcd.h"
#include <stdint.h>
#include "midi.h"
#include "midiplay.h"

#define VOICES 15
#define UPDATE_RATE 30 //In updates per second
#define GRAVITY -5/1000
#define UPPER_SCREEN_BOUND 0
#define LOWER_SCREEN_BOUND 350
#define MAX_DOWNWARD_VELOCITY -12
#define VELOCITY_MODIFIER 2
#define JUMP_VELOCITY 15

extern const Picture background; // A 240x320 background image
extern const Picture ball; // A 19x19 purple ball with white boundaries

int x = 100;
int y = 160;
int v = 0; //velocity
char physics_enabled = 0;
char start_game = 0;
int current_score = 0;
int high_score = 0;


uint8_t notes[] = { 60,62,64,65,67,69,71,72,71,69,67,65,64,62,60,0 };
uint8_t num = sizeof notes / sizeof notes[0] - 1;

struct {
    uint8_t in_use;
    uint8_t note;
    uint8_t chan;
    uint8_t volume;
    int     step;
    int     offset;
} voice[VOICES];

void init_lcd_spi(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIOB->MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER5 | GPIO_MODER_MODER8 | GPIO_MODER_MODER11 | GPIO_MODER_MODER14);
    GPIOB->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER14_0;
    GPIOB->MODER |= GPIO_MODER_MODER3_1 | GPIO_MODER_MODER5_1;
    GPIOB->AFR[0] &= ~(GPIO_AFRL_AFR3 | GPIO_AFRL_AFR5);
    GPIOB->BSRR = GPIO_BSRR_BS_8 | GPIO_BSRR_BS_11 | GPIO_BSRR_BS_14;

    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1->CR1 &= ~SPI_CR1_SPE;
    SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;
	SPI1->CR1 |= SPI_CR1_SPE;
}

void init_button(){
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~GPIO_MODER_MODER6;

    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR6;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR6_1;


    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI6;
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI6_PB;
    EXTI->RTSR |= EXTI_RTSR_TR6;
    EXTI->IMR |= EXTI_IMR_MR6;
    NVIC->ISER[0] |= 1<<7;
}

void EXTI4_15_IRQHandler(){
    EXTI->PR |= EXTI_PR_PR6;
    start_game = 1;
    v = JUMP_VELOCITY;
}

void init_tim7(){
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 4800-1;
    TIM7->ARR = (10000 / UPDATE_RATE) - 1;

    TIM7->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= 1 << 18;
    NVIC->IP[4] |= 1<<22; //Lower Priority of Tim 7 ISR
    TIM7->CR1 |= TIM_CR1_CEN;
}

void TIM7_IRQHandler(){ //LCD update and physics calculations
    TIM7->SR &= ~TIM_SR_UIF;
    if(physics_enabled){
        int dv = v<=MAX_DOWNWARD_VELOCITY? 0 : -1;
        v += dv;
        int dy = (y > LOWER_SCREEN_BOUND && v < 0) || (y < UPPER_SCREEN_BOUND && v > 0) ? 0 : v/VELOCITY_MODIFIER;
        y -= dy;
        update2(x,y);
    }
}

void enable_physics(){
    physics_enabled = 1;
}

void disable_physics(){
    physics_enabled = 0;
}

void init_tim6(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = (48000000 / (2 * RATE)) - 1;
    TIM6->ARR = 1;

    TIM6->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= 1 << 17;
    TIM6->CR2 &= ~TIM_CR2_MMS;
    TIM6->CR2 |= TIM_CR2_MMS_1;

    TIM6->CR1 |= TIM_CR1_CEN;

}

void TIM6_DAC_IRQHandler(void)
{
    TIM6->SR &= ~TIM_SR_UIF;
    int sample = 0;
    for(int x=0; x < sizeof voice / sizeof voice[0]; x++) {
        if (voice[x].in_use) {
            voice[x].offset += voice[x].step;
            if (voice[x].offset >= N<<16)
                voice[x].offset -= N<<16;
            sample += (wavetable[voice[x].offset>>16] * voice[x].volume) >> 4;
        }
    }
    sample = (sample >> 10) + 2048;
    if (sample > 4095)
        sample = 4095;
    else if (sample < 0)
        sample = 0;
    DAC->DHR12R1 = sample;
}

void init_dac(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	DAC->CR &= ~(DAC_CR_TSEL1);
	DAC->CR |= DAC_CR_TEN1;
	DAC->CR |= DAC_CR_EN1;
    DMA1_Channel5->CNDTR = 23;
}

void note_off(int time, int chan, int key, int velo)
{
    int n;
    for(n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (voice[n].in_use && voice[n].note == key) {
            voice[n].in_use = 0; // disable it first...
            voice[n].chan = 0;   // ...then clear its values
            voice[n].note = key;
            voice[n].step = step[key];
            return;
        }
    }
}

void note_on(int time, int chan, int key, int velo)
{
    if (velo == 0) {
        note_off(time, chan, key, velo);
        return;
    }
    int n;
    for(n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (voice[n].in_use == 0) {
            voice[n].note = key;
            voice[n].step = step[key];
            voice[n].offset = 0;
            voice[n].volume = velo;
            voice[n].chan = chan;
            voice[n].in_use = 1;
            return;
        }
    }
}

void set_tempo(int time, int value, const MIDI_Header *hdr)
{
    // This assumes that the TIM2 prescaler divides by 48.
    // It sets the timer to produce an interrupt every N
    // microseconds, where N is the new tempo (value) divided by
    // the number of divisions per beat specified in the MIDI file header.
    TIM2->ARR = value/hdr->divisions - 1;
}

const float pitch_array[] = {
0.943874, 0.945580, 0.947288, 0.948999, 0.950714, 0.952432, 0.954152, 0.955876,
0.957603, 0.959333, 0.961067, 0.962803, 0.964542, 0.966285, 0.968031, 0.969780,
0.971532, 0.973287, 0.975046, 0.976807, 0.978572, 0.980340, 0.982111, 0.983886,
0.985663, 0.987444, 0.989228, 0.991015, 0.992806, 0.994599, 0.996396, 0.998197,
1.000000, 1.001807, 1.003617, 1.005430, 1.007246, 1.009066, 1.010889, 1.012716,
1.014545, 1.016378, 1.018215, 1.020054, 1.021897, 1.023743, 1.025593, 1.027446,
1.029302, 1.031162, 1.033025, 1.034891, 1.036761, 1.038634, 1.040511, 1.042390,
1.044274, 1.046160, 1.048051, 1.049944, 1.051841, 1.053741, 1.055645, 1.057552,
};

void pitch_wheel_change(int time, int chan, int value)
{
    //float multiplier = pow(STEP1, (value - 8192.0) / 8192.0);
    float multiplier = pitch_array[value >> 8];
    for(int n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (voice[n].in_use && voice[n].chan == chan) {
            voice[n].step = step[voice[n].note] * multiplier;
        }
    }
}

void init_tim2(int n) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 47;
    TIM2->ARR = n-1;

    TIM2->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= 1 << 15;
    TIM2->CR1 |= TIM_CR1_ARPE;

    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF;    

    midi_play();
}

void new_game(){
    LCD_Clear(0);
    LCD_DrawString(90,100, YELLOW, BLACK, "NEW GAME:", 16, 1);
    LCD_DrawString(20,120, YELLOW, BLACK, "Press Any Button To Play!", 16, 1);
    while(start_game == 0);
}

int main(void)
{
    init_button();
    LCD_Setup(); // this will call init_lcd_spi()
    init_tim7();
    new_game();
    LCD_DrawPicture(0,0,&background);
    enable_physics();
    

    init_wavetable_hybrid2();
    init_dac();
    init_tim6();
    MIDI_Player *mp = midi_init(midifile);
    // The default rate for a MIDI file is 2 beats per second
    // with 48 ticks per beat.  That's 500000/48 microseconds.
    init_tim2(10417);
    for(;;) {
        asm("wfi");
        // If we hit the end of the MIDI file, start over.
        if (mp->nexttick == MAXTICKS)
            mp = midi_init(midifile);
    }
}

