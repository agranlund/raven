#include <stdarg.h>
#include <stdio.h>
#include <mint/arch/nf_ops.h>
#include <mint/mintbind.h>
#include <osbind.h>
#include <stdint.h>
#include <errno.h>


long nf_exec(const char *cmd)
{
	struct nf_ops *nf_ops;
	long res = -32;
	
	if ((nf_ops = nf_init()) != NULL)
	{
		long hostexec_id = NF_GET_ID(nf_ops, NF_ID_HOSTEXEC);
		
		if (hostexec_id)
        	res = nf_ops->call(hostexec_id | 1, cmd);
	}
	return res;
}


long nf_execv(long argc, const char *const *argv)
{
	struct nf_ops *nf_ops;
	long res = -32;
	
	if ((nf_ops = nf_init()) != NULL)
	{
		long hostexec_id = NF_GET_ID(nf_ops, NF_ID_HOSTEXEC);
		
		if (hostexec_id)
        	res = nf_ops->call(hostexec_id | 2, argc, argv);
	}
	return res;
}
