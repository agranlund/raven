#ifndef _ISA_H_
#define _ISA_H_

//----------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------
#define C__ISA  0x5F495341          /* '_ISA' */
#define C__PNP  0x5F504E50          /* '_PNP' */

#define ISA_ENDIAN_BE       0       /* Big endian                       */
#define ISA_ENDIAN_LEAS     1       /* Little endian: Address swapped   */
#define ISA_ENDIAN_LELS     2       /* Little endian: Lane swapped      */

#define ISA_MAX_CARDS       8
#define ISA_MAX_CARD_DEVS   5
#define ISA_MAX_DEVS        (ISA_MAX_CARDS * ISA_MAX_CARD_DEVS)

#define ISA_MAX_DEV_IDS     5
#define ISA_MAX_DEV_PORT    8
#define ISA_MAX_DEV_MEM     4
#define ISA_MAX_DEV_IRQ     2
#define ISA_MAX_DEV_DMA     2

//----------------------------------------------------------------------
// Device
//----------------------------------------------------------------------
typedef struct
{
    unsigned int    id[ISA_MAX_DEV_IDS];
    unsigned int    mem[ISA_MAX_DEV_MEM];
    unsigned short  port[ISA_MAX_DEV_PORT];
    unsigned char   irq[ISA_MAX_DEV_IRQ];
    unsigned char   dma[ISA_MAX_DEV_DMA];
} isa_dev_t;


//----------------------------------------------------------------------
// Bus interface
//----------------------------------------------------------------------
typedef struct
{
    unsigned short  version;
    unsigned int    iobase;
    unsigned int    membase;
    unsigned short  irqmask;
    unsigned char   drqmask;
    unsigned char   endian;

    void            (*outp)(unsigned short port, unsigned char data);
    void            (*outpw)(unsigned short port, unsigned short data);
    unsigned char   (*inp)(unsigned short port);
    unsigned short  (*inpw)(unsigned short addr);

    void            (*outp_buf)(unsigned short port, unsigned char* buf, int count);
    void            (*outpw_buf)(unsigned short port, unsigned short* buf, int count);
    void            (*inp_buf)(unsigned short port, unsigned char* buf, int count);
    void            (*inpw_buf)(unsigned short port, unsigned short* buf, int count);

    unsigned int    (*irq_set)(unsigned char irq, unsigned int func);
    unsigned int    (*irq_en)(unsigned char irq, unsigned char enabled);

    unsigned short  numdevs;
    isa_dev_t       devs[ISA_MAX_DEVS];

} isa_t;


#endif // _ISA_H_
