#ifndef _STDARG_H
#ifndef _ANSI_STDARG_H_
#ifndef __need___va_list
#define _STDARG_H
#define _ANSI_STDARG_H_
#endif /* not __need___va_list */
#undef __need___va_list

/* Define __va_list.  */

#ifndef __VA_LIST
#define __VA_LIST
typedef char *__va_list;
#define __va_list __va_list
#endif


/* Define the standard macros for the user,
   if this invocation was from the user program.  */
#ifdef _STDARG_H

#define va_start(ap, parmN) ((ap) = (va_list)...)
#define va_arg(ap, type)    \
    ((sizeof(type) == 1) ? \
    (*(type *)((ap += 2) - 1)) : \
    (*((type *)(ap))++))
#define va_end(ap)	(void)0

#define __va_copy(d,s)	(d) = (s)
#if !defined(__STRICT_ANSI__) || __STDC_VERSION__ + 0 >= 199900L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#define va_copy(d,s)	__va_copy(d,s)
#endif

/* Define va_list, if desired, from __va_list. */
/* We deliberately do not define va_list when called from
   stdio.h, because ANSI C says that stdio.h is not supposed to define
   va_list.  stdio.h needs to have access to that data type, 
   but must not use that name.  It should use the name __va_list,
   which is safe because it is reserved for the implementation.  */

#if !defined (_VA_LIST_) || defined (__BSD_NET2__) || defined (____386BSD____) || defined (__bsdi__) || defined (__sequent__) || defined (__FreeBSD__) || defined(WINNT)
/* The macro _VA_LIST_DEFINED is used in Windows NT 3.5  */
#ifndef _VA_LIST_DEFINED
/* The macro _VA_LIST is used in SCO Unix 3.2.  */
#ifndef _VA_LIST
/* The macro _VA_LIST_T_H is used in the Bull dpx2  */
#ifndef _VA_LIST_T_H
/* The macro __va_list__ is used by BeOS.  */
#ifndef __va_list__
typedef __va_list va_list;
#endif /* not __va_list__ */
#endif /* not _VA_LIST_T_H */
#endif /* not _VA_LIST */
#endif /* not _VA_LIST_DEFINED */
#if !(defined (__BSD_NET2__) || defined (____386BSD____) || defined (__bsdi__) || defined (__sequent__) || defined (__FreeBSD__))
#define _VA_LIST_
#endif
#ifndef _VA_LIST
#define _VA_LIST
#endif
#ifndef _VA_LIST_DEFINED
#define _VA_LIST_DEFINED
#endif
#ifndef _VA_LIST_T_H
#define _VA_LIST_T_H
#endif
#ifndef __va_list__
#define __va_list__
#endif

#endif /* not _VA_LIST_, except on certain systems */

#endif /* _STDARG_H */

#endif /* not _ANSI_STDARG_H_ */
#endif /* not _STDARG_H */
