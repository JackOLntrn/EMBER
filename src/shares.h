/** @file shares.h
 * This file contains extern declarations of shares and queues 
 * which are used in multiple files of the EMBER project.
 * 
 * @author Jackson Cordova
 * @date November 2025
 */

#ifndef _SHARES_H_
#define _SHARES_H_

#include "taskshare.h"
#include "taskqueue.h"
#include <ESP32Encoder.h>
#include "DRV8871Driver.h"


extern Share<int32_t> panRefCount;
extern Share<int32_t> tiltRefCount;
extern Share<uint16_t> hotIndex;
extern Share<bool> fire;
extern Share<bool> spray;
extern float Frame[768]; // place frame in static memory to avoid large stack usage
extern ESP32Encoder enc1; // create encoder object for motor 1
extern ESP32Encoder enc2; // create encoder object for motor 2
extern DRV8871Driver Motor1;
extern DRV8871Driver Motor2; 


#endif