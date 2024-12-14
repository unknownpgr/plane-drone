#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "communication-protocol.h"

void communication_init();

void communication_send_serial(const char *format, ...);
void communication_receive_serial(char *buffer, uint32_t size);

void communication_send_radio(const char *format, ...);
bool communication_receive_radio(ControllerData *protocol);
void communication_send_ack(uint8_t *bytes, uint8_t size);