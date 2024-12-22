
#include <portab.h>
#include <gembind.h>


	WORD
form_keybd(form, obj, nxt_obj, thechar, pnxt_obj, pchar)
	LONG 		form;
	WORD		obj, thechar, *pnxt_obj, *pchar;
{
	addr_in[0] = form;
	int_in[0] = obj;
	int_in[1] = nxt_obj;
	int_in[2] = thechar;
	crys_if(FORM_KEYBD);
	*pnxt_obj = int_out[1];
	*pchar = int_out[2];
	return int_out[0];
}
