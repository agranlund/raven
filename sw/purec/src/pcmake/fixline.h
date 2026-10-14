#ifndef _FIXLINE_H_
#define _FIXLINE_H_

/*
 * PureC shows the wrong line number for warnings and errors
 * if multi-line block comments are used.
 * For a comment spanning N lines the drift is N-1, and drifts from
 * multiple comments keeps adding up.
 * Comments inside included headers do not cause this issue.
 *
 * fixline() returns the corrected line number
 */

 int fixline(const char *src_file, int line);

#endif /* _FIXLINE_H_ */
