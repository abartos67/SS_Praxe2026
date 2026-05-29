#include "MKL25Z4.h"
#include "wdog.h"
#include "rgb.h"
#include "littleHelper.h"

void hra(void);
void vyhra(void);
void kontrola();
void kontrolaTlacitka(void);
void nastaveni(void);
void vypnuti_led();
void zapnuti_led();

uint32_t xorshift32(uint32_t *state);

int cervenaAktivni = 0;
int ledka = 7;
int zmacknuto = 0;

uint32_t seedgeneratoru = 152;

typedef enum {
    ST_IDLE,
    ST_GAME,
    ST_WIN
} StavHry;

volatile StavHry stav = ST_IDLE;
volatile int restartHry = 0;

// =========================
// XORSHIFT RANDOM
// =========================
uint32_t xorshift32(uint32_t *state)
{
    uint32_t x = *state;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    return *state = x;
}

// =========================
// RESET STAV HRY
// =========================
void restart_hry(void)
{
    vypnuti_led();
    rgb_set(Off, Off, Off);

    cervenaAktivni = 0;
    zmacknuto = 0;
    ledka = 7;

    stav = ST_IDLE;
}

// =========================
// MAIN
// =========================
__attribute__((weak)) int main(void)
{
    wdog_init(WDOG_CONF_DIS);
    nastaveni();
    rgb_nastaveni();
    kontrola();

    restart_hry();

    while (1)
    {
        // =========================
        // HARD RESTART
        // =========================
        if (restartHry == 1)
        {
            restartHry = 0;

            restart_hry();

            stav = ST_GAME;

            // startovní RGB animace
            for (int i = 0; i < 2; i++)
            {
                rgb_set(On, Off, Off);
            }

            rgb_set(Off, Off, Off);
        }

        // =========================
        // GAME LOOP
        // =========================
        if (stav == ST_GAME)
        {
            hra();
        }

        // =========================
        // WIN LOOP
        // =========================
        if (stav == ST_WIN)
        {
            vyhra();
        }
    }
}

// =========================
// INTERRUPT
// =========================
void PORTA_IRQHandler(void)
{
    PORTA->PCR[4]  |= 1 << PORT_PCR_ISF_SHIFT;
    PORTA->PCR[5]  |= 1 << PORT_PCR_ISF_SHIFT;
    PORTA->PCR[12] |= 1 << PORT_PCR_ISF_SHIFT;

    // =========================
    // START / RESET (PA5)
    // =========================
    if (!(GPIOA->PDIR & (1 << 5)))
    {
        restartHry = 1;
        stav = ST_GAME;
    }

    // =========================
    // GAME BUTTON (PA4)
    // =========================
    if (!(GPIOA->PDIR & (1 << 4)))
    {
        if (cervenaAktivni == 1 && zmacknuto == 0)
        {
            zmacknuto = 1;
            ledka++;

            switch (ledka)
            {
                case 8:  GPIOB->PDOR &= ~(1 << 8); break;
                case 9:  GPIOB->PDOR &= ~(1 << 9); break;
                case 10: GPIOB->PDOR &= ~(1 << 10); break;
                case 11: GPIOB->PDOR &= ~(1 << 11); break;
                case 12: GPIOC->PDOR &= ~(1 << 10); break;
                case 13: GPIOC->PDOR &= ~(1 << 11); break;
                case 14: GPIOC->PDOR &= ~(1 << 12); break;
                case 15: GPIOC->PDOR &= ~(1 << 13); break;
            }

            if (ledka >= 15)
            {
                stav = ST_WIN;
            }
        }
        else if (cervenaAktivni == 0)
        {
            ledka = 7;
            vypnuti_led();
        }
    }
}

// =========================
// HRA
// =========================
void hra(void)
{
    uint32_t nahodna_led =
        (xorshift32(&seedgeneratoru) & 3) + 1;

    // =========================
    // CERVENA
    // =========================
    if (nahodna_led == 1)
    {
        rgb_set(On, Off, Off);
        cervenaAktivni = 1;
    }

    // =========================
    // ZELENA
    // =========================
    else if (nahodna_led == 2)
    {
        rgb_set(Off, On, Off);
        cervenaAktivni = 0;
    }

    // =========================
    // MODRA
    // =========================
    else if (nahodna_led == 3)
    {
        rgb_set(Off, Off, On);
        cervenaAktivni = 0;
    }

    // =========================
    // ZLUTA
    // =========================
    else if (nahodna_led == 4)
    {
        rgb_set(On, On, Off);
        cervenaAktivni = 0;
    }

    zmacknuto = 0;

    for (volatile int i = 0; i < 1800000; i++) {}
}

// =========================
// VÝHRA
// =========================
void vyhra(void)
{
    while (stav == ST_WIN)
    {
        // =========================
        // RESTART PRI STISKU TLACITKA
        // =========================
        if (restartHry == 1)
        {
            restartHry = 0;

            // reset bodu
            vypnuti_led();
            ledka = 7;
            zmacknuto = 0;
            cervenaAktivni = 0;

            // znovu spusteni hry
            stav = ST_GAME;

            return;
        }

        // blikani zelene
        rgb_set(Off, On, Off);

        for (volatile int i = 0; i < 1800000; i++) {}

        rgb_set(Off, Off, Off);

        for (volatile int i = 0; i < 1800000; i++) {}
    }
}
void nastaveni(void)
{
    NVIC_SetPriority(PORTA_IRQn,2);
    NVIC_EnableIRQ(PORTA_IRQn);

    PORTA->PCR[4]|=0b1010<<PORT_PCR_IRQC_SHIFT;
    PORTA->PCR[5]|=0b1010<<PORT_PCR_IRQC_SHIFT;

    for(int i=8;i<12;i++){
    PORTB->PCR[i]|=1<<PORT_PCR_MUX_SHIFT;
    }

    for(int i=10;i<14;i++){
    PORTC->PCR[i]|=1<<PORT_PCR_MUX_SHIFT;
    }
    
    PORTA->PCR[4]|=1<<PORT_PCR_MUX_SHIFT;
    PORTA->PCR[5]|=1<<PORT_PCR_MUX_SHIFT;
    PORTA->PCR[12]|=1<<PORT_PCR_MUX_SHIFT;

    GPIOB->PDDR|=(1<<8);
    GPIOB->PDDR|=(1<<9);
    GPIOB->PDDR|=(1<<10);
    GPIOB->PDDR|=(1<<11);
    GPIOC->PDDR|=(1<<10);
    GPIOC->PDDR|=(1<<11);
    GPIOC->PDDR|=(1<<12);
    GPIOC->PDDR|=(1<<13);

    GPIOA->PDDR&=~(1<<4);
    GPIOA->PDDR&=~(1<<5);
    GPIOA->PDDR&=~(1<<12);

    GPIOB->PDOR|=(1<<8);
    GPIOB->PDOR|=(1<<9);
    GPIOB->PDOR|=(1<<10);
    GPIOB->PDOR|=(1<<11);
    GPIOC->PDOR|=(1<<10);
    GPIOC->PDOR|=(1<<11);
    GPIOC->PDOR|=(1<<12);
    GPIOC->PDOR|=(1<<13);
}

void vypnuti_led(){
    GPIOB->PDOR|=(1<<8);
    GPIOB->PDOR|=(1<<9);
    GPIOB->PDOR|=(1<<10);
    GPIOB->PDOR|=(1<<11);
    GPIOC->PDOR|=(1<<10);
    GPIOC->PDOR|=(1<<11);
    GPIOC->PDOR|=(1<<12);
    GPIOC->PDOR|=(1<<13);
}

void zapnuti_led(){
    GPIOB->PDOR&=~(1<<8);
    GPIOB->PDOR&=~(1<<9);
    GPIOB->PDOR&=~(1<<10);
    GPIOB->PDOR&=~(1<<11);
    GPIOC->PDOR&=~(1<<10);
    GPIOC->PDOR&=~(1<<11);
    GPIOC->PDOR&=~(1<<12);
    GPIOC->PDOR&=~(1<<13);
}

void kontrola(void)
{
    GPIOB->PDOR&=~(1<<18);
    heavyFunction();
    GPIOB->PDOR|=(1<<18);
    GPIOB->PDOR&=~(1<<19);
    heavyFunction();
    GPIOB->PDOR|=(1<<19);
    GPIOD->PDOR&=~(1<<1);
    heavyFunction();
    GPIOD->PDOR|=(1<<1);
    zapnuti_led();
    heavyFunction();
    vypnuti_led();
}
