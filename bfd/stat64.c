#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define _FILE_OFFSET_BITS 64

#define __asm__(x)
#include <sys/stat.h>
#undef __asm__

#include "libcwrap.h"

#if defined (__cplusplus)
extern "C" {
#endif

#ifdef __LINUX_GLIBC_WRAP_H

#if __GLIBC_PREREQ(2, 33)

#include <stdarg.h>
#include <fcntl.h>

/*
 * avoid references to stat/lstat/fstat, which are only available in glibc >= 2.33
 */

extern int __fxstat(int __ver, int __fildes, struct stat *__stat_buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3)));
extern int __xstat(int __ver, const char *__filename,
      struct stat *__stat_buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
extern int __lxstat(int __ver, const char *__filename,
       struct stat *__stat_buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
extern int __fxstatat (int __ver, int __fildes, const char *__filename,
         struct stat *__stat_buf, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4)));
#ifdef __USE_LARGEFILE64
extern int __xstat64(int __ver, const char *__filename,
      struct stat64 *__stat_buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
extern int __lxstat64(int __ver, const char *__filename,
       struct stat64 *__stat_buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
extern int __fxstat64(int __ver, int __fildes, struct stat64 *__stat_buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3)));
#endif

__attribute__((__nothrow__))
int stat(const char *__path, struct stat *__statbuf)
{
	return __xstat(1, __path, __statbuf);
}

__attribute__((__nothrow__)) 
int lstat (const char *__path, struct stat *__statbuf)
{
	return __lxstat(1, __path, __statbuf);
}

__attribute__((__nothrow__)) 
int fstat(int __fd, struct stat *__statbuf)
{
	return __fxstat(1, __fd, __statbuf);
}

__attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)))
int stat64(const char *__restrict __path, struct stat64 *__restrict __statbuf)
{
	return __xstat64(1, __path, __statbuf);
}

__attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)))
int lstat64 (const char *__restrict __path, struct stat64 *__restrict __statbuf)
{
	return __lxstat64(1, __path, __statbuf);
}

__attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)))
int fstat64(int __fd, struct stat64 *__statbuf)
{
	return __fxstat64(1, __fd, __statbuf);
}

__attribute__((__nothrow__)) 
int fstatat(int __fd, const char *__restrict __filename, struct stat *__restrict __statbuf, int __flag)
{
	return __fxstatat(1, __fd, __filename, __statbuf, __flag);
}

#undef fcntl
int fcntl64(int fd, int cmd, ...)
{
  va_list ap;
  void *arg;

  va_start (ap, cmd);
  arg = va_arg (ap, void *);
  va_end (ap);

  return fcntl(fd, cmd, arg);
}

#endif /* __GLIBC_PREREQ */

#endif /* __linux__ */

#if defined (__cplusplus)
}
#endif
