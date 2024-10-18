#ifndef __CORE_COMMUNICATION_H__
#define __CORE_COMMUNICATION_H__

void communication_init();

void communication_send_serial(const char *format, ...);
void communication_send_radio(const char *format, ...);

#endif