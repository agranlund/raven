//---------------------------------------------------------------------
// ch559.c
// utility functions from WCH
//---------------------------------------------------------------------
#include "system.h"
#include <stdio.h>
#include <string.h>

#define CH559UART1_FIFO_EN 0 //Enable CH559 serial port 1 receiving FIFO (receive and send 8 bytes each)

#if CH559UART1_FIFO_EN
UINT8 CH559UART1_FIFO_CNT = 0;
#define CH559UART1_FIFO_TRIG 7 // FIFO full 7 bytes trigger interrupt (1, 2, 4, or 7 bytes)
#endif

#if defined(DEBUG)
/*******************************************************************************
* Function Name  : CH559UART0Init()
* Description    : CH559����0��ʼ��,Ĭ��ʹ��T1��UART0�Ĳ����ʷ�����,Ҳ����ʹ��T2
                   ��Ϊ�����ʷ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559UART0Init(UINT32 bps)
{
    UINT32 x;
    UINT8 x2; 

    //PORT_CFG |= bP3_OC;
    P3_DIR |= bTXD;
    P3_PU |= bTXD;
    P3 |= bTXD;
    
    SM0 = 0;
    SM1 = 1;
    SM2 = 0;                                                                   //����0ʹ��ģʽ1
                                                                               //ʹ��Timer1��Ϊ�����ʷ�����
    RCLK = 0;                                                                  //UART0����ʱ��
    TCLK = 0;                                                                  //UART0����ʱ��
    PCON |= SMOD;
    x = 10 * FREQ_SYS / bps / 16;                                             //���������Ƶ��ע��x��ֵ��Ҫ���                            
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x++;                                                       //��������

    TMOD = TMOD & ~ bT1_GATE & ~ bT1_CT & ~ MASK_T1_MOD | bT1_M1;              //0X20��Timer1��Ϊ8λ�Զ����ض�ʱ��
    T2MOD = T2MOD | bTMR_CLK | bT1_CLK;                                        //Timer1ʱ��ѡ��
    TH1 = 0-x;                                                                 //12MHz����,buad/12Ϊʵ�������ò�����
    TR1 = 1;                                                                   //������ʱ��1

	REN = 1;                                                                   //����0����ʹ��
	TI = 1;
}

/*******************************************************************************
* Function Name  : CH559UART0RcvByte()
* Description    : CH559UART0����һ���ֽ�
* Input          : None
* Output         : None
* Return         : SBUF
*******************************************************************************/
UINT8  CH559UART0RcvByte(void)
{
    while(!RI);
    RI = 0;
    return SBUF;
}

/*******************************************************************************
* Function Name  : CH559UART0SendByte(UINT8 SendDat)
* Description    : CH559UART0����һ���ֽ�
* Input          : UINT8 SendDat��Ҫ���͵�����
* Output         : None
* Return         : None
*******************************************************************************/
void CH559UART0SendByte(UINT8 SendDat)
{
    while (!TI);
    TI = 0;
    SBUF = SendDat;
}
#endif


/************************************************* ******************************
* Function Name: CH559UART1Reset()
* Description: CH559UART1 port soft reset
* Input: None
* Output: None
* Return: None
************************************************** *****************************/
void CH559UART1Reset(void)
{
    SER1_IER |= bIER_RESET; //This bit can be automatically cleared to reset the serial port register
}

/************************************************* ******************************
* Function Name: CH559UART1Init(UINT8 DIV,UINT8 mode,UINT8 pin)
* Description: CH559 UART1 initialization setting
* Input:
                   UINT8 DIV sets the frequency division coefficient, clock frequency=Fsys/DIV, DIV cannot be 0
                   UINT8 mode, mode selection, 1: normal serial port mode; 0: 485 mode
                   UINT8 pin, serial port pin selection;
                   When mode=1
                   0: RXD1=P4.0, TXD1 is off;
                   1: RXD1&TXD1=P4.0&P4.4;
                   2: RXD1&TXD1=P2.6&P2.7;
                   3: RXD1&TXD1&TNOW=P2.6&P2.7&P2.5;
                   When mode=0
                   0: meaningless
                   1: Connect P5.4&P5.5 to 485, TNOW=P4.4;
                   2: P5.4&P5.5 connect to 485;
                   3: P5.4&P5.5 connect to 485, TNOW=P2.5;
                   UINT32 bps, transfer rate in baud
                   UINT8 databits, databits per word, must be 5, 6, 7 or 8
* Output: None
* Return: None
************************************************** *****************************/
void CH559UART1InitEx(UINT8 DIV,UINT8 mode,UINT8 pin,UINT32 bps,UINT8 databits)
{
    UINT32 x;
    UINT8 x2;

    SER1_LCR |= bLCR_DLAB; // DLAB bit is 1, write DLL, DLM and DIV registers
    SER1_DIV = DIV; // Prescaler
    x = 10 * FREQ_SYS *2 / DIV / 16 / bps;
    x2 = x% 10;
    x /= 10;
    if (x2 >= 5) x ++; //Rounding
    SER1_DLM = x>>8;
    SER1_DLL = x&0xff;
    SER1_LCR &= ~bLCR_DLAB; //DLAB bit is 0 to prevent modification of UART1 baud rate and clock
    if(mode == 1) {
        //Close RS485 mode RS485_EN = 0, can not be omitted
        XBUS_AUX |= bALE_CLK_EN;
    } else if(mode == 0) {
        //Enable RS485 mode RS485_EN = 1;
        UHUB1_CTRL |= bUH1_DISABLE;
        PIN_FUNC &= ~bXBUS_CS_OE;
        PIN_FUNC |= bXBUS_AL_OE;
        XBUS_AUX &= ~bALE_CLK_EN;
        SER1_MCR |= bMCR_HALF; //485 mode can only use half-duplex mode
    }
    SER1_LCR = (SER1_LCR & ~MASK_U1_WORD_SZ) | ((databits - 5) & MASK_U1_WORD_SZ); //Line control, 5, 6, 7 or 8 databits
    SER1_LCR &= ~(bLCR_PAR_EN | bLCR_STOP_BIT); //Wireless path interval, no parity, 1 stop bit

    SER1_MCR &= ~bMCR_TNOW;
    //SER1_IER |= bIER_EN_MODEM_O;
    SER1_IER |= ((pin << 4) & MASK_U1_PIN_MOD); //Serial port mode configuration
    //SER1_IER |= /*bIER_MODEM_CHG | bIER_LINE_STAT | bIER_THR_EMPTY | bIER_RECV_RD;//Interrupt enable configuration

    if (mode == 0)
    {
        switch(pin) {
            case 0:
                break;
            case 1:
                P4_DIR |= bTNOW_;
                P4_PU |= bTNOW_;
                break;
            case 2:
                break;
            case 3:
                P2_DIR |= bTNOW;
                P2_PU |= bTNOW;
                break;
        }
    }
    else
    {
        switch(pin) {
            case 0:
                break;
            case 1:
                P4_DIR |= bTXD1_;
                P4_PU |= bTXD1_;
                break;
            case 2:
                P2_DIR |= bTXD1;
                P2_PU |= bTXD1;
                break;
            case 3:
                P2_DIR |= bTXD1 | bTNOW;
                P2_PU |= bTXD1 | bTNOW;
                break;
        }
    }

#if CH559UART1_FIFO_EN
    SER1_FCR |= (MASK_U1_FIFO_TRIG & (CH559UART1_FIFO_TRIG << 5)) | bFCR_T_FIFO_CLR | bFCR_R_FIFO_CLR | bFCR_FIFO_EN;
#endif
    //SER1_MCR |= bMCR_OUT2; //MODEM control register
    SER1_ADDR |= 0xff; //Close multi-machine communication
}

void CH559UART1Init(UINT32 bps)
{
    CH559UART1InitEx(1, 1, 2, bps, 8);
}


/************************************************* ******************************
* Function Name: CH559UART1RcvByte()
* Description: CH559UART1 receives one byte
* Input: None
* Output: None
* Return: correct: UINT8 Rcvdat; receive data
************************************************** *****************************/
UINT8 CH559UART1RcvByte(void)
{
    while((SER1_LSR & bLSR_DATA_RDY) == 0);
    return SER1_RBR;
}


/************************************************* ******************************
* Function Name: CH559UART1Rcv(PUINT8 buf,UINT8 len)
* Description: CH559UART1 receives multiple bytes, FIFO must be opened
* Input: PUINT8 buf data storage buffer
                   UINT8 len Data pre-receive length
* Output: None
* Return: None
************************************************** *****************************/
#if CH559UART1_FIFO_EN
void CH559UART1Rcv(PUINT8 buf,UINT8 len)
{
    UINT8 j = 0;
    while(len) {
         if(len >= CH559UART1_FIFO_TRIG) {
            //The pre-receive length exceeds the FIFO receive trigger level
            while((SER1_IIR & U1_INT_RECV_RDY) == 0);//Waiting for data available interrupt
            for(UINT8 i=0;i<CH559UART1_FIFO_TRIG;i++) {
                *(buf+j) = SER1_RBR;
                j++;
            }
            len -= CH559UART1_FIFO_TRIG;
        } else {
            while((SER1_LSR & bLSR_DATA_RDY) == 0);//Wait for the data to be ready
            *(buf+j) = SER1_RBR;
            j++;
            len--;
        }
    }

}
#endif

/************************************************* ******************************
* Function Name: CH559UART1SendByte(UINT8 SendDat)
* Description: CH559UART1 sends one byte
* Input: UINT8 SendDat; the data to be sent
* Output: None
* Return: None
************************************************** *****************************/
void CH559UART1SendByte(UINT8 SendDat)
{
#if CH559UART1_FIFO_EN
    while(1) {
        if(SER1_LSR & bLSR_T_FIFO_EMP) {
            CH559UART1_FIFO_CNT=8;//FIFO empty can fill up to 8 bytes
        }
        if (CH559UART1_FIFO_CNT!=0) {
            SER1_THR = SendDat;
            CH559UART1_FIFO_CNT--;
            break;
        }
        while ((SER1_LSR & bLSR_T_FIFO_EMP) == 0 );//It is found that the FIFO is full and can only wait for the first 1 byte to be sent
    }
#else
    while ((SER1_LSR & bLSR_T_ALL_EMP) == 0 );//Do not open FIFO, wait for the completion of 1 byte transmission
    SER1_THR = SendDat;
#endif
}



/*******************************************************************************
* Function Name  : Flash_Op_Unlock
* Description    : Flash��������
* Input          : flash_type: bCODE_WE(Code Flash); bDATA_WE(Data Flash) 
* Output         : None
* Return         : 0xFF(Flash Operation Flags Error)/0x00(Flash Operation Flags Correct)
*******************************************************************************/
static UINT8 Flash_Op_Unlock( UINT8 flash_type )
{
    bool ea_sts;
    
    /* Disable all INTs to prevent writing GLOBAL_CFG from failing in safe mode. */
    ea_sts = EA;                                
    EA = 0;
    
    /* Enable Flash writing operations. */
    SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= flash_type;
    SAFE_MOD = 0x00;

    /* Restore all INTs. */
    EA = ea_sts;
    
    return 0x00;
}

/*******************************************************************************
* Function Name  : Flash_Op_Lock
* Description    : Flash��������
* Input          : flash_type: bCODE_WE(Code Flash)/bDATA_WE(Data Flash) 
* Output         : None
* Return         : None
*******************************************************************************/
static void Flash_Op_Lock( UINT8 flash_type )
{
    bool ea_sts;
    
    /* Disable all INTs to prevent writing GLOBAL_CFG from failing in safe mode. */
    ea_sts = EA;                                
    EA = 0;
    
    /* Disable Flash writing operations. */
    SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~flash_type;
    SAFE_MOD = 0x00;
    
    /* Restore all INTs. */
    EA = ea_sts;
}

/*******************************************************************************
* Function Name  : EraseBlock(UINT16 Addr)
* Description    : CodeFlash块擦除(1KB)，全部数据位写1
* Input          : UINT16 Addr
* Output         : None
* Return         : None
*******************************************************************************/
static UINT8 EraseBlock( UINT16 Addr )
{
	ROM_ADDR = Addr;

    while (!(ROM_STATUS & bROM_ADDR_OK));

	if ( ROM_STATUS & bROM_ADDR_OK ) {                                          // 操作地址有效
		ROM_CTRL = ROM_CMD_ERASE;
		return( ( ROM_STATUS ^ bROM_ADDR_OK ) & 0x7F );                           // 返回状态,0x00=success, 0x01=time out(bROM_CMD_TOUT), 0x02=unknown command(bROM_CMD_ERR)
	}
	else{ 
        return( 0x40 );}
}

/*******************************************************************************
* Function Name  : ProgWord( UINT16 Addr, UINT16 Data )
* Description    : 写EEPROM，双字节写
* Input          : UNIT16 Addr,写地址
                   UINT16 Data,数据
* Output         : None
* Return         : SUCESS 
*******************************************************************************/
static UINT8 ProgWord( UINT16 Addr, UINT16 Data )
{
	ROM_ADDR = Addr;
	ROM_DATA = Data;

    while (!(ROM_STATUS & bROM_ADDR_OK));

	if ( ROM_STATUS & bROM_ADDR_OK ) {                                           // 操作地址有效
		ROM_CTRL = ROM_CMD_PROG;
		return( ( ROM_STATUS ^ bROM_ADDR_OK ) & 0x7F );                            // 返回状态,0x00=success, 0x01=time out(bROM_CMD_TOUT), 0x02=unknown command(bROM_CMD_ERR)
	}
	else return( 0x40 );
}

/*******************************************************************************
* Function Name  : EraseDataFlash(UINT16 Addr)
* Description    : DataFlash块擦除(1KB)，全部数据位写1
* Input          : UINT16 Addr
* Output         : None
* Return         : UINT8 status
*******************************************************************************/
UINT8 CH559EraseDataFlash(UINT16 Addr)
{
    UINT8 status;
    if( Flash_Op_Unlock( bDATA_WE ) ) {
        return( 0xFF );
    }
    status = EraseBlock(Addr);
    Flash_Op_Lock( bDATA_WE );
    return status;
}

/*******************************************************************************
* Function Name  : WriteDataFlash(UINT16 Addr,PUINT8 buf,UINT16 len)
* Description    : DataFlash写
* Input          : UINT16 Addr，PUINT16 buf,UINT16 len
* Output         : None
* Return         : 
*******************************************************************************/
UINT8 CH559WriteDataFlash(UINT16 Addr,PUINT8 buf,UINT16 len)
{
    UINT16 j,tmp;   
    UINT8 status;

    if( Flash_Op_Unlock( bDATA_WE ) ) {
        return( 0xFF );
    }
    for(j=0;j<len;j=j+2)
    {
        tmp = buf[j+1];
        tmp <<= 8;
        tmp += buf[j];			
        status = ProgWord(Addr,tmp);
        if( status != 0x00 )
        {
            return status;
        }
        Addr = Addr + 2;
    }
    Flash_Op_Lock( bDATA_WE );
    return status;
}
