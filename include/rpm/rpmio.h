#ifndef	H_RPMIO
#define	H_RPMIO

/** \ingroup rpmio
 * \file rpmio.h
 *
 * RPM I/O API (Fd_t is RPM equivalent to libc's FILE)
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <rpm/rpmtypes.h>
#include <rpm/rpmsw.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \ingroup rpmio
 * \name RPMIO Interface.
 */

/** \ingroup rpmio
 * strerror(3) clone.
 */
const char * Fstrerror(FD_t fd);

/** \ingroup rpmio
 * Like fread(3) but with read(3)-style return values.
 */
ssize_t Fread(void * buf, size_t size, size_t nmemb, FD_t fd);

/** \ingroup rpmio
 * Like fwrite(3) but with write(3)-style return values.
 */
ssize_t Fwrite(const void * buf, size_t size, size_t nmemb, FD_t fd);

/** \ingroup rpmio
 * fseek(3) clone.
 */
int Fseek(FD_t fd, off_t offset, int whence);

/** \ingroup rpmio
 * ftell(3) clone.
 */
off_t Ftell(FD_t fd);

/** \ingroup rpmio
 * fclose(3) clone.
 */
int Fclose( FD_t fd);

/** \ingroup rpmio
 * fdopen(3) clone.
 *
 * See Fopen() for details.
 */
FD_t	Fdopen(FD_t ofd, const char * fmode);

/** \ingroup rpmio
 * fopen(3) clone with compression support.
 *
 * The `fmode` parameter is based on that of `fopen(3)`, but may also include a
 * compression method (`type` and `flags`) to use when opening the stream.
 * See `rpm-payloadflags`(7) manual for details.
 */
FD_t	Fopen(const char * path,
			const char * fmode);


/** \ingroup rpmio
 * fflush(3) clone.
 */
int Fflush(FD_t fd);

/** \ingroup rpmio
 * ferror(3) clone.
 */
int Ferror(FD_t fd);

/** \ingroup rpmio
 * fileno(3) clone.
 */
int Fileno(FD_t fd);

/** \ingroup rpmio
 * fcntl(2) clone.
 */
int Fcntl(FD_t fd, int op, void *lip);

/** \ingroup rpmio
 * Get informative description (eg file name) from fd for diagnostic output.
 */
const char * Fdescr(FD_t fd);

/** \ingroup rpmio
 * \name RPMIO Utilities.
 */

/** \ingroup rpmio
 * Return the size of the backing file of the descriptor.
 */
off_t	fdSize(FD_t fd);

/** \ingroup rpmio
 * dup(2) clone.
 */
FD_t fdDup(int fdno);

/** \ingroup rpmio
 * Reference a file descriptor.
 */
FD_t fdLink(FD_t fd);

/** \ingroup rpmio
 * Dereference a file descriptor. This does NOT close the file.
 */
FD_t fdFree(FD_t fd);

/**
 * Copy file descriptor into another.
 */
off_t ufdCopy(FD_t sfd, FD_t tfd);

/** \ingroup rpmio
 * Identify per-desciptor I/O operation statistics.
 */
typedef enum fdOpX_e {
    FDSTAT_READ		= 0,	/*!< Read statistics index. */
    FDSTAT_WRITE	= 1,	/*!< Write statistics index. */
    FDSTAT_SEEK		= 2,	/*!< Seek statistics index. */
    FDSTAT_CLOSE	= 3,	/*!< Close statistics index */
    FDSTAT_DIGEST	= 4,	/*!< Digest statistics index. */
    FDSTAT_MAX		= 5
} fdOpX;

/** \ingroup rpmio
 * File operation statistics.
 */
rpmop fdOp(FD_t fd, fdOpX opx);

#ifdef __cplusplus
}
#endif

#endif	/* H_RPMIO */
