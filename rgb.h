#ifndef RGB_LED_H_
#define RGB_LED_H_

typedef enum {
    Off = 0,
    On = 1
} Stav;

extern Stav aktualniR;
extern Stav aktualniG;
extern Stav aktualniB;

void rgb_set(Stav r, Stav g, Stav b);
void rgb_nastaveni();


#endif /* RGB_LED_H_ */


