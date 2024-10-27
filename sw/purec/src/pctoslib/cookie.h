/*****************************************************************************
 *	TOS/COOKIE.H
 *****************************************************************************/

#define COOKIE_SND      0x5f534e44l /* '_SND' */
#define COOKIE_CPU      0x5f435055l /* '_CPU' */
#define COOKIE_FPU      0x5f465055l /* '_FPU' */
#define COOKIE_FLK      0x5f464c4bl /* '_FLK' */

#define COOKIE_MINT     0x4d694e54l /* 'MiNT' */
#define COOKIE_FSEL     0x4653454cl /* 'FSEL' */
#define COOKIE_RSVF     0x52535646l /* 'RSVF' */
#define COOKIE_FVDI     0x66564449l /* 'fVDI' */
#define COOKIE_ICFS     0x49434653l /* 'ICFS' */
#define COOKIE_MAGX     0x4d616758l /* 'MagX' */
#define COOKIE_GNVA     0x476e7661l /* 'Gnva' */
#define COOKIE_XSSI     0x58535349l /* 'XSSI' */
#define COOKIE_XFSL     0x7846534Cl /* 'xFSL' */
#define COOKIE_UFSL     0x5546534Cl /* 'UFSL' */
#define COOKIE_MagicMac 0x4d674d63l /* 'MgMc', Magic Mac */

#define C___NF 0x5F5F4E46L     /* Native features proposal */
#define C__AKP 0x5F414B50L     /* Keyboard/Language Configuration */

/*
 * newer MiNTLib versions might have this
 * already, rename ours to avoid link errors
 */

#define Cookie_JarInstalled CK_JarInstalled
#define Cookie_UsedEntries CK_UsedEntries
#define Cookie_JarSize CK_jarSize
#define Cookie_ResizeJar CK_ResizeJar
#define Cookie_ReadJar CK_ReadJar
#define Cookie_WriteJar CK_WriteJar
#define Cookie_SetOptions CK_SetOptions

int Cookie_JarInstalled(void);
int Cookie_UsedEntries(void);
int Cookie_JarSize(void);
int Cookie_ResizeJar(int newsize);
int Cookie_ReadJar(unsigned long id, long *value);
int Cookie_WriteJar(unsigned long id, long value);
void Cookie_SetOptions(int increment, unsigned long xbra_id);

short __has_no_ssystem(void);

#ifdef __GNUC__
extern long __ck_zero; /* use a variable rather than a constant to avoid a mov3q */
extern long __ck_one;
extern long __ck_minusone;
#else
#define __ck_zero 0
#define __ck_one 1
#define __ck_minusone (-1l)
#endif
