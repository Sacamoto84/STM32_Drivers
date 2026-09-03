#ifndef Arduino_h
#define Arduino_h

#define DEC 10

#define HIGH 0x1
#define LOW  0x0

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define SERIAL  0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define CHANGE 1
#define FALLING 2
#define RISING 3

#define NOW  uwTick

// #define abs(x) ((x)>0?(x):-(x)) // patched for STM32 build
// #define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
// #define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define interrupts()   __enable_irq()
#define noInterrupts() __disable_irq()

#define clockCyclesPerMicrosecond()  ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

#define lowByte(w)  ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit)  (((value) >> (bit)) & 0x01)
#define bitSet(value, bit)   ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#ifndef _NOP
#define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

#define bit(b) (1UL << (b))
//macro added for compatibility
#ifndef _BV
  #define _BV(bit) (1 << (bit))
#endif
#ifndef cbi
  #define cbi(reg, bitmask) *reg &= ~bitmask
#endif
#ifndef sbi
  #define sbi(reg, bitmask) *reg |= bitmask
#endif

typedef unsigned int word;
typedef bool boolean __attribute__((deprecated));
typedef uint8_t byte;


#define _FORCE_INLINE_ __attribute__((__always_inline__)) __inline__
#define  FORCE_INLINE  __attribute__((always_inline)) inline
#define NO_INLINE      __attribute__((noinline))
#define _UNUSED        __attribute__((unused))






#endif
