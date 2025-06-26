/* ymodem utility
 *
 * based on ymodem for RTD Serial Recovery (rtdsr)
 * copyright (c) 2011 Pete B. <xtreamerdev@gmail.com>
 * based on ymodem.c for bootldr, copyright (c) 2001 John G Dorsey
 * baded on ymodem.c for reimage, copyright (c) 2009 Rich M Legrand
 * crc16 function from PIC CRC16, by Ashley Roll & Scott Dattalo
 * crc32 function from crc32.c by Craig Bruce
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "tos.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "unistd.h"
#include "ymodem.h"

static long hz200s(void) { return *((volatile unsigned long*)0x4ba); }
static unsigned long hz200(void) { return (unsigned long) Supexec(hz200s); }

void _sleep(long seconds) {
    unsigned long ticks = seconds * 200;
    unsigned long h1 = hz200();
    while (ticks != 0) {
        unsigned long h2 = hz200();
        if (h2 != h1) {
            h1 = h2;
            ticks--;
        }
    }
}

#define RV_PADDR_UART2  0x20000020UL
#define IOB(base,offs)  *((volatile unsigned char*)(base + offs))
#define UART_LSR        0x14
#define UART_RHR        0x00
#define UART_THR        0x00

static bool uart_txrdy(void) {
    return (IOB(RV_PADDR_UART2, UART_LSR) & (1 << 5)) ? true : false;
}
static bool uart_rxrdy(void) {
    return (IOB(RV_PADDR_UART2, UART_LSR) & (1 << 0)) ? true : false;
}
static void uart_send(unsigned char data) {
    while (!uart_txrdy()) { }
    IOB(RV_PADDR_UART2, UART_THR) = data;
}
static unsigned char uart_recv(void) {
    while (!uart_rxrdy()) { }
    return IOB(RV_PADDR_UART2, UART_RHR);
}

static void _putchar(int c) {
    uart_send((unsigned char)c);
}

static int _getchar(int timeout) {
    unsigned long ticks = 200UL * timeout;
    unsigned long h1 = hz200();
    while (!uart_rxrdy() && (ticks != 0)) {
        unsigned long h2 = hz200();
        if (h2 != h1) { 
            h1 = h2;
            ticks--;
        }
    }
    return (ticks == 0) ? -1 : (int)uart_recv();
}

static int serial_read(void) {
    if (uart_rxrdy()) {
        while(1) {
            while(uart_rxrdy()) {
                (void)uart_recv();
            }
            sleep(1);
            if (!uart_rxrdy()) {
                return 0;
            }
        }
    }
    return 0;
}

#ifdef WITH_CRC32
/* http://csbruce.com/~csbruce/software/crc32.c */
static unsigned long crc32(const unsigned char* buf, unsigned long count)
{
	unsigned long crc = 0xFFFFFFFFUL;
	unsigned long i;

	/* This static table adds 1K */
	static const unsigned long crc_table[256] = {
		0x00000000UL,0x77073096UL,0xEE0E612CUL,0x990951BAUL,0x076DC419UL,0x706AF48FUL,0xE963A535UL,
		0x9E6495A3UL,0x0EDB8832UL,0x79DCB8A4UL,0xE0D5E91EUL,0x97D2D988UL,0x09B64C2BUL,0x7EB17CBDUL,
		0xE7B82D07UL,0x90BF1D91UL,0x1DB71064UL,0x6AB020F2UL,0xF3B97148UL,0x84BE41DEUL,0x1ADAD47DUL,
		0x6DDDE4EBUL,0xF4D4B551UL,0x83D385C7UL,0x136C9856UL,0x646BA8C0UL,0xFD62F97AUL,0x8A65C9ECUL,
		0x14015C4FUL,0x63066CD9UL,0xFA0F3D63UL,0x8D080DF5UL,0x3B6E20C8UL,0x4C69105EUL,0xD56041E4UL,
		0xA2677172UL,0x3C03E4D1UL,0x4B04D447UL,0xD20D85FDUL,0xA50AB56BUL,0x35B5A8FAUL,0x42B2986CUL,
		0xDBBBC9D6UL,0xACBCF940UL,0x32D86CE3UL,0x45DF5C75UL,0xDCD60DCFUL,0xABD13D59UL,0x26D930ACUL,
		0x51DE003AUL,0xC8D75180UL,0xBFD06116UL,0x21B4F4B5UL,0x56B3C423UL,0xCFBA9599UL,0xB8BDA50FUL,
		0x2802B89EUL,0x5F058808UL,0xC60CD9B2UL,0xB10BE924UL,0x2F6F7C87UL,0x58684C11UL,0xC1611DABUL,
		0xB6662D3DUL,0x76DC4190UL,0x01DB7106UL,0x98D220BCUL,0xEFD5102AUL,0x71B18589UL,0x06B6B51FUL,
		0x9FBFE4A5UL,0xE8B8D433UL,0x7807C9A2UL,0x0F00F934UL,0x9609A88EUL,0xE10E9818UL,0x7F6A0DBBUL,
		0x086D3D2DUL,0x91646C97UL,0xE6635C01UL,0x6B6B51F4UL,0x1C6C6162UL,0x856530D8UL,0xF262004EUL,
		0x6C0695EDUL,0x1B01A57BUL,0x8208F4C1UL,0xF50FC457UL,0x65B0D9C6UL,0x12B7E950UL,0x8BBEB8EAUL,
		0xFCB9887CUL,0x62DD1DDFUL,0x15DA2D49UL,0x8CD37CF3UL,0xFBD44C65UL,0x4DB26158UL,0x3AB551CEUL,
		0xA3BC0074UL,0xD4BB30E2UL,0x4ADFA541UL,0x3DD895D7UL,0xA4D1C46DUL,0xD3D6F4FBUL,0x4369E96AUL,
		0x346ED9FCUL,0xAD678846UL,0xDA60B8D0UL,0x44042D73UL,0x33031DE5UL,0xAA0A4C5FUL,0xDD0D7CC9UL,
		0x5005713CUL,0x270241AAUL,0xBE0B1010UL,0xC90C2086UL,0x5768B525UL,0x206F85B3UL,0xB966D409UL,
		0xCE61E49FUL,0x5EDEF90EUL,0x29D9C998UL,0xB0D09822UL,0xC7D7A8B4UL,0x59B33D17UL,0x2EB40D81UL,
		0xB7BD5C3BUL,0xC0BA6CADUL,0xEDB88320UL,0x9ABFB3B6UL,0x03B6E20CUL,0x74B1D29AUL,0xEAD54739UL,
		0x9DD277AFUL,0x04DB2615UL,0x73DC1683UL,0xE3630B12UL,0x94643B84UL,0x0D6D6A3EUL,0x7A6A5AA8UL,
		0xE40ECF0BUL,0x9309FF9DUL,0x0A00AE27UL,0x7D079EB1UL,0xF00F9344UL,0x8708A3D2UL,0x1E01F268UL,
		0x6906C2FEUL,0xF762575DUL,0x806567CBUL,0x196C3671UL,0x6E6B06E7UL,0xFED41B76UL,0x89D32BE0UL,
		0x10DA7A5AUL,0x67DD4ACCUL,0xF9B9DF6FUL,0x8EBEEFF9UL,0x17B7BE43UL,0x60B08ED5UL,0xD6D6A3E8UL,
		0xA1D1937EUL,0x38D8C2C4UL,0x4FDFF252UL,0xD1BB67F1UL,0xA6BC5767UL,0x3FB506DDUL,0x48B2364BUL,
		0xD80D2BDAUL,0xAF0A1B4CUL,0x36034AF6UL,0x41047A60UL,0xDF60EFC3UL,0xA867DF55UL,0x316E8EEFUL,
		0x4669BE79UL,0xCB61B38CUL,0xBC66831AUL,0x256FD2A0UL,0x5268E236UL,0xCC0C7795UL,0xBB0B4703UL,
		0x220216B9UL,0x5505262FUL,0xC5BA3BBEUL,0xB2BD0B28UL,0x2BB45A92UL,0x5CB36A04UL,0xC2D7FFA7UL,
		0xB5D0CF31UL,0x2CD99E8BUL,0x5BDEAE1DUL,0x9B64C2B0UL,0xEC63F226UL,0x756AA39CUL,0x026D930AUL,
		0x9C0906A9UL,0xEB0E363FUL,0x72076785UL,0x05005713UL,0x95BF4A82UL,0xE2B87A14UL,0x7BB12BAEUL,
		0x0CB61B38UL,0x92D28E9BUL,0xE5D5BE0DUL,0x7CDCEFB7UL,0x0BDBDF21UL,0x86D3D2D4UL,0xF1D4E242UL,
		0x68DDB3F8UL,0x1FDA836EUL,0x81BE16CDUL,0xF6B9265BUL,0x6FB077E1UL,0x18B74777UL,0x88085AE6UL,
		0xFF0F6A70UL,0x66063BCAUL,0x11010B5CUL,0x8F659EFFUL,0xF862AE69UL,0x616BFFD3UL,0x166CCF45UL,
		0xA00AE278UL,0xD70DD2EEUL,0x4E048354UL,0x3903B3C2UL,0xA7672661UL,0xD06016F7UL,0x4969474DUL,
		0x3E6E77DBUL,0xAED16A4AUL,0xD9D65ADCUL,0x40DF0B66UL,0x37D83BF0UL,0xA9BCAE53UL,0xDEBB9EC5UL,
		0x47B2CF7FUL,0x30B5FFE9UL,0xBDBDF21CUL,0xCABAC28AUL,0x53B39330UL,0x24B4A3A6UL,0xBAD03605UL,
		0xCDD70693UL,0x54DE5729UL,0x23D967BFUL,0xB3667A2EUL,0xC4614AB8UL,0x5D681B02UL,0x2A6F2B94UL,
		0xB40BBE37UL,0xC30C8EA1UL,0x5A05DF1BUL,0x2D02EF8DUL
	};

	for (i=0; i<count; i++) {
		crc = (crc >> 8) ^ crc_table[(crc ^ buf[i]) & 0xFFUL];
	}
	return(crc ^ 0xFFFFFFFFUL);
}
#endif

/* http://www.ccsinfo.com/forum/viewtopic.php?t=24977 */
static unsigned short crc16(const unsigned char *buf, unsigned long count)
{
	unsigned short crc = 0;
	int i;

	while(count--) {
		crc = crc ^ (*buf++ << 8);

		for (i=0; i<8; i++) {
			if (crc & 0x8000) {
				crc = (crc << 1) ^ 0x1021;
			} else {
				crc = (crc << 1);
			}
		}
	}
	return crc;
}

static const char *u32_to_str(unsigned long val)
{
	/* Maximum number of decimal digits in u32 is 10 */
	static char num_str[11];
	int  pos = 10;
	num_str[10] = 0;

	if (val == 0) {
		/* If already zero then just return zero */
		return "0";
	}

	while ((val != 0) && (pos > 0)) {
		num_str[--pos] = (val % 10) + '0';
		val /= 10;
	}

	return &num_str[pos];
}

static unsigned long str_to_u32(char* str)
{
	const char *s = str;
	unsigned long acc;
	int c;

	/* strip leading spaces if any */
	do {
		c = *s++;
	} while (c == ' ');

	for (acc = 0; (c >= '0') && (c <= '9'); c = *s++) {
		c -= '0';
		acc *= 10;
		acc += c;
	}
	return acc;
}

/* Returns 0 on success, 1 on corrupt packet, -1 on error (timeout): */
static int receive_packet(char *data, int *length)
{
	int i, c;
	unsigned int packet_size;

	*length = 0;

	c = _getchar(PACKET_TIMEOUT);
	if (c < 0) {
		return -1;
	}

	switch(c) {
	case SOH:
		packet_size = PACKET_SIZE;
		break;
	case STX:
		packet_size = PACKET_1K_SIZE;
		break;
	case EOT:
		return 0;
	case CAN:
		c = _getchar(PACKET_TIMEOUT);
		if (c == CAN) {
			*length = -1;
			return 0;
		}
	default:
		/* This case could be the result of corruption on the first octet
		* of the packet, but it's more likely that it's the user banging
		* on the terminal trying to abort a transfer. Technically, the
		* former case deserves a NAK, but for now we'll just treat this
		* as an abort case.
		*/
        printf("abort\n");
		*length = -1;
		return 0;
	}

	*data = (char)c;

	for(i = 1; i < (packet_size + PACKET_OVERHEAD); ++i) {
		c = _getchar(PACKET_TIMEOUT);
		if (c < 0) {
			return -1;
		}
		data[i] = (char)c;
	}

	/* Just a sanity check on the sequence number/complement value.
	 * Caller should check for in-order arrival.
	 */
    if ((unsigned char)data[PACKET_SEQNO_INDEX] != (((unsigned char)data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff)) {
        return 1;
	}

    if (crc16((const unsigned char*) (data + PACKET_HEADER), packet_size + PACKET_TRAILER) != 0) {
		return 1;
	}
	*length = packet_size;

	return 0;
}

/* Returns the length of the file received, or 0 on error: */
unsigned long ymodem_receive(const char* path)
{
	unsigned char packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
	int packet_length, i, file_done, session_done, crc_nak/*, crc_tries*/;
	unsigned int packets_received, errors, first_try = 1;
	char file_size[FILE_SIZE_LENGTH], *file_ptr, *file_name;
    char file_path[FILE_PATH_LENGTH + FILE_NAME_LENGTH];
	unsigned long size = 0;
    int fhandle = -1;

    file_path[0] = 0;
    if (path && *path) {
        strcat(file_path, path);
    }
    if (file_path[0]) {
        size_t l = strlen(file_path);
        if (file_path[l-1] != '/' && file_path[l-1] != '\\') {
            strcat(file_path, "\\");
        }
    }
    file_name = &file_path[strlen(file_path)];
	file_name[0] = 0;

    printf("Target folder: %s\n", file_path[0] ? file_path : ".");
    printf("Awaiting connection (Press ESC to cancel)\n");

    for (session_done = 0, errors = 0; ; ) {
		/*crc_tries =*/ crc_nak = 1;
		if (!first_try) {
			_putchar(CRC);
		}
		first_try = 0;
		for (packets_received = 0, file_done = 0; ; ) {
            /* ESC to abort */
            if (Cconis() < 0) {
                int c = Crawcin() & 0xff;
                if (c == 27) {
                    _putchar(CAN);
                    _putchar(CAN);
                    _sleep(1);
                    if (fhandle > 0) {
                        Fclose(fhandle);
                    }
                    printf("abort\n");
                    return 0;
                }
            }

			switch (receive_packet((char*)packet_data, &packet_length)) {

			case 0:
				errors = 0;
				switch (packet_length) {
					case -1:  /* abort */
						_putchar(ACK);
						return 0;
					case 0:   /* end of transmission */
						_putchar(ACK);
						/* Should add some sort of sanity check on the number of
						 * packets received and the advertised file length.
						 */
						file_done = 1;
						break;
					default:  /* normal packet */
					if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff)) {
						_putchar(NAK);
					} else {
						if (packets_received == 0) {
							/* The spec suggests that the whole data section should
							 * be zeroed, but I don't think all senders do this. If
							 * we have a NULL filename and the first few digits of
							 * the file length are zero, we'll call it empty.
							 */
							for (i = PACKET_HEADER; i < PACKET_HEADER + 4; i++) {
								if (packet_data[i] != 0) {
									break;
								}
							}
							if (i < PACKET_HEADER + 4) {  /* filename packet has data */
								for (file_ptr = (char*) (packet_data + PACKET_HEADER), i = 0; *file_ptr && i < FILE_NAME_LENGTH; ) {
									file_name[i++] = *file_ptr++;
								}
								file_name[i++] = '\0';
								for (++file_ptr, i = 0; *file_ptr != ' ' && i < FILE_SIZE_LENGTH; ) {
									file_size[i++] = *file_ptr++;
								}
								file_size[i++] = '\0';
								size = str_to_u32(file_size);

                                if (fhandle > 0) {
                                    printf("done\n");
                                    Fclose(fhandle);
                                }
                                printf("%s (%ld bytes)", file_name, size);
                                fhandle = (int)Fcreate(file_path, 0);
                                if (fhandle <= 0) {
									_putchar(CAN);
									_putchar(CAN);
									_sleep(1);
                                    printf("file create failed\n");
                                    return 0;
                                }

                                _putchar(ACK);
								_putchar(crc_nak ? CRC : NAK);
								crc_nak = 0;
							} else {  /* filename packet is empty; end session */
								_putchar(ACK);
								file_done = 1;
								session_done = 1;
                                if (fhandle > 0) {
                                    Fclose(fhandle);
                                }
                                printf("done\n");
								break;
							}
						} else {
                            if (fhandle > 0) {
                                printf(".");
                                Fwrite(fhandle, packet_length, &packet_data[PACKET_HEADER]);
                            }
							_putchar(ACK);
						}
						++packets_received;
					}  /* sequence number ok */
				}
				break;

			default:
				if (packets_received != 0) {
					if (++errors >= MAX_ERRORS) {
						_putchar(CAN);
						_putchar(CAN);
						_sleep(1);
                        if (fhandle > 0) {
                            Fclose(fhandle);
                        }
						printf("too many errors - aborted.\n");
						return 0;
					}
				}
				_putchar(CRC);
			}
			if (file_done) {
				break;
			}
		}  /* receive packets */

		if (session_done)
			break;

	}  /* receive files */

#if 0    
	printf("\n");
	if (size > 0) {
		printf("read:%s\n", file_name);
#ifdef WITH_CRC32
		printf("crc32:0x%08x, len:0x%08x\n", crc32(buf, size), size);
#else
		printf("len:0x%08x\n", size);
#endif
	}
#endif
	return size;
}


#if 0


static void send_packet(unsigned char *data, int block_no)
{
	int count, crc, packet_size;

	/* We use a short packet for block 0 - all others are 1K */
	if (block_no == 0) {
		packet_size = PACKET_SIZE;
	} else {
		packet_size = PACKET_1K_SIZE;
	}
	crc = crc16(data, packet_size);
	/* 128 byte packets use SOH, 1K use STX */
	_putchar((block_no==0)?SOH:STX);
	_putchar(block_no & 0xFF);
	_putchar(~block_no & 0xFF);

	for (count=0; count<packet_size; count++) {
		_putchar(data[count]);
	}
	_putchar((crc >> 8) & 0xFF);
	_putchar(crc & 0xFF);
}

/* Send block 0 (the filename block). filename might be truncated to fit. */
static void send_packet0(char* filename, unsigned long size)
{
	unsigned long count = 0;
	unsigned char block[PACKET_SIZE];
	const char* num;

	if (filename) {
		while (*filename && (count < PACKET_SIZE-FILE_SIZE_LENGTH-2)) {
			block[count++] = *filename++;
		}
		block[count++] = 0;

		num = u32_to_str(size);
		while(*num) {
			block[count++] = *num++;
		}
	}

	while (count < PACKET_SIZE) {
		block[count++] = 0;
	}

	send_packet(block, 0);
}


static void send_data_packets(unsigned char* data, unsigned long size)
{
	int blockno = 1;
	unsigned long send_size;
	int ch;

	while (size > 0) {
		if (size > PACKET_1K_SIZE) {
			send_size = PACKET_1K_SIZE;
		} else {
			send_size = size;
		}

		send_packet(data, blockno);
		ch = _getchar(PACKET_TIMEOUT);
		if (ch == ACK) {
			blockno++;
			data += send_size;
			size -= send_size;
		} else {
			if((ch == CAN) || (ch == -1)) {
				return;
			}
		}
	}

	do {
		_putchar(EOT);
		ch = _getchar(PACKET_TIMEOUT);
	} while((ch != ACK) && (ch != -1));

	/* Send last data packet */
	if (ch == ACK) {
		ch = _getchar(PACKET_TIMEOUT);
		if (ch == CRC) {
			do {
				send_packet0(0, 0);
				ch = _getchar(PACKET_TIMEOUT);
			} while((ch != ACK) && (ch != -1));
		}
	}
}

unsigned long ymodem_send(unsigned char* buf, unsigned long size, char* filename)
{
	int ch, crc_nak = 1;

	printf("Ymodem send:\n");

	/* Flush the RX FIFO, after a cool off delay */
	_sleep(1);
	while (serial_read() >= 0);

	/* Not in the specs, just for balance */
	do {
		_putchar(CRC);
		ch = _getchar(1);
	} while (ch < 0);

	if (ch == CRC) {
		do {
			send_packet0(filename, size);
			/* When the receiving program receives this block and successfully
			 * opened the output file, it shall acknowledge this block with an ACK
			 * character and then proceed with a normal XMODEM file transfer
			 * beginning with a "C" or NAK tranmsitted by the receiver.
			 */
			ch = _getchar(PACKET_TIMEOUT);

			if (ch == ACK) {
				ch = _getchar(PACKET_TIMEOUT);
				if (ch == CRC) {
					send_data_packets(buf, size);
					printf("\nsent:%s\n", filename);
#ifdef WITH_CRC32
					printf("crc32:0x%08x, len:0x%08x\n", crc32(buf, size), size);
#else
					printf("len:0x%08x\n", size);
#endif
					return size;
				}
			} else if ((ch == CRC) && (crc_nak)) {
				crc_nak = 0;
				continue;
			} else if ((ch != NAK) || (crc_nak)) {
				break;
			}
		} while(1);
	}
	_putchar(CAN);
	_putchar(CAN);
	_sleep(1);
	printf("\naborted.\n");
	return 0;
}

#endif
