
#ifndef _IFLINK_H
#define _IFLINK_H 1

#ifndef _FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

extern int	if_link		(char*, char*);
extern int	if_getlnkflags	(char*, short*);
extern int	if_setlnkflags	(char*, short);
extern int	if_getifflags	(char*, short*);
extern int	if_setifflags	(char*, short);

__END_DECLS

#endif /* mint/iflink.h */
