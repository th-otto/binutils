/* obstack.c - subroutines used implicitly by object stack macros
   Copyright (C) 1988 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 1, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */
#include <ansidecl.h>
#include <sysdep.h>
#include "obstack.h"


/* Determine default alignment.  */
struct fooalign {char x; double d;};
#define DEFAULT_ALIGNMENT ((char *)&((struct fooalign *) 0)->d - (char *)0)
/* If malloc were really smart, it would round addresses to DEFAULT_ALIGNMENT.
   But in fact it might be less smart and round addresses to as much as
   DEFAULT_ROUNDING.  So we prepare for it to do that.  */
union fooround {long x; double d;};
#define DEFAULT_ROUNDING (sizeof (union fooround))

/* When we copy a long block of data, this is the unit to do it with.
   On some machines, copying successive ints does not work;
   in such a case, redefine COPYING_UNIT to `long' (if that works)
   or `char' as a last resort.  */
#ifndef COPYING_UNIT
#define COPYING_UNIT int
#endif

/* The non-GNU-C macros copy the obstack into this global variable
   to avoid multiple evaluation.  */

struct obstack *_obstack;

/* Initialize an obstack H for use.  Specify chunk size SIZE (0 means default).
   Objects start on multiples of ALIGNMENT (0 means use default).
   CHUNKFUN is the function to use to allocate chunks,
   and FREEFUN the function to free them.  */

void DEFUN(_obstack_begin,(h, size, alignment, chunkfun, freefun),
	   struct obstack *h AND
	   int size AND
	   int alignment AND
	   PTR (*chunkfun) () AND
	   void (*freefun) ())
{
  register struct _obstack_chunk* chunk; /* points to new chunk */

  if (alignment == 0)
    alignment = DEFAULT_ALIGNMENT;
  if (size == 0)
    /* Default size is what GNU malloc can fit in a 4096-byte block.  */
    {
      /* 12 is sizeof (mhead) and 4 is EXTRA from GNU malloc.
	 Use the values for range checking, because if range checking is off,
	 the extra bytes won't be missed terribly, but if range checking is on
	 and we used a larger request, a whole extra 4096 bytes would be
	 allocated.

	 These number are irrelevant to the new GNU malloc.  I suspect it is
	 less sensitive to the size of the request.  */
      int extra = ((((12 + DEFAULT_ROUNDING - 1) & ~(DEFAULT_ROUNDING - 1))
		    + 4 + DEFAULT_ROUNDING - 1)
		   & ~(DEFAULT_ROUNDING - 1));
      size = 4096 - extra;
    }

  h->chunkfun = chunkfun;
  h->freefun = freefun;
  h->chunk_size = size;
  h->alignment_mask = alignment - 1;

  chunk	= h->chunk = (struct _obstack_chunk *)(*h->chunkfun) (h->chunk_size);
  h->next_free = h->object_base = chunk->contents;
  h->chunk_limit = chunk->limit
   = (char *) chunk + h->chunk_size;
  chunk->prev = 0;
}

/* Allocate a new current chunk for the obstack *H
   on the assumption that LENGTH bytes need to be added
   to the current object, or a new object of length LENGTH allocated.
   Copies any partial object from the end of the old chunk
   to the beginning of the new one.  

   The function must be "int" so it can be used in non-ANSI C
   compilers in a : expression.  */

int
DEFUN(_obstack_newchunk,(h, length),
      struct obstack *h AND
      int length)
{
  register struct _obstack_chunk*	old_chunk = h->chunk;
  register struct _obstack_chunk*	new_chunk;
  register long	new_size;
  register int obj_size = h->next_free - h->object_base;
  register int i;
  int already;

  /* Compute size for new chunk.  */
  new_size = (obj_size + length) + (obj_size >> 3) + 100;
  if (new_size < h->chunk_size)
    new_size = h->chunk_size;

  /* Allocate and initialize the new chunk.  */
  new_chunk = h->chunk = (struct _obstack_chunk *)(*h->chunkfun) (new_size);
  new_chunk->prev = old_chunk;
  new_chunk->limit = h->chunk_limit = (char *) new_chunk + new_size;

  /* Move the existing object to the new chunk.
     Word at a time is fast and is safe if the object
     is sufficiently aligned.  */
  if (h->alignment_mask + 1 >= DEFAULT_ALIGNMENT)
    {
      for (i = obj_size / sizeof (COPYING_UNIT) - 1;
	   i >= 0; i--)
	((COPYING_UNIT *)new_chunk->contents)[i]
	  = ((COPYING_UNIT *)h->object_base)[i];
      /* We used to copy the odd few remaining bytes as one extra COPYING_UNIT,
	 but that can cross a page boundary on a machine
	 which does not do strict alignment for COPYING_UNITS.  */
      already = obj_size / sizeof (COPYING_UNIT) * sizeof (COPYING_UNIT);
    }
  else
    already = 0;
  /* Copy remaining bytes one by one.  */
  for (i = already; i < obj_size; i++)
    new_chunk->contents[i] = h->object_base[i];

  h->object_base = new_chunk->contents;
  h->next_free = h->object_base + obj_size;
return 0;
}

/* Return nonzero if object OBJ has been allocated from obstack H.
   This is here for debugging.
   If you use it in a program, you are probably losing.  */

int
DEFUN(_obstack_allocated_p, (h, obj), 
      struct obstack *h AND
      PTR obj)
{
  register struct _obstack_chunk*  lp; /* below addr of any objects in this chunk */
  register struct _obstack_chunk*  plp;	/* point to previous chunk if any */

  lp = (h)->chunk;
  while (lp != 0 && ((PTR)lp > obj || (PTR)(lp)->limit < obj))
      {
	plp = lp -> prev;
	lp = plp;
      }
  return lp != 0;
}

/* Free objects in obstack H, including OBJ and everything allocate
   more recently than OBJ.  If OBJ is zero, free everything in H.  */

#ifdef __STDC__
#undef obstack_free
void
obstack_free (struct obstack *h, PTR obj)
#else
int
_obstack_free (h, obj)
     struct obstack *h;
     PTR obj;
#endif
{
  register struct _obstack_chunk*  lp;	/* below addr of any objects in this chunk */
  register struct _obstack_chunk*  plp;	/* point to previous chunk if any */

  lp = (h)->chunk;
  /* We use >= because there cannot be an object at the beginning of a chunk.
     But there can be an empty object at that address
     at the end of another chunk.  */
  while (lp != 0 && ((PTR)lp >= obj || (PTR)(lp)->limit < obj))
    {
      plp = lp -> prev;
      (*h->freefun) ((PTR) lp);
      lp = plp;
    }
  if (lp)
    {
      (h)->object_base = (h)->next_free = (char *)(obj);
      (h)->chunk_limit = lp->limit;
      (h)->chunk = lp;
    }
  else if (obj != 0)
    /* obj is not in any of the chunks! */
    abort ();
}

/* Let same .o link with output of gcc and other compilers.  */

#ifdef __STDC__
int
_obstack_free (h, obj)
     struct obstack *h;
     PTR obj;
{
  obstack_free (h, obj);
  return 0;
}
#endif

/* #if 0 */
/* These are now turned off because the applications do not use it
   and it uses bcopy via obstack_grow, which causes trouble on sysV.  */

/* Now define the functional versions of the obstack macros.
   Define them to simply use the corresponding macros to do the job.  */

#ifdef __STDC__
/* These function definitions do not work with non-ANSI preprocessors;
   they won't pass through the macro names in parentheses.  */

/* The function names appear in parentheses in order to prevent
   the macro-definitions of the names from being expanded there.  */

PTR (obstack_base) (obstack)
     struct obstack *obstack;
{
  return obstack_base (obstack);
}

PTR (obstack_next_free) (obstack)
     struct obstack *obstack;
{
  return obstack_next_free (obstack);
}

int (obstack_object_size) (obstack)
     struct obstack *obstack;
{
  return obstack_object_size (obstack);
}

int (obstack_room) (obstack)
     struct obstack *obstack;
{
  return obstack_room (obstack);
}

void (obstack_grow) (obstack, ptr, length)
     struct obstack *obstack;
     PTR ptr;
     int length;
{
(void)  obstack_grow (obstack, ptr, length);
}

void (obstack_grow0) (obstack, ptr, length)
     struct obstack *obstack;
     PTR ptr;
     int length;
{
(void)  obstack_grow0 (obstack, ptr, length);
}

void (obstack_1grow) (obstack, character)
     struct obstack *obstack;
     int character;
{
(void)  obstack_1grow (obstack, character);
}

void (obstack_blank) (obstack, length)
     struct obstack *obstack;
     int length;
{
(void)  obstack_blank (obstack, length);
}

void (obstack_1grow_fast) (obstack, character)
     struct obstack *obstack;
     int character;
{
  obstack_1grow_fast (obstack, character);
}

void (obstack_blank_fast) (obstack, length)
     struct obstack *obstack;
     int length;
{
  obstack_blank_fast (obstack, length);
}

PTR (obstack_finish) (obstack)
     struct obstack *obstack;
{
  return obstack_finish (obstack);
}

PTR (obstack_alloc) (obstack, length)
     struct obstack *obstack;
     int length;
{
  return obstack_alloc (obstack, length);
}

PTR (obstack_copy) (obstack, ptr, length)
     struct obstack *obstack;
     PTR ptr;
     int length;
{
  return obstack_copy (obstack, ptr, length);
}

PTR (obstack_copy0) (obstack, ptr, length)
     struct obstack *obstack;
     PTR ptr;
     int length;
{
  return obstack_copy0 (obstack, ptr, length);
}

#endif /* __STDC__ */


