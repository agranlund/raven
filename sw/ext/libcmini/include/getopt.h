#ifndef _GETOPT_H_
#define _GETOPT_H_

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus


int getopt(int argc, char * const argv[], const char *optstring);

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _GETOPT_H_ */
