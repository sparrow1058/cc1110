/******************************************************************************
*                                                                             *
*        **********                                                           *
*       ************                                                          *
*      ***        ***                                                         *
*     ***    ++    ***                                                        *
*     ***   +  +   ***                      CHIPCON                           *
*     ***   +                                                                 *
*     ***   +  +   ***                                                        *
*     ***    ++    ***                                                        *
*      ***        ***                                                         *
*       ************                                                          *
*        **********                                                           *
*                                                                             *
*******************************************************************************

Filename:     cul.h
Target:       cc2510
Author:       EFU/ KJA
Revised:      20/6-2006
Revision:     1.0

Description:
Chipcon Utility Library

Provides utility functions for DMA administration, timer administration, SPP
handling, etc.

******************************************************************************/
#ifndef CUL_H
#define CUL_H


#include "hal.h"


typedef void FUNCTION(void);

/******************************************************************************
*******************     DMA administration functions         ******************
******************************************************************************/

// The number of DMA channels available for the DMA administrator
#define DMA_ADM_NUMBER_OF_CHANNELS      5

// Structure for the DMA administration table
typedef struct {
    BYTE        assigned;
    FUNCTION*   callBackFunction;
} DMA_TABLE_ENTRY;



/******************************************************************************
* @fn  culDmaInit
*
* @brief
*      This function resets the table for assigning DMA channels 1 to 4, the
*      table consisting of the DMA descriptors 1 to 4 and clears all interrupt
*      flags and disarms all DMA channels 1 to 4. The configuration for
*      channel 0 is left unchanged. The address of the DMA descriptors is set
*      in register DMA1CFG.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void culDmaInit(void);

/******************************************************************************
* @fn  dma_IRQ
*
* @brief
*     This interrupt routine is run each time the DMA generates an interrupt.
*     According to the interrupt flag of the DMA channel, the corresponding
*     callBackFunction is called. The interrupt flags are cleared.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
__interrupt void dma_IRQ (void);

/******************************************************************************
* @fn  culDmaAlloc
*
* @brief
*      This function administrates the allocation of the DMA channels 1 to 4.
*      This function, along with culDmaFreeChannel(...), keeps track of the
*      availability of the DMA channels. When this function is called, the
*      table _dmaTable_ is searched for an unused channel. If an unused channel
*      is found, the channel is reserved and the number of the assigned channel
*      is handed to the calling function. A pointer to the descriptor of the
*      assigned DMA channel is returned. At the time of editing the table, all
*      interrupts are disabled in order to avoid that the table is edited by an
*      interrupt routine before the table is updated. If no DMA channel is
*      available, the function returns 0 and the channel number pointed to by
*      _pDmaChannelNumber_ is set to 0.
*
*      This function leaves to the caller to arm the DMA channel and initiate
*      the transfer. Alternatively, the function culDmaSetUpChannel(...) could
*      be used to simplify the use of the DMA.
*
*      NOTE:   If the number pointed to by _pDmaChannelNumber_ is set to 0 by
*              this function, the allocation was unsuccessful. This must not be
*              interpreted as if DMA channel 0 has been  used.
*
* Parameters:
*
* @param  UINT8*    pDmaChannelNumber
*         Pointer to where the assigned DMA channel number is to be stored. If
*         this value is set to 0 by this function, no DMA channel was assigned.
*         (NOT that DMA channel 0 was used!)
* @param  FUNCTION* callBackFunction
*         This parameter is a pointer to the function to be called from an
*         interrupt routine when the DMA transfer is finished. The function
*         must be a [void <functionName>(void)] type. The configuration of
*         the interrupt routine must be performed manually. If no function
*         is to be called, please set this parameter to NULL (0x00);
*
* @return DMA_DESC*
*         Pointer to the assigned DMA structure. The calling function must set
*         this descriptor occording to the desired DMA transfer.
*
******************************************************************************/
DMA_DESC* culDmaAllocChannel(UINT8* pDmaChannelNumber, FUNCTION* callBackFunction);

/******************************************************************************
* @fn  culDmaFreeChannel
*
* @brief
*      This function frees a channel assigned by culDmaAlloc(...). When a
*      DMA channel no longer is in use, this function should be called with
*      the channel number as a parameter in order to make the  DMA channel
*      available for other functionality.
*
*      NOTE: Be careful not to free a channel which is still in use.
*
* Parameters:
*
* @param  BYTE	 dmaChannelNumber - Describe value.
*         Number of the channel to be freed. This number should be identical
*         to the number returned by culDmaAlloc(...) when the channel was
*         requested.
*
* @return void
*
******************************************************************************/
void culDmaFreeChannel(BYTE dmaChannelNumber);


/******************************************************************************
* @fn  culDmaSetUpChannel
*
* @brief
*      This function is used to initiate a DMA transfer. The function requests
*      an available DMA channel by using culDmaAlloc(...). The DMA descriptor
*      is copied into the list of assigned DMA desriptors. The channel is then
*      armed (meaning that the channel is active but not nescessarily started.)
*      If the Boolean expression _immediately_ is true, the DMA data transfer
*      is started. The number of the DMA channel used is returned. This
*      identifier should be used with the function culDmaFreeChannel(...) when
*      the DMA channel is no longer needed in order to make the channel
*      available for other transfers.
*
* Parameters:
*
* @param  DMA_DESC*	 pDmaDescriptor
*         Pointer to the DMA descriptor structure the channel data is to be
*         fetched from. The data at this pointer is copied into the list of
*         active DMA channels.
* @param  BOOL	 immediately
*         If this value is TRUE, the DMA transfer is started as soon as the
*         channel setup is complete. If FALSE, the channel will wait for either
*         a DMA trigger or that the transfer is initiated manually (through
*         the register DMAREQ).
* @param  FUNCTION*	 callBackFunction
*         This parameter is a pointer to the function to be called from an
*         interrupt routine when the DMA transfer is finished. The function
*         must be a [void <functionName>(void)] type. The configuration of
*         the interrupt routine must be performed manually. If no function is
*         to be called, please set this parameter to NULL (0x00);
*
* @return BYTE
*         The returned value indicates the DMA channel number used for the
*         transfer. If 0 is returned, the function call was unsuccessful,
*         most probably because all DMA channels were occupied.
*
*         NOTE: The return value 0 (0x00) does NOT indicate that DMA
*               channel 0 was used.
*
******************************************************************************/
BYTE culDmaSetUpChannel(DMA_DESC* pDmaDescriptor, BOOL immediately, FUNCTION* callBackFunction);


/******************************************************************************
* @fn  culSetDmaChannel0CallBack
*
* @brief
*      This function sets the callbackfunction to be called when DMA channel
*      0 finishes its transfer. This function is used in order to avoid
*      changing the DMA ISR.
*
* Parameters:
*
* @param  FUNCTION*	 pCallBackFunction
*
* @return void
*
******************************************************************************/
void culSetDmaChannel0CallBack(FUNCTION* pCallBackFunction);


/******************************************************************************
* @fn  culClearDmaChannel0CallBack
*
* @brief
*      This function clears the callbackfunction to be called when DMA
*      channel 0 finishes.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void culClearDmaChannel0CallBack(void);


/******************************************************************************
* @fn  culDMAToFlash
*
* @brief       Description of the function.
*      This function configures DMA transfer of _length_ bytes starting from
*      _srcAddr_ to the flash. The flash address to be written must be set in
*      the register FADDR. The flash write address consists of 15 bits dividing
*      the 128 KB flash memory into 4 byte segments. Hence, each flash write must
*      contain at least 4 bytes (zero-padded if nescessary). The flash controller
*      issues a DMA trigger each time a new byte is desired.
*
* Parameters:
*
* @param  DMA_DESC*	pDmaChannel
*         A pointer to the DMA channel structure to be used for the transfer.
* @param  BYTE*      pSrcAddr
*         The start address in __xdata space of the data to be transferred.
* @param  WORD	      length
*         The number of bytes to be transferred.
* @param  BOOL	      generateInterrupt
*         If TRUE, the DMA channel will generate an interrupt request upon
*         completion.
*
* @return void
*
******************************************************************************/
void culDMAToFlash(DMA_DESC* pDmaChannel, BYTE __xdata* , WORD length, BOOL generateInterrupt);


/******************************************************************************
* @fn  culDmaToAes
*
* @brief
*      This function configures a DMA descriptor for transferring data to be
*      converted by the AES module. The function _culDmaFromAes(...)_ is used
*      to set up a DMA descriptor for transferring converted data from the AES
*      module.
*
* Parameters:
*
* @param  DMA_DESC*	 pDmaChannel
*         Pointer to the DMA descriptor to be used for DMA transfer of data to
*         be converted.
* @param  BYTE*	  pSrcAddr
*         Pointer to the start address of the data to be transferred to the AES
*         module. The length of the data to be converted should be a multiplum
*         of 16 bytes, as the AES module operates on blocks of 16 bytes
*         (128 bit). The data should be zero-padded at the end if nescessary, to
*         make the total byte count a multiplum of 16 bytes.
* @param  WORD	 length
*         Number of bytes to be converted in total. This number should be a
*         multiplum of 16 bytes, as the AES module operates on blocks of 16
*         bytes (128 bit).
* @param  BOOL	 generateInterrupt
*         If this parameter is TRUE the DMA channel will generate an interrupt
*         request when done. In order to generate an interrupt, the lines
*         INT_ENABLE(INUM_DMA, INT_ON); and EA = TRUE; must also be included.
*
* @return void
*
******************************************************************************/
void culDmaToAes(DMA_DESC* pDmaChannel, BYTE * pSrcAddr, WORD length, BOOL generateInterrupt);


/******************************************************************************
* @fn  culDmaFromAes
*
* @brief
*      This function configures a DMA descriptor for transferring converted
*      data from the AES module. The function _culDmaToAes(...)_ is used to set
*      up a DMA descriptor for transferring data to be converted to the AES
*      module.
*
* Parameters:
*
* @param  DMA_DESC* pDmaChannel
*         Pointer to the DMA descriptor to be used for DMA transfer of data
*         converted by the AES module.
* @param  BYTE*	 pDstAddr
*         Pointer to the start address where the converted data is to be stored.
*         The length of this data field should be a multiplum of 16 bytes, as
*         the AES module operates on blocks of 16 bytes (128 bit).
* @param  WORD	 length
*         Number of bytes to be converted in total. This number should be a
*         multiplum of 16 bytes, as the AES module operates on blocks of 16
*         bytes (128 bit).
* @param  BOOL	 generateInterrupt
*         If this parameter is TRUE the DMA channel will generate an interrupt
*         request when done. In order to generate an interrupt, the lines
*         INT_ENABLE(INUM_DMA, INT_ON); and EAL = TRUE; must also be included.
*
* @return BOOL
*         Function returns TRUE if the configuration was successful.
*
******************************************************************************/
BOOL culDmaFromAes(DMA_DESC* pDmaChannel, BYTE __xdata* pDstAddr, WORD length, BOOL generateInterrupt);


/******************************************************************************
* @fn  culDmaFromRadio
*
* @brief
*      This function configures DMA transfer from the radio Rx FiFo. The first
*      byte of the data to be transferred contains the number of bytes to be
*      transfer. The Rx FiFo may contain up to 128 bytes. When done, the DMA
*      channel may generate an interrupt.
*
* Parameters:
*
* @param  DMA_DESC*	 pDmaChannel
*         A pointer to the DMA channel structure to be used for the transfer.
* @param  BYTE*	 pDstAddr
*         The start address in __xdata space to where the data is to be stored.
* @param  BOOL	 generateInterrupt
*         If TRUE, the DMA channel will generate an interrupt request upon
*         completion.
*
* @return void
*
******************************************************************************/
void culDmaFromRadio(DMA_DESC* pDmaChannel, BYTE* pDstAddr, BOOL generateInterrupt);



/******************************************************************************
* @fn  culDmaToRadio
*
* @brief
*      This function configures DMA transfer to the radio Tx FiFo.
*
* Parameters:
*
* @param  DMA_DESC*  pDmaChannel
*         A pointer to the DMA channel structure to be used for the transfer.
* @param  WORD       length
* @param  BYTE*      pSrcAddr
*         The start address in __xdata space of the data to be transferred.
* @param  BOOL       generateInterrupt
*         If TRUE, the DMA channel will generate an interrupt request upon
*         completion.
*
* @return void
*
******************************************************************************/
void culDmaToRadio(DMA_DESC* pDmaChannel, WORD length, BYTE * pSrcAddr, BOOL generateInterrupt);


/******************************************************************************
* @fn  culDmaToUart0
*
* @brief       Description of the function.
*      This function configures a DMA transfer to UART0.
*
* Parameters:
*
* @param  DMA_DESC*	 pDmaChannel
*         A pointer to the DMA channel structure to be used for the transfer.
* @param  WORD	 length
*         The number of bytes to be transferred.
* @param  BYTE*	 pSrcAddr
*         The start address in __xdata space of the data to be transferred.
* @param  BOOL	 generateInterrupt
*         If TRUE, the DMA channel will generate an interrupt request upon completion.
*
* @return void
*
******************************************************************************/
void culDmaToUart0(DMA_DESC* pDmaChannel, WORD length, BYTE * pSrcAddr, BOOL generateInterrupt);



/******************************************************************************
*******************      TIMER4 administrator functions    ********************
******************************************************************************/

// Structure for the TIMER administrator entry table
typedef struct {
    BYTE        counter;
    BYTE        timeout;
    FUNCTION*   callBackFunction;
} TIMER4_TABLE_ENTRY;

#define TIMER_ADM_TABLE_LENGTH   3


/******************************************************************************
* @fn  culTimer4AdmInit
*
* @brief
*      This function sets up the timer 4 administration. The table is cleared,
*      timer 4 is set up to generate interrupts at a 1m-second interval,
*      timer 4 interrupt is enabled and the timer is turned on.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void culTimer4AdmInit(void);


/******************************************************************************
* @fn  T4_Adm_IRQ
*
* @brief
*     This interrupt routine is run each time timer 4 generates an interrupt
*     request (every m-second). The table is parsed through, and the counters
*     are incremented. If a counter is equal to the timeout value in the table,
*     the _callBackFunction_ is called and the counter is reset. At the end,
*     the interrupt flags are cleared.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
__interrupt void T4_Adm_IRQ (void);


/******************************************************************************
* @fn  culTimer4AdmSet
*
* @brief
*      This function arranges calling of the _callBackFunction_ at a desired
*      time interval in m-seconds. Up to _TIMER_ADM_TABLE_LENGTH_ different
*      functions can be set up to be run at different time intervals. This way
*      timer 4 is configured to run many interrupt routines at different time
*      intervals, thus leaving the other timers free to be configured as desired.
*
* Parameters:
*
* @param  DWORD	 timeout
*         The number of m-seconds between each time the _callBackFunction_
*         is run.
* @param  FUNCTION*	 callBackFunction
*         Pointer to the function to be run at the given interval. The
*         _callBackFunction_ must be of the type _void callBackFunction(void)_,
*         i.e. no arguments and no return value.
*
* @return BYTE
*         The entry number in the table. The value 0xFF indicates that an
*         error has occured and that the registration was unsuccessful. The
*         returned value must be used as an argument when the entry is to be
*         cleared using the function _culTimer4AdmClear(...)_.
*
******************************************************************************/
BYTE culTimer4AdmSet(DWORD timeout, FUNCTION* callBackFunction);



/******************************************************************************
* @fn  culTimer4AdmClear
*
* @brief
*      This function clears the table entry _entry_ from the table of functions
*      to be called at different timeouts.
*
* Parameters:
*
* @param  BYTE	 entry
*         The index number of the entry in the table that is to be cleared.
*         This value must be identical to the returned value from the function
*         _culTimer4AdmSet(...)_.
*
* @return void
*
******************************************************************************/
void culTimer4AdmClear(BYTE entry);



/******************************************************************************
* @fn  culTimer4AdmReset
*
* @brief
*      This function resets the counter of the entry in question.
*
* Parameters:
*
* @param  BYTE	 entry
*         The index number of the entry in the table that is reset. This value
*         must be identical to the returned value from the function
*         _culTimer4AdmSet(...)_.
*
* @return void
*
******************************************************************************/
void culTimer4AdmReset(BYTE entry);


/******************************************************************************
*******************      Simple Packet Protocol (SPP)      ********************
******************************************************************************/
#define SPP_MAX_PAYLOAD_LENGTH          122 // This length may be extended up to 250
#define SPP_HEADER_AND_FOOTER_LENGTH    3
#define SPP_FOOTER_LENGTH               0
#define SPP_ACK_LENGTH                  0

typedef struct{
    BYTE payloadLength;
    BYTE destAddress;
    BYTE srcAddress;
    BYTE flags;
    BYTE payload[SPP_MAX_PAYLOAD_LENGTH + SPP_FOOTER_LENGTH];
}SPP_STRUCT;

#define BROADCAST_ADDRESS               0
#define WAIT_UNTIL_RECEIVE              0

#define SEQUENCE_BIT                    0x80
#define RETRANSMISSION                  0x04
#define ACK                             0x02
#define DO_ACK                          0x01
#define DO_NOT_ACK                      0x00


#define ACK_TIMEOUT                     15
#define ACK_RETRIES                     3

// TX status flags
#define TX_IN_PROGRESS                  0x80
#define TX_SUCCESSFUL                   0x40
#define DEST_UNREACHABLE                0x20
#define TX_IDLE                         0x00

// RX status flags
#define RX_IN_PROGRESS                  0x80
#define PACKET_RECEIVED                 0x40
#define RX_WAIT                         0x20
#define RX_COMPLETE                     0x10
#define RX_IDLE                         0x00

// sppSend return values
#define CHANNEL_BUSY                    0x04
#define TOO_LONG                        0x08



/******************************************************************************
* @fn  sppInit
*
* @brief
*      This function initialises the Simple Packet Protocol (SPP). Two DMA
*      channels are requested from the DMA administrator and set up to transfer
*      data to and from the Rx FiFo and Tx FiFo respectively. The Timer 4
*      administrator is also set up, and this unit will be used to generate
*      interrupts when a packet is not ACK'ed by the recipient whithin some time.
*      The radio is configured to transmit at a given frequency and to
*      automatically calculate and insert a CRC value when in transmit and to
*      check the CRC value in receive.
*
* Parameters:
*
* @param  UINT32 frequency
*         The desired Radio Frequency in kHz.
* @param  BYTE	 address
*         This nodes address.
*
* @return BOOL
*         Returns TRUE if the configuration is successful and FALSE otherwise.
*
******************************************************************************/
BOOL sppInit(UINT32 frequency, BYTE address);


/******************************************************************************
* @fn  spp_rf_IRQ
*
* @brief
*      This ISR handles the possible RF interrupts that may occur when using the
*      SPP. When the start of a packet is received, the RX-state will be set
*      accordingly.When a packet is sent and no ACK is requested, the TX_DONE
*      interrupt will be run. Reception of a complete packet will only be used
*      for receiving an ACK. This ISR will handle the ACK reception.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
__interrupt void spp_rf_IRQ(void);

/******************************************************************************
* @fn  rf_error_IRQ(
*
* @brief
*      This function is run if an Radio error interrupt (RFIF) is generated.
*      If TX underrun or RX overflow occurs, the corresponding FIFO is flushed.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
__interrupt void rf_error_IRQ(void);

/******************************************************************************
* @fn  sppSetAddress
*
* @brief
*      This function changes the local address.
*
* Parameters:
*
* @param  BYTE	 address
*         This nodes new address.
*
* @return void
*
******************************************************************************/
void sppSetAddress(BYTE address);


/******************************************************************************
* @fn  sppSend
*
* @brief
*      This function transfers _length_ bytes (up to 122) starting at
*      _pTransferData_ to the _recipient_. The number of bytes to be
*      transferred, flags, and destination- and source-address are inserted
*      before the Tx DMA channel is set up to transfer the payload to the Tx
*      FiFo. After the DMA channel is started, the transmitter is turned on.
*      If an ACK is expected in response, the corresponding flags and settings
*      are configured.
*
* Parameters:
*
* @param  SPP_TX_STRUCT*	 pPacketPointer
*         Pointer to the struct containing the transfer data.
*
* @return BYTE
*         Returns TRUE if the transfer is successful, TOO_LONG if the length of
*         the data is too long and CHANNEL_BUSY if the channel is not available.
*
******************************************************************************/
BYTE sppSend(SPP_STRUCT* pPacketPointer);



/******************************************************************************
* @fn  sppReceive
*
* @brief
*      This function enables reception of up to 128 bytes including header and
*      trailer. The received data will be transferred by the DMA to the data
*      space starting at _pReceiveData_. The DMA is armed and reception is
*      turned on. The reception of data will generate DMA triggers. When the
*      whole packet has been received and transferred, the DMA will generate an
*      interrupt and run the function _rxCallBack_, which is defined above.
*
* Parameters:
*
* @param  SPP_RX_STRUCT*	 pReceiveData
*         Pointer to where the received data are to be stored.
*
* @return void
*
******************************************************************************/
void sppReceive(SPP_STRUCT *pReceiveData);


/******************************************************************************
* @fn  setRxCallBackFunction
*
* @brief
*     This function is used to set a user-specified call-back function
*     to be run when a correct packet addressed for me is received.
*
* Parameters:
*
* @param  FUNCTION	 callBackFunction
*
* @return void
*
******************************************************************************/
void sppSetRxCallBackFunction(FUNCTION callBackFunction);




/******************************************************************************
*******************       Radio (simple usage of SPP)       *******************
******************************************************************************/

//------------------------------------------------------------------------------------------------------
// void radioInit(...);
//
//  Description:
//      This function initializes the radio by calling sppInit. The frequency of operation is set and
//      this nodes address is stored. The radio is set up for handling non-IEEE 802.15.4 packets.
//
//  Arguments:
//      UINT32 frequency
//          The radio frequency to be used.
//      BYTE localAddress
//          The address of this node.
//
//  Return value:
//      void
//------------------------------------------------------------------------------------------------------
void radioInit(UINT32 frequency, BYTE localAddress);

//------------------------------------------------------------------------------------------------------
// BYTE radioSend(...);
//
//  Description:
//      This function sends data of a given length either to a specified recipient (may be broadcast)
//      using the radio. If the number of bytes to be transferred is larger than the size of the TX FIFO,
//      the data is split over an adequate number of packets. The function uses the SPP library. If the
//      radio is busy, or the packet is sent but not ACK'ed, the transmission is retried. If the retires
//      fails, the function returns FALSE. If the packet is ACK'ed correctly by the receiver, the
//      function returns TRUE.
//
//  Arguments:
//      BYTE* transmitData
//                      Pointer to the data to be sent.
//      WORD dataLenght
//          The number of bytes to be transferred.
//      BYTE remoteAddress
//          The address of the node to receive the packet. 0 is broadcast address (BROADCAST_ADDRESS).
//      BYTE doAck
//          Set to DO_ACK if the packet is to be ACK'ed and DO_NOT_ACK otherwise.
//
//  Return value:
//      BOOL
//          TRUE if the sent packet is acked by the recipient and false otherwise.
//------------------------------------------------------------------------------------------------------
BOOL radioSend(BYTE* transmitData, WORD dataLength, BYTE remoteAddress, BYTE doAck);


//------------------------------------------------------------------------------------------------------
// BYTE radioReceive(...);
//
//  Description:
//      This function turns on the radio receiver and waits until either a correctly addressed packet
//      packet is received or the unit has been waiting for a specified time period. The function employs
//      the SPP library for radio access.
//
//  Arguments:
//      BYTE** receiveData
//                      Double pointer to the received data. This way of reference reduces the RAM requirement.
//      BYTE* lenght
//          Pointer to the length of the received data.
//      WORD timeout
//          Value indicating how long the receiver will wait for a packet in ms. If no packet is received
//          within this timeout period, false is returned.
//          If timeout is set to 0, the function will not return before a packet is received.
//      BYTE* sender
//          The function will fill in the packets source address at this location.
//
//  Return value:
//      BOOL
//          TRUE if a packet has been received and FALSE if no packet has been received within the timeout
//          period.
//------------------------------------------------------------------------------------------------------
BOOL radioReceive(BYTE** receiveData, BYTE* length, WORD timeout, BYTE* sender);



/******************************************************************************
*******************       FLASH LIFE EXTENDER (FLEX)       ********************
******************************************************************************/
typedef struct{
    enum{
        ERASED          = 3,
        VALID           = 2,
        OBSOLETE        = 0,
    }pageState;//: 2;
    enum{
        VERSION_0       = 0,
        VERSION_1       = 1,
        VERSION_2       = 2,
        VERSION_3       = 3,
    } version;//: 6;
    BYTE identifier;
    WORD eraseCounter;
}FLEX_PAGE_HEADER;




#define FLEX_ALLOC(kb)                                              \
    BYTE __code flexPages = (kb/2);                                 \
    BYTE __code flexBuf[kb/2 * 2048] _at_ (0x10000 - (kb/2 * 2048)) \


/******************************************************************************
* @fn  flexFetch
*
* @brief
*
* Parameters:
*
* @param  void* pDataPointer
* @param  BYTE	 id
* @param  BYTE	 length
* @param  void* initValue
*
* @return BOOL
*
******************************************************************************/
BOOL flexFetch(void* pDataPointer, BYTE id, BYTE length, void* initValue);


/******************************************************************************
* @fn  flexCommit
*
* @brief       Description of the function.
*
* Parameters:
*
* @param  void*
* @param  BYTE	 id
* @param  BYTE	 length
*
* @return BOOL
*
******************************************************************************/
BOOL flexCommit(void* pDataPointer, BYTE id, BYTE length);

#define FLASH_WRITE_BUFFER_LENGTH    60


#define FLEX_NOT_FOUND      0x08
#define FLEX_DATA_VALID     0x02
#define FLEX_DATA_UNUSED    0xFF
#define FLEX_DATA_OBSOLETE  0x00

#endif  // CUL_H
