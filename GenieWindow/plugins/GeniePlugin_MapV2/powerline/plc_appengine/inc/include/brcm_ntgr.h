/*
Copyright 2008-2011 Broadcom Corporation

This program is�the proprietary software of Broadcom Corporation and/or its
licensors, and may only be used, duplicated, modified�or distributed pursuant to
the terms and conditions of a separate, written license agreement executed
between you and Broadcom (an "Authorized License").� Except as set forth in an
Authorized License, Broadcom grants no license (express or implied), right to
use, or waiver of any kind with respect to the Software, and Broadcom expressly
reserves all rights in and to the Software and all intellectual property rights
therein.� IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL
USE OF THE SOFTWARE.�
�
Except as expressly set forth in the Authorized License,
�
1. This program, including its structure, sequence and organization, constitutes
   the valuable trade secrets of Broadcom, and you shall use all reasonable
   efforts to protect the confidentiality thereof, and to use this information
   only in connection with your use of Broadcom integrated circuit products.
�
2.�TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE�IS PROVIDED "AS IS" AND
   WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
   WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
   THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS�ANY AND ALL IMPLIED WARRANTIES
   OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
   LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
   OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
   USE OR PERFORMANCE OF THE SOFTWARE.

3.�TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
   LICENSORS BE LIABLE FOR�(i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
   EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
   USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
   THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
   ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
   LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
   ANY LIMITED REMEDY.
*/

#ifndef BRCM_NTGR_H_
#define BRCM_NTGR_H_

/* API Version 1.4 - 13686*/

#ifdef __cplusplus
extern "C" {
#endif


#define	PLC_MAC_ADDRESS_SIZE	      6 // the number of bytes of MAC address
#define	PLC_MAX_DEVICES	            16 // Max number of devices allowed
#define	PLC_QOS_BLOCK_MAC_SIZE	    PLC_MAC_ADDRESS_SIZE
#define	PLC_QOS_PORTLIST_MAX_ITEM	  8 // Max entries of port list for QoS
#define	PLC_QOS_MACLIST_MAX_ITEM	  8 // Max entries of MAC address list for QoS

#define	PLC_MAX_FORWARD_TABLE_SIZE	1024 // Max entries of forwarding table

/* device location */
#define	PLC_LOCAL_DEVICE	0x0
#define	PLC_REMOTE_DEVICE	0x1

/* device network type */
#define	PLC_PRIVATE	"Private"
#define	PLC_PUBLIC	"Public"

/* status */
#define	DISABLE	0x0
#define	ENABLE	0x1

/* LED status */
#define	PLC_LED_OFF	0x0
#define	PLC_LED_ON	0x1

#define	PLC_GET_FAIL    	                0
#define	PLC_GET_SUCCESS	                  1

#define	PLC_SET_FAIL    	                0
#define	PLC_SET_SUCCESS 	                1
#define	PLC_SET_DEVICE_PASSWORD_KEY_ERROR	2

/* QoS Priority*/
#define	PLC_QOS_PRIORITY_LOW						0x00
#define	PLC_QOS_PRIORITY_NORMAL					0x01
#define	PLC_QOS_PRIORITY_HIGH						0x02
#define	PLC_QOS_PRIORITY_HIGHEST				0x03

typedef	unsigned char		PLC_MACAddress[PLC_MAC_ADDRESS_SIZE];
typedef	int				      BOOL;
typedef	unsigned char		BYTE;

// the list of devices discovered
typedef struct PLC_DiscoveryDeviceList_Str
{
	BYTE	PLC_DeviceMACAddr[PLC_MAX_DEVICES][PLC_MAC_ADDRESS_SIZE];
} PLC_DiscoveryDeviceList;

typedef struct PLC_DeviceQoSMACAddr_Str
{
	BYTE	PLC_QOS_MACAddr[PLC_QOS_BLOCK_MAC_SIZE];
	BYTE	PLC_QOS_MACAddr_Priority; 	// The MAC address priority,
                                    // PLC_QOS_PRIORITY_LOW,
                                    // PLC_QOS_PRIORITY_NORMAL,
                                    // PLC_QOS_PRIORITY_HIGH or
                                    // PLC_QOS_PRIORITY_ HIGHEST.
} PLC_DeviceQoSMACAddr;

typedef struct PLC_DeviceQoSPortList_Str
{
	int		PLC_PortNumber;
	BYTE	PLC_PortNumber_Priority; 	// The port number priority,
                                  // PLC_QOS_PRIORITY_LOW,
                                  // PLC_QOS_PRIORITY_NORMAL,
                                  // PLC_QOS_PRIORITY_HIGH or
                                  // PLC_QOS_PRIORITY_ HIGHEST.
} PLC_DeviceQoSPortList;

typedef struct PLC_DeviceQoSStatus_Str
{
	BYTE	PLC_QoSMACListItem;	// How many MAC address entries in the table.

	PLC_DeviceQoSMACAddr PLC_MACAddrList[PLC_QOS_MACLIST_MAX_ITEM];
									// MAC address entries.

	BYTE	PLC_QoSPortListItem;		// the number of entries of port list

	PLC_DeviceQoSPortList PLC_PortList[PLC_QOS_PORTLIST_MAX_ITEM];
									// port list

	BYTE	PLC_VLANStatus;			// VLAN tag status: ENABLE or DISABLE.

	BYTE	PLC_TOSStatus;			// TOS status: ENABLE or DISABLE.

} PLC_DeviceQoSStatus;

// forwarding table
typedef struct PLC_DeviceForwardTableList_Str
{
	BYTE	PLC_DeviceForwardMACAddr[PLC_MAX_FORWARD_TABLE_SIZE][PLC_MAC_ADDRESS_SIZE];
} PLC_DeviceForwardTableList;

// link rate table
typedef struct PLC_DeviceLinkRateTable_Str
{
	BYTE	PLC_DeviceMACAddr[PLC_MAX_DEVICES][PLC_MAC_ADDRESS_SIZE];
	BYTE		PLC_DeviceTXRate[PLC_MAX_DEVICES];
	BYTE		PLC_DeviceRXRate[PLC_MAX_DEVICES];
} PLC_DeviceLinkRateTable;

/** To find all devices connected in the network.
 *
 * @param DeviceNumber        The number of PLC devices connected in the network.
 * @param DeviceMACAddrList   The list of the MAC address of PLC devices connected
 *                            in the network.
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCDiscovery(int *DeviceNumber, PLC_DiscoveryDeviceList *DeviceMACAddrList);


/** To get the MAC address of the local device.
 *
 * @param DeviceMACAddress  MAC address of the local PLC device.
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCGetLocalDevice(PLC_MACAddress DeviceMACAddress);


/** To get the model name of the device.
 *
 * @param DeviceMACAddress  MAC address of the PLC device.
 * @param DeviceModelName   The device model name represented as a char string
 *                          (Max. chars= 17, including NULL)
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCGetDeviceModelName(PLC_MACAddress DeviceMACAddress, char *DeviceModelName);


/** To get the version of the firmware running in the device.
 *
 * @param DeviceMACAddress  MAC address of the PLC device.
 * @param DeviceFWVersion   Firmware version represented as a char string
 *                          (Max. chars = 25, including NULL)
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCGetDeviceFWVersion(PLC_MACAddress DeviceMACAddress, char *DeviceFWVersion);


/** To get device name.
 *
 * @param DeviceMACAddress  MAC address of the PLC device.
 * @param DeviceName        The device name represented as a char string
 *                          (Max. chars = 13, including NULL)
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCGetDeviceName(PLC_MACAddress DeviceMACAddress, char *DeviceName);


/** To get the devices network type.
 *
 * @param DeviceMACAddress    MAC address of the PLC device.
 * @param DeviceNetworkType   The device network type.
 *                            (PLC_PRIVATE or PLC_PUBLIC)
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCGetNetworkType(PLC_MACAddress DeviceMACAddress, char *DeviceNetworkType);


/** To get the device LED status.
 *
 * @param DeviceMACAddress  MAC address of the PLC device.
 * @param DeviceLedStatus   LED status.
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCGetDeviceLedStatus(PLC_MACAddress DeviceMACAddress, int *DeviceLedStatus);


/** To get the device link rate.
 *
 * @param DeviceMACAddress        MAC address of the PLC device.
 * @param NumOfTable			        The number of entries in link rate table.
 * @param DeviceLinkRateTable			The device link rate table.
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCGetDeviceLinkRate(
  PLC_MACAddress DeviceMACAddress,
  int *NumOfTable,
  PLC_DeviceLinkRateTable *DeviceLinkRateTable);


/** To get the status of QoS set in the device.
 *
 * @param DeviceMACAddress  MAC address of the PLC device.
 * @param QoSStatus         QoS status.
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCGetDeviceQoSStatus(PLC_MACAddress DeviceMACAddress, PLC_DeviceQoSStatus *QoSStatus);


/** To get the device forwarding table.
 *
 * @param DevMACAddr    The MAC address of the PLC device.
 * @param NumOfEntries  The number of entries in forward table
 * @param ForwardTable  The device forward table.
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCGetDeviceForwardtable(
  PLC_MACAddress DevMACAddr,
  int *NumOfEntries,
  PLC_DeviceForwardTableList *ForwardTable);

/** To get the version of the firmware running in the device for firmware
 *  upgrade control. (ex 0.2.0.3)
 *
 * @param DeviceMACAddress        The MAC address of the PLC device.
 * @param DeviceFWUpgradeVersion  Firmware version represented as a char string.
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCGetDeviceFWUpgradeVersion(PLC_MACAddress DeviceMACAddress,
                                  char* DeviceFWUpgradeVersion);

/** To get the version of the firmware running in the device for firmware
 *  upgrade control.
 *
 * @param DeviceMACAddress        The MAC address of the PLC device.
 * @param DeviceFWUpgradeVersion  Firmware version represented as a char string.
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCGetDeviceFWUpgradeRegion(PLC_MACAddress DeviceMACAddress,
                                  char* DeviceFWUpgradeRegion);

/** To set the devices name.
 *
 * @param DeviceMACAddress    The MAC address of the PLC device.
 * @param DeviceName          The device name.
 *                            (Max. chars = 12, including a NULL)
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCSetDeviceName(PLC_MACAddress DeviceMACAddress, char DeviceName[]);


/** To set the devices led status.
 *
 * @param DeviceMACAddress  MAC address of the PLC device.
 * @param DeviceLedStatus   LED status.
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCSetDeviceLedStatus(PLC_MACAddress DeviceMACAddress, int DeviceLedStatus);


/** To set the device security key.
 *
 * @param DeviceMACAddress  MAC address of the PLC device.
 * @param SecurityKey			  The device security key represented as a char string
 *                           (Max chars = 64, min chars = 8, including a NULL)
 * @param DevicePassword		The device password represented as a char string
 *                           (Max chars = 20, including a NULL)
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
int	PLCSetDeviceSecurityKey(
  PLC_MACAddress DeviceMACAddress,
  char SecurityKey[],
  char DevicePassword[]);


/** To set the QoS.
 *
 * @param DeviceMACAddress  MAC address of the PLC device.
 * @param QoSStatus			    The Setting of QoS structure.
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 * (Note: it takes about 6 secs to set QoS settings in the PLC device)
 */
BOOL PLCSetDeviceQoS(PLC_MACAddress DeviceMACAddress, PLC_DeviceQoSStatus* QoSStatus);


/** Reset the device configuration to factory default.
 *
 * @param DeviceMACAddress  MAC address of the PLC device.
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCSetDeviceResetDefault(PLC_MACAddress DeviceMACAddress);

/** Help to check if user input correct password (DAK) for the device.
 *  Utility can check if user input correct DAK value individual before
 *  official do encryption key change.
 *
 * @param DeviceMACAddress  MAC address of the PLC device.
 * @param DevicePassword    The device password represented as a char string.
 *
 *
 * @return BOOL PLC_GET_SUCCESS if successful otherwise returns PLC_GET_FAIL.
 */
BOOL PLCCheckDevicePassword(PLC_MACAddress DeviceMACAddress, char DevicePassword[]);

#ifdef __cplusplus
}
#endif

#endif // BRCM_NTGR_H_
