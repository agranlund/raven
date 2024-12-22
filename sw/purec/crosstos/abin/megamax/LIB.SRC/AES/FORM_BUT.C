
#include <portab.h>
#include <gembind.h>


	WORD
form_button(form, obj, clks, pnxt_obj)
	LONG 		form;
	WORD		obj, clks, *pnxt_obj;
{
	addr_in[0] = form;
	int_in[0] = obj;
	int_in[1] = clks;
	crys_if(FORM_BUTTON);
	*pnxt_obj = int_out[1];
	return int_out[0];
}
