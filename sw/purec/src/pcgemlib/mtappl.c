#include "gem_aesP.h"

_WORD __magix;

#undef appl_find
#undef wind_get

static _WORD has_appl_getinfo(void)
{
    static _WORD has_agi = -1; /* do the check only once */
    _WORD out1 = 0, out2, out3, out4;
    
    /* check for appl_getinfo() being present */
    if (has_agi < 0)
    {
        has_agi = 0;
        /* AES 4.0? */
        if (gl_ap_version >= 0x400)
             has_agi = 1;
        else
        /* Mag!X 2.0? */
        if (__magix >= 0x200)
            has_agi = 2;
        else
        if (appl_find( "?AGI\0\0\0\0") >= 0)
            has_agi = 3;
        else
        /* WiNX >= 2.2 ? */
        if (wind_get(0, WF_WINX, &out1, &out2, &out3, &out4) == WF_WINX &&
        	(out1 & 0xfff) >= 0x220)
            has_agi = 4;
    }
    return has_agi;
}


#undef appl_xgetinfo
#undef appl_getinfo
_WORD appl_xgetinfo(_WORD type, _WORD *out1, _WORD *out2, _WORD *out3, _WORD *out4)
{
    _WORD ret;

    /* no appl_getinfo? return error code */
	if (!has_appl_getinfo() || (ret = appl_getinfo(type, out1, out2, out3, out4)) == 0)
	{
	    if (out1 != NULL)
	    	*out1 = 0;
	    if (out2 != NULL)
		    *out2 = 0;
		if (out3 != NULL)
		    *out3 = 0;
		if (out4 != NULL)
		    *out4 = 0;
		return 0;
	}
	return ret;
}


_WORD mt_appl_getinfo(_WORD type, _WORD *out1, _WORD *out2, _WORD *out3, _WORD *out4, _WORD *global_aes)
{
	_WORD ret;
	AES_PARAMS(130,1,5,0,0);

	aes_intin[0] = type;
    /* no appl_getinfo? return error code */
	if (!has_appl_getinfo() || (ret = AES_TRAP(aes_params)) == 0)
	{
	    if (out1 != NULL)
	    	*out1 = 0;
	    if (out2 != NULL)
		    *out2 = 0;
		if (out3 != NULL)
		    *out3 = 0;
		if (out4 != NULL)
		    *out4 = 0;
		return 0;
	}
	return ret;
}


_WORD mt_appl_getinfo_str(_WORD type, char *out1, char *out2, char *out3, char *out4, _WORD *global_aes)
{
	AES_PARAMS(130,1,1,4,0);

	if (!has_appl_getinfo())
		return 0;

	aes_intin[0] = type;
	
	aes_addrin[0] = out1;
	aes_addrin[1] = out2;
	aes_addrin[2] = out3;
	aes_addrin[3] = out4;
	
	return AES_TRAP(aes_params);
}


_WORD mt_appl_init(_WORD *global_aes)
{
	AES_PARAMS(10,0,1,0,0);
	return AES_TRAP(aes_params);
}


_WORD mt_appl_exit(_WORD *global_aes)
{
	AES_PARAMS(19,0,1,0,0);
	return AES_TRAP(aes_params);
}
