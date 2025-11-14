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


extern Share<int32_t> panRefCount;
extern Share<int32_t> tiltRefCount;
extern Share<bool> fire;


#endif