#ifndef COLOR_H
#define COLOR_H

/*   Color		      Code    */
#define COLOR_RESET	"\033[0;m"

/* 	    Regular colors	    */
#define RBLACK 		"\033[0;30m"
#define RRED 		  "\033[0;31m"
#define RGREEN 		"\033[0;32m"
#define RYELLOW 	"\033[0;33m"
#define RBLUE 		"\033[0;34m"
#define RMAGENTA 	"\033[0;35m"
#define RCYAN 		"\033[0;36m"
#define RWHITE 		"\033[0;37m"

/*	Bright and bold colors  */
#define BBLACK		"\033[1;30m"
#define BRED		  "\033[1;31m"
#define BGREEN		"\033[1;32m"
#define BYELLOW		"\033[1;33m"
#define BBLUE		  "\033[1;34m"
#define BMAGENTA	"\033[1;35m"
#define BCYAN		  "\033[1;36m"
#define BWHITE		"\033[1;37m"

/* 
 * Macro that colors `s` with `c` and resets it
 * @param s: char *
 * @param c: color macro
 */
#define COLOR(s, c) c s COLOR_RESET

#endif
