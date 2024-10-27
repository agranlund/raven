#include <stdarg.h>
#include <stdio.h>
#include <mint/arch/nf_ops.h>
#include <mint/mintbind.h>
#include <osbind.h>
#include <stdint.h>
#include <errno.h>

/*** ---------------------------------------------------------------------- ***/

int nf_debugvprintf(const char *format, va_list args)
{
	struct nf_ops *ops;
	long nfid_stderr;
	int ret;
	
	if ((ops = nf_init()) == NULL ||
		(nfid_stderr = NF_GET_ID(ops, NF_ID_STDERR)) == 0)
	{
#ifdef ENOSYS
		errno = ENOSYS;
#else
		errno = EIO;
#endif
		return -1;
	}	
	{
#if defined(_PUREC_SOURCE) || !defined(HAVE_VASPRINTF)
		static char buf[2048];
		
#ifdef HAVE_VSNPRINTF
		ret = vsnprintf(buf, sizeof(buf), format, args);
#else
		ret = vsprintf(buf, format, args);
#endif
		ret = (int)ops->call(nfid_stderr | 0, (uint32_t)virt_to_phys(buf));
#else
		char *buf = NULL;
		
		ret = vasprintf(&buf, format, args);
		if (buf)
		{
			ret = (int)ops->call(nfid_stderr | 0, (uint32_t)virt_to_phys(buf));
			free(buf);
		}
#endif
	}

	return ret;
}

/*** ---------------------------------------------------------------------- ***/

int nf_debugprintf(const char *format, ...)
{
	int ret;
	va_list args;
	
	va_start(args, format);
	ret = nf_debugvprintf(format, args);
	va_end(args);
	return ret;
}
