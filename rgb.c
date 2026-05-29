#include "MKL25Z4.h"
#include "rgb.h"


/* globalni promenne pro aktualni stav */
Stav aktualniR;
Stav aktualniG;
Stav aktualniB;



void rgb_set(Stav r, Stav g, Stav b)
{
    aktualniR = r;
    aktualniG = g;
    aktualniB = b;

    if(r==On && g==On && b==On){
        GPIOB->PDOR &= ~(1u << 18);
        GPIOB->PDOR &= ~(1u << 19);
        GPIOD->PDOR &= ~(1u << 1);
    }
    else if(r==Off && g==Off && b==Off){
        GPIOB->PDOR |= (1u << 18);
        GPIOB->PDOR |= (1u << 19);
        GPIOD->PDOR |= (1u << 1);
    }
    else if(r==On && g==Off && b==Off){
        GPIOB->PDOR &= ~(1u << 18);
        GPIOB->PDOR |= (1u << 19);
        GPIOD->PDOR |= (1u << 1);
    }
    else if(r==Off && g==On && b==Off){
        GPIOB->PDOR |= (1u << 18);
        GPIOB->PDOR &= ~(1u << 19);
        GPIOD->PDOR |= (1u << 1);
    }
    else if(r==Off && g==Off && b==On){
        GPIOB->PDOR |= (1u << 18);
        GPIOB->PDOR |= (1u << 19);
        GPIOD->PDOR &= ~(1u << 1);
    }
    else if(r==Off && g==On && b==On){
        GPIOB->PDOR |= (1u << 18);
        GPIOB->PDOR &= ~(1u << 19);
        GPIOD->PDOR &= ~(1u << 1);
    }
    else if(r==On && g==Off && b==On){
        GPIOB->PDOR &= ~(1u << 18);
        GPIOB->PDOR |= (1u << 19);
        GPIOD->PDOR &= ~(1u << 1);
    }
    else if(r==On && g==On && b==Off){
        GPIOB->PDOR &= ~(1u << 18);
        GPIOB->PDOR &= ~(1u << 19);
        GPIOD->PDOR |= (1u << 1);
    }
}

void rgb_nastaveni(){
    PORTB->PCR[18]|=1<<PORT_PCR_MUX_SHIFT;
    PORTB->PCR[19]|=1<<PORT_PCR_MUX_SHIFT;
    PORTD->PCR[1]|=1<<PORT_PCR_MUX_SHIFT;

    GPIOB->PDDR|=(1<<18);
    GPIOB->PDDR|=(1<<19);
    GPIOD->PDDR|=(1<<1); 

    GPIOB->PDOR|=(1<<18);
    GPIOB->PDOR|=(1<<19);
    GPIOD->PDOR|=(1<<1);
}

