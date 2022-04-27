#include "stm32f0xx.h"
#include "lcd.h"
#include <stdint.h>
#include "midi.h"
#include "midiplay.h"
#include <stdlib.h>

#define VOICES 15

#define UPDATE_RATE 30 //In updates per second
#define NEW_GAME_START_X 30
#define NEW_GAME_START_Y 160
#define PIPE_START_X 240

#define UPPER_SCREEN_BOUND 0
#define LOWER_SCREEN_BOUND 350
#define MAX_DOWNWARD_VELOCITY -12
#define VELOCITY_MODIFIER 2
#define JUMP_VELOCITY 15
#define PIPE_SPEED 5

#define PIPE_HORIZONTAL_PAD 5
#define PIPE_VERTICAL_PAD 0
#define BIRD_VERTICAL_PAD 14
#define BIRD_HORIZONTAL_PAD -1

#define BIRD_HEIGHT 24
#define BIRD_WIDTH 34
#define PIPE_HEIGHT 235
#define PIPE_WIDTH 40

#define DIFFICULTY_EASY 60
#define DIFFICULTY_MED  50
#define DIFFICULTY_HARD 40

#define BACKGROUND backgroundFlappy
#define BIRD_NORMAL bird1
#define BIRD_FLAP bird3
#define PIPE_TOP brick_wall
#define PIPE_BOTTOM brick_wall

#define TempPicturePtr(name,width,height) Picture name[(width)*(height)/6+2] = { {width,height,2} }

extern const Picture bird1;
extern const Picture bird3;
extern const Picture brick_wall;
extern const Picture backgroundFlappy;

int bird_x = NEW_GAME_START_X;
int bird_y = NEW_GAME_START_Y;
int bird_v = 0; //velocity

int pipe_x = PIPE_START_X;
int top_pipe_y = 0;
int bottom_pipe_y = 0;


char physics_enabled = 0;
char start_game = 0;
int player_score = 0;
int high_score = 0;
unsigned int seed = 0;

char pipes_on_screen = 0;
int difficulty = DIFFICULTY_EASY;

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
    bird_v = JUMP_VELOCITY;
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
        bird_v = bird_y <= UPPER_SCREEN_BOUND ? -1 : bird_v;
        int dv = bird_v<=MAX_DOWNWARD_VELOCITY? 0 : -1;
        bird_v += dv;
        int dy = (bird_y > LOWER_SCREEN_BOUND && bird_v < 0) ? 0 : bird_v/VELOCITY_MODIFIER;
        bird_y -= dy;

        if(bird_v > JUMP_VELOCITY-3) {
            draw_flapping(bird_x,bird_y);
        }
        else{
            draw_bird_normal(bird_x,bird_y); //TODO - Add in pipe drawing, timing, and spacing logic
        }
        if(!pipes_on_screen && rand() %20 == 5){
            pipe_x = PIPE_START_X;
            int spacing = difficulty + rand() % 15;
            int center = 50 + rand() % (225-difficulty);
            top_pipe_y = center - spacing - PIPE_HEIGHT;
            bottom_pipe_y = center + spacing;
            pipes_on_screen = 1;
        }
        if(pipes_on_screen){
            pipe_x -= PIPE_SPEED;
            if(pipe_x < 0-PIPE_WIDTH){
                pipes_on_screen = 0;
            }
            draw_pipes(pipe_x,top_pipe_y,bottom_pipe_y);
        }
    }
}


void endGame()
{
    start_game = 0;
    disable_physics();
    stop_music(); //TODO - Figure out how to restart song from beginning
    new_game();
}

void collisionDetection()
{
    //Case 0 (Bird hits bottom)
    if(bird_y >= 281) endGame();
    //Case 1 (Bird hits top)
    else if (bird_y <= 0) endGame();
    //Case 2 (Bird hits left side of bottom pipe)
    //Case 3 (Bird hits top side of the bottom pipe)
    //Case 4 (Bird hits left side of top pipe)
    //Case 5 (Bird hits bottom of top pipe)
}



void enable_physics(){
    physics_enabled = 1;
}

void disable_physics(){
    physics_enabled = 0;
}


void draw_bird_normal(int x, int y)
{
    TempPicturePtr(tmp, BIRD_WIDTH + BIRD_HORIZONTAL_PAD,BIRD_HEIGHT + BIRD_VERTICAL_PAD); // Create a temporary 50x50 image.
    pic_subset(tmp, &BACKGROUND, x-tmp->width/2, y-tmp->height/2); // Copy the background
    pic_overlay(tmp, BIRD_HORIZONTAL_PAD / 2,BIRD_VERTICAL_PAD / 2, &BIRD_NORMAL, BIRD_NORMAL.transparent); // Overlay the ball
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
}

void draw_flapping(int x, int y)
{
    TempPicturePtr(tmp, BIRD_WIDTH + BIRD_HORIZONTAL_PAD,BIRD_HEIGHT + BIRD_VERTICAL_PAD); // Create a temporary 50x50 image.
    pic_subset(tmp, &BACKGROUND, x-tmp->width/2, y-tmp->height/2); // Copy the background
    pic_overlay(tmp, BIRD_HORIZONTAL_PAD / 2,BIRD_VERTICAL_PAD / 2, &BIRD_FLAP, BIRD_FLAP.transparent); // Overlay the ball
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
}

void draw_pipes(int x, int top_y, int bottom_y){
    TempPicturePtr(pipe, PIPE_WIDTH + PIPE_HORIZONTAL_PAD,210); 

    pic_subset(pipe, &BACKGROUND, x-pipe->width, top_y-pipe->height); // Copy the background
    pic_overlay(pipe, 0,0, &PIPE_BOTTOM, PIPE_TOP.transparent); // Overlay the ball 
    LCD_DrawPicture(x-pipe->width,top_y-pipe->height, pipe); // Draw

    pic_subset(pipe, &BACKGROUND, x-pipe->width, bottom_y-pipe->height); // Copy the background
    pic_overlay(pipe, 0,0, &PIPE_TOP, PIPE_BOTTOM.transparent); // Overlay the ball 
    LCD_DrawPicture(x-pipe->width,bottom_y-pipe->height, pipe); // Draw
}

void draw_pipes2(int x, int top_y, int bottom_y){
    TempPicturePtr(pipe, PIPE_WIDTH + PIPE_HORIZONTAL_PAD,210); 
    TempPicturePtr(background, PIPE_HORIZONTAL_PAD, 210);

    pic_overlay(pipe, 0,0, &PIPE_BOTTOM, PIPE_TOP.transparent); // Overlay the ball 
    pic_subset(background, &BACKGROUND, x+pipe->width, top_y); // Copy the background
    pic_overlay(pipe, PIPE_WIDTH, 0, &background, 0);
    LCD_DrawPicture(x-pipe->width,top_y-pipe->height, pipe); // Draw

    pic_subset(background, &BACKGROUND, x+pipe->width, bottom_y); // Copy the background
    pic_overlay(pipe, PIPE_WIDTH, 0, &background, 0);
    LCD_DrawPicture(x-pipe->width,bottom_y-pipe->height, pipe); // Draw
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
extern const Picture brick_wall;
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

void start_music(){
    DAC->CR = DAC_CR_EN1;
}

void stop_music(){
    DAC->CR &= ~DAC_CR_EN1;
}

void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF;    

    midi_play();
}

void draw_num(u16 x,u16 y,u16 fc, u16 bc, int num, u8 size, u8 mode){
    int tempnum = num;
    int digits = 0;
    int tens = 1;
    do{
        digits++;
        tens *= 10;
        tempnum -= tempnum % tens;

    }while(tempnum > 0);
    tens = 1;
    for(int i = 1; i <= digits; i++){
        LCD_DrawChar(x + 8*(digits-i),y,fc,bc,num % 10 + 48, size, mode);
        num -= num % 10;
        num /= 10;
    }
}

void new_game(){
    LCD_Clear(0);
    int current_line = 50;

    if(seed > 0) { 
        LCD_DrawString(90,current_line, YELLOW, BLACK, "GAME OVER!" + player_score, 16, 1);
        current_line += 30;
        LCD_DrawString(80,current_line, YELLOW, BLACK, "YOUR SCORE: ", 16, 1);
        draw_num(173,current_line, YELLOW, BLACK, player_score, 16, 1);
        current_line += 20;
    }
    LCD_DrawString(80,current_line, YELLOW, BLACK, "HIGH SCORE: ", 16, 1);
    draw_num(173,current_line, YELLOW, BLACK, high_score, 16, 1);
    current_line += 40;
    LCD_DrawString(90,current_line, YELLOW, BLACK, seed? "NEW GAME?":"NEW GAME:", 16, 1);
    current_line += 20;
    LCD_DrawString(20,current_line, YELLOW, BLACK, "Press Any Button To Play!", 16, 1);

    while(start_game == 0){
        seed >= UINTMAX_MAX ? 1 : seed++;
    }
    srand(seed);
    start_new_game();
}

void start_new_game(){
    bird_x = NEW_GAME_START_X;
    bird_y = NEW_GAME_START_Y;
    bird_v = 0;
    pipe_x = PIPE_START_X;
    top_pipe_y = 0;
    bottom_pipe_y = 0;
    player_score = 0;
    pipes_on_screen = 0;
    difficulty = DIFFICULTY_EASY;
    LCD_DrawPicture(0,0,&BACKGROUND);
    enable_physics();
    start_music();
}

int main(void)
{
    init_button();
    LCD_Setup(); // this will call init_lcd_spi()
    init_tim7();

    init_wavetable_hybrid2();
    init_dac();
    init_tim6();
    MIDI_Player *mp = midi_init(midifile);
    init_tim2(10417);
    char flag = '0';

    new_game();
    for(;;) {
        if(physics_enabled)
        {
            collisionDetection();
        }
        if(player_score > 5 && player_score <= 10){
            difficulty = DIFFICULTY_MED;
        }
        else if(player_score > 10){
            difficulty = DIFFICULTY_HARD;
        }
        // If we hit the end of the MIDI file, start over.
        if (mp->nexttick == MAXTICKS)
            mp = midi_init(midifile);
    }
}
