/** @file task_webserver.h
 * task for running a simple web server to demonstrate IoT functionality
 * 
 * @author Jackson Cordova
 * @date December 2025
 */

 #ifndef _task_webserver_h_
 #define _task_webserver_h_
 #undef USE_LAN

 void task_webserver (void* p_params);
 void setup_wifi(void);
 extern float Frame[768]; // place frame in static memory to avoid large stack usage
 #endif