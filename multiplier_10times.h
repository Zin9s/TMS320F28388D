#ifndef _MULTIPLIER_10TIMES_H_
#define _MULTIPLIER_10TIMES_H_
//#############################################################################
//
// FILE:   multiplier_10times.h
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
extern float fVal;           //Holds the input argument to the task
extern float fResult;        //The arsine of the input argument
extern uint32_t iCLA1Task1Count;
extern uint32_t iCLA1Task2Count;

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

#endif //end of _MULTIPLIER_10TIMES_H_ definition

//
// End of file
//
