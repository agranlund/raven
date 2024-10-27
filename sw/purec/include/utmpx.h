#ifndef __UTMPX_H__
#define __UTMPX_H__

#ifndef _FEATURES_H
# include <features.h>
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <utmp.h>

#define	UTMPX_FILE	"c:\\tmp\\utmpx"
#define	WTMPX_FILE	"c:\\tmp\\wtmpx"

#define	ut_name	ut_user
#define ut_xtime ut_tv.tv_sec

struct utmpx
  {
	char	ut_user[32];		/* user login name */
	char	ut_id[4]; 		/* inittab id */
	char	ut_line[32];		/* device name (console, lnxx) */
	pid_t	ut_pid;			/* process id */
	short	ut_type; 		/* type of entry */
	struct exit_status ut_exit;     /* process termination/exit status */
	struct timeval ut_tv;		/* time entry was made */
	long	ut_session;		/* session ID, used for windowing */
	long	pad[5];			/* reserved for future use */
	short	ut_syslen;		/* significant length of ut_host */
					/*   including terminating null */
	char	ut_host[257];		/* remote host name */
	char    pad2[2];
  } ;

/*	Definitions for ut_type						*/

#define	EMPTY		0
#define	RUN_LVL		1
#define	BOOT_TIME	2
#define	OLD_TIME	3
#define	NEW_TIME	4
#define	INIT_PROCESS	5	/* Process spawned by "init" */
#define	LOGIN_PROCESS	6	/* A "getty" process waiting for login */
#define	USER_PROCESS	7	/* A user process */
#define	DEAD_PROCESS	8
#define	ACCOUNTING	9

#define	UTMAXTYPE	ACCOUNTING	/* Largest legal value of ut_type */

/*	Special strings or formats used in the "ut_line" field when	*/
/*	accounting for something other than a process.			*/
/*	No string for the ut_line field can be more than 11 chars +	*/
/*	a NULL in length.						*/

#define	RUNLVL_MSG	"run-level %c"
#define	BOOT_MSG	"system boot"
#define	OTIME_MSG	"old time"
#define	NTIME_MSG	"new time"
#define MOD_WIN		10

extern void endutxent(void);
extern struct utmpx *getutxent(void);
extern struct utmpx *getutxid(const struct utmpx *);
extern struct utmpx *getutxline(const struct utmpx *);
extern struct utmpx *pututxline(const struct utmpx *); 
extern void setutxent(void);
extern int utmpxname(const char *);
extern struct utmpx *makeutx(const struct utmpx *);
extern struct utmpx *modutx(const struct utmpx *);
extern void getutmp(const struct utmpx *, struct utmp *);
extern void getutmpx(const struct utmp *, struct utmpx *);
extern void updwtmp(const char *, const struct utmp *);
extern void updwtmpx(const char *, const struct utmpx *);

#endif /* __UTMPX_H__ */
