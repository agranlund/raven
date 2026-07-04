/*

Project:    asm56k
Author:     M.Buras (sqward)

Early pipeline restriction checks

*/

#ifndef _PIPELINERESTRICTION_H_
#define _PIPELINERESTRICTION_H_


void	PipeLineReset	        (void);
void    PipeLineNewInst     	(void);
void	PipeLineNewAguReg   	(void);
void	PipeLineNewSrcAguReg	(uint reg);
void	PipeLineNewDstAguReg	(uint reg);
void	PipeLineNewSrcEA	    (bcode *reg);
void	PipeLineNewDstEA	    (bcode *reg);

#endif /* _PIPELINERESTRICTION_H_ */
