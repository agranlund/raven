#ifndef __VARARGS_H__
#define __VARARGS_H__

#ifdef __STDARG_H__

#undef va_start
#undef va_arg
#undef va_end

#else

typedef void *va_list;

#endif


#define va_dcl va_list va_alist
#define va_start(list) ((list) = (va_list)&va_alist)
#define va_end(list)
#define va_arg(ap, type)    \
    ((sizeof(type) == 1) ? \
    (*(type *)(((char *)ap += 2) - 1)) : \
    (*((type *)(ap))++))

#endif /* __VARARGS_H__ */

