/*******************************
 * Projet Cortex  -  Dronolab  *
 * http://dronolab.etsmtl.ca/  *
 *******************************/

#include "twimsg.h"

const bool TwiMsg::READ = true;
const bool TwiMsg::WRITE = false;

TwiMsg::TwiMsg() : size(1), pos(1), readmode(false) {
}

void TwiMsg::reset(bool read_nowrite) {
    size = 1;
    pos = 1;
    readmode = read_nowrite;
}

void TwiMsg::data_to_send(uint8_t data) {
    buffer[size].write = data;
    size++;
}

void TwiMsg::data_to_recv(uint8_t * data) {
    buffer[size].read = data;
    size++;
}

void TwiMsg::address(uint8_t data) {
    buffer[0].addr = data;
}

void TwiMsg::mode(bool read_nowrite) {
    readmode = read_nowrite;
}

void TwiMsg::get_addr(uint8_t & data) {
    data = buffer[0].addr;
}

bool TwiMsg::get_write(uint8_t & data) {
    data = buffer[pos].write;
    pos++;
    return pos == size;
}

bool TwiMsg::get_read(uint8_t **data) {
    *data = buffer[pos].read;
    pos++;
    return pos == size;
}

bool TwiMsg::is_empty() {
    return pos == size;
}

bool TwiMsg::get_mode() {
    return readmode;
}

bool TwiMsg::is_one_msg_remaning(){

	return ((size - pos) == 1);
}


