/**
 * @file main.c
 *
 * @brief MAC Example App 1 Nobeacon - Coordinator
 *
 * This is the source code of a simple MAC example. It implements the
 * firmware for the coordinator of a network with star topology.
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
 // 9/20/2010 - Put in some code to track execution.   -jkl
 // 9/26/2010 - Modified usr_mcps_data_ind to show time stamp and A/D input.    -jkl

/* === INCLUDES ============================================================ */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
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
/** Defines the short address of the coordinator. */
#define COORD_SHORT_ADDR                (0x0000)
/** Defines the maximum number of devices this coordinator will handle. */
#define MAX_NUMBER_OF_DEVICES           (2)
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

/** Defines the scan duration time. */
#define SCAN_DURATION_COORDINATOR       (1)

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

/** This array stores all device related information. */
associated_device_t device_list[MAX_NUMBER_OF_DEVICES];

/* === PROTOTYPES ========================================================== */

static void led_off_cb(void *parameter);
static bool assign_new_short_addr(uint64_t addr64, uint16_t *addr16);

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Main function of the coordinator application
 *
 * This function initializes the MAC, initiates a MLME reset request
 * (@ref wpan_mlme_reset_req()), and implements a the main loop.
 */
int main(void)
{
	uint8_t buf[] = "\nwaiting..\n";
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
    pal_led(LED_NWK_SETUP, LED_OFF);    // indicating network is started
    pal_led(LED_DATA, LED_OFF);         // indicating data reception
	
	pal_sio_tx(SIO_1, buf, strlen((char *)buf));
    /*
     * The stack is initialized above, hence the global interrupts are enabled
     * here.
     */
    pal_global_irq_enable();

    /*
     * Reset the MAC layer to the default values.
     * This request will cause a mlme reset confirm message ->
     * usr_mlme_reset_conf
     */
    wpan_mlme_reset_req(true);
	
	  //pal_led(LED_3, LED_ON);	// debug

     /* Start a timer switching off the LED. */   // debug
   // pal_timer_start(TIMER_LED_OFF,
   //                 500000,
   //                 TIMEOUT_RELATIVE,
   //                 (FUNC_PTR)led_off_cb,
   //                 NULL);						// end debug

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
         * Set the short address of this node.
         * Use: bool wpan_mlme_set_req(uint8_t PIBAttribute,
         *                             void *PIBAttributeValue);
         *
         * This request leads to a set confirm message -> usr_mlme_set_conf
         */
        uint8_t short_addr[2];

        short_addr[0] = (uint8_t)COORD_SHORT_ADDR;          // low byte
        short_addr[1] = (uint8_t)(COORD_SHORT_ADDR >> 8);   // high byte
        wpan_mlme_set_req(macShortAddress, short_addr);
    }
    else
    {
        /* Something went wrong; restart. */
        wpan_mlme_reset_req(true);
    }
}



/**
 * @brief Callback function usr_mlme_set_conf
 *
 * @param status        Result of requested PIB attribute set operation
 * @param PIBAttribute  Updated PIB attribute
 */
void usr_mlme_set_conf(uint8_t status, uint8_t PIBAttribute)
{
    if ((status == MAC_SUCCESS) && (PIBAttribute == macShortAddress))
    {
        /*
         * Allow other devices to associate to this coordinator.
         * Use: bool wpan_mlme_set_req(uint8_t PIBAttribute,
         *                             void *PIBAttributeValue);
         *
         * This request leads to a set confirm message -> usr_mlme_set_conf
         */
         uint8_t association_permit = true;

         wpan_mlme_set_req(macAssociationPermit, &association_permit);
    }
    else if ((status == MAC_SUCCESS) && (PIBAttribute == macAssociationPermit))
    {
        /*
         * Set RX on when idle to enable the receiver as default.
         * Use: bool wpan_mlme_set_req(uint8_t PIBAttribute,
         *                             void *PIBAttributeValue);
         *
         * This request leads to a set confirm message -> usr_mlme_set_conf
         */
         bool rx_on_when_idle = true;

         wpan_mlme_set_req(macRxOnWhenIdle, &rx_on_when_idle);
    }
    else if ((status == MAC_SUCCESS) && (PIBAttribute == macRxOnWhenIdle))
    {
        /*
         * Initiate an active scan over all channels to determine
         * which channel to use.
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
                           SCAN_DURATION_COORDINATOR,
                           DEFAULT_CHANNEL_PAGE);
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
    /*
     * We are not interested in the actual scan result,
     * because we start our network on the pre-defined channel anyway.
     * Start a nonbeacon-enabled network
     * Use: bool wpan_mlme_start_req(uint16_t PANId,
     *                               uint8_t LogicalChannel,
     *                               uint8_t ChannelPage,
     *                               uint8_t BeaconOrder,
     *                               uint8_t SuperframeOrder,
     *                               bool PANCoordinator,
     *                               bool BatteryLifeExtension,
     *                               bool CoordRealignment)
     *
     * This request leads to a start confirm message -> usr_mlme_start_conf
     */
     wpan_mlme_start_req(DEFAULT_PAN_ID,
                         DEFAULT_CHANNEL,
                         DEFAULT_CHANNEL_PAGE,
                         15, 15,
                         true, false, false);

    /* Keep compiler happy. */
    status = status;
    ScanType = ScanType;
    ChannelPage = ChannelPage;
    UnscannedChannels = UnscannedChannels;
    ResultListSize = ResultListSize;
    ResultList = ResultList;
}



/**
 * @brief Callback function usr_mlme_start_conf
 *
 * @param status        Result of requested start operation
 */
void usr_mlme_start_conf(uint8_t status)
{
    if (status == MAC_SUCCESS)
    {
        /*
         * Network is established.
         * Waiting for association indication from a device.
         * -> usr_mlme_associate_ind
         */
        pal_led(LED_NWK_SETUP, LED_ON);
    }
    else
    {
		//pal_led(LED_3, LED_ON);		// just for debug!
        /* Something went wrong; restart. */
        wpan_mlme_reset_req(true);
    }
}



/**
 * @brief Callback function usr_mlme_associate_ind
 *
 * @param DeviceAddress         Extended address of device requesting association
 * @param CapabilityInformation Capabilities of device requesting association
 */
void usr_mlme_associate_ind(uint64_t DeviceAddress,
                            uint8_t CapabilityInformation)
{
    /*
     * Any device is allowed to join the network.
     * Use: bool wpan_mlme_associate_resp(uint64_t DeviceAddress,
     *                                    uint16_t AssocShortAddress,
     *                                    uint8_t status);
     *
     * This response leads to comm status indication -> usr_mlme_comm_status_ind
     * Get the next available short address for this device.
     */
    uint16_t associate_short_addr = macShortAddress_def;

    if (assign_new_short_addr(DeviceAddress, &associate_short_addr) == true)
    {
	//pal_led(LED_2, LED_ON);		// just for debug!    --- OK came here
        wpan_mlme_associate_resp(DeviceAddress,
                                 associate_short_addr,
                                 ASSOCIATION_SUCCESSFUL);
    }
    else
    {
	//pal_led(LED_3, LED_ON);		// just for debug!     -- not here
        wpan_mlme_associate_resp(DeviceAddress, associate_short_addr,
                                         PAN_AT_CAPACITY);
    }
	//while(1);	  // for debug

    /* Keep compiler happy. */
    CapabilityInformation = CapabilityInformation;
}



/**
 * @brief Callback function usr_mlme_comm_status_ind
 *
 * @param SrcAddrSpec      Pointer to source address specification
 * @param DstAddrSpec      Pointer to destination address specification
 * @param status           Result for related response operation
 */
void usr_mlme_comm_status_ind(wpan_addr_spec_t *SrcAddrSpec,
                              wpan_addr_spec_t *DstAddrSpec,
                              uint8_t status)
{
    if (status == MAC_SUCCESS)
    {
	//pal_led(LED_3, LED_ON);		// just for debug! -- gets here just fine
	//while(1);
        /*
         * Now the association of the device has been successful and its
         * information, like address, could  be stored.
         * But for the sake of simple handling it has been done
         * during assignment of the short address within the function
         * assign_new_short_addr()
         */
    }

    /* Keep compiler happy. */
    SrcAddrSpec = SrcAddrSpec;
    DstAddrSpec = DstAddrSpec;
}



/**
 * @brief Callback function usr_mcps_data_ind
 *
 * @param SrcAddrSpec      Pointer to source address specification
 * @param DstAddrSpec      Pointer to destination address specification
 * @param msduLength       Number of octets contained in MSDU
 * @param msdu             Pointer to MSDU
 * @param mpduLinkQuality  LQI measured during reception of the MPDU
 * @param DSN              DSN of the received data frame.
 * @param Timestamp        The time, in symbols, at which the data were received.
 */
void usr_mcps_data_ind(wpan_addr_spec_t *SrcAddrSpec,
                       wpan_addr_spec_t *DstAddrSpec,
                       uint8_t msduLength,
                       uint8_t *msdu,
                       uint8_t mpduLinkQuality,
                       uint8_t DSN,
                       uint32_t Timestamp)
{
    uint8_t buf[20];		// this is for the output
	//uint8_t blen = msduLength;  maybe later
	/*
     * Dummy data has been received successfully.
     * Application code could be added here ...
     */
    pal_led(LED_DATA, LED_ON);
	
	// dig the data out of the packet
		
	
	// display it along with the timestamp
	sprintf((char *)buf, "%x %lx\n\r", msdu[0],Timestamp);
	//sprintf((char *)buf, "time: %lx\n\r", Timestamp);
	pal_sio_tx(SIO_1, buf, strlen((char *)buf));

     /* Start a timer switching off the LED. */
    pal_timer_start(TIMER_LED_OFF,
                    500000,
                    TIMEOUT_RELATIVE,
                    (FUNC_PTR)led_off_cb,
                    NULL);

    /* Keep compiler happy. */
    SrcAddrSpec = SrcAddrSpec;
    DstAddrSpec = DstAddrSpec;
    msduLength = msduLength;
    msdu = msdu;
    mpduLinkQuality = mpduLinkQuality;
    DSN = DSN;
    Timestamp = Timestamp;
}



/**
 * @brief Callback function switching off the LED
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be used
 *                  to indicated LED to be switched off)
 */
static void led_off_cb(void *parameter)
{
    pal_led(LED_DATA, LED_OFF);
	//pal_led(LED_3, LED_OFF);			// for debug

    parameter = parameter;  /* Keep compiler happy. */
}



/**
 * @brief Application specific function to assign a short address
 */
static bool assign_new_short_addr(uint64_t addr64, uint16_t *addr16)
{
    uint8_t i;

    /* Check if device has been associated before. */
    for (i = 0; i < MAX_NUMBER_OF_DEVICES; i++)
    {
        if (device_list[i].short_addr == 0x0000)
        {
            /* If the short address is 0x0000, it has not been used before. */
            continue;
        }
        if (device_list[i].ieee_addr == addr64)
        {
            /* Assign the previously assigned short address again. */
            *addr16 = device_list[i].short_addr;
            return true;
        }
    }

    for (i = 0; i < MAX_NUMBER_OF_DEVICES; i++)
    {
        if (device_list[i].short_addr == 0x0000)
        {
            *addr16 = i + 0x0001;
            device_list[i].short_addr = i + 0x0001; /* Get next short address. */
            device_list[i].ieee_addr = addr64;      /* Store extended address. */
            return true;
        }
    }

    /* If we are here, no short address could be assigned. */
    return false;
}

/* EOF */
