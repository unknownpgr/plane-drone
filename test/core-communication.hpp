#pragma once

void communication_init();

void communication_send_serial(const char *format, ...);
void communication_send_radio(const char *format, ...);