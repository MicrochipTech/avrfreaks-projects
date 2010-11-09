/**
 * @file main.c
 *
 * @brief MAC Example App 1 Nobeacon - Device
 *
 * This is the source code of a simple MAC example. It implements the
 * firmware for all devices of a network with star topology.
 *
 * $Id: main.c 20068 2010-01-28 12:38:40Z sschneid $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2009, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
 // using some leds for debug.  9/20/2010   -jkl
 // 9/26/2010 - added code to app_timer_cb to read and send ADC chan 0.  -jkl

/* === INCLUDES ============================================================ */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pal.h"
#include "tal.h"
#include "mac_api.h"
#include "app_config.h"
#include "ieee_const.h"

/* === TYPES =============================================================== */

typedef struct associated_device_tag
{
    uint16_t short_addr;
    uint64_t ieee_addr;
}
/** This type definition of a structure can store the short address and the
 *  extended address of a device.
 */
associated_device_t;

/* === MACROS ============================================================== */

/** Defines the default channel. */
#if (TAL_TYPE == AT86RF212)
    #ifdef CHINESE_BAND
        #define DEFAULT_CHANNEL                 (0)
        #define DEFAULT_CHANNEL_PAGE            (5)
    #else
        #define DEFAULT_CHANNEL                 (1)
        #define DEFAULT_CHANNEL_PAGE            (0)
    #endif  /* #ifdef CHINESE_BAND */
#else
#define DEFAULT_CHANNEL                 (20)
#define DEFAULT_CHANNEL_PAGE            (0)
#endif  /* #if (TAL_TYPE == AT86RF212) */
/** Defines the PAN ID of the network. */
#define DEFAULT_PAN_ID                  (0xBABE)
/** This is a device which will communicate with a single coordinator.
 *  Therefore, the maximum number of devices this code needs to
 *  handle is one.
 */
#define MAX_NUMBER_OF_DEVICES           (1)

/** This is the time period in micro seconds for data transmissions. */
#define DATA_TX_PERIOD                  (2000000)

/** Defines the bit mask of channels that should be scanned. */
#if (TAL_TYPE == AT86RF212)
    #if (DEFAULT_CHANNEL == 0)
        #define SCAN_ALL_CHANNELS       (0x00000001)
    #else
        #define SCAN_ALL_CHANNELS       (0x000007FE)
    #endif
#else
#define SCAN_ALL_CHANNELS               (0x07FFF800)
#endif
/** Defines the short scan duration time. */
#define SCAN_DURATION_SHORT             (5)
/** Defines the long scan duration time. */
#define SCAN_DURATION_LONG              (6)

#if (NO_OF_LEDS >= 3)
#define LED_START                       (LED_0)
#define LED_NWK_SETUP                   (LED_1)
#define LED_DATA                        (LED_2)
#elif (NO_OF_LEDS == 2)
#define LED_START                       (LED_0)
#define LED_NWK_SETUP                   (LED_0)
#define LED_DATA                        (LED_1)
#else
#define LED_START                       (LED_0)
#define LED_NWK_SETUP                   (LED_0)
#define LED_DATA                        (LED_0)
#endif

/* === GLOBALS ============================================================= */

/** This structure stores the short and extended address of the coordinator. */
associated_device_t coord_addr;
/** This variable stores the short address of this device. */
uint16_t own_short_addr;

/* === PROTOTYPES ========================================================== */
// comment out for debug
static void app_timer_cb(void *parameter);
static void network_search_indication_cb(void *parameter);
static void data_exchange_led_off_cb(void *parameter);

/* === IMPLEMENTATION ====================================================== */


/**
 * @brief Main function of the device application
 */
int main(void)
{
    /* Initialize the MAC layer and its underlying layers, like PAL, TAL, BMM. */
    if (wpan_init() != MAC_SUCCESS)
    {
        /*
         * Stay here; we need a valid IEEE address.
         * Check kit documentation how to create an IEEE address
         * and to store it into the EEPROM.
         */
        pal_alert();
    }
    /* Initialize LEDs. */
    pal_led_init();
    pal_led(LED_START, LED_ON);         // indicating application is started
    pal_led(LED_NWK_SETUP, LED_OFF);    // indicating node is associated
    pal_led(LED_DATA, LED_OFF);         // indicating successfull data transmission
    /*
     * The stack is initialized above, hence the global interrupts are enabled
     * here.
     */
    pal_global_irq_enable();
	
/*
		pal_led(LED_3, LED_ON);					// for debug
        // Start a timer switching off the LED
        pal_timer_start(TIMER_LED_OFF,
                        500000,
                        TIMEOUT_RELATIVE,
                        (FUNC_PTR)data_exchange_led_off_cb,
                        NULL);						// end debug
*/

    /*
     * Reset the MAC layer to the default values.
     * This request will cause a mlme reset confirm message ->
     * usr_mlme_reset_conf
     */
    wpan_mlme_reset_req(true);

    /* Main loop */
    while (1)
    {
        wpan_task();
    }
}



/**
 * @brief Callback function usr_mlme_reset_conf
 *
 * @param status Result of the reset procedure
 */
void usr_mlme_reset_conf(uint8_t status)
{
    if (status == MAC_SUCCESS)
    {
        /*
         * Initiate an active scan over all channels to determine
         * which channel is used by the coordinator.
         * Use: bool wpan_mlme_scan_req(uint8_t ScanType,
         *                              uint32_t ScanChannels,
         *                              uint8_t ScanDuration,
         *                              uint8_t ChannelPage);
         *
         * This request leads to a scan confirm message -> usr_mlme_scan_conf
         * Scan for about 50 ms on each channel -> ScanDuration = 1
         * Scan for about 1/2 second on each channel -> ScanDuration = 5
         * Scan for about 1 second on each channel -> ScanDuration = 6
         */
        wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
                           SCAN_ALL_CHANNELS,
                           SCAN_DURATION_SHORT,
                           DEFAULT_CHANNEL_PAGE);

        /* Indicate network scanning by a LED flashing. */
        pal_timer_start(TIMER_LED_OFF,
                        500000,
                        TIMEOUT_RELATIVE,
                        (FUNC_PTR)network_search_indication_cb,
                        NULL);
    }
    else
    {
        /* Something went wrong; restart. */
        wpan_mlme_reset_req(true);
    }
}



/**
 * @brief Callback function usr_mlme_scan_conf
 *
 * @param status            Result of requested scan operation
 * @param ScanType          Type of scan performed
 * @param ChannelPage       Channel page on which the scan was performed
 * @param UnscannedChannels Bitmap of unscanned channels
 * @param ResultListSize    Number of elements in ResultList
 * @param ResultList        Pointer to array of scan results
 */
void usr_mlme_scan_conf(uint8_t status,
                        uint8_t ScanType,
                        uint8_t ChannelPage,
                        uint32_t UnscannedChannels,
                        uint8_t ResultListSize,
                        void *ResultList)
{
    if (status == MAC_SUCCESS)
    {
        wpan_pandescriptor_t *coordinator;
        uint8_t i;
		//pal_led(LED_0, LED_OFF);			// just for debug
		//pal_led(LED_1, LED_OFF);			// just for debug
		//pal_led(LED_2, LED_ON);			// just for debug - do we ever get here? Yes we do. 9/20/2010
		//while(1);
        /*
         * Analyze the ResultList.
         * Assume that the first entry of the result list is our coodinator.
         */
        coordinator = (wpan_pandescriptor_t *)ResultList;

        for (i = 0; i < ResultListSize; i++)
        {
            /*
             * Check if the PAN descriptor belongs to our coordinator.
             * Check if coordinator allows association.
             */
            if ((coordinator->LogicalChannel == DEFAULT_CHANNEL) &&
                (coordinator->ChannelPage == DEFAULT_CHANNEL_PAGE) &&
                (coordinator->CoordAddrSpec.PANId == DEFAULT_PAN_ID) &&
                ((coordinator->SuperframeSpec & ((uint16_t)1 << ASSOC_PERMIT_BIT_POS)) == ((uint16_t)1 << ASSOC_PERMIT_BIT_POS))
               )
            {
				//pal_led(LED_3, LED_ON);			// just for debug - do we ever get here? Yes we do. 9/20/2010
				//while(1);
                /* Store the coordinator's address. */
                if (coordinator->CoordAddrSpec.AddrMode == WPAN_ADDRMODE_SHORT)
                {
                    ADDR_COPY_DST_SRC_16(coord_addr.short_addr, coordinator->CoordAddrSpec.Addr.short_address);
				//	pal_led(LED_1, LED_ON);
                }
                else if (coordinator->CoordAddrSpec.AddrMode == WPAN_ADDRMODE_LONG)
                {
                    ADDR_COPY_DST_SRC_64(coord_addr.ieee_addr, coordinator->CoordAddrSpec.Addr.long_address);
				//	pal_led(LED_0, LED_ON);
                }
                else
                {
                    /* Something went wrong; restart. */
                    wpan_mlme_reset_req(true);
                    return;
                }
				//while(1);					// debug
                /*
                 * Associate to our coordinator.
                 * Use: bool wpan_mlme_associate_req(uint8_t LogicalChannel,
                 *                                   uint8_t ChannelPage,
                 *                                   wpan_addr_spec_t *CoordAddrSpec,
                 *                                   uint8_t CapabilityInformation);
                 * This request will cause a mlme associate confirm message ->
                 * usr_mlme_associate_conf.
                 */
                wpan_mlme_associate_req(coordinator->LogicalChannel,
                                        coordinator->ChannelPage,
                                        &(coordinator->CoordAddrSpec),
                                        WPAN_CAP_ALLOCADDRESS);
                return;
            }

            /* Get the next PAN descriptor. */
            coordinator++;
        }
		//while(1);		// debug hang
        /*
         * If here, the result list does not contain our expected coordinator.
         * Let's scan again.
         */
        wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
                           SCAN_ALL_CHANNELS,
                           SCAN_DURATION_SHORT,
                           DEFAULT_CHANNEL_PAGE);
    }
    else if (status == MAC_NO_BEACON)
    {
        /*
         * No beacon is received; no coordiantor is located.
         * Scan again, but used longer scan duration.
         */
        wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
                           SCAN_ALL_CHANNELS,
                           SCAN_DURATION_LONG,
                           DEFAULT_CHANNEL_PAGE);
    }
    else
    {
        /* Something went wrong; restart. */
        wpan_mlme_reset_req(true);
    }

    /* Keep compiler happy. */
    ScanType = ScanType;
    ChannelPage = ChannelPage;
    UnscannedChannels = UnscannedChannels;
}



/**
 * @brief Callback function usr_mlme_associate_conf
 *
 * @param AssocShortAddress    Short address allocated by the coordinator
 * @param status               Result of requested association operation
 */
void usr_mlme_associate_conf(uint16_t AssocShortAddress, uint8_t status)
{
		//pal_led(LED_2, LED_ON);			// just for debug - do we ever get here? 
		//while(1);
    if (status == MAC_SUCCESS)		
	//if (1==1)					// just playing for debug
    {
		//pal_led(LED_0, LED_ON);
        /* Store own short address. */
        own_short_addr = AssocShortAddress;

        /* Stop timer used for search indication (same as used for data transmission). */
		pal_timer_stop(TIMER_LED_OFF);		
        pal_led(LED_NWK_SETUP, LED_ON);

        // Start a timer that sends some data to the coordinator every 2 seconds.
		
        pal_timer_start(TIMER_TX_DATA,		
                        DATA_TX_PERIOD,
                        TIMEOUT_RELATIVE,
                        (FUNC_PTR)app_timer_cb,
                        NULL);
					
    }
    else
    {
		//pal_led(LED_3, LED_ON);
        /* Something went wrong; restart. */
        wpan_mlme_reset_req(true);
    }
	//while(1);
}



/**
 * @brief Callback function for the application timer
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
 //#if 0
 // done just for debug
static void app_timer_cb(void *parameter)
{
    /*
     * Send some data and restart timer.
     * Use: bool wpan_mcps_data_req(uint8_t SrcAddrMode,
     *                              wpan_addr_spec_t *DstAddrSpec,
     *                              uint8_t msduLength,
     *                              uint8_t *msdu,
     *                              uint8_t msduHandle,
     *                              uint8_t TxOptions);
     *
     * This request will cause a mcps data confirm message ->
     * usr_mcps_data_conf
     */

    uint8_t src_addr_mode;
    wpan_addr_spec_t dst_addr;
    uint8_t payload;
    static uint8_t msduHandle = 0;

    src_addr_mode = WPAN_ADDRMODE_SHORT;

    dst_addr.AddrMode = WPAN_ADDRMODE_SHORT;
    dst_addr.PANId = DEFAULT_PAN_ID;
    ADDR_COPY_DST_SRC_16(dst_addr.Addr.short_address, coord_addr.short_addr);

    //payload = (uint8_t)rand();  // any dummy data
	
	// let's digitize a single channel and send the value
	// for now, this is channel 0
	
	// gather the results of the last reading
	// If ADSC is 0, then conversion is done.
	if ( (ADCSRA & _BV(ADSC)) == 0) {
		payload = ADCH;
	}
	else {
		//send back a value of 0
		payload = 0;
	}
	
	// let's start another reading
	//local_temp = (ADMUX & 0xF0) | (chan# & 0x0F); // could allow other channels
	//ADMUX = local_temp;
	ADCSRA |= _BV(ADSC);		// start the new conversion
	
	// for debug, always send the same value
	//payload = 0x55;		// working!
	
    msduHandle++;               // increment handle
    wpan_mcps_data_req(src_addr_mode,
                       &dst_addr,
                       1,
                       &payload,
                       msduHandle,
                       WPAN_TXOPT_ACK);

    pal_timer_start(TIMER_TX_DATA,
                    DATA_TX_PERIOD,
                    TIMEOUT_RELATIVE,
                    (FUNC_PTR)app_timer_cb,
                    NULL);

    parameter = parameter;  /* Keep compiler happy. */
}
//#endif


/**
 * Callback function usr_mcps_data_conf
 *
 * @param msduHandle  Handle of MSDU handed over to MAC earlier
 * @param status      Result for requested data transmission request
 * @param Timestamp   The time, in symbols, at which the data were transmitted
 *
 */
void usr_mcps_data_conf(uint8_t msduHandle, uint8_t status, uint32_t Timestamp)
{
    if (status == MAC_SUCCESS)
    {
        /*
         * Dummy data has been transmitted successfully.
         * Application code could be added here ...
         */
        pal_led(LED_DATA, LED_ON);
        // Start a timer switching off the LED
        pal_timer_start(TIMER_LED_OFF,
                        500000,
                        TIMEOUT_RELATIVE,
                        (FUNC_PTR)data_exchange_led_off_cb,
                        NULL);
    }

    /* Keep compiler happy. */
    msduHandle = msduHandle;
    Timestamp = Timestamp;
}



/**
 * @brief Callback function indicating network search
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void network_search_indication_cb(void *parameter)
{
    pal_led(LED_NWK_SETUP, LED_TOGGLE);

    // Re-start led timer again
    pal_timer_start(TIMER_LED_OFF,
                    500000,
                    TIMEOUT_RELATIVE,
                    (FUNC_PTR)network_search_indication_cb,
                    NULL);

    parameter = parameter;    /* Keep compiler happy. */
}



/**
 * @brief Callback function switching off the LED
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void data_exchange_led_off_cb(void *parameter)
{
    pal_led(LED_DATA, LED_OFF);
	//pal_led(LED_3, LED_OFF);				// just for debug

    parameter = parameter;    /* Keep compiler happy. */
}

/* EOF */
