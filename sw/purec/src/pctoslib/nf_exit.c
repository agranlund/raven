#include <stdarg.h>
#include <stdio.h>
#include <mint/arch/nf_ops.h>
#include <mint/mintbind.h>
#include <osbind.h>
#include <stdint.h>
#include <errno.h>


long nf_exit(int exitcode)
{
	struct nf_ops *ops;
	long res = 0;

	if ((ops = nf_init()) != NULL)
	{
		long exit_id = NF_GET_ID(ops, NF_ID_EXIT);
		
		if (exit_id)
        	res = ops->call(exit_id | 0, (long)exitcode);
	}
	return res;
}
