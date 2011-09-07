/**
 * \file
 *
 * \brief USBC Device Driver header file.
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef _USBC_DEVICE_H_
#define _USBC_DEVICE_H_

#include "compiler.h"
#include "preprocessor.h"


//! \ingroup usb_device_group
//! \defgroup udd_group USB Device Driver (UDD)
//! USBC low-level driver for USB Device mode
//!
//! @warning Bit-masks are used instead of bit-fields because PB registers
//! require 32-bit write accesses while AVR32-GCC 4.0.2 builds 8-bit
//! accesses even when volatile unsigned int bit-fields are specified.
//! @{

//! @name USBC Device IP properties
//! These macros give access to IP properties
//! @{
  //! Get maximal number of endpoints
#define  UDD_get_endpoint_max_nbr()          (((Rd_bitfield(AVR32_USBC_ufeatures, AVR32_USBC_UFEATURES_EPT_NBR_MAX_MASK) - 1) & ((1 << AVR32_USBC_UFEATURES_EPT_NBR_MAX_SIZE) - 1)) + 1)
//! @}

//! @name USBC Device speeds management
//! @{
  //! Enable/disable device low-speed mode
#define  udd_low_speed_enable()              (Set_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_LS_MASK))
#define  udd_low_speed_disable()             (Clr_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_LS_MASK))
  //! Test if device low-speed mode is forced
#define  Is_udd_low_speed_enable()           (Tst_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_LS_MASK))

#ifdef AVR32_USBC_UDCON_SPDCONF
  //! Enable high speed mode
#  define   udd_high_speed_enable()          (Wr_bitfield(AVR32_USBC.udcon, AVR32_USBC_UDCON_SPDCONF_MASK, 0))
  //! Disable high speed mode
#  define   udd_high_speed_disable()         (Wr_bitfield(AVR32_USBC.udcon, AVR32_USBC_UDCON_SPDCONF_MASK, 3))
  //! Test if controller is in full speed mode
#  define   Is_udd_full_speed_mode()         (Rd_bitfield(AVR32_USBC.usbsta, AVR32_USBC_USBSTA_SPEED_MASK) == AVR32_USBC_USBSTA_SPEED_FULL)
#else
#  define   udd_high_speed_enable()          do { } while (0)
#  define   udd_high_speed_disable()         do { } while (0)
#  define   Is_udd_full_speed_mode()         true
#endif
//! @}

//! @name USBC Device HS test mode management
//! @{
#ifdef AVR32_USBC_UDCON_SPDCONF
  //! Enable high speed test mode
#  define   udd_enable_hs_test_mode()        (Wr_bitfield(AVR32_USBC.udcon, AVR32_USBC_UDCON_SPDCONF_MASK, 2))
#  define   udd_enable_hs_test_mode_j()      (Set_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_TSTJ_MASK))
#  define   udd_enable_hs_test_mode_k()      (Set_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_TSTK_MASK))
#  define   udd_enable_hs_test_mode_packet() (Set_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_TSTPCKT_MASK))
#endif
//! @}

//! @name USBC Device vbus management
//! @{
#define  udd_enable_vbus_interrupt()         (Set_bits(AVR32_USBC.usbcon, AVR32_USBC_USBCON_VBUSTE_MASK))
#define  udd_disable_vbus_interrupt()        (Clr_bits(AVR32_USBC.usbcon, AVR32_USBC_USBCON_VBUSTE_MASK))
#define  Is_udd_vbus_interrupt_enabled()     (Tst_bits(AVR32_USBC.usbcon, AVR32_USBC_USBCON_VBUSTE_MASK))
#define  Is_udd_vbus_high()                  (Tst_bits(AVR32_USBC.usbsta, AVR32_USBC_USBSTA_VBUS_MASK))
#define  Is_udd_vbus_low()                   (!Is_udd_vbus_high())
#define  udd_ack_vbus_transition()           (AVR32_USBC.usbstaclr = AVR32_USBC_USBSTACLR_VBUSTIC_MASK)
#define  udd_raise_vbus_transition()         (AVR32_USBC.usbstaset = AVR32_USBC_USBSTASET_VBUSTIS_MASK)
#define  Is_udd_vbus_transition()            (Tst_bits(AVR32_USBC.usbsta, AVR32_USBC_USBSTA_VBUSTI_MASK))
//! @}


//! @name USBC device attach control
//! These macros manage the USBC Device attach.
//! @{
  //! detaches from USB bus
#define  udd_detach_device()                 (Set_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_DETACH_MASK))
  //! attaches to USB bus
#define  udd_attach_device()                 (Clr_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_DETACH_MASK))
  //! test if the device is detached
#define  Is_udd_detached()                   (Tst_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_DETACH_MASK))
//! @}


//! @name USBC device bus events control
//! These macros manage the USBC Device bus events.
//! @{

//! Initiates a remote wake-up event
//! @{
#define  udd_initiate_remote_wake_up()       (Set_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_RMWKUP_MASK))
#define  Is_udd_pending_remote_wake_up()     (Tst_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_RMWKUP_MASK))
//! @}

//! Manage upstream resume event (=remote wakeup)
//! The USB driver sends a resume signal called "Upstream Resume"
//! @{
#define  udd_enable_remote_wake_up_interrupt()     (AVR32_USBC.udinteset = AVR32_USBC_UDINTESET_UPRSMES_MASK)
#define  udd_disable_remote_wake_up_interrupt()    (AVR32_USBC.udinteclr = AVR32_USBC_UDINTECLR_UPRSMEC_MASK)
#define  Is_udd_remote_wake_up_interrupt_enabled() (Tst_bits(AVR32_USBC.udinte, AVR32_USBC_UDINTE_UPRSME_MASK))
#define  udd_ack_remote_wake_up_start()            (AVR32_USBC.udintclr = AVR32_USBC_UDINTCLR_UPRSMC_MASK)
#define  udd_raise_remote_wake_up_start()          (AVR32_USBC.udintset = AVR32_USBC_UDINTSET_UPRSMS_MASK)
#define  Is_udd_remote_wake_up_start()             (Tst_bits(AVR32_USBC.udint, AVR32_USBC_UDINT_UPRSM_MASK))
//! @}

//! Manage end of resume event (=remote wakeup)
//! The USB controller detects a valid "End of Resume" signal initiated by the host
//! @{
#define  udd_enable_resume_interrupt()             (AVR32_USBC.udinteset = AVR32_USBC_UDINTESET_EORSMES_MASK)
#define  udd_disable_resume_interrupt()            (AVR32_USBC.udinteclr = AVR32_USBC_UDINTECLR_EORSMEC_MASK)
#define  Is_udd_resume_interrupt_enabled()         (Tst_bits(AVR32_USBC.udinte, AVR32_USBC_UDINTE_EORSME_MASK))
#define  udd_ack_resume()                          (AVR32_USBC.udintclr = AVR32_USBC_UDINTCLR_EORSMC_MASK)
#define  udd_raise_resume()                        (AVR32_USBC.udintset = AVR32_USBC_UDINTSET_EORSMS_MASK)
#define  Is_udd_resume()                           (Tst_bits(AVR32_USBC.udint, AVR32_USBC_UDINT_EORSM_MASK))
//! @}

//! Manage wake-up event (=usb line activity)
//! The USB controller is reactivated by a filtered non-idle signal from the lines
//! @{
#define  udd_enable_wake_up_interrupt()            (AVR32_USBC.udinteset = AVR32_USBC_UDINTESET_WAKEUPES_MASK)
#define  udd_disable_wake_up_interrupt()           (AVR32_USBC.udinteclr = AVR32_USBC_UDINTECLR_WAKEUPEC_MASK)
#define  Is_udd_wake_up_interrupt_enabled()        (Tst_bits(AVR32_USBC.udinte, AVR32_USBC_UDINTE_WAKEUPE_MASK))
#define  udd_ack_wake_up()                         (AVR32_USBC.udintclr = AVR32_USBC_UDINTCLR_WAKEUPC_MASK)
#define  udd_raise_wake_up()                       (AVR32_USBC.udintset = AVR32_USBC_UDINTSET_WAKEUPS_MASK)
#define  Is_udd_wake_up()                          (Tst_bits(AVR32_USBC.udint, AVR32_USBC_UDINT_WAKEUP_MASK))
//! @}

//! Manage reset event
//! Set when a USB "End of Reset" has been detected
//! @{
#define  udd_enable_reset_interrupt()              (AVR32_USBC.udinteset = AVR32_USBC_UDINTESET_EORSTES_MASK)
#define  udd_disable_reset_interrupt()             (AVR32_USBC.udinteclr = AVR32_USBC_UDINTECLR_EORSTEC_MASK)
#define  Is_udd_reset_interrupt_enabled()          (Tst_bits(AVR32_USBC.udinte, AVR32_USBC_UDINTE_EORSTE_MASK))
#define  udd_ack_reset()                           (AVR32_USBC.udintclr = AVR32_USBC_UDINTCLR_EORSTC_MASK)
#define  udd_raise_reset()                         (AVR32_USBC.udintset = AVR32_USBC_UDINTSET_EORSTS_MASK)
#define  Is_udd_reset()                            (Tst_bits(AVR32_USBC.udint, AVR32_USBC_UDINT_EORST_MASK))
//! @}

//! Manage sart of frame event
//! @{
#define  udd_enable_sof_interrupt()                (AVR32_USBC.udinteset = AVR32_USBC_UDINTESET_SOFES_MASK)
#define  udd_disable_sof_interrupt()               (AVR32_USBC.udinteclr = AVR32_USBC_UDINTECLR_SOFEC_MASK)
#define  Is_udd_sof_interrupt_enabled()            (Tst_bits(AVR32_USBC.udinte, AVR32_USBC_UDINTE_SOFE_MASK))
#define  udd_ack_sof()                             (AVR32_USBC.udintclr = AVR32_USBC_UDINTCLR_SOFC_MASK)
#define  udd_raise_sof()                           (AVR32_USBC.udintset = AVR32_USBC_UDINTSET_SOFS_MASK)
#define  Is_udd_sof()                              (Tst_bits(AVR32_USBC.udint, AVR32_USBC_UDINT_SOF_MASK))
#define  udd_frame_number()                        (Rd_bitfield(AVR32_USBC.udfnum, AVR32_USBC_UDFNUM_FNUM_MASK))
#define  Is_udd_frame_number_crc_error()           (Tst_bits(AVR32_USBC.udfnum, AVR32_USBC_UDFNUM_FNCERR_MASK))
//! @}

//! Manage suspend event
//! @{
#define  udd_enable_suspend_interrupt()            (AVR32_USBC.udinteset = AVR32_USBC_UDINTESET_SUSPES_MASK)
#define  udd_disable_suspend_interrupt()           (AVR32_USBC.udinteclr = AVR32_USBC_UDINTECLR_SUSPEC_MASK)
#define  Is_udd_suspend_interrupt_enabled()        (Tst_bits(AVR32_USBC.udinte, AVR32_USBC_UDINTE_SUSPE_MASK))
#define  udd_ack_suspend()                         (AVR32_USBC.udintclr = AVR32_USBC_UDINTCLR_SUSPC_MASK)
#define  udd_raise_suspend()                       (AVR32_USBC.udintset = AVR32_USBC_UDINTSET_SUSPS_MASK)
#define  Is_udd_suspend()                          (Tst_bits(AVR32_USBC.udint, AVR32_USBC_UDINT_SUSP_MASK))
//! @}

//! @}

//! @name USBC device address control
//! These macros manage the USBC Device address.
//! @{
  //! enables USB device address
#define  udd_enable_address()                      (Set_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_ADDEN_MASK))
  //! disables USB device address
#define  udd_disable_address()                     (Clr_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_ADDEN_MASK))
#define  Is_udd_address_enabled()                  (Tst_bits(AVR32_USBC.udcon, AVR32_USBC_UDCON_ADDEN_MASK))
  //! configures the USB device address
#define  udd_configure_address(addr)               (Wr_bitfield(AVR32_USBC.udcon, AVR32_USBC_UDCON_UADD_MASK, addr))
  //! gets the currently configured USB device address
#define  udd_get_configured_address()              (Rd_bitfield(AVR32_USBC.udcon, AVR32_USBC_UDCON_UADD_MASK))
//! @}


//! @name USBC Device endpoint drivers
//! These macros manage the common features of the endpoints.
//! @{

//! Generic macro for USBC registers that can be arrayed
//! @{
#define USBC_ARRAY(reg,index)              ((&AVR32_USBC.reg)[(index)])
//! @}

//! @name USBC Devcice endpoint configguration
//! @{
  //! enables the selected endpoint
#define  udd_enable_endpoint(ep)                   (Set_bits(AVR32_USBC.uerst, AVR32_USBC_UERST_EPEN0_MASK << (ep)))
  //! disables the selected endpoint
#define  udd_disable_endpoint(ep)                  (Clr_bits(AVR32_USBC.uerst, AVR32_USBC_UERST_EPEN0_MASK << (ep)))
  //! tests if the selected endpoint is enabled
#define  Is_udd_endpoint_enabled(ep)               (Tst_bits(AVR32_USBC.uerst, AVR32_USBC_UERST_EPEN0_MASK << (ep)))
  //! resets the selected endpoint
#define  udd_reset_endpoint(ep)                    (Set_bits(AVR32_USBC.uerst, AVR32_USBC_UERST_EPRST0_MASK << (ep)),\
                                                   Clr_bits(AVR32_USBC.uerst, AVR32_USBC_UERST_EPRST0_MASK << (ep)))
  //! tests if the selected endpoint is being reset
#define  Is_udd_resetting_endpoint(ep)             (Tst_bits(AVR32_USBC.uerst, AVR32_USBC_UERST_EPRST0_MASK << (ep)))

  //! configures the selected endpoint type
#define  udd_configure_endpoint_type(ep, type)     (Wr_bitfield(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_EPTYPE_MASK, type))
  //! gets the configured selected endpoint type
#define  udd_get_endpoint_type(ep)                 ((Rd_bitfield(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_EPTYPE_MASK))>>AVR32_USBC_UECFG0_EPTYPE_OFFSET)
  //! enables the bank autoswitch for the selected endpoint
//#define  udd_enable_endpoint_bank_autoswitch(ep)   (Set_bits(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_AUTOSW_MASK))
  //! disables the bank autoswitch for the selected endpoint
//#define  udd_disable_endpoint_bank_autoswitch(ep)   (Clr_bits(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_AUTOSW_MASK))
//#define  Is_udd_endpoint_bank_autoswitch_enabled(ep) (Tst_bits(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_AUTOSW_MASK))
  //! configures the selected endpoint direction
#define  udd_configure_endpoint_direction(ep, dir) (Wr_bitfield(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_EPDIR_MASK, dir))
  //! gets the configured selected endpoint direction
#define  udd_get_endpoint_direction(ep)            (Rd_bitfield(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_EPDIR_MASK))
#define  Is_udd_endpoint_in(ep)                    (Tst_bits(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_EPDIR_MASK))
  //! Bounds given integer size to allowed range and rounds it up to the nearest
  //! available greater size, then applies register format of USBC controller
  //! for endpoint size bit-field.
#define  udd_format_endpoint_size(size)            (32 - clz(((uint32_t)min(max(size, 8), 1024) << 1) - 1) - 1 - 3)
  //! configures the selected endpoint size
#define  udd_configure_endpoint_size(ep, size)     (Wr_bitfield(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_EPSIZE_MASK, udd_format_endpoint_size(size)))
  //! gets the configured selected endpoint size
#define  udd_get_endpoint_size(ep)                 (8 << Rd_bitfield(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_EPSIZE_MASK))
  //! configures the selected endpoint number of banks
#define  udd_configure_endpoint_bank(ep, bank)     (Wr_bitfield(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_EPBK_MASK, bank))
  //! gets the configured selected endpoint number of banks
#define  udd_get_endpoint_bank(ep)                 (Rd_bitfield(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_EPBK_MASK))

  //! configures selected endpoint in one step
#define  udd_configure_endpoint(ep, type, dir, size, bank) \
(\
   Wr_bits(USBC_ARRAY(uecfg0,ep), AVR32_USBC_UECFG0_EPTYPE_MASK |\
                                  AVR32_USBC_UECFG0_EPDIR_MASK  |\
                                  AVR32_USBC_UECFG0_EPSIZE_MASK |\
                                  AVR32_USBC_UECFG0_EPBK_MASK,   \
            (((uint32_t)(type) << AVR32_USBC_UECFG0_EPTYPE_OFFSET) & AVR32_USBC_UECFG0_EPTYPE_MASK) |\
            (((uint32_t)(dir ) << AVR32_USBC_UECFG0_EPDIR_OFFSET ) & AVR32_USBC_UECFG0_EPDIR_MASK ) |\
            ( (uint32_t)udd_format_endpoint_size(size) << AVR32_USBC_UECFG0_EPSIZE_OFFSET         ) |\
            (((uint32_t)(bank) << AVR32_USBC_UECFG0_EPBK_OFFSET  ) & AVR32_USBC_UECFG0_EPBK_MASK  ))\
)
//! returns the control direction
#define  udd_control_direction()                   (Rd_bitfield(USBC_ARRAY(uesta0(EP_CONTROL), AVR32_USBC_UESTA0_CTRLDIR_MASK))

  //! resets the data toggle sequence
#define  udd_reset_data_toggle(ep)                 (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_RSTDTS_MASK)
  //! tests if the data toggle sequence is being reset
#define  Is_udd_data_toggle_reset(ep)              (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_RSTDT_MASK))
  //! returns data toggle
#define  udd_data_toggle(ep)                       (Rd_bitfield(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_DTSEQ_MASK))
//! @}


//! @name USBC Device control endpoint
//! These macros contorl the endpoints.
//! @{

//! @name USBC Device control endpoint interrupts
//! These macros control the endpoints interrupts.
//! @{
  //! enables the selected endpoint interrupt
#define  udd_enable_endpoint_interrupt(ep)         (AVR32_USBC.udinteset = AVR32_USBC_UDINTESET_EP0INTES_MASK << (ep))
  //! disables the selected endpoint interrupt
#define  udd_disable_endpoint_interrupt(ep)        (AVR32_USBC.udinteclr = AVR32_USBC_UDINTECLR_EP0INTEC_MASK << (ep))
  //! tests if the selected endpoint interrupt is enabled
#define  Is_udd_endpoint_interrupt_enabled(ep)     (Tst_bits(AVR32_USBC.udinte, AVR32_USBC_UDINTE_EP0INTE_MASK << (ep)))
  //! tests if an interrupt is triggered by the selected endpoint
#define  Is_udd_endpoint_interrupt(ep)             (Tst_bits(AVR32_USBC.udint, AVR32_USBC_UDINT_EP0INT_MASK << (ep)))
  //! returns the lowest endpoint number generating an endpoint interrupt or MAX_PEP_NB if none
#define  udd_get_interrupt_endpoint_number()       (ctz(((AVR32_USBC.udint >> AVR32_USBC_UDINT_EP0INT_OFFSET) &\
                                                   (AVR32_USBC.udinte >> AVR32_USBC_UDINTE_EP0INTE_OFFSET)) |\
                                                   (1 << MAX_PEP_NB)))
//! @}

//! @name USBC Device control endpoint errors
//! These macros control the endpoint errors.
//! @{
  //! enables the STALL handshake
#define  udd_enable_stall_handshake(ep)            (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_STALLRQS_MASK)
  //! disables the STALL handshake
#define  udd_disable_stall_handshake(ep)           (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_STALLRQC_MASK)
  //! tests if STALL handshake request is running
#define  Is_udd_endpoint_stall_requested(ep)       (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_STALLRQ_MASK))
  //! tests if STALL sent
#define  Is_udd_stall(ep)                          (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_STALLEDI_MASK))
  //! acks STALL sent
#define  udd_ack_stall(ep)                         (USBC_ARRAY(uesta0clr,ep) = AVR32_USBC_UESTA0CLR_STALLEDIC_MASK)
  //! raises STALL sent
#define  udd_raise_stall(ep)                       (USBC_ARRAY(uesta0set,ep) = AVR32_USBC_UESTA0SET_STALLEDIS_MASK)
  //! enables STALL sent interrupt
#define  udd_enable_stall_interrupt(ep)            (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_STALLEDES_MASK)
  //! disables STALL sent interrupt
#define  udd_disable_stall_interrupt(ep)           (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_STALLEDEC_MASK)
  //! tests if STALL sent interrupt is enabled
#define  Is_udd_stall_interrupt_enabled(ep)        (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_STALLEDE_MASK))

  //! tests if a RAM access error occur
#define  Is_udd_ram_access_error(ep)               (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_RAMACCERI_MASK))

  //! tests if NAK OUT received
#define  Is_udd_nak_out(ep)                        (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_NAKOUTI_MASK))
  //! acks NAK OUT received
#define  udd_ack_nak_out(ep)                       (USBC_ARRAY(uesta0clr,ep) = AVR32_USBC_UESTA0CLR_NAKOUTIC_MASK)
  //! raises NAK OUT received
#define  udd_raise_nak_out(ep)                     (USBC_ARRAY(uesta0set,ep) = AVR32_USBC_UESTA0SET_NAKOUTIS_MASK)
  //! enables NAK OUT interrupt
#define  udd_enable_nak_out_interrupt(ep)          (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_NAKOUTES_MASK)
  //! disables NAK OUT interrupt
#define  udd_disable_nak_out_interrupt(ep)         (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_NAKOUTEC_MASK)
  //! tests if NAK OUT interrupt is enabled
#define  Is_udd_nak_out_interrupt_enabled(ep)      (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_NAKOUTE_MASK))

  //! tests if NAK IN received
#define  Is_udd_nak_in(ep)                         (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_NAKINI_MASK))
  //! acks NAK IN received
#define  udd_ack_nak_in(ep)                        (USBC_ARRAY(uesta0clr,ep) = AVR32_USBC_UESTA0CLR_NAKINIC_MASK)
  //! raises NAK IN received
#define  udd_raise_nak_in(ep)                      (USBC_ARRAY(uesta0set,ep) = AVR32_USBC_UESTA0SET_NAKINIS_MASK)
  //! enables NAK IN interrupt
#define  udd_enable_nak_in_interrupt(ep)           (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_NAKINES_MASK)
  //! disables NAK IN interrupt
#define  udd_disable_nak_in_interrupt(ep)          (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_NAKINEC_MASK)
  //! tests if NAK IN interrupt is enabled
#define  Is_udd_nak_in_interrupt_enabled(ep)       (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_NAKINE_MASK))

  //! acks endpoint isochronous overflow interrupt
#define  udd_ack_overflow_interrupt(ep)            (USBC_ARRAY(uesta0clr,ep) = AVR32_USBC_UESTA0CLR_OVERFIC_MASK)
  //! raises endpoint isochronous overflow interrupt
#define  udd_raise_overflow_interrupt(ep)          (USBC_ARRAY(uesta0set,ep) = AVR32_USBC_UESTA0SET_OVERFIS_MASK)
  //! tests if an overflow occurs
#define  Is_udd_overflow(ep)                       (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_OVERFI_MASK))
  //! enables overflow interrupt
#define  udd_enable_overflow_interrupt(ep)         (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_OVERFES_MASK)
  //! disables overflow interrupt
#define  udd_disable_overflow_interrupt(ep)        (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_OVERFEC_MASK)
  //! tests if overflow interrupt is enabled
#define  Is_udd_overflow_interrupt_enabled(ep)     (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_OVERFE_MASK))

  //! acks endpoint isochronous underflow interrupt
#define  udd_ack_underflow_interrupt(ep)           (USBC_ARRAY(uesta0clr,ep) = AVR32_USBC_UESTA0CLR_UNDERFIC_MASK)
  //! raises endpoint isochronous underflow interrupt
#define  udd_raise_underflow_interrupt(ep)         (USBC_ARRAY(uesta0set,ep) = AVR32_USBC_UESTA0SET_UNDERFIS_MASK)
  //! tests if an underflow occurs
#define  Is_udd_underflow(ep)                      (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_UNDERFI_MASK))
  //! enables underflow interrupt
#define  udd_enable_underflow_interrupt(ep)        (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_RXSTPES_MASK)
  //! disables underflow interrupt
#define  udd_disable_underflow_interrupt(ep)       (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_RXSTPEC_MASK)
  //! tests if underflow interrupt is enabled
#define  Is_udd_underflow_interrupt_enabled(ep)    (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_RXSTPE_MASK))

  //! tests if CRC ERROR ISO OUT detected
#define  Is_udd_crc_error(ep)                      (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_STALLEDI_MASK))
  //! acks CRC ERROR ISO OUT detected
#define  udd_ack_crc_error(ep)                     (USBC_ARRAY(uesta0clr,ep) = AVR32_USBC_UESTA0CLR_STALLEDIC_MASK)
  //! raises CRC ERROR ISO OUT detected
#define  udd_raise_crc_error(ep)                   (USBC_ARRAY(uesta0set,ep) = AVR32_USBC_UESTA0SET_STALLEDIS_MASK)
  //! enables CRC ERROR ISO OUT detected interrupt
#define  udd_enable_crc_error_interrupt(ep)        (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_STALLEDES_MASK)
  //! disables CRC ERROR ISO OUT detected interrupt
#define  udd_disable_crc_error_interrupt(ep)       (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_STALLEDEC_MASK)
  //! tests if CRC ERROR ISO OUT detected interrupt is enabled
#define  Is_udd_crc_error_interrupt_enabled(ep)    (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_STALLEDE_MASK))
//! @}

//! @name USBC Device control endpoint banks
//! These macros control the endpoint banks.
//! @{

  //! tests if endpoint read allowed
//#define  Is_udd_read_enabled(ep)                   (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_RWALL_MASK))
  //! tests if endpoint write allowed
//#define  Is_udd_write_enabled(ep)                  (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_RWALL_MASK))

  //! clears FIFOCON bit
#define  udd_ack_fifocon(ep)                       (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_FIFOCONC_MASK)
  //! tests if FIFOCON bit set
#define  Is_udd_fifocon(ep)                        (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_FIFOCON_MASK))

  //! Enable NYET
#define  udd_disable_nyet(ep)                      (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_NYETDISS_MASK)
  //! Disable NYET
#define  udd_enable_nyet(ep)                       (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_NYETDISC_MASK)

//! Enable busy on bank0
#define  udd_enable_busy_bank0(ep)                 (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_BUSY0S_MASK)
  //! Disable busy on bank0
#define  udd_disable_busy_bank0(ep)                (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_BUSY0C_MASK)
  //! Enable busy on bank1
#define  udd_enable_busy_bank1(ep)                 (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_BUSY1S_MASK)
  //! Disable busy on bank1
#define  udd_disable_busy_bank1(ep)                (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_BUSY1C_MASK)

  //! returns the number of busy banks
#define  udd_nb_busy_bank(ep)                      (Rd_bitfield(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_NBUSYBK_MASK))
  //! returns the number of the current bank
#define  udd_current_bank(ep)                      (Rd_bitfield(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_CURRBK_MASK))

  //! kills last bank
#define  udd_kill_last_in_bank(ep)                 (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_KILLBKS_MASK)
  //! tests if last bank killed
#define  Is_udd_last_in_bank_killed(ep)            (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_KILLBK_MASK))
  //! forces all banks full (OUT) or free (IN) interrupt
#define  udd_force_bank_interrupt(ep)              (USBC_ARRAY(uesta0set,ep) = AVR32_USBC_UESTA0SET_NBUSYBKS_MASK)
  //! unforces all banks full (OUT) or free (IN) interrupt
#define  udd_unforce_bank_interrupt(ep)            (USBC_ARRAY(uesta0set,ep) = AVR32_USBC_UESTA0SET_NBUSYBKS_MASK)
  //! enables all banks full (OUT) or free (IN) interrupt
#define  udd_enable_bank_interrupt(ep)             (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_NBUSYBKES_MASK)
  //! disables all banks full (OUT) or free (IN) interrupt
#define  udd_disable_bank_interrupt(ep)            (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_NBUSYBKEC_MASK)
  //! tests if all banks full (OUT) or free (IN) interrupt enabled
#define  Is_udd_bank_interrupt_enabled(ep)         (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_NBUSYBKE_MASK))

  //! tests if SHORT PACKET received
#define  Is_udd_short_packet(ep)                   (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_SHORTPACKETI_MASK))
  //! acks SHORT PACKET received
#define  udd_ack_short_packet(ep)                  (USBC_ARRAY(uesta0clr,ep) = AVR32_USBC_UESTA0CLR_SHORTPACKETIC_MASK)
  //! raises SHORT PACKET received
#define  udd_raise_short_packet(ep)                (USBC_ARRAY(uesta0set,ep) = AVR32_USBC_UESTA0SET_SHORTPACKETIS_MASK)
  //! enables SHORT PACKET received interrupt
#define  udd_enable_short_packet_interrupt(ep)     (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_SHORTPACKETES_MASK)
  //! disables SHORT PACKET received interrupt
#define  udd_disable_short_packet_interrupt(ep)    (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_SHORTPACKETEC_MASK)
  //! tests if SHORT PACKET received interrupt is enabled
#define  Is_udd_short_packet_interrupt_enabled(ep) (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_SHORTPACKETE_MASK))

//! @}

//! @name USBC Device control endpoint transfer
//! These macros control the endpoint transfers.
//! @{

  //! tests if SETUP received
#define  Is_udd_setup_received(ep)                    (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_RXSTPI_MASK))
  //! acks SETUP received
#define  udd_ack_setup_received(ep)                   ((USBC_ARRAY(uesta0clr,ep) = AVR32_USBC_UESTA0CLR_RXSTPIC_MASK))
  //! raises SETUP received
#define  udd_raise_setup_received(ep)                 (USBC_ARRAY(uesta0set,ep) = AVR32_USBC_UESTA0SET_RXSTPIS_MASK)
  //! enables SETUP received interrupt
#define  udd_enable_setup_received_interrupt(ep)      (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_RXSTPES_MASK)
  //! disables SETUP received interrupt
#define  udd_disable_setup_received_interrupt()       (USBC_ARRAY(uecon0clr(EP_CONTROL) = AVR32_USBC_UECON0CLR_RXSTPEC_MASK)
  //! tests if SETUP received interrupt is enabled
#define  Is_udd_setup_received_interrupt_enabled(ep)  (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_RXSTPE_MASK))

  //! tests if OUT received
#define  Is_udd_out_received(ep)                   (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_RXOUTI_MASK))
  //! acks OUT received
#define  udd_ack_out_received(ep)                  ((USBC_ARRAY(uesta0clr,ep) = AVR32_USBC_UESTA0CLR_RXOUTIC_MASK))
  //! raises OUT received
#define  udd_raise_out_received(ep)                (USBC_ARRAY(uesta0set,ep) = AVR32_USBC_UESTA0SET_RXOUTIS_MASK)
  //! enables OUT received interrupt
#define  udd_enable_out_received_interrupt(ep)     (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_RXOUTES_MASK)
  //! disables OUT received interrupt
#define  udd_disable_out_received_interrupt(ep)    (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_RXOUTEC_MASK)
  //! tests if OUT received interrupt is enabled
#define  Is_udd_out_received_interrupt_enabled(ep) (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_RXOUTE_MASK))

  //! tests if IN sending
#define  Is_udd_in_send(ep)                        (Tst_bits(USBC_ARRAY(uesta0,ep), AVR32_USBC_UESTA0_TXINI_MASK))
  //! acks IN sending
//#define  udd_ack_in_send(ep)                       ((USBC_ARRAY(uesta0clr,ep) = AVR32_USBC_UESTA0CLR_TXINIC_MASK), Usb_set_desc_tab_ept_byte_cnt(ep, udd_current_bank(ep), 0))
#define  udd_ack_in_send(ep)                       (USBC_ARRAY(uesta0clr,ep) = AVR32_USBC_UESTA0CLR_TXINIC_MASK)
  //! raises IN sending
#define  udd_raise_in_send(ep)                     (USBC_ARRAY(uesta0set,ep) = AVR32_USBC_UESTA0SET_TXINIS_MASK)
  //! enables IN sending interrupt
#define  udd_enable_in_send_interrupt(ep)          (USBC_ARRAY(uecon0set,ep) = AVR32_USBC_UECON0SET_TXINES_MASK)
  //! disables IN sending interrupt
#define  udd_disable_in_send_interrupt(ep)         (USBC_ARRAY(uecon0clr,ep) = AVR32_USBC_UECON0CLR_TXINEC_MASK)
  //! tests if IN sending interrupt is enabled
#define  Is_udd_in_send_interrupt_enabled(ep)      (Tst_bits(USBC_ARRAY(uecon0,ep), AVR32_USBC_UECON0_TXINE_MASK))
//! @}


//! @name USB Device endpoints descriptor table management
//! @{

#define udd_udesc_set_buf0_addr(ep,buf)                \
   udd_g_ep_table[ep*2].endpoint_pipe_address = buf
#define udd_udesc_rst_buf0_size(ep)                    \
   udd_g_ep_table[ep*2].SIZES.multi_packet_size = 0
#define udd_udesc_get_buf0_size(ep)                    \
   udd_g_ep_table[ep*2].SIZES.multi_packet_size
#define udd_udesc_set_buf0_size(ep,size)               \
   udd_g_ep_table[ep*2].SIZES.multi_packet_size = size
#define udd_udesc_rst_buf0_ctn(ep)                     \
   udd_g_ep_table[ep*2].SIZES.byte_count = 0
#define udd_udesc_get_buf0_ctn(ep)                     \
   udd_g_ep_table[ep*2].SIZES.byte_count
#define udd_udesc_set_buf0_ctn(ep,size)                \
   udd_g_ep_table[ep*2].SIZES.byte_count = size
#define udd_udesc_set_buf0_autozlp(ep,val)             \
   udd_g_ep_table[ep*2].SIZES.auto_zlp = val


#define Usb_set_desc_tab_addr_reg(addr) (Wr_bitfield(AVR32_USBC.udesc, AVR32_USBC_UDESC_MASK, addr))

struct avr32_usbc_udesc_sizes_t {
	unsigned int auto_zlp:1;
	unsigned int multi_packet_size:15;
	unsigned int res:1;
	unsigned int byte_count:15;
};

struct avr32_usbc_udesc_bk_ctrl_stat_t {
	unsigned int res:13;
	unsigned int underfi:1;
	unsigned int overfi:1;
	unsigned int crc:1;
	unsigned int res2:16;
};

struct avr32_usbc_udesc_ep_ctrl_stat_t {
	unsigned int res:8;
	unsigned int pipe_error_status:8;
	unsigned int pipe_error_number_max:4;
	unsigned int pep_num:4;
	unsigned int res2:1;
	unsigned int uhaddr:7;
};

typedef struct {
	uint8_t *endpoint_pipe_address;
	union {
		unsigned long sizes;
		struct avr32_usbc_udesc_sizes_t SIZES;
	};
	union {
		unsigned long bk_ctrl_stat;
		struct avr32_usbc_udesc_bk_ctrl_stat_t BK_CTRL_STAT;
	};
	union {
		unsigned long ep_ctrl_stat;
		struct avr32_usbc_udesc_ep_ctrl_stat_t EP_CTRL_STAT;
	};
} usb_desc_table_t;

//! @}

//! @}
//! @}
//! @}

#endif // _USBC_DEVICE_H_
