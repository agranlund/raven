#include "gemdos.h"

int vt52_out(char c, file_t* dev)
{
    int retval = 0;

    switch(dev->term.state)
    {
        case not_term:
        {
            retval = fputc(c, dev->fd);
        }
            break;

        case normal:
        {
            switch(c)
            {
                case 0x0a: /* LF */
                case 0x0b: /* LF */
                case 0x0c: /* LF */
                {
                    retval = fputc(c, dev->fd);
                }
                    break;

                case 0x0d: /* CR */
                {
                    /* simulate CR */
                //    fprintf(dev->fd, "\033[999D");
                }
                    break;

                case 0x1b: /* ESC */
                {
                    dev->term.state = escaped;
                }
                    break;

                default:
                {
                    retval = fputc(c, dev->fd);
                }
                    break;
            }
        }   
            break;

        case escaped:
        {
            dev->term.state = normal; 

            switch(c)
            {
                case 'A': fprintf(dev->fd, "\033[A"); break;           /* Cursor up    */
                case 'B': fprintf(dev->fd, "\033[B"); break;           /* Cursor down  */
                case 'C': fprintf(dev->fd, "\033[C"); break;           /* Cursor right */
                case 'D': fprintf(dev->fd, "\033[D"); break;           /* Cursor left  */
                case 'E': fprintf(dev->fd, "\033[H\033[2J"); break;    /* Clear and home */

                case 'H': fprintf(dev->fd, "\033[H"); break;           /* Cursor home */
                case 'I': fprintf(dev->fd, "\033[A"); break;           /* Reverse LF */
                case 'J': fprintf(dev->fd, "\033[J"); break;           /* Erase to EOS */
                case 'K': fprintf(dev->fd, "\033[K"); break;           /* Erase to EOL */
                case 'L': break; /* Insert line */
                case 'M': break; /* Delete line */
        
                case 'd': fprintf(dev->fd, "\033[1J");   break; /* Erase from home */
                case 'e': fprintf(dev->fd, "\033[?25h"); break; /* Cursor ON */
                case 'f': fprintf(dev->fd, "\033[?25l"); break; /* Cursor OFF */
                case 'j': fprintf(dev->fd, "\033[s");    break; /* Save cursor pos */
                case 'k': fprintf(dev->fd, "\033[u");    break; /* Restore cursor pos */
                case 'l': fprintf(dev->fd, "\033[2K\033[1G"); break; /* Erase line */
                case 'o': fprintf(dev->fd, "\033[1K");   break; /* Erase from bol */
                case 'p': fprintf(dev->fd, "\033[7m");   break; /* Reverse video on */
                case 'q': fprintf(dev->fd, "\033[27m");  break; /* Reverse video off */
                case 'v': break; /* Line wrap on */
                case 'w': break; /* Line wrap off */

                case 'b':
                case 'c':
                case 'Y':
                {
                    dev->term.state = extended;
                }
                    break;

                default:
                {
                }
                    break;
            }

            dev->term.tmp = c;
        }
            break;

        case extended:
        {
            dev->term.state = normal; 

            switch(dev->term.tmp)
            {
                case 'b': fprintf(dev->fd, "\033[%dm", 30 + (c & 7)); break; /* Set FG */
                case 'c': fprintf(dev->fd, "\033[%dm", 40 + (c & 7)); break; /* Set BG */
                case 'Y': dev->term.state = set_xy; break;
                default:
                {
                }
                    break;
            }

            dev->term.tmp = c;
        }
            break;

        case set_xy:
        {
            dev->term.state = normal;
        }
            break;
    }

    return retval;
}
