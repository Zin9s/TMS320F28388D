#ifndef _LEDCOUNTER_H_
#define _LEDCOUNTER_H_
//#############################################################################
//
// FILE:   LEDCounter.h
//
// TITLE:  Test code
//
//#############################################################################

//
// Included Files
//
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// Defines
//

//
// Globals
//

//
//Task 1 (C) Variables
//
extern int32_t iCLA1Task1Count;
extern int32_t iLedD1Signal;

//
//Task 2 (C) Variables
//

//
//Task 3 (C) Variables
//

//
//Task 4 (C) Variables
//

//
//Task 5 (C) Variables
//

//
//Task 6 (C) Variables
//

//
//Task 7 (C) Variables
//

//
//Task 8 (C) Variables
//
extern int32_t iCLA1Task8Count;

//
//Common (C) Variables
//

//
// Function Prototypes
//
// The following are symbols defined in the CLA assembly code
// Including them in the shared header file makes them
// .global and the main CPU can make use of them.
//
__interrupt void Cla1Task1();
__interrupt void Cla1Task2();
__interrupt void Cla1Task3();
__interrupt void Cla1Task4();
__interrupt void Cla1Task5();
__interrupt void Cla1Task6();
__interrupt void Cla1Task7();
__interrupt void Cla1Task8();

#ifdef __cplusplus
}
#endif // extern "C"

#endif //end of _LEDCOUNTER_H_ definition

//
// End of file
//
