// *************************************************************************************************
// This file defines interface for clock related functions for the CC1110/CC2510 family of
// RF system-on-chips from Texas Instruments.
// Copyright 2007 Texas Instruments, Inc.
// *************************************************************************************************

#ifndef _DRIVER_CLOCK_H_
#define _DRIVER_CLOCK_H_

// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Define section
#define CLOCK_SRC_XOSC      0x00u  /*  High speed Crystal Oscillator Control */
#define CLOCK_SRC_HFRC      0x01u  /*  Low power RC Oscillator */

// *************************************************************************************************
// Prototype section
extern void clock_set_main_src(u8 source);

#endif /*_DRIVER_CLOCK_H_*/
