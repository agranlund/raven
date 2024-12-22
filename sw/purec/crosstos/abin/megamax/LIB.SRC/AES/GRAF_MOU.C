
#include <portab.h>
#include <gembind.h>

	WORD
graf_mouse(m_number, m_addr)
	WORD		m_number;
	LONG		m_addr;
{
	GR_MNUMBER = m_number;
	GR_MADDR = m_addr;
	return( crys_if( GRAF_MOUSE ) );
}

