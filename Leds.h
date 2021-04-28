//Yaroslav Yanin 1620969   yyanin@ucsc.edu

//LEDS_GET() returns the value of the E latch register
#ifndef LEDS_H
#define LEDS_H





#define LEDS_INIT() do {\
    TRISE=(0);\
    LATE=(0);\
} while(0);
#define LEDS_SET(x)(LATE=(x))
#define LEDS_GET()(LATE)

#endif