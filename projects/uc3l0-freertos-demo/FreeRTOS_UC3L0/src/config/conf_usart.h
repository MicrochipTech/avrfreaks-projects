/*****************************************************************************
 *
 * \file
 *
 * \brief FreeRTOS demonstration configuration for AVR UC3.
 *
 * Copyright (c) 2011 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 ******************************************************************************/


#ifndef CONF_USART_H
#define CONF_USART_H

#define configDBG_USART              USART0
#define configDBG_USART_RX_PIN       USART0_RXD_PIN
#define configDBG_USART_RX_FUNCTION  USART0_RXD_FUNCTION
#define configDBG_USART_TX_PIN       USART0_TXD_PIN
#define configDBG_USART_TX_FUNCTION  USART0_TXD_FUNCTION
#define configDBG_USART_IRQ          USART0_IRQ
#define configDBG_USART_BAUDRATE     57600

#define serialPORT_USART              USART
#define serialPORT_USART_RX_PIN       USART_RXD_PIN
#define serialPORT_USART_RX_FUNCTION  USART_RXD_FUNCTION
#define serialPORT_USART_TX_PIN       USART_TXD_PIN
#define serialPORT_USART_TX_FUNCTION  USART_TXD_FUNCTION
#define serialPORT_USART_IRQ          USART_IRQ
#define serialPORT_USART_BAUDRATE     57600

#endif //CONF_USART_H