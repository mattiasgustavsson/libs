/*
 * This is a modified version of the original LibXDiff. Basically, I merged
 * it into a single-file header-only library, and added some casts to make
 * the code compile in both C and C++. I place my changes under the same
 * license as the original code.
 *                                                  /Mattias Gustavsson
 * Do this:
 *		#define LIBXDIFF_IMPLEMENTATION
 * before including this file in *one* C/C++ file to get the implementation
 *
 ****************************************************************************
 *
 *  LibXDiff by Davide Libenzi ( File Differential Library )
 *  Copyright (C) 2003  Davide Libenzi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 */


/* BEGIN xdiff.h */

#if !defined(XDIFF_H)
#define XDIFF_H

//#ifdef __cplusplus
//extern "C" {
//#endif /* #ifdef __cplusplus */
//

#define XDF_NEED_MINIMAL (1 << 1)

#define XDL_PATCH_NORMAL '-'
#define XDL_PATCH_REVERSE '+'
#define XDL_PATCH_MODEMASK ((1 << 8) - 1)
#define XDL_PATCH_IGNOREBSPACE (1 << 8)
	
#define XDL_MMB_READONLY (1 << 0)

#define XDL_MMF_ATOMIC (1 << 0)

#define XDL_BDOP_INS 1
#define XDL_BDOP_CPY 2
#define XDL_BDOP_INSB 3



typedef struct s_memallocator {
	void *priv;
	void *(*malloc)(void *, unsigned int);
	void (*free)(void *, void *);
	void *(*realloc)(void *, void *, unsigned int);
} memallocator_t;

typedef struct s_mmblock {
	struct s_mmblock *next;
	unsigned long flags;
	long size, bsize;
	char *ptr;
} mmblock_t;

typedef struct s_mmfile {
	unsigned long flags;
	mmblock_t *head, *tail;
	long bsize, fsize, rpos;
	mmblock_t *rcur, *wcur;
} mmfile_t;

typedef struct s_mmbuffer {
	char *ptr;
	long size;
} mmbuffer_t;

typedef struct s_xpparam {
	unsigned long flags;
} xpparam_t;

typedef struct s_xdemitcb {
	void *priv;
	int (*outf)(void *, mmbuffer_t *, int);
} xdemitcb_t;

typedef struct s_xdemitconf {
	long ctxlen;
} xdemitconf_t;

typedef struct s_bdiffparam {
	long bsize;
} bdiffparam_t;


int xdl_set_allocator(memallocator_t const *malt);
void *xdl_malloc(unsigned int size);
void xdl_free(void *ptr);
void *xdl_realloc(void *ptr, unsigned int size);

int xdl_init_mmfile(mmfile_t *mmf, long bsize, unsigned long flags);
void xdl_free_mmfile(mmfile_t *mmf);
int xdl_mmfile_iscompact(mmfile_t *mmf);
int xdl_seek_mmfile(mmfile_t *mmf, long off);
long xdl_read_mmfile(mmfile_t *mmf, void *data, long size);
long xdl_write_mmfile(mmfile_t *mmf, void const *data, long size);
long xdl_writem_mmfile(mmfile_t *mmf, mmbuffer_t *mb, int nbuf);
void *xdl_mmfile_writeallocate(mmfile_t *mmf, long size);
long xdl_mmfile_ptradd(mmfile_t *mmf, char *ptr, long size, unsigned long flags);
long xdl_copy_mmfile(mmfile_t *mmf, long size, xdemitcb_t *ecb);
void *xdl_mmfile_first(mmfile_t *mmf, long *size);
void *xdl_mmfile_next(mmfile_t *mmf, long *size);
long xdl_mmfile_size(mmfile_t *mmf);
int xdl_mmfile_cmp(mmfile_t *mmf1, mmfile_t *mmf2);
int xdl_mmfile_compact(mmfile_t *mmfo, mmfile_t *mmfc, long bsize, unsigned long flags);

int xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t const *xpp,
	     xdemitconf_t const *xecfg, xdemitcb_t *ecb);
int xdl_patch(mmfile_t *mf, mmfile_t *mfp, int mode, xdemitcb_t *ecb,
	      xdemitcb_t *rjecb);

int xdl_merge3(mmfile_t *mmfo, mmfile_t *mmf1, mmfile_t *mmf2, xdemitcb_t *ecb,
	       xdemitcb_t *rjecb);

int xdl_bdiff_mb(mmbuffer_t *mmb1, mmbuffer_t *mmb2, bdiffparam_t const *bdp, xdemitcb_t *ecb);
int xdl_bdiff(mmfile_t *mmf1, mmfile_t *mmf2, bdiffparam_t const *bdp, xdemitcb_t *ecb);
int xdl_rabdiff_mb(mmbuffer_t *mmb1, mmbuffer_t *mmb2, xdemitcb_t *ecb);
int xdl_rabdiff(mmfile_t *mmf1, mmfile_t *mmf2, xdemitcb_t *ecb);
long xdl_bdiff_tgsize(mmfile_t *mmfp);
int xdl_bpatch(mmfile_t *mmf, mmfile_t *mmfp, xdemitcb_t *ecb);
int xdl_bpatch_multi(mmbuffer_t *base, mmbuffer_t *mbpch, int n, xdemitcb_t *ecb);


//#ifdef __cplusplus
//}
//#endif /* #ifdef __cplusplus */

#endif /* #if !defined(XDIFF_H) */

/* END xdiff.h */



#ifdef LIBXDIFF_IMPLEMENTATION
#undef LIBXDIFF_IMPLEMENTATION


/* BEGIN xinclude.h */

#if !defined(XINCLUDE_H)
#define XINCLUDE_H

#if defined(HAVE_WINCONFIG_H)
#include "winconfig.h"
#endif /* #if defined(HAVE_CONFIG_H) */

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif /* #if defined(HAVE_CONFIG_H) */

#if defined(HAVE_STDIO_H)
#include <stdio.h>
#endif /* #if defined(HAVE_STDIO_H) */

#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif /* #if defined(HAVE_STDLIB_H) */

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif /* #if defined(HAVE_UNISTD_H) */

#if defined(HAVE_STRING_H)
#include <string.h>
#endif /* #if defined(HAVE_STRING_H) */

#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif /* #if defined(HAVE_LIMITS_H) */


/*
#include "xmacros.h"
#include "xmissing.h"
#include "xdiff.h"
#include "xtypes.h"
#include "xutils.h"
#include "xadler32.h"
#include "xprepare.h"
#include "xdiffi.h"
#include "xemit.h"
#include "xbdiff.h"
*/


#endif /* #if !defined(XINCLUDE_H) */

/* END xinclude.h */


/* BEGIN xmacros.h */

#if !defined(XMACROS_H)
#define XMACROS_H


#define XDL_MIN(a, b) ((a) < (b) ? (a): (b))
#define XDL_MAX(a, b) ((a) > (b) ? (a): (b))
#define XDL_ABS(v) ((v) >= 0 ? (v): -(v))
#define XDL_ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#define XDL_ADDBITS(v, b) ((v) + ((v) >> (b)))
#define XDL_MASKBITS(b) ((1UL << (b)) - 1)
#define XDL_HASHLONG(v, b) (XDL_ADDBITS((unsigned long) (v), b) & XDL_MASKBITS(b))
#define XDL_PTRFREE(p) do { if (p) { xdl_free(p); (p) = NULL; } } while (0)
#define XDL_RECMATCH(r1, r2) ((r1)->size == (r2)->size && memcmp((r1)->ptr, (r2)->ptr, (r1)->size) == 0)
#define XDL_LE32_PUT(p, v) do { \
	unsigned char *__p = (unsigned char *) (p); \
	*__p++ = (unsigned char) (v); \
	*__p++ = (unsigned char) ((v) >> 8); \
	*__p++ = (unsigned char) ((v) >> 16); \
	*__p = (unsigned char) ((v) >> 24); \
} while (0)
#define XDL_LE32_GET(p, v) do { \
	unsigned char const *__p = (unsigned char const *) (p); \
	(v) = (unsigned long) __p[0] | ((unsigned long) __p[1]) << 8 | \
		((unsigned long) __p[2]) << 16 | ((unsigned long) __p[3]) << 24; \
} while (0)


#endif /* #if !defined(XMACROS_H) */

/* END macros.h */


/* BEGIN xmissing.h */

#if !defined(XMISSING_H)
#define XMISSING_H


#if !defined(CHAR_BIT)
#define CHAR_BIT 8
#endif /* #if !defined(CHAR_BIT) */



#if !defined(HAVE_MEMCHR)
void *memchr(void const *p, int c, long n);
#endif /* #if !defined(HAVE_MEMCHR) */

#if !defined(HAVE_MEMCMP)
int memcmp(void const *p1, void const *p2, long n);
#endif /* #if !defined(HAVE_MEMCMP) */

#if !defined(HAVE_MEMCPY)
void *memcpy(void *d, void const *s, long n);
#endif /* #if !defined(HAVE_MEMCPY) */

#if !defined(HAVE_MEMSET)
void *memset(void *d, int c, long n);
#endif /* #if !defined(HAVE_MEMSET) */

#if !defined(HAVE_STRLEN)
long strlen(char const *s);
#endif /* #if !defined(HAVE_STRLEN) */



#endif /* #if !defined(XMISSING_H) */

/* END xmissing.h */


/* BEGIN xtypes.h */

#if !defined(XTYPES_H)
#define XTYPES_H



typedef struct s_chanode {
	struct s_chanode *next;
	long icurr;
} chanode_t;

typedef struct s_chastore {
	chanode_t *head, *tail;
	long isize, nsize;
	chanode_t *ancur;
	chanode_t *sncur;
	long scurr;
} chastore_t;

typedef struct s_xrecord {
	struct s_xrecord *next;
	char const *ptr;
	long size;
	unsigned long ha;
} xrecord_t;

typedef struct s_xdfile {
	chastore_t rcha;
	long nrec;
	unsigned int hbits;
	xrecord_t **rhash;
	long dstart, dend;
	xrecord_t **recs;
	char *rchg;
	long *rindex;
	long nreff;
	unsigned long *ha;
} xdfile_t;

typedef struct s_xdfenv {
	xdfile_t xdf1, xdf2;
} xdfenv_t;



#endif /* #if !defined(XTYPES_H) */

/* END xtypes.h */


/* BEGIN xutils.h */

#if !defined(XUTILS_H)
#define XUTILS_H



long xdl_bogosqrt(long n);
int xdl_emit_diffrec(char const *rec, long size, char const *pre, long psize,
		     xdemitcb_t *ecb);
int xdl_mmfile_outf(void *priv, mmbuffer_t *mb, int nbuf);
int xdl_cha_init(chastore_t *cha, long isize, long icount);
void xdl_cha_free(chastore_t *cha);
void *xdl_cha_alloc(chastore_t *cha);
void *xdl_cha_first(chastore_t *cha);
void *xdl_cha_next(chastore_t *cha);
long xdl_guess_lines(mmfile_t *mf);
unsigned long xdl_hash_record(char const **data, char const *top);
unsigned int xdl_hashbits(unsigned int size);
int xdl_num_out(char *out, long val);
long xdl_atol(char const *str, char const **next);
int xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2, xdemitcb_t *ecb);



#endif /* #if !defined(XUTILS_H) */

/* END xutils.h */


/* BEGIN xadler32.h */

#if !defined(XADLER32_H)
#define XADLER32_H



unsigned long xdl_adler32(unsigned long adler, unsigned char const *buf,
			  unsigned int len);



#endif /* #if !defined(XADLER32_H) */

/* END xadler32.h */


/* BEGIN xprepare.h */

#if !defined(XPREPARE_H)
#define XPREPARE_H



int xdl_prepare_env(mmfile_t *mf1, mmfile_t *mf2, xpparam_t const *xpp,
		    xdfenv_t *xe);
void xdl_free_env(xdfenv_t *xe);



#endif /* #if !defined(XPREPARE_H) */

/* END xprepare.h */


/* BEGIN xdiffi.h */

#if !defined(XDIFFI_H)
#define XDIFFI_H


typedef struct s_diffdata {
	long nrec;
	unsigned long const *ha;
	long *rindex;
	char *rchg;
} diffdata_t;

typedef struct s_xdalgoenv {
	long mxcost;
	long snake_cnt;
	long heur_min;
} xdalgoenv_t;

typedef struct s_xdchange {
	struct s_xdchange *next;
	long i1, i2;
	long chg1, chg2;
} xdchange_t;



int xdl_recs_cmp(diffdata_t *dd1, long off1, long lim1,
		 diffdata_t *dd2, long off2, long lim2,
		 long *kvdf, long *kvdb, int need_min, xdalgoenv_t *xenv);
int xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t const *xpp,
		xdfenv_t *xe);
int xdl_build_script(xdfenv_t *xe, xdchange_t **xscr);
void xdl_free_script(xdchange_t *xscr);
int xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t const *xecfg);


#endif /* #if !defined(XDIFFI_H) */

/* END xdiffi.h */


/* BEGIN xemit.h */

#if !defined(XEMIT_H)
#define XEMIT_H



int xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t const *xecfg);



#endif /* #if !defined(XEMIT_H) */

/* END xemit.h */


/* BEGIN xbdiff.h */

#if !defined(XBDIFF_H)
#define XBDIFF_H


#define XDL_BPATCH_HDR_SIZE (4 + 4)
#define XDL_MIN_BLKSIZE 16
#define XDL_INSBOP_SIZE (1 + 4)
#define XDL_COPYOP_SIZE (1 + 4 + 4)



unsigned long xdl_mmb_adler32(mmbuffer_t *mmb);
unsigned long xdl_mmf_adler32(mmfile_t *mmf);



#endif /* #if !defined(XBDIFF_H) */

/* END xbdiff.h */





/* BEGIN xadler32.c */

/* largest prime smaller than 65536 */
#define BASE 65521L

/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */
#define NMAX 5552


#define DO1(buf, i)  { s1 += buf[i]; s2 += s1; }
#define DO2(buf, i)  DO1(buf, i); DO1(buf, i + 1);
#define DO4(buf, i)  DO2(buf, i); DO2(buf, i + 2);
#define DO8(buf, i)  DO4(buf, i); DO4(buf, i + 4);
#define DO16(buf)    DO8(buf, 0); DO8(buf, 8);



unsigned long xdl_adler32(unsigned long adler, unsigned char const *buf,
			  unsigned int len) {
	int k;
	unsigned long s1 = adler & 0xffff;
	unsigned long s2 = (adler >> 16) & 0xffff;

	if (!buf)
		return 1;

	while (len > 0) {
		k = len < NMAX ? len :NMAX;
		len -= k;
		while (k >= 16) {
			DO16(buf);
			buf += 16;
			k -= 16;
		}
		if (k != 0)
			do {
				s1 += *buf++;
				s2 += s1;
			} while (--k);
		s1 %= BASE;
		s2 %= BASE;
	}

	return (s2 << 16) | s1;
}

/* END xadler32.c */


/* BEGIN xalloc.c */

static void *wrap_malloc(void *priv, unsigned int size) {

	return malloc(size);
}


static void wrap_free(void *priv, void *ptr) {

	free(ptr);
}


static void *wrap_realloc(void *priv, void *ptr, unsigned int size) {

	return realloc(ptr, size);
}


static memallocator_t xmalt = { NULL, wrap_malloc, wrap_free, wrap_realloc };



int xdl_set_allocator(memallocator_t const *malt) {
	xmalt = *malt;
	return 0;
}


void *xdl_malloc(unsigned int size) {
	return xmalt.malloc ? xmalt.malloc(xmalt.priv, size): NULL;
}


void xdl_free(void *ptr) {
	if (xmalt.free)
		xmalt.free(xmalt.priv, ptr);
}


void *xdl_realloc(void *ptr, unsigned int size) {
	return xmalt.realloc ? xmalt.realloc(xmalt.priv, ptr, size): NULL;
}

/* END xalloc.x */


/* BEGIN xbdiff. c */

typedef struct s_bdrecord {
	struct s_bdrecord *next;
	unsigned long fp;
	char const *ptr;
} bdrecord_t;

typedef struct s_bdfile {
	char const *data, *top;
	chastore_t cha;
	unsigned int fphbits;
	bdrecord_t **fphash;
} bdfile_t;



static int xdl_prepare_bdfile(mmbuffer_t *mmb, long fpbsize, bdfile_t *bdf) {
	unsigned int fphbits;
	long i, size, hsize;
	char const *base, *data, *top;
	bdrecord_t *brec;
	bdrecord_t **fphash;

	fphbits = xdl_hashbits((unsigned int) (mmb->size / fpbsize) + 1);
	hsize = 1 << fphbits;
	if (!(fphash = (bdrecord_t **) xdl_malloc(hsize * sizeof(bdrecord_t *)))) {

		return -1;
	}
	for (i = 0; i < hsize; i++)
		fphash[i] = NULL;

	if (xdl_cha_init(&bdf->cha, sizeof(bdrecord_t), hsize / 4 + 1) < 0) {

		xdl_free(fphash);
		return -1;
	}

	if (!(size = mmb->size)) {
		bdf->data = bdf->top = NULL;
	} else {
		bdf->data = data = base = mmb->ptr;
		bdf->top = top = mmb->ptr + mmb->size;

		if ((data += (size / fpbsize) * fpbsize) == top)
			data -= fpbsize;

		for (; data >= base; data -= fpbsize) {
			if (!(brec = (bdrecord_t *) xdl_cha_alloc(&bdf->cha))) {

				xdl_cha_free(&bdf->cha);
				xdl_free(fphash);
				return -1;
			}

			brec->fp = xdl_adler32(0, (unsigned char const *) data,
					       XDL_MIN(fpbsize, (long) (top - data)));
			brec->ptr = data;

			i = (long) XDL_HASHLONG(brec->fp, fphbits);
			brec->next = fphash[i];
			fphash[i] = brec;
		}
	}

	bdf->fphbits = fphbits;
	bdf->fphash = fphash;

	return 0;
}


static void xdl_free_bdfile(bdfile_t *bdf) {

	xdl_free(bdf->fphash);
	xdl_cha_free(&bdf->cha);
}


unsigned long xdl_mmb_adler32(mmbuffer_t *mmb) {

	return mmb->size ? xdl_adler32(0, (unsigned char const *) mmb->ptr, mmb->size): 0;
}


unsigned long xdl_mmf_adler32(mmfile_t *mmf) {
	unsigned long fp = 0;
	long size;
	char const *blk;

	if ((blk = (char const *) xdl_mmfile_first(mmf, &size)) != NULL) {
		do {
			fp = xdl_adler32(fp, (unsigned char const *) blk, size);
		} while ((blk = (char const *) xdl_mmfile_next(mmf, &size)) != NULL);
	}
	return fp;
}


int xdl_bdiff_mb(mmbuffer_t *mmb1, mmbuffer_t *mmb2, bdiffparam_t const *bdp, xdemitcb_t *ecb) {
	long i, rsize, size, bsize, csize, msize, moff;
	unsigned long fp;
	char const *blk, *base, *data, *top, *ptr1, *ptr2;
	bdrecord_t *brec;
	bdfile_t bdf;
	mmbuffer_t mb[2];
	unsigned char cpybuf[32];

	if ((bsize = bdp->bsize) < XDL_MIN_BLKSIZE)
		bsize = XDL_MIN_BLKSIZE;
	if (xdl_prepare_bdfile(mmb1, bsize, &bdf) < 0) {

		return -1;
	}

	/*
	 * Prepare and emit the binary patch file header. It will be used
	 * to verify that that file being patched matches in size and fingerprint
	 * the one that generated the patch.
	 */
	fp = xdl_mmb_adler32(mmb1);
	size = mmb1->size;
	XDL_LE32_PUT(cpybuf, fp);
	XDL_LE32_PUT(cpybuf + 4, size);

	mb[0].ptr = (char *) cpybuf;
	mb[0].size = 4 + 4;

	if (ecb->outf(ecb->priv, mb, 1) < 0) {

		xdl_free_bdfile(&bdf);
		return -1;
	}

	if ((blk = (char const *) mmb2->ptr) != NULL) {
		size = mmb2->size;
		for (base = data = blk, top = data + size; data < top;) {
			rsize = XDL_MIN(bsize, (long) (top - data));
			fp = xdl_adler32(0, (unsigned char const *) data, rsize);

			i = (long) XDL_HASHLONG(fp, bdf.fphbits);
			for (msize = 0, brec = bdf.fphash[i]; brec; brec = brec->next)
				if (brec->fp == fp) {
					csize = XDL_MIN((long) (top - data), (long) (bdf.top - brec->ptr));
					for (ptr1 = brec->ptr, ptr2 = data; csize && *ptr1 == *ptr2;
					     csize--, ptr1++, ptr2++);

					if ((csize = (long) (ptr1 - brec->ptr)) > msize) {
						moff = (long) (brec->ptr - bdf.data);
						msize = csize;
					}
				}

			if (msize < XDL_COPYOP_SIZE) {
				data++;
			} else {
				if (data > base) {
					i = (long) (data - base);
					if (i > 255) {
						cpybuf[0] = XDL_BDOP_INSB;
						XDL_LE32_PUT(cpybuf + 1, i);

						mb[0].ptr = (char *) cpybuf;
						mb[0].size = XDL_INSBOP_SIZE;
					} else {
						cpybuf[0] = XDL_BDOP_INS;
						cpybuf[1] = (unsigned char) i;

						mb[0].ptr = (char *) cpybuf;
						mb[0].size = 2;
					}
					mb[1].ptr = (char *) base;
					mb[1].size = i;

					if (ecb->outf(ecb->priv, mb, 2) < 0) {

						xdl_free_bdfile(&bdf);
						return -1;
					}
				}

				data += msize;

				cpybuf[0] = XDL_BDOP_CPY;
				XDL_LE32_PUT(cpybuf + 1, moff);
				XDL_LE32_PUT(cpybuf + 5, msize);

				mb[0].ptr = (char *) cpybuf;
				mb[0].size = XDL_COPYOP_SIZE;

				if (ecb->outf(ecb->priv, mb, 1) < 0) {

					xdl_free_bdfile(&bdf);
					return -1;
				}
				base = data;
			}
		}
		if (data > base) {
			i = (long) (data - base);
			if (i > 255) {
				cpybuf[0] = XDL_BDOP_INSB;
				XDL_LE32_PUT(cpybuf + 1, i);

				mb[0].ptr = (char *) cpybuf;
				mb[0].size = XDL_INSBOP_SIZE;
			} else {
				cpybuf[0] = XDL_BDOP_INS;
				cpybuf[1] = (unsigned char) i;

				mb[0].ptr = (char *) cpybuf;
				mb[0].size = 2;
			}
			mb[1].ptr = (char *) base;
			mb[1].size = i;

			if (ecb->outf(ecb->priv, mb, 2) < 0) {

				xdl_free_bdfile(&bdf);
				return -1;
			}
		}
	}

	xdl_free_bdfile(&bdf);

	return 0;
}


int xdl_bdiff(mmfile_t *mmf1, mmfile_t *mmf2, bdiffparam_t const *bdp, xdemitcb_t *ecb) {
	mmbuffer_t mmb1, mmb2;

	if (!xdl_mmfile_iscompact(mmf1) || !xdl_mmfile_iscompact(mmf2)) {

		return -1;
	}

	if ((mmb1.ptr = (char *) xdl_mmfile_first(mmf1, &mmb1.size)) == NULL)
		mmb1.size = 0;
	if ((mmb2.ptr = (char *) xdl_mmfile_first(mmf2, &mmb2.size)) == NULL)
		mmb2.size = 0;

	return xdl_bdiff_mb(&mmb1, &mmb2, bdp, ecb);
}


long xdl_bdiff_tgsize(mmfile_t *mmfp) {
	long tgsize = 0, size, off, csize;
	char const *blk;
	unsigned char const *data, *top;

	if ((blk = (char const *) xdl_mmfile_first(mmfp, &size)) == NULL ||
	    size < XDL_BPATCH_HDR_SIZE) {

		return -1;
	}
	blk += XDL_BPATCH_HDR_SIZE;
	size -= XDL_BPATCH_HDR_SIZE;

	do {
		for (data = (unsigned char const *) blk, top = data + size;
		     data < top;) {
			if (*data == XDL_BDOP_INS) {
				data++;
				csize = (long) *data++;
				tgsize += csize;
				data += csize;
			} else if (*data == XDL_BDOP_INSB) {
				data++;
				XDL_LE32_GET(data, csize);
				data += 4;
				tgsize += csize;
				data += csize;
			} else if (*data == XDL_BDOP_CPY) {
				data++;
				XDL_LE32_GET(data, off);
				data += 4;
				XDL_LE32_GET(data, csize);
				data += 4;
				tgsize += csize;
			} else {

				return -1;
			}
		}
	} while ((blk = (char const *) xdl_mmfile_next(mmfp, &size)) != NULL);

	return tgsize;
}


/* END xbdiff.c */


/* BEGIN xpatchi.c */

#define XDL_MOBF_MINALLOC 128


typedef struct s_mmoffbuffer {
	long off, size;
	char *ptr;
} mmoffbuffer_t;



static int xdl_copy_range(mmfile_t *mmf, long off, long size, xdemitcb_t *ecb) {
	if (xdl_seek_mmfile(mmf, off) < 0) {

		return -1;
	}
	if (xdl_copy_mmfile(mmf, size, ecb) != size) {

		return -1;
	}

	return 0;
}


int xdl_bpatch(mmfile_t *mmf, mmfile_t *mmfp, xdemitcb_t *ecb) {
	long size, off, csize, osize;
	unsigned long fp, ofp;
	char const *blk;
	unsigned char const *data, *top;
	mmbuffer_t mb;

	if ((blk = (char const *) xdl_mmfile_first(mmfp, &size)) == NULL ||
	    size < XDL_BPATCH_HDR_SIZE) {

		return -1;
	}
	ofp = xdl_mmf_adler32(mmf);
	osize = xdl_mmfile_size(mmf);
	XDL_LE32_GET(blk, fp);
	XDL_LE32_GET(blk + 4, csize);
	if (fp != ofp || csize != osize) {

		return -1;
	}

	blk += XDL_BPATCH_HDR_SIZE;
	size -= XDL_BPATCH_HDR_SIZE;

	do {
		for (data = (unsigned char const *) blk, top = data + size;
		     data < top;) {
			if (*data == XDL_BDOP_INS) {
				data++;

				mb.size = (long) *data++;
				mb.ptr = (char *) data;
				data += mb.size;

				if (ecb->outf(ecb->priv, &mb, 1) < 0) {

					return -1;
				}
			} else if (*data == XDL_BDOP_INSB) {
				data++;
				XDL_LE32_GET(data, csize);
				data += 4;

				mb.size = csize;
				mb.ptr = (char *) data;
				data += mb.size;

				if (ecb->outf(ecb->priv, &mb, 1) < 0) {

					return -1;
				}
			} else if (*data == XDL_BDOP_CPY) {
				data++;
				XDL_LE32_GET(data, off);
				data += 4;
				XDL_LE32_GET(data, csize);
				data += 4;

				if (xdl_copy_range(mmf, off, csize, ecb) < 0) {

					return -1;
				}
			} else {

				return -1;
			}
		}
	} while ((blk = (char const *) xdl_mmfile_next(mmfp, &size)) != NULL);

	return 0;
}


static unsigned long xdl_mmob_adler32(mmoffbuffer_t *obf, int n) {
	unsigned long ha;

	for (ha = 0; n > 0; n--, obf++)
		ha = xdl_adler32(ha, (unsigned char const *) obf->ptr, obf->size);

	return ha;
}


static long xdl_mmob_size(mmoffbuffer_t *obf, int n) {

	return n > 0 ? obf[n - 1].off + obf[n - 1].size: 0;
}


static mmoffbuffer_t *xdl_mmob_new(mmoffbuffer_t **probf, int *pnobf, int *paobf) {
	int aobf;
	mmoffbuffer_t *cobf, *rrobf;

	if (*pnobf >= *paobf) {
		aobf = 2 * (*paobf) + 1;
		if ((rrobf = (mmoffbuffer_t *)
		     xdl_realloc(*probf, aobf * sizeof(mmoffbuffer_t))) == NULL) {

			return NULL;
		}
		*probf = rrobf;
		*paobf = aobf;
	}
	cobf = (*probf) + (*pnobf);
	(*pnobf)++;

	return cobf;
}


static int xdl_mmob_find_cntr(mmoffbuffer_t *obf, int n, long off) {
	int i, lo, hi;

	for (lo = -1, hi = n; hi - lo > 1;) {
		i = (hi + lo) / 2;
		if (off < obf[i].off)
			hi = i;
		else
			lo = i;
	}

	return (lo >= 0 && off >= obf[lo].off && off < obf[lo].off + obf[lo].size) ? lo: -1;
}


static int xdl_bmerge(mmoffbuffer_t *obf, int n, mmbuffer_t *mbfp, mmoffbuffer_t **probf,
		      int *pnobf) {
	int i, aobf, nobf;
	long ooff, off, csize;
	unsigned long fp, ofp;
	unsigned char const *data, *top;
	mmoffbuffer_t *robf, *cobf;

	if (mbfp->size < XDL_BPATCH_HDR_SIZE) {

		return -1;
	}
	data = (unsigned char const *) mbfp->ptr;
	top = data + mbfp->size;

	ofp = xdl_mmob_adler32(obf, n);
	XDL_LE32_GET(data, fp);
	data += 4;
	XDL_LE32_GET(data, csize);
	data += 4;
	if (fp != ofp || csize != xdl_mmob_size(obf, n)) {

		return -1;
	}
	aobf = XDL_MOBF_MINALLOC;
	nobf = 0;
	if ((robf = (mmoffbuffer_t *) xdl_malloc(aobf * sizeof(mmoffbuffer_t))) == NULL) {

		return -1;
	}

	for (ooff = 0; data < top;) {
		if (*data == XDL_BDOP_INS) {
			data++;

			if ((cobf = xdl_mmob_new(&robf, &nobf, &aobf)) == NULL) {

				xdl_free(robf);
				return -1;
			}
			cobf->off = ooff;
			cobf->size = (long) *data++;
			cobf->ptr = (char *) data;

			data += cobf->size;
			ooff += cobf->size;
		} else if (*data == XDL_BDOP_INSB) {
			data++;
			XDL_LE32_GET(data, csize);
			data += 4;

			if ((cobf = xdl_mmob_new(&robf, &nobf, &aobf)) == NULL) {

				xdl_free(robf);
				return -1;
			}
			cobf->off = ooff;
			cobf->size = csize;
			cobf->ptr = (char *) data;

			data += cobf->size;
			ooff += cobf->size;
		} else if (*data == XDL_BDOP_CPY) {
			data++;
			XDL_LE32_GET(data, off);
			data += 4;
			XDL_LE32_GET(data, csize);
			data += 4;

			if ((i = xdl_mmob_find_cntr(obf, n, off)) < 0) {

				xdl_free(robf);
				return -1;
			}
			off -= obf[i].off;
			for (; i < n && csize > 0; i++, off = 0) {
				if ((cobf = xdl_mmob_new(&robf, &nobf, &aobf)) == NULL) {

					xdl_free(robf);
					return -1;
				}
				cobf->off = ooff;
				cobf->size = XDL_MIN(csize, obf[i].size - off);
				cobf->ptr = obf[i].ptr + off;

				ooff += cobf->size;
				csize -= cobf->size;
			}
			if (csize > 0) {

				xdl_free(robf);
				return -1;
			}
		} else {

			xdl_free(robf);
			return -1;
		}
	}
	*probf = robf;
	*pnobf = nobf;

	return 0;
}


static int xdl_bmerge_synt(mmoffbuffer_t *obf, int n, xdemitcb_t *ecb) {
	int i;
	mmbuffer_t *mb;

	if ((mb = (mmbuffer_t *) xdl_malloc(n * sizeof(mmbuffer_t))) == NULL) {

		return -1;
	}
	for (i = 0; i < n; i++) {
		mb[i].ptr = obf[i].ptr;
		mb[i].size = obf[i].size;
	}
	if (ecb->outf(ecb->priv, mb, n) < 0) {

		xdl_free(mb);
		return -1;
	}
	xdl_free(mb);

	return 0;
}


int xdl_bpatch_multi(mmbuffer_t *base, mmbuffer_t *mbpch, int n, xdemitcb_t *ecb) {
	int i, nobf, fnobf;
	mmoffbuffer_t *obf, *fobf;

	nobf = 1;
	if ((obf = (mmoffbuffer_t *) xdl_malloc(nobf * sizeof(mmoffbuffer_t))) == NULL) {

		return -1;
	}
	obf->off = 0;
	obf->ptr = base->ptr;
	obf->size = base->size;
	for (i = 0; i < n; i++) {
		if (xdl_bmerge(obf, nobf, &mbpch[i], &fobf, &fnobf) < 0) {

			xdl_free(obf);
			return -1;
		}
		xdl_free(obf);

		obf = fobf;
		nobf = fnobf;
	}
	if (xdl_bmerge_synt(obf, nobf, ecb) < 0) {

		xdl_free(obf);
		return -1;
	}
	xdl_free(obf);

	return 0;
}

/* END xpatchi.c */


/* BEGIN xdiffi.c */

#define XDL_MAX_COST_MIN 256
#define XDL_HEUR_MIN_COST 256
#define XDL_LINE_MAX (long)((1UL << (8 * sizeof(long) - 1)) - 1)
#define XDL_SNAKE_CNT 20
#define XDL_K_HEUR 4



typedef struct s_xdpsplit {
	long i1, i2;
	int min_lo, min_hi;
} xdpsplit_t;



/*
 * See "An O(ND) Difference Algorithm and its Variations", by Eugene Myers.
 * Basically considers a "box" (off1, off2, lim1, lim2) and scan from both
 * the forward diagonal starting from (off1, off2) and the backward diagonal
 * starting from (lim1, lim2). If the K values on the same diagonal crosses
 * returns the furthest point of reach. We might end up having to expensive
 * cases using this algorithm is full, so a little bit of heuristic is needed
 * to cut the search and to return a suboptimal point.
 */
static long xdl_split(unsigned long const *ha1, long off1, long lim1,
		      unsigned long const *ha2, long off2, long lim2,
		      long *kvdf, long *kvdb, int need_min, xdpsplit_t *spl,
		      xdalgoenv_t *xenv) {
	long dmin = off1 - lim2, dmax = lim1 - off2;
	long fmid = off1 - off2, bmid = lim1 - lim2;
	long odd = (fmid - bmid) & 1;
	long fmin = fmid, fmax = fmid;
	long bmin = bmid, bmax = bmid;
	long ec, d, i1, i2, prev1, best, dd, v, k;

	/*
	 * Set initial diagonal values for both forward and backward path.
	 */
	kvdf[fmid] = off1;
	kvdb[bmid] = lim1;

	for (ec = 1;; ec++) {
		int got_snake = 0;

		/*
		 * We need to extent the diagonal "domain" by one. If the next
		 * values exits the box boundaries we need to change it in the
		 * opposite direction because (max - min) must be a power of two.
		 * Also we initialize the extenal K value to -1 so that we can
		 * avoid extra conditions check inside the core loop.
		 */
		if (fmin > dmin)
			kvdf[--fmin - 1] = -1;
		else
			++fmin;
		if (fmax < dmax)
			kvdf[++fmax + 1] = -1;
		else
			--fmax;

		for (d = fmax; d >= fmin; d -= 2) {
			if (kvdf[d - 1] >= kvdf[d + 1])
				i1 = kvdf[d - 1] + 1;
			else
				i1 = kvdf[d + 1];
			prev1 = i1;
			i2 = i1 - d;
			for (; i1 < lim1 && i2 < lim2 && ha1[i1] == ha2[i2]; i1++, i2++);
			if (i1 - prev1 > xenv->snake_cnt)
				got_snake = 1;
			kvdf[d] = i1;
			if (odd && bmin <= d && d <= bmax && kvdb[d] <= i1) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				return ec;
			}
		}

		/*
		 * We need to extent the diagonal "domain" by one. If the next
		 * values exits the box boundaries we need to change it in the
		 * opposite direction because (max - min) must be a power of two.
		 * Also we initialize the extenal K value to -1 so that we can
		 * avoid extra conditions check inside the core loop.
		 */
		if (bmin > dmin)
			kvdb[--bmin - 1] = XDL_LINE_MAX;
		else
			++bmin;
		if (bmax < dmax)
			kvdb[++bmax + 1] = XDL_LINE_MAX;
		else
			--bmax;

		for (d = bmax; d >= bmin; d -= 2) {
			if (kvdb[d - 1] < kvdb[d + 1])
				i1 = kvdb[d - 1];
			else
				i1 = kvdb[d + 1] - 1;
			prev1 = i1;
			i2 = i1 - d;
			for (; i1 > off1 && i2 > off2 && ha1[i1 - 1] == ha2[i2 - 1]; i1--, i2--);
			if (prev1 - i1 > xenv->snake_cnt)
				got_snake = 1;
			kvdb[d] = i1;
			if (!odd && fmin <= d && d <= fmax && i1 <= kvdf[d]) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				return ec;
			}
		}

		if (need_min)
			continue;

		/*
		 * If the edit cost is above the heuristic trigger and if
		 * we got a good snake, we sample current diagonals to see
		 * if some of the, have reached an "interesting" path. Our
		 * measure is a function of the distance from the diagonal
		 * corner (i1 + i2) penalized with the distance from the
		 * mid diagonal itself. If this value is above the current
		 * edit cost times a magic factor (XDL_K_HEUR) we consider
		 * it interesting.
		 */
		if (got_snake && ec > xenv->heur_min) {
			for (best = 0, d = fmax; d >= fmin; d -= 2) {
				dd = d > fmid ? d - fmid: fmid - d;
				i1 = kvdf[d];
				i2 = i1 - d;
				v = (i1 - off1) + (i2 - off2) - dd;

				if (v > XDL_K_HEUR * ec && v > best &&
				    off1 + xenv->snake_cnt <= i1 && i1 < lim1 &&
				    off2 + xenv->snake_cnt <= i2 && i2 < lim2) {
					for (k = 1; ha1[i1 - k] == ha2[i2 - k]; k++)
						if (k == xenv->snake_cnt) {
							best = v;
							spl->i1 = i1;
							spl->i2 = i2;
							break;
						}
				}
			}
			if (best > 0) {
				spl->min_lo = 1;
				spl->min_hi = 0;
				return ec;
			}

			for (best = 0, d = bmax; d >= bmin; d -= 2) {
				dd = d > bmid ? d - bmid: bmid - d;
				i1 = kvdb[d];
				i2 = i1 - d;
				v = (lim1 - i1) + (lim2 - i2) - dd;

				if (v > XDL_K_HEUR * ec && v > best &&
				    off1 < i1 && i1 <= lim1 - xenv->snake_cnt &&
				    off2 < i2 && i2 <= lim2 - xenv->snake_cnt) {
					for (k = 0; ha1[i1 + k] == ha2[i2 + k]; k++)
						if (k == xenv->snake_cnt - 1) {
							best = v;
							spl->i1 = i1;
							spl->i2 = i2;
							break;
						}
				}
			}
			if (best > 0) {
				spl->min_lo = 0;
				spl->min_hi = 1;
				return ec;
			}
		}

		/*
		 * Enough is enough. We spent too much time here and now we collect
		 * the furthest reaching path using the (i1 + i2) measure.
		 */
		if (ec >= xenv->mxcost) {
			long fbest, fbest1, bbest, bbest1;

			fbest = -1;
			for (d = fmax; d >= fmin; d -= 2) {
				i1 = XDL_MIN(kvdf[d], lim1);
				i2 = i1 - d;
				if (lim2 < i2)
					i1 = lim2 + d, i2 = lim2;
				if (fbest < i1 + i2) {
					fbest = i1 + i2;
					fbest1 = i1;
				}
			}

			bbest = XDL_LINE_MAX;
			for (d = bmax; d >= bmin; d -= 2) {
				i1 = XDL_MAX(off1, kvdb[d]);
				i2 = i1 - d;
				if (i2 < off2)
					i1 = off2 + d, i2 = off2;
				if (i1 + i2 < bbest) {
					bbest = i1 + i2;
					bbest1 = i1;
				}
			}

			if ((lim1 + lim2) - bbest < fbest - (off1 + off2)) {
				spl->i1 = fbest1;
				spl->i2 = fbest - fbest1;
				spl->min_lo = 1;
				spl->min_hi = 0;
			} else {
				spl->i1 = bbest1;
				spl->i2 = bbest - bbest1;
				spl->min_lo = 0;
				spl->min_hi = 1;
			}
			return ec;
		}
	}

	return -1;
}


/*
 * Rule: "Divide et Impera". Recursively split the box in sub-boxes by calling
 * the box splitting function. Note that the real job (marking changed lines)
 * is done in the two boundary reaching checks.
 */
int xdl_recs_cmp(diffdata_t *dd1, long off1, long lim1,
		 diffdata_t *dd2, long off2, long lim2,
		 long *kvdf, long *kvdb, int need_min, xdalgoenv_t *xenv) {
	unsigned long const *ha1 = dd1->ha, *ha2 = dd2->ha;

	/*
	 * Shrink the box by walking through each diagonal snake (SW and NE).
	 */
	for (; off1 < lim1 && off2 < lim2 && ha1[off1] == ha2[off2]; off1++, off2++);
	for (; off1 < lim1 && off2 < lim2 && ha1[lim1 - 1] == ha2[lim2 - 1]; lim1--, lim2--);

	/*
	 * If one dimension is empty, then all records on the other one must
	 * be obviously changed.
	 */
	if (off1 == lim1) {
		char *rchg2 = dd2->rchg;
		long *rindex2 = dd2->rindex;

		for (; off2 < lim2; off2++)
			rchg2[rindex2[off2]] = 1;
	} else if (off2 == lim2) {
		char *rchg1 = dd1->rchg;
		long *rindex1 = dd1->rindex;

		for (; off1 < lim1; off1++)
			rchg1[rindex1[off1]] = 1;
	} else {
		long ec;
		xdpsplit_t spl;

		/*
		 * Divide ...
		 */
		if ((ec = xdl_split(ha1, off1, lim1, ha2, off2, lim2, kvdf, kvdb,
				    need_min, &spl, xenv)) < 0) {

			return -1;
		}

		/*
		 * ... et Impera.
		 */
		if (xdl_recs_cmp(dd1, off1, spl.i1, dd2, off2, spl.i2,
				 kvdf, kvdb, spl.min_lo, xenv) < 0 ||
		    xdl_recs_cmp(dd1, spl.i1, lim1, dd2, spl.i2, lim2,
				 kvdf, kvdb, spl.min_hi, xenv) < 0) {

			return -1;
		}
	}

	return 0;
}


int xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t const *xpp,
		xdfenv_t *xe) {
	long ndiags;
	long *kvd, *kvdf, *kvdb;
	xdalgoenv_t xenv;
	diffdata_t dd1, dd2;

	if (xdl_prepare_env(mf1, mf2, xpp, xe) < 0) {

		return -1;
	}

	/*
	 * Allocate and setup K vectors to be used by the differential algorithm.
	 * One is to store the forward path and one to store the backward path.
	 */
	ndiags = xe->xdf1.nreff + xe->xdf2.nreff + 3;
	if (!(kvd = (long *) xdl_malloc((2 * ndiags + 2) * sizeof(long)))) {

		xdl_free_env(xe);
		return -1;
	}
	kvdf = kvd;
	kvdb = kvdf + ndiags;
	kvdf += xe->xdf2.nreff + 1;
	kvdb += xe->xdf2.nreff + 1;

	xenv.mxcost = xdl_bogosqrt(ndiags);
	if (xenv.mxcost < XDL_MAX_COST_MIN)
		xenv.mxcost = XDL_MAX_COST_MIN;
	xenv.snake_cnt = XDL_SNAKE_CNT;
	xenv.heur_min = XDL_HEUR_MIN_COST;

	dd1.nrec = xe->xdf1.nreff;
	dd1.ha = xe->xdf1.ha;
	dd1.rchg = xe->xdf1.rchg;
	dd1.rindex = xe->xdf1.rindex;
	dd2.nrec = xe->xdf2.nreff;
	dd2.ha = xe->xdf2.ha;
	dd2.rchg = xe->xdf2.rchg;
	dd2.rindex = xe->xdf2.rindex;

	if (xdl_recs_cmp(&dd1, 0, dd1.nrec, &dd2, 0, dd2.nrec,
			 kvdf, kvdb, (xpp->flags & XDF_NEED_MINIMAL) != 0, &xenv) < 0) {

		xdl_free(kvd);
		xdl_free_env(xe);
		return -1;
	}

	xdl_free(kvd);

	return 0;
}


static xdchange_t *xdl_add_change(xdchange_t *xscr, long i1, long i2, long chg1, long chg2) {
	xdchange_t *xch;

	if (!(xch = (xdchange_t *) xdl_malloc(sizeof(xdchange_t))))
		return NULL;

	xch->next = xscr;
	xch->i1 = i1;
	xch->i2 = i2;
	xch->chg1 = chg1;
	xch->chg2 = chg2;

	return xch;
}


static int xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo) {
	long ix, ixo, ixs, ixref, grpsiz, nrec = xdf->nrec;
	char *rchg = xdf->rchg, *rchgo = xdfo->rchg;
	xrecord_t **recs = xdf->recs;

	/*
	 * This is the same of what GNU diff does. Move back and forward
	 * change groups for a consistent and pretty diff output. This also
	 * helps in finding joineable change groups and reduce the diff size.
	 */
	for (ix = ixo = 0;;) {
		/*
		 * Find the first changed line in the to-be-compacted file.
		 * We need to keep track of both indexes, so if we find a
		 * changed lines group on the other file, while scanning the
		 * to-be-compacted file, we need to skip it properly. Note
		 * that loops that are testing for changed lines on rchg* do
		 * not need index bounding since the array is prepared with
		 * a zero at position -1 and N.
		 */
		for (; ix < nrec && !rchg[ix]; ix++)
			while (rchgo[ixo++]);
		if (ix == nrec)
			break;

		/*
		 * Record the start of a changed-group in the to-be-compacted file
		 * and find the end of it, on both to-be-compacted and other file
		 * indexes (ix and ixo).
		 */
		ixs = ix;
		for (ix++; rchg[ix]; ix++);
		for (; rchgo[ixo]; ixo++);

		do {
			grpsiz = ix - ixs;

			/*
			 * If the line before the current change group, is equal to
			 * the last line of the current change group, shift backward
			 * the group.
			 */
			while (ixs > 0 && recs[ixs - 1]->ha == recs[ix - 1]->ha &&
			       XDL_RECMATCH(recs[ixs - 1], recs[ix - 1])) {
				rchg[--ixs] = 1;
				rchg[--ix] = 0;

				/*
				 * This change might have joined two change groups,
				 * so we try to take this scenario in account by moving
				 * the start index accordingly (and so the other-file
				 * end-of-group index).
				 */
				for (; rchg[ixs - 1]; ixs--);
				while (rchgo[--ixo]);
			}

			/*
			 * Record the end-of-group position in case we are matched
			 * with a group of changes in the other file (that is, the
			 * change record before the enf-of-group index in the other
			 * file is set).
			 */
			ixref = rchgo[ixo - 1] ? ix: nrec;

			/*
			 * If the first line of the current change group, is equal to
			 * the line next of the current change group, shift forward
			 * the group.
			 */
			while (ix < nrec && recs[ixs]->ha == recs[ix]->ha &&
			       XDL_RECMATCH(recs[ixs], recs[ix])) {
				rchg[ixs++] = 0;
				rchg[ix++] = 1;

				/*
				 * This change might have joined two change groups,
				 * so we try to take this scenario in account by moving
				 * the start index accordingly (and so the other-file
				 * end-of-group index). Keep tracking the reference
				 * index in case we are shifting together with a
				 * corresponding group of changes in the other file.
				 */
				for (; rchg[ix]; ix++);
				while (rchgo[++ixo])
					ixref = ix;
			}
		} while (grpsiz != ix - ixs);

		/*
		 * Try to move back the possibly merged group of changes, to match
		 * the recorded postion in the other file.
		 */
		while (ixref < ix) {
			rchg[--ixs] = 1;
			rchg[--ix] = 0;
			while (rchgo[--ixo]);
		}
	}

	return 0;
}


int xdl_build_script(xdfenv_t *xe, xdchange_t **xscr) {
	xdchange_t *cscr = NULL, *xch;
	char *rchg1 = xe->xdf1.rchg, *rchg2 = xe->xdf2.rchg;
	long i1, i2, l1, l2;

	/*
	 * Trivial. Collects "groups" of changes and creates an edit script.
	 */
	for (i1 = xe->xdf1.nrec, i2 = xe->xdf2.nrec; i1 >= 0 || i2 >= 0; i1--, i2--)
		if (rchg1[i1 - 1] || rchg2[i2 - 1]) {
			for (l1 = i1; rchg1[i1 - 1]; i1--);
			for (l2 = i2; rchg2[i2 - 1]; i2--);

			if (!(xch = xdl_add_change(cscr, i1, i2, l1 - i1, l2 - i2))) {
				xdl_free_script(cscr);
				return -1;
			}
			cscr = xch;
		}

	*xscr = cscr;

	return 0;
}


void xdl_free_script(xdchange_t *xscr) {
	xdchange_t *xch;

	while ((xch = xscr) != NULL) {
		xscr = xscr->next;
		xdl_free(xch);
	}
}


int xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t const *xpp,
	     xdemitconf_t const *xecfg, xdemitcb_t *ecb) {
	xdchange_t *xscr;
	xdfenv_t xe;

	if (xdl_do_diff(mf1, mf2, xpp, &xe) < 0) {

		return -1;
	}
	if (xdl_change_compact(&xe.xdf1, &xe.xdf2) < 0 ||
	    xdl_change_compact(&xe.xdf2, &xe.xdf1) < 0 ||
	    xdl_build_script(&xe, &xscr) < 0) {

		xdl_free_env(&xe);
		return -1;
	}
	if (xscr) {
		if (xdl_emit_diff(&xe, xscr, ecb, xecfg) < 0) {

			xdl_free_script(xscr);
			xdl_free_env(&xe);
			return -1;
		}
		xdl_free_script(xscr);
	}
	xdl_free_env(&xe);

	return 0;
}

/* END xdiffi.c */


/* BEGIN xemit.c */
static long xdl_get_rec(xdfile_t *xdf, long ri, char const **rec) {

	*rec = xdf->recs[ri]->ptr;

	return xdf->recs[ri]->size;
}


static int xdl_emit_record(xdfile_t *xdf, long ri, char const *pre, xdemitcb_t *ecb) {
	long size, psize = strlen(pre);
	char const *rec;

	size = xdl_get_rec(xdf, ri, &rec);
	if (xdl_emit_diffrec(rec, size, pre, psize, ecb) < 0) {

		return -1;
	}

	return 0;
}


/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 */
static xdchange_t *xdl_get_hunk(xdchange_t *xscr, xdemitconf_t const *xecfg) {
	xdchange_t *xch, *xchp;

	for (xchp = xscr, xch = xscr->next; xch; xchp = xch, xch = xch->next)
		if (xch->i1 - (xchp->i1 + xchp->chg1) > 2 * xecfg->ctxlen)
			break;

	return xchp;
}


int xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t const *xecfg) {
	long s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;

	for (xch = xche = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(xch, xecfg);

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

		lctx = xecfg->ctxlen;
		lctx = XDL_MIN(lctx, xe->xdf1.nrec - (xche->i1 + xche->chg1));
		lctx = XDL_MIN(lctx, xe->xdf2.nrec - (xche->i2 + xche->chg2));

		e1 = xche->i1 + xche->chg1 + lctx;
		e2 = xche->i2 + xche->chg2 + lctx;

		/*
		 * Emit current hunk header.
		 */
		if (xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2, ecb) < 0)
			return -1;

		/*
		 * Emit pre-context.
		 */
		for (; s1 < xch->i1; s1++)
			if (xdl_emit_record(&xe->xdf1, s1, " ", ecb) < 0)
				return -1;

		for (s1 = xch->i1, s2 = xch->i2;; xch = xch->next) {
			/*
			 * Merge previous with current change atom.
			 */
			for (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				if (xdl_emit_record(&xe->xdf1, s1, " ", ecb) < 0)
					return -1;

			/*
			 * Removes lines from the first file.
			 */
			for (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				if (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					return -1;

			/*
			 * Adds lines from the second file.
			 */
			for (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				if (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					return -1;

			if (xch == xche)
				break;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		for (s1 = xche->i1 + xche->chg1; s1 < e1; s1++)
			if (xdl_emit_record(&xe->xdf1, s1, " ", ecb) < 0)
				return -1;
	}

	return 0;
}

/* END xemit.c */


/* BEGIN xmerge3.c */

#define XDL_MERGE3_BLKSIZE (1024 * 8)
#define XDL_MERGE3_CTXLEN 3



int xdl_merge3(mmfile_t *mmfo, mmfile_t *mmf1, mmfile_t *mmf2, xdemitcb_t *ecb,
	       xdemitcb_t *rjecb) {
	xpparam_t xpp;
	xdemitconf_t xecfg;
	xdemitcb_t xecb;
	mmfile_t mmfp;

	if (xdl_init_mmfile(&mmfp, XDL_MERGE3_BLKSIZE, XDL_MMF_ATOMIC) < 0) {

		return -1;
	}

	xpp.flags = 0;

	xecfg.ctxlen = XDL_MERGE3_CTXLEN;

	xecb.priv = &mmfp;
	xecb.outf = xdl_mmfile_outf;

	if (xdl_diff(mmfo, mmf2, &xpp, &xecfg, &xecb) < 0) {

		xdl_free_mmfile(&mmfp);
		return -1;
	}

	if (xdl_patch(mmf1, &mmfp, XDL_PATCH_NORMAL, ecb, rjecb) < 0) {

		xdl_free_mmfile(&mmfp);
		return -1;
	}

	xdl_free_mmfile(&mmfp);

	return 0;
}

/* END xmerge3.c */


/* BEGIN missing.c */

#if !defined(HAVE_MEMCHR)

void *memchr(void const *p, int c, long n) {
	char const *pc = p;

	for (; n; n--, pc++)
		if (*pc == (char) c)
			return (void*)pc;
	return NULL;
}

#endif /* #if !defined(HAVE_MEMCHR) */


#if !defined(HAVE_MEMCMP)

int memcmp(void const *p1, void const *p2, long n) {
	char const *pc1 = p1, *pc2 = p2;

	for (; n; n--, pc1++, pc2++)
		if (*pc1 != *pc2)
			return *pc1 - *pc2;
	return 0;
}

#endif /* #if !defined(HAVE_MEMCMP) */


#if !defined(HAVE_MEMCPY)

void *memcpy(void *d, void const *s, long n) {
	char *dc = d;
	char const *sc = s;

	for (; n; n--, dc++, sc++)
		*dc = *sc;
	return d;
}

#endif /* #if !defined(HAVE_MEMCPY) */


#if !defined(HAVE_MEMSET)

void *memset(void *d, int c, long n) {
	char *dc = d;

	for (; n; n--, dc++)
		*dc = (char) c;
	return d;
}

#endif /* #if !defined(HAVE_MEMSET) */


#if !defined(HAVE_STRLEN)

long strlen(char const *s) {
	char const *tmp;

	for (tmp = s; *s; s++);
	return (long) (s - tmp);
}

#endif /* #if !defined(HAVE_STRLEN) */

/* END xmissing.c */


/* BEGIN xpatchi.c */

#define XDL_MAX_FUZZ 3
#define XDL_MIN_SYNCLINES 4



typedef struct s_recinfo {
	char const *ptr;
	long size;
} recinfo_t;

typedef struct s_recfile {
	mmfile_t *mf;
	long nrec;
	recinfo_t *recs;
} recfile_t;

typedef struct s_hunkinfo {
	long s1, s2;
	long c1, c2;
	long cmn, radd, rdel, pctx, sctx;
} hunkinfo_t;

typedef struct s_patchstats {
	long adds, dels;
} patchstats_t;

typedef struct s_patch {
	recfile_t rf;
	hunkinfo_t hi;
	long hkrec;
	long hklen;
	long flags;
	patchstats_t ps;
	int fuzzies;
} patch_t;




static int xdl_load_hunk_info(char const *line, long size, hunkinfo_t *hki);
static int xdl_init_recfile(mmfile_t *mf, int ispatch, recfile_t *rf);
static void xdl_free_recfile(recfile_t *rf);
static char const *xdl_recfile_get(recfile_t *rf, long irec, long *size);
static int xdl_init_patch(mmfile_t *mf, long flags, patch_t *pch);
static void xdl_free_patch(patch_t *pch);
static int xdl_load_hunk(patch_t *pch, long hkrec);
static int xdl_first_hunk(patch_t *pch);
static int xdl_next_hunk(patch_t *pch);
static int xdl_line_match(patch_t *pch, const char *s, long ns, char const *m, long nm);
static int xdl_hunk_match(recfile_t *rf, long irec, patch_t *pch, int mode, int fuzz);
static int xdl_find_hunk(recfile_t *rf, long ibase, patch_t *pch, int mode,
			 int fuzz, long *hkpos, int *exact);
static int xdl_emit_rfile_line(recfile_t *rf, long line, xdemitcb_t *ecb);
static int xdl_flush_section(recfile_t *rf, long start, long top, xdemitcb_t *ecb);
static int xdl_apply_hunk(recfile_t *rf, long hkpos, patch_t *pch, int mode,
			  long *ibase, xdemitcb_t *ecb);
static int xdl_reject_hunk(recfile_t *rf, patch_t *pch, int mode,
			   xdemitcb_t *rjecb);
static int xdl_process_hunk(recfile_t *rff, patch_t *pch, long *ibase, int mode,
			    xdemitcb_t *ecb, xdemitcb_t *rjecb);




static int xdl_load_hunk_info(char const *line, long size, hunkinfo_t *hki) {
	char const *next;

	/*
	 * The diff header format should be:
	 *
	 *   @@ -OP,OC +NP,NC @@
	 *
	 * Unfortunately some software avoid to emit OP or/and NP in case
	 * of not existing old or new file (it should be mitted as zero).
	 * We need to handle both syntaxes.
	 */
	if (memcmp(line, "@@ -", 4))
		return -1;
	line += 4;
	size -= 4;

	if (!size || !XDL_ISDIGIT(*line))
		return -1;
	hki->s1 = xdl_atol(line, &next);
	size -= next - line;
	line = next;
	if (!size)
		return -1;
	if (*line == ',') {
		size--, line++;
		if (!size || !XDL_ISDIGIT(*line))
			return -1;
		hki->c1 = xdl_atol(line, &next);
		size -= next - line;
		line = next;
		if (!size || *line != ' ')
			return -1;
		size--, line++;
	} else if (*line == ' ') {
		size--, line++;
		hki->c1 = hki->s1;
		hki->s1 = 0;
	} else
		return -1;

	if (!size || *line != '+')
		return -1;
	size--, line++;
	if (!size || !XDL_ISDIGIT(*line))
		return -1;
	hki->s2 = xdl_atol(line, &next);
	size -= next - line;
	line = next;
	if (!size)
		return -1;
	if (*line == ',') {
		size--, line++;
		if (!size || !XDL_ISDIGIT(*line))
			return -1;
		hki->c2 = xdl_atol(line, &next);
		size -= next - line;
		line = next;
		if (!size || *line != ' ')
			return -1;
		size--, line++;
	} else if (*line == ' ') {
		size--, line++;
		hki->c2 = hki->s2;
		hki->s2 = 0;
	} else
		return -1;
	if (size < 2 || memcmp(line, "@@", 2) != 0)
		return -1;

	/*
	 * We start from zero, so decrement by one unless it's the special position
	 * '0' inside the unified diff (new or deleted file).
	 */
	if (hki->s1 > 0 && hki->c1 > 0)
		hki->s1--;
	if (hki->s2 > 0 && hki->c2 > 0)
		hki->s2--;

	return 0;
}


static int xdl_init_recfile(mmfile_t *mf, int ispatch, recfile_t *rf) {
	long narec, nrec, bsize;
	recinfo_t *recs, *rrecs;
	char const *blk, *cur, *top, *eol;

	narec = xdl_guess_lines(mf);
	if (!(recs = (recinfo_t *) xdl_malloc(narec * sizeof(recinfo_t)))) {

		return -1;
	}
	nrec = 0;
	if ((cur = blk = (char const*) xdl_mmfile_first(mf, &bsize)) != NULL) {
		for (top = blk + bsize;;) {
			if (cur >= top) {
				if (!(cur = blk = (char const*) xdl_mmfile_next(mf, &bsize)))
					break;
				top = blk + bsize;
			}
			if (nrec >= narec) {
				narec *= 2;
				if (!(rrecs = (recinfo_t *)
				      xdl_realloc(recs, narec * sizeof(recinfo_t)))) {

					xdl_free(recs);
					return -1;
				}
				recs = rrecs;
			}
			recs[nrec].ptr = cur;
			if (!(eol = (char const*) memchr(cur, '\n', top - cur)))
				eol = top - 1;
			recs[nrec].size = (long) (eol - cur) + 1;
			if (ispatch && *cur == '\\' && nrec > 0 && recs[nrec - 1].size > 0 &&
			    recs[nrec - 1].ptr[recs[nrec - 1].size - 1] == '\n')
				recs[nrec - 1].size--;
			else
				nrec++;
			cur = eol + 1;
		}
	}
	rf->mf = mf;
	rf->nrec = nrec;
	rf->recs = recs;

	return 0;
}


static void xdl_free_recfile(recfile_t *rf) {

	xdl_free(rf->recs);
}


static char const *xdl_recfile_get(recfile_t *rf, long irec, long *size) {

	if (irec < 0 || irec >= rf->nrec)
		return NULL;
	*size = rf->recs[irec].size;

	return rf->recs[irec].ptr;
}


static int xdl_init_patch(mmfile_t *mf, long flags, patch_t *pch) {

	if (xdl_init_recfile(mf, 1, &pch->rf) < 0) {

		return -1;
	}
	pch->hkrec = 0;
	pch->hklen = 0;
	pch->flags = flags;
	pch->ps.adds = pch->ps.dels = 0;
	pch->fuzzies = 0;

	return 0;
}


static void xdl_free_patch(patch_t *pch) {

	xdl_free_recfile(&pch->rf);
}


static int xdl_load_hunk(patch_t *pch, long hkrec) {
	long size, i, nb;
	char const *line;

	for (;; hkrec++) {
		pch->hkrec = hkrec;
		if (!(line = xdl_recfile_get(&pch->rf, pch->hkrec, &size)))
			return 0;
		if (*line == '@')
			break;
	}
	if (xdl_load_hunk_info(line, size, &pch->hi) < 0) {

		return -1;
	}
	pch->hi.cmn = pch->hi.radd = pch->hi.rdel = pch->hi.pctx = pch->hi.sctx = 0;
	for (i = pch->hkrec + 1, nb = 0;
	     (line = xdl_recfile_get(&pch->rf, i, &size)) != NULL; i++) {
		if (*line == '@' || *line == '\n')
			break;
		if (*line == ' ') {
			nb++;
			pch->hi.cmn++;
		} else if (*line == '+') {
			if (pch->hi.radd + pch->hi.rdel == 0)
				pch->hi.pctx = nb;
			nb = 0;
			pch->hi.radd++;
		} else if (*line == '-') {
			if (pch->hi.radd + pch->hi.rdel == 0)
				pch->hi.pctx = nb;
			nb = 0;
			pch->hi.rdel++;
		} else {

			return -1;
		}
	}
	pch->hi.sctx = nb;
	if (pch->hi.cmn + pch->hi.radd != pch->hi.c2 ||
	    pch->hi.cmn + pch->hi.rdel != pch->hi.c1) {

		return -1;
	}
	pch->hklen = i - pch->hkrec - 1;

	return 1;
}


static int xdl_first_hunk(patch_t *pch) {

	return xdl_load_hunk(pch, 0);
}


static int xdl_next_hunk(patch_t *pch) {

	return xdl_load_hunk(pch, pch->hkrec + pch->hklen + 1);
}


static int xdl_line_match(patch_t *pch, const char *s, long ns, char const *m, long nm) {

	for (; ns > 0 && (s[ns - 1] == '\r' || s[ns - 1] == '\n'); ns--);
	for (; nm > 0 && (m[nm - 1] == '\r' || m[nm - 1] == '\n'); nm--);
	if (pch->flags & XDL_PATCH_IGNOREBSPACE) {
		for (; ns > 0 && (*s == ' ' || *s == '\t'); ns--, s++);
		for (; ns > 0 && (s[ns - 1] == ' ' || s[ns - 1] == '\t'); ns--);
		for (; nm > 0 && (*m == ' ' || *m == '\t'); nm--, m++);
		for (; nm > 0 && (m[nm - 1] == ' ' || m[nm - 1] == '\t'); nm--);
	}

	return ns == nm && memcmp(s, m, ns) == 0;
}


static int xdl_hunk_match(recfile_t *rf, long irec, patch_t *pch, int mode, int fuzz) {
	long i, j, z, fsize, psize, ptop, pfuzz, sfuzz, misses;
	char const *fline, *pline;

	/*
	 * Limit fuzz to not be greater than the prefix and suffix context.
	 */
	pfuzz = fuzz < pch->hi.pctx ? fuzz: pch->hi.pctx;
	sfuzz = fuzz < pch->hi.sctx ? fuzz: pch->hi.sctx;

	/*
	 * First loop through the prefix fuzz area. In this loop we simply
	 * note mismatching lines. We allow missing lines here, that is,
	 * some prefix context lines are missing.
	 */
	for (z = pfuzz, misses = 0, i = irec, j = pch->hkrec + 1,
	     ptop = pch->hkrec + 1 + pch->hklen - sfuzz;
	     z > 0 && i < rf->nrec && j < ptop; i++, j++, z--) {
		if (!(pline = xdl_recfile_get(&pch->rf, j, &psize)))
			return 0;
		if (!(fline = xdl_recfile_get(rf, i, &fsize)) ||
		    !xdl_line_match(pch, fline, fsize, pline + 1, psize - 1))
			misses++;
	}
	if (misses > fuzz)
		return 0;

	/*
	 * Strict match loop.
	 */
	for (; i < rf->nrec && j < ptop; i++, j++) {
		for (; j < ptop; j++) {
			if (!(pline = xdl_recfile_get(&pch->rf, j, &psize)))
				return 0;
			if (*pline == ' ' || *pline == mode)
				break;
		}
		if (j == ptop)
			break;
		if (!(fline = xdl_recfile_get(rf, i, &fsize)) ||
		    !xdl_line_match(pch, fline, fsize, pline + 1, psize - 1))
			return 0;
	}
	for (; j < ptop; j++)
		if (!(pline = xdl_recfile_get(&pch->rf, j, &psize)) ||
		    *pline == ' ' || *pline == mode)
			return 0;

	/*
	 * Finally loop through the suffix fuzz area. In this loop we simply
	 * note mismatching lines. We allow missing lines here, that is,
	 * some suffix context lines are missing.
	 */
	for (z = sfuzz; z > 0 && i < rf->nrec; i++, j++, z--) {
		if (!(pline = xdl_recfile_get(&pch->rf, j, &psize)))
			return 0;
		if (!(fline = xdl_recfile_get(rf, i, &fsize)) ||
		    !xdl_line_match(pch, fline, fsize, pline + 1, psize - 1))
			misses++;
	}

	return misses <= fuzz;
}


static int xdl_find_hunk(recfile_t *rf, long ibase, patch_t *pch, int mode,
			 int fuzz, long *hkpos, int *exact) {
	long hpos, hlen, i, j;
	long pos[2];

	hpos = mode == '-' ? pch->hi.s1: pch->hi.s2;
	hlen = mode == '-' ? pch->hi.cmn + pch->hi.rdel: pch->hi.cmn + pch->hi.radd;
	if (xdl_hunk_match(rf, hpos, pch, mode, fuzz)) {
		*hkpos = hpos;
		*exact = 1;
		return 1;
	}
	for (i = 1;; i++) {
		/*
		 * We allow a negative starting hunk position, up to the
		 * number of prefix context lines.
		 */
		j = 0;
		if (hpos - i >= ibase - pch->hi.pctx)
			pos[j++] = hpos - i;
		if (hpos + i + hlen <= rf->nrec)
			pos[j++] = hpos + i;
		if (!j)
			break;
		for (j--; j >= 0; j--)
			if (xdl_hunk_match(rf, pos[j], pch, mode, fuzz)) {
				*hkpos = pos[j];
				*exact = 0;
				return 1;
			}
	}

	return 0;
}


static int xdl_emit_rfile_line(recfile_t *rf, long line, xdemitcb_t *ecb) {
	mmbuffer_t mb;

	if (!(mb.ptr = (char *) xdl_recfile_get(rf, line, &mb.size)) ||
	    ecb->outf(ecb->priv, &mb, 1) < 0) {

		return -1;
	}

	return 0;
}


static int xdl_flush_section(recfile_t *rf, long start, long top, xdemitcb_t *ecb) {
	long i;

	for (i = start; i <= top; i++) {
		if (xdl_emit_rfile_line(rf, i, ecb) < 0) {

			return -1;
		}
	}

	return 0;
}


static int xdl_apply_hunk(recfile_t *rf, long hkpos, patch_t *pch, int mode,
			  long *ibase, xdemitcb_t *ecb) {
	long j, psize, ptop;
	char const *pline;
	mmbuffer_t mb;

	/*
	 * The hunk starting position (hkpos) can be negative, up to the number
	 * of prefix context lines. Since this function only emit the core of
	 * the hunk (the remaining lines are flushed by xdl_flush_section() calls)
	 * we need to normalize it by adding the number of prefix context lines.
	 * The normalized value of the starting position is then greater/equal
	 * to zero.
	 */
	hkpos += pch->hi.pctx;
	if (xdl_flush_section(rf, *ibase, hkpos - 1, ecb) < 0) {

		return -1;
	}
	*ibase = hkpos;
	for (j = pch->hkrec + 1 + pch->hi.pctx,
	     ptop = pch->hkrec + 1 + pch->hklen - pch->hi.sctx; j < ptop; j++) {
		if (!(pline = xdl_recfile_get(&pch->rf, j, &psize))) {

			return -1;
		}
		if (*pline == ' ') {
			if (xdl_emit_rfile_line(rf, *ibase, ecb) < 0) {

				return -1;
			}
			(*ibase)++;
		} else if (*pline != mode) {
			mb.ptr = (char *) pline + 1;
			mb.size = psize - 1;
			if (ecb->outf(ecb->priv, &mb, 1) < 0) {

				return -1;
			}
			pch->ps.adds++;
		} else {
			(*ibase)++;
			pch->ps.dels++;
		}
	}

	return 0;
}


static int xdl_reject_hunk(recfile_t *rf, patch_t *pch, int mode,
			   xdemitcb_t *rjecb) {
	long i, size, s1, s2, c1, c2;
	char const *line, *pre;
	mmbuffer_t mb;

	if (mode == '-') {
		s1 = pch->hi.s1;
		s2 = pch->hi.s2;
		c1 = pch->hi.c1;
		c2 = pch->hi.c2;
	} else {
		s1 = pch->hi.s2;
		s2 = pch->hi.s1;
		c1 = pch->hi.c2;
		c2 = pch->hi.c1;
	}
	s1 += pch->ps.adds - pch->ps.dels;
	if (xdl_emit_hunk_hdr(s1 + 1, c1, s2 + 1, c2, rjecb) < 0) {

		return -1;
	}
	for (i = pch->hkrec + 1;
	     (line = xdl_recfile_get(&pch->rf, i, &size)) != NULL; i++) {
		if (*line == '@' || *line == '\n')
			break;
		if (mode == '-' || *line == ' ') {
			mb.ptr = (char *) line;
			mb.size = size;
			if (rjecb->outf(rjecb->priv, &mb, 1) < 0) {

				return -1;
			}
		} else {
			pre = *line == '+' ? "-": "+";
			if (xdl_emit_diffrec(line + 1, size - 1, pre, strlen(pre),
					     rjecb) < 0) {

				return -1;
			}
		}
	}

	return 0;
}


static int xdl_process_hunk(recfile_t *rff, patch_t *pch, long *ibase, int mode,
			    xdemitcb_t *ecb, xdemitcb_t *rjecb) {
	int fuzz, exact, hlen, maxfuzz;
	long hkpos;

	hlen = mode == '-' ? pch->hi.cmn + pch->hi.rdel: pch->hi.cmn + pch->hi.radd;
	maxfuzz = XDL_MAX_FUZZ;
	if (hlen - maxfuzz < XDL_MIN_SYNCLINES)
		maxfuzz = hlen - XDL_MIN_SYNCLINES;
	if (maxfuzz < 0)
		maxfuzz = 0;
	for (fuzz = 0; fuzz <= maxfuzz; fuzz++) {
		if (xdl_find_hunk(rff, *ibase, pch, mode, fuzz,
				  &hkpos, &exact)) {
			if (xdl_apply_hunk(rff, hkpos, pch, mode,
					   ibase, ecb) < 0) {

				return -1;
			}
			if (!exact || fuzz)
				pch->fuzzies++;

			return 0;
		}
	}
	if (xdl_reject_hunk(rff, pch, mode, rjecb) < 0) {

		return -1;
	}

	return 0;
}


int xdl_patch(mmfile_t *mf, mmfile_t *mfp, int mode, xdemitcb_t *ecb,
	      xdemitcb_t *rjecb) {
	int hkres, exact;
	long hkpos, ibase;
	recfile_t rff;
	patch_t pch;

	if (xdl_init_recfile(mf, 0, &rff) < 0) {

		return -1;
	}
	if (xdl_init_patch(mfp, mode & ~XDL_PATCH_MODEMASK, &pch) < 0) {

		xdl_free_recfile(&rff);
		return -1;
	}
	mode &= XDL_PATCH_MODEMASK;
	ibase = 0;
	if ((hkres = xdl_first_hunk(&pch)) > 0) {
		do {
			if (xdl_process_hunk(&rff, &pch, &ibase, mode,
					     ecb, rjecb) < 0) {
				xdl_free_patch(&pch);
				xdl_free_recfile(&rff);
				return -1;
			}
		} while ((hkres = xdl_next_hunk(&pch)) > 0);
	}
	if (hkres < 0) {

		xdl_free_patch(&pch);
		xdl_free_recfile(&rff);
		return -1;
	}
	if (xdl_flush_section(&rff, ibase, rff.nrec - 1, ecb) < 0) {

		xdl_free_patch(&pch);
		xdl_free_recfile(&rff);
		return -1;
	}
	xdl_free_patch(&pch);
	xdl_free_recfile(&rff);

	return pch.fuzzies;
}

/* END xpatchi.c */


/* BEGIN xprepare.c */

#define XDL_KPDIS_RUN 4
#define XDL_MAX_EQLIMIT 1024
#define XDL_SIMSCAN_WINDOWN 100


typedef struct s_xdlclass {
	struct s_xdlclass *next;
	unsigned long ha;
	char const *line;
	long size;
	long idx;
} xdlclass_t;

typedef struct s_xdlclassifier {
	unsigned int hbits;
	long hsize;
	xdlclass_t **rchash;
	chastore_t ncha;
	long count;
} xdlclassifier_t;



static int xdl_init_classifier(xdlclassifier_t *cf, long size) {
	long i;

	cf->hbits = xdl_hashbits((unsigned int) size);
	cf->hsize = 1 << cf->hbits;

	if (xdl_cha_init(&cf->ncha, sizeof(xdlclass_t), size / 4 + 1) < 0) {

		return -1;
	}
	if (!(cf->rchash = (xdlclass_t **) xdl_malloc(cf->hsize * sizeof(xdlclass_t *)))) {

		xdl_cha_free(&cf->ncha);
		return -1;
	}
	for (i = 0; i < cf->hsize; i++)
		cf->rchash[i] = NULL;

	cf->count = 0;

	return 0;
}


static void xdl_free_classifier(xdlclassifier_t *cf) {
	xdl_free(cf->rchash);
	xdl_cha_free(&cf->ncha);
}


static int xdl_classify_record(xdlclassifier_t *cf, xrecord_t **rhash, unsigned int hbits,
			       xrecord_t *rec) {
	long hi;
	char const *line;
	xdlclass_t *rcrec;

	line = rec->ptr;
	hi = (long) XDL_HASHLONG(rec->ha, cf->hbits);
	for (rcrec = cf->rchash[hi]; rcrec; rcrec = rcrec->next)
		if (rcrec->ha == rec->ha && rcrec->size == rec->size &&
		    !memcmp(line, rcrec->line, rec->size))
			break;

	if (!rcrec) {
		if (!(rcrec = (xdlclass_t*) xdl_cha_alloc(&cf->ncha))) {

			return -1;
		}
		rcrec->idx = cf->count++;
		rcrec->line = line;
		rcrec->size = rec->size;
		rcrec->ha = rec->ha;
		rcrec->next = cf->rchash[hi];
		cf->rchash[hi] = rcrec;
	}

	rec->ha = (unsigned long) rcrec->idx;

	hi = (long) XDL_HASHLONG(rec->ha, hbits);
	rec->next = rhash[hi];
	rhash[hi] = rec;

	return 0;
}


static int xdl_prepare_ctx(mmfile_t *mf, long narec, xpparam_t const *xpp,
			   xdlclassifier_t *cf, xdfile_t *xdf) {
	unsigned int hbits;
	long i, nrec, hsize, bsize;
	unsigned long hav;
	char const *blk, *cur, *top, *prev;
	xrecord_t *crec;
	xrecord_t **recs, **rrecs;
	xrecord_t **rhash;
	unsigned long *ha;
	char *rchg;
	long *rindex;

	if (xdl_cha_init(&xdf->rcha, sizeof(xrecord_t), narec / 4 + 1) < 0) {

		return -1;
	}
	if (!(recs = (xrecord_t **) xdl_malloc(narec * sizeof(xrecord_t *)))) {

		xdl_cha_free(&xdf->rcha);
		return -1;
	}

	hbits = xdl_hashbits((unsigned int) narec);
	hsize = 1 << hbits;
	if (!(rhash = (xrecord_t **) xdl_malloc(hsize * sizeof(xrecord_t *)))) {

		xdl_free(recs);
		xdl_cha_free(&xdf->rcha);
		return -1;
	}
	for (i = 0; i < hsize; i++)
		rhash[i] = NULL;

	nrec = 0;
	if ((cur = blk = (char const*) xdl_mmfile_first(mf, &bsize)) != NULL) {
		for (top = blk + bsize;;) {
			if (cur >= top) {
				if (!(cur = blk = (char const*) xdl_mmfile_next(mf, &bsize)))
					break;
				top = blk + bsize;
			}
			prev = cur;
			hav = xdl_hash_record(&cur, top);
			if (nrec >= narec) {
				narec *= 2;
				if (!(rrecs = (xrecord_t **) xdl_realloc(recs, narec * sizeof(xrecord_t *)))) {

					xdl_free(rhash);
					xdl_free(recs);
					xdl_cha_free(&xdf->rcha);
					return -1;
				}
				recs = rrecs;
			}
			if (!(crec = (xrecord_t*) xdl_cha_alloc(&xdf->rcha))) {

				xdl_free(rhash);
				xdl_free(recs);
				xdl_cha_free(&xdf->rcha);
				return -1;
			}
			crec->ptr = prev;
			crec->size = (long) (cur - prev);
			crec->ha = hav;
			recs[nrec++] = crec;

			if (xdl_classify_record(cf, rhash, hbits, crec) < 0) {

				xdl_free(rhash);
				xdl_free(recs);
				xdl_cha_free(&xdf->rcha);
				return -1;
			}
		}
	}

	if (!(rchg = (char *) xdl_malloc(nrec + 2))) {

		xdl_free(rhash);
		xdl_free(recs);
		xdl_cha_free(&xdf->rcha);
		return -1;
	}
	memset(rchg, 0, nrec + 2);

	if (!(rindex = (long *) xdl_malloc((nrec + 1) * sizeof(long)))) {

		xdl_free(rchg);
		xdl_free(rhash);
		xdl_free(recs);
		xdl_cha_free(&xdf->rcha);
		return -1;
	}
	if (!(ha = (unsigned long *) xdl_malloc((nrec + 1) * sizeof(unsigned long)))) {

		xdl_free(rindex);
		xdl_free(rchg);
		xdl_free(rhash);
		xdl_free(recs);
		xdl_cha_free(&xdf->rcha);
		return -1;
	}

	xdf->nrec = nrec;
	xdf->recs = recs;
	xdf->hbits = hbits;
	xdf->rhash = rhash;
	xdf->rchg = rchg + 1;
	xdf->rindex = rindex;
	xdf->nreff = 0;
	xdf->ha = ha;
	xdf->dstart = 0;
	xdf->dend = nrec - 1;

	return 0;
}


static void xdl_free_ctx(xdfile_t *xdf) {
	xdl_free(xdf->rhash);
	xdl_free(xdf->rindex);
	xdl_free(xdf->rchg - 1);
	xdl_free(xdf->ha);
	xdl_free(xdf->recs);
	xdl_cha_free(&xdf->rcha);
}


static int xdl_clean_mmatch(char const *dis, long i, long s, long e) {
	long r, rdis0, rpdis0, rdis1, rpdis1;

	/*
	 * Limits the window the is examined during the similar-lines
	 * scan. The loops below stops when dis[i - r] == 1 (line that
	 * has no match), but there are corner cases where the loop
	 * proceed all the way to the extremities by causing huge
	 * performance penalties in case of big files.
	 */
	if (i - s > XDL_SIMSCAN_WINDOWN)
		s = i - XDL_SIMSCAN_WINDOWN;
	if (e - i > XDL_SIMSCAN_WINDOWN)
		e = i + XDL_SIMSCAN_WINDOWN;

	/*
	 * Scans the lines before 'i' to find a run of lines that either
	 * have no match (dis[j] == 0) or have multiple matches (dis[j] > 1).
	 * Note that we always call this function with dis[i] > 1, so the
	 * current line (i) is already a multimatch line.
	 */
	for (r = 1, rdis0 = 0, rpdis0 = 1; (i - r) >= s; r++) {
		if (!dis[i - r])
			rdis0++;
		else if (dis[i - r] == 2)
			rpdis0++;
		else
			break;
	}
	/*
	 * If the run before the line 'i' found only multimatch lines, we
	 * return 0 and hence we don't make the current line (i) discarded.
	 * We want to discard multimatch lines only when they appear in the
	 * middle of runs with nomatch lines (dis[j] == 0).
	 */
	if (rdis0 == 0)
		return 0;
	for (r = 1, rdis1 = 0, rpdis1 = 1; (i + r) <= e; r++) {
		if (!dis[i + r])
			rdis1++;
		else if (dis[i + r] == 2)
			rpdis1++;
		else
			break;
	}
	/*
	 * If the run after the line 'i' found only multimatch lines, we
	 * return 0 and hence we don't make the current line (i) discarded.
	 */
	if (rdis1 == 0)
		return 0;
	rdis1 += rdis0;
	rpdis1 += rpdis0;

	return rpdis1 * XDL_KPDIS_RUN < (rpdis1 + rdis1);
}


/*
 * Try to reduce the problem complexity, discard records that have no
 * matches on the other file. Also, lines that have multiple matches
 * might be potentially discarded if they happear in a run of discardable.
 */
static int xdl_cleanup_records(xdfile_t *xdf1, xdfile_t *xdf2) {
	long i, nm, rhi, nreff, mlim;
	unsigned long hav;
	xrecord_t **recs;
	xrecord_t *rec;
	char *dis, *dis1, *dis2;

	if (!(dis = (char *) xdl_malloc(xdf1->nrec + xdf2->nrec + 2))) {

		return -1;
	}
	memset(dis, 0, xdf1->nrec + xdf2->nrec + 2);
	dis1 = dis;
	dis2 = dis1 + xdf1->nrec + 1;

	if ((mlim = xdl_bogosqrt(xdf1->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	for (i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart]; i <= xdf1->dend; i++, recs++) {
		hav = (*recs)->ha;
		rhi = (long) XDL_HASHLONG(hav, xdf2->hbits);
		for (nm = 0, rec = xdf2->rhash[rhi]; rec; rec = rec->next)
			if (rec->ha == hav && ++nm == mlim)
				break;
		dis1[i] = (nm == 0) ? 0: (nm >= mlim) ? 2: 1;
	}

	if ((mlim = xdl_bogosqrt(xdf2->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	for (i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart]; i <= xdf2->dend; i++, recs++) {
		hav = (*recs)->ha;
		rhi = (long) XDL_HASHLONG(hav, xdf1->hbits);
		for (nm = 0, rec = xdf1->rhash[rhi]; rec; rec = rec->next)
			if (rec->ha == hav && ++nm == mlim)
				break;
		dis2[i] = (nm == 0) ? 0: (nm >= mlim) ? 2: 1;
	}

	for (nreff = 0, i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart];
	     i <= xdf1->dend; i++, recs++) {
		if (dis1[i] == 1 ||
		    (dis1[i] == 2 && !xdl_clean_mmatch(dis1, i, xdf1->dstart, xdf1->dend))) {
			xdf1->rindex[nreff] = i;
			xdf1->ha[nreff] = (*recs)->ha;
			nreff++;
		} else
			xdf1->rchg[i] = 1;
	}
	xdf1->nreff = nreff;

	for (nreff = 0, i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart];
	     i <= xdf2->dend; i++, recs++) {
		if (dis2[i] == 1 ||
		    (dis2[i] == 2 && !xdl_clean_mmatch(dis2, i, xdf2->dstart, xdf2->dend))) {
			xdf2->rindex[nreff] = i;
			xdf2->ha[nreff] = (*recs)->ha;
			nreff++;
		} else
			xdf2->rchg[i] = 1;
	}
	xdf2->nreff = nreff;

	xdl_free(dis);

	return 0;
}


/*
 * Early trim initial and terminal matching records.
 */
static int xdl_trim_ends(xdfile_t *xdf1, xdfile_t *xdf2) {
	long i, lim;
	xrecord_t **recs1, **recs2;

	recs1 = xdf1->recs;
	recs2 = xdf2->recs;
	for (i = 0, lim = XDL_MIN(xdf1->nrec, xdf2->nrec); i < lim;
	     i++, recs1++, recs2++)
		if ((*recs1)->ha != (*recs2)->ha)
			break;

	xdf1->dstart = xdf2->dstart = i;

	recs1 = xdf1->recs + xdf1->nrec - 1;
	recs2 = xdf2->recs + xdf2->nrec - 1;
	for (lim -= i, i = 0; i < lim; i++, recs1--, recs2--)
		if ((*recs1)->ha != (*recs2)->ha)
			break;

	xdf1->dend = xdf1->nrec - i - 1;
	xdf2->dend = xdf2->nrec - i - 1;

	return 0;
}


static int xdl_optimize_ctxs(xdfile_t *xdf1, xdfile_t *xdf2) {
	if (xdl_trim_ends(xdf1, xdf2) < 0 ||
	    xdl_cleanup_records(xdf1, xdf2) < 0) {

		return -1;
	}

	return 0;
}


int xdl_prepare_env(mmfile_t *mf1, mmfile_t *mf2, xpparam_t const *xpp,
		    xdfenv_t *xe) {
	long enl1, enl2;
	xdlclassifier_t cf;

	enl1 = xdl_guess_lines(mf1) + 1;
	enl2 = xdl_guess_lines(mf2) + 1;

	if (xdl_init_classifier(&cf, enl1 + enl2 + 1) < 0) {

		return -1;
	}

	if (xdl_prepare_ctx(mf1, enl1, xpp, &cf, &xe->xdf1) < 0) {

		xdl_free_classifier(&cf);
		return -1;
	}
	if (xdl_prepare_ctx(mf2, enl2, xpp, &cf, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		return -1;
	}

	xdl_free_classifier(&cf);

	if (xdl_optimize_ctxs(&xe->xdf1, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf2);
		xdl_free_ctx(&xe->xdf1);
		return -1;
	}

	return 0;
}


void xdl_free_env(xdfenv_t *xe) {
	xdl_free_ctx(&xe->xdf2);
	xdl_free_ctx(&xe->xdf1);
}

/* END xprepare.c */



/* BEGIN xrabdiff.c */

/*
 *  xrabdiff by Davide Libenzi (Rabin's polynomial fingerprint based delta generator)
 *  Copyright (C) 2006  Davide Libenzi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 *
 *  Hints, ideas and code for the implementation came from:
 *
 *  Rabin's original paper: http://www.xmailserver.org/rabin.pdf
 *  Chan & Lu's paper:      http://www.xmailserver.org/rabin_impl.pdf
 *  Broder's paper:         http://www.xmailserver.org/rabin_apps.pdf
 *  LBFS source code:       http://www.fs.net/sfswww/lbfs/
 *  Geert Bosch's post:     http://marc.theaimsgroup.com/?l=git&m=114565424620771&w=2
 *
 */

/* #include "xinclude.h" */


#if !defined(XRABPLY_TYPE32) && !defined(XRABPLY_TYPE64)
#define XRABPLY_TYPE64 long long
#define XV64(v) ((xply_word) v ## ULL)
#endif

//#include "xrabply.c"
/* BEGIN xrabply.c */

#if defined(XRABPLY_TYPE64)

#if !defined(XV64)
#define XV64(v) ((xply_word) v ## ULL)
#endif

#define XRAB_ROOTPOLY XV64(0x36f7381af4d70d33)

#define XRAB_SHIFT 53
#define XRAB_WNDSIZE 20

typedef unsigned XRABPLY_TYPE64 xply_word;

static const xply_word T[256] = {
	XV64(0x0), XV64(0x36f7381af4d70d33), XV64(0x5b19482f1d791755), XV64(0x6dee7035e9ae1a66),
	XV64(0x80c5a844ce252399), XV64(0xb632905e3af22eaa), XV64(0xdbdce06bd35c34cc), XV64(0xed2bd871278b39ff),
	XV64(0x18b50899c4a4732), XV64(0x377c6893689d4a01), XV64(0x5a9218a681335067), XV64(0x6c6520bc75e45d54),
	XV64(0x814ef8cd526f64ab), XV64(0xb7b9c0d7a6b86998), XV64(0xda57b0e24f1673fe), XV64(0xeca088f8bbc17ecd),
	XV64(0x316a11338948e64), XV64(0x35e19909cc438357), XV64(0x580fe93c25ed9931), XV64(0x6ef8d126d13a9402),
	XV64(0x83d30957f6b1adfd), XV64(0xb524314d0266a0ce), XV64(0xd8ca4178ebc8baa8), XV64(0xee3d79621f1fb79b),
	XV64(0x29df19aa4dec956), XV64(0x346ac9805009c465), XV64(0x5984b9b5b9a7de03), XV64(0x6f7381af4d70d330),
	XV64(0x825859de6afbeacf), XV64(0xb4af61c49e2ce7fc), XV64(0xd94111f17782fd9a), XV64(0xefb629eb8355f0a9),
	XV64(0x62d422671291cc8), XV64(0x30da7a3c85fe11fb), XV64(0x5d340a096c500b9d), XV64(0x6bc33213988706ae),
	XV64(0x86e8ea62bf0c3f51), XV64(0xb01fd2784bdb3262), XV64(0xddf1a24da2752804), XV64(0xeb069a5756a22537),
	XV64(0x7a612afed635bfa), XV64(0x31512ab519b456c9), XV64(0x5cbf5a80f01a4caf), XV64(0x6a48629a04cd419c),
	XV64(0x8763baeb23467863), XV64(0xb19482f1d7917550), XV64(0xdc7af2c43e3f6f36), XV64(0xea8dcadecae86205),
	XV64(0x53be33549bd92ac), XV64(0x33ccdb2fbd6a9f9f), XV64(0x5e22ab1a54c485f9), XV64(0x68d59300a01388ca),
	XV64(0x85fe4b718798b135), XV64(0xb309736b734fbc06), XV64(0xdee7035e9ae1a660), XV64(0xe8103b446e36ab53),
	XV64(0x4b0b3bcd5f7d59e), XV64(0x32478ba62120d8ad), XV64(0x5fa9fb93c88ec2cb), XV64(0x695ec3893c59cff8),
	XV64(0x84751bf81bd2f607), XV64(0xb28223e2ef05fb34), XV64(0xdf6c53d706abe152), XV64(0xe99b6bcdf27cec61),
	XV64(0xc5a844ce2523990), XV64(0x3aadbc56168534a3), XV64(0x5743cc63ff2b2ec5), XV64(0x61b4f4790bfc23f6),
	XV64(0x8c9f2c082c771a09), XV64(0xba681412d8a0173a), XV64(0xd7866427310e0d5c), XV64(0xe1715c3dc5d9006f),
	XV64(0xdd1d4c57e187ea2), XV64(0x3b26ecdf8acf7391), XV64(0x56c89cea636169f7), XV64(0x603fa4f097b664c4),
	XV64(0x8d147c81b03d5d3b), XV64(0xbbe3449b44ea5008), XV64(0xd60d34aead444a6e), XV64(0xe0fa0cb45993475d),
	XV64(0xf4c255fdac6b7f4), XV64(0x39bb1d452e11bac7), XV64(0x54556d70c7bfa0a1), XV64(0x62a2556a3368ad92),
	XV64(0x8f898d1b14e3946d), XV64(0xb97eb501e034995e), XV64(0xd490c534099a8338), XV64(0xe267fd2efd4d8e0b),
	XV64(0xec775d6468cf0c6), XV64(0x38304dccb25bfdf5), XV64(0x55de3df95bf5e793), XV64(0x632905e3af22eaa0),
	XV64(0x8e02dd9288a9d35f), XV64(0xb8f5e5887c7ede6c), XV64(0xd51b95bd95d0c40a), XV64(0xe3ecada76107c939),
	XV64(0xa77c66a937b2558), XV64(0x3c80fe7067ac286b), XV64(0x516e8e458e02320d), XV64(0x6799b65f7ad53f3e),
	XV64(0x8ab26e2e5d5e06c1), XV64(0xbc455634a9890bf2), XV64(0xd1ab260140271194), XV64(0xe75c1e1bb4f01ca7),
	XV64(0xbfc96e30f31626a), XV64(0x3d0baef9fbe66f59), XV64(0x50e5decc1248753f), XV64(0x6612e6d6e69f780c),
	XV64(0x8b393ea7c11441f3), XV64(0xbdce06bd35c34cc0), XV64(0xd0207688dc6d56a6), XV64(0xe6d74e9228ba5b95),
	XV64(0x9616779abefab3c), XV64(0x3f965f635f38a60f), XV64(0x52782f56b696bc69), XV64(0x648f174c4241b15a),
	XV64(0x89a4cf3d65ca88a5), XV64(0xbf53f727911d8596), XV64(0xd2bd871278b39ff0), XV64(0xe44abf088c6492c3),
	XV64(0x8ea37f037a5ec0e), XV64(0x3e1d0feac372e13d), XV64(0x53f37fdf2adcfb5b), XV64(0x650447c5de0bf668),
	XV64(0x882f9fb4f980cf97), XV64(0xbed8a7ae0d57c2a4), XV64(0xd336d79be4f9d8c2), XV64(0xe5c1ef81102ed5f1),
	XV64(0x18b50899c4a47320), XV64(0x2e42308330737e13), XV64(0x43ac40b6d9dd6475), XV64(0x755b78ac2d0a6946),
	XV64(0x9870a0dd0a8150b9), XV64(0xae8798c7fe565d8a), XV64(0xc369e8f217f847ec), XV64(0xf59ed0e8e32f4adf),
	XV64(0x193e581058ee3412), XV64(0x2fc9600aac393921), XV64(0x4227103f45972347), XV64(0x74d02825b1402e74),
	XV64(0x99fbf05496cb178b), XV64(0xaf0cc84e621c1ab8), XV64(0xc2e2b87b8bb200de), XV64(0xf41580617f650ded),
	XV64(0x1ba3a98afc30fd44), XV64(0x2d54919008e7f077), XV64(0x40bae1a5e149ea11), XV64(0x764dd9bf159ee722),
	XV64(0x9b6601ce3215dedd), XV64(0xad9139d4c6c2d3ee), XV64(0xc07f49e12f6cc988), XV64(0xf68871fbdbbbc4bb),
	XV64(0x1a28f903607aba76), XV64(0x2cdfc11994adb745), XV64(0x4131b12c7d03ad23), XV64(0x77c6893689d4a010),
	XV64(0x9aed5147ae5f99ef), XV64(0xac1a695d5a8894dc), XV64(0xc1f41968b3268eba), XV64(0xf703217247f18389),
	XV64(0x1e984abfb58d6fe8), XV64(0x286f72a5415a62db), XV64(0x45810290a8f478bd), XV64(0x73763a8a5c23758e),
	XV64(0x9e5de2fb7ba84c71), XV64(0xa8aadae18f7f4142), XV64(0xc544aad466d15b24), XV64(0xf3b392ce92065617),
	XV64(0x1f131a3629c728da), XV64(0x29e4222cdd1025e9), XV64(0x440a521934be3f8f), XV64(0x72fd6a03c06932bc),
	XV64(0x9fd6b272e7e20b43), XV64(0xa9218a6813350670), XV64(0xc4cffa5dfa9b1c16), XV64(0xf238c2470e4c1125),
	XV64(0x1d8eebac8d19e18c), XV64(0x2b79d3b679ceecbf), XV64(0x4697a3839060f6d9), XV64(0x70609b9964b7fbea),
	XV64(0x9d4b43e8433cc215), XV64(0xabbc7bf2b7ebcf26), XV64(0xc6520bc75e45d540), XV64(0xf0a533ddaa92d873),
	XV64(0x1c05bb251153a6be), XV64(0x2af2833fe584ab8d), XV64(0x471cf30a0c2ab1eb), XV64(0x71ebcb10f8fdbcd8),
	XV64(0x9cc01361df768527), XV64(0xaa372b7b2ba18814), XV64(0xc7d95b4ec20f9272), XV64(0xf12e635436d89f41),
	XV64(0x14ef8cd526f64ab0), XV64(0x2218b4cfd2214783), XV64(0x4ff6c4fa3b8f5de5), XV64(0x7901fce0cf5850d6),
	XV64(0x942a2491e8d36929), XV64(0xa2dd1c8b1c04641a), XV64(0xcf336cbef5aa7e7c), XV64(0xf9c454a4017d734f),
	XV64(0x1564dc5cbabc0d82), XV64(0x2393e4464e6b00b1), XV64(0x4e7d9473a7c51ad7), XV64(0x788aac69531217e4),
	XV64(0x95a1741874992e1b), XV64(0xa3564c02804e2328), XV64(0xceb83c3769e0394e), XV64(0xf84f042d9d37347d),
	XV64(0x17f92dc61e62c4d4), XV64(0x210e15dceab5c9e7), XV64(0x4ce065e9031bd381), XV64(0x7a175df3f7ccdeb2),
	XV64(0x973c8582d047e74d), XV64(0xa1cbbd982490ea7e), XV64(0xcc25cdadcd3ef018), XV64(0xfad2f5b739e9fd2b),
	XV64(0x16727d4f822883e6), XV64(0x2085455576ff8ed5), XV64(0x4d6b35609f5194b3), XV64(0x7b9c0d7a6b869980),
	XV64(0x96b7d50b4c0da07f), XV64(0xa040ed11b8daad4c), XV64(0xcdae9d245174b72a), XV64(0xfb59a53ea5a3ba19),
	XV64(0x12c2cef357df5678), XV64(0x2435f6e9a3085b4b), XV64(0x49db86dc4aa6412d), XV64(0x7f2cbec6be714c1e),
	XV64(0x920766b799fa75e1), XV64(0xa4f05ead6d2d78d2), XV64(0xc91e2e98848362b4), XV64(0xffe9168270546f87),
	XV64(0x13499e7acb95114a), XV64(0x25bea6603f421c79), XV64(0x4850d655d6ec061f), XV64(0x7ea7ee4f223b0b2c),
	XV64(0x938c363e05b032d3), XV64(0xa57b0e24f1673fe0), XV64(0xc8957e1118c92586), XV64(0xfe62460bec1e28b5),
	XV64(0x11d46fe06f4bd81c), XV64(0x272357fa9b9cd52f), XV64(0x4acd27cf7232cf49), XV64(0x7c3a1fd586e5c27a),
	XV64(0x9111c7a4a16efb85), XV64(0xa7e6ffbe55b9f6b6), XV64(0xca088f8bbc17ecd0), XV64(0xfcffb79148c0e1e3),
	XV64(0x105f3f69f3019f2e), XV64(0x26a8077307d6921d), XV64(0x4b467746ee78887b), XV64(0x7db14f5c1aaf8548),
	XV64(0x909a972d3d24bcb7), XV64(0xa66daf37c9f3b184), XV64(0xcb83df02205dabe2), XV64(0xfd74e718d48aa6d1)
};

static const xply_word U[256] = {
	XV64(0x0), XV64(0x1c3eb44b122426b2), XV64(0xe8a508cd09f4057), XV64(0x12b4e4c7c2bb66e5),
	XV64(0x1d14a119a13e80ae), XV64(0x12a1552b31aa61c), XV64(0x139ef19571a1c0f9), XV64(0xfa045de6385e64b),
	XV64(0xcde7a29b6aa0c6f), XV64(0x10e0ce62a48e2add), XV64(0x2542aa566354c38), XV64(0x1e6a9eee74116a8a),
	XV64(0x11cadb3017948cc1), XV64(0xdf46f7b05b0aa73), XV64(0x1f408bbcc70bcc96), XV64(0x37e3ff7d52fea24),
	XV64(0x19bcf4536d5418de), XV64(0x58240187f703e6c), XV64(0x1736a4dfbdcb5889), XV64(0xb081094afef7e3b),
	XV64(0x4a8554acc6a9870), XV64(0x1896e101de4ebec2), XV64(0xa2205c61cf5d827), XV64(0x161cb18d0ed1fe95),
	XV64(0x15628e7adbfe14b1), XV64(0x95c3a31c9da3203), XV64(0x1be8def60b6154e6), XV64(0x7d66abd19457254),
	XV64(0x8762f637ac0941f), XV64(0x14489b2868e4b2ad), XV64(0x6fc7fefaa5fd448), XV64(0x1ac2cba4b87bf2fa),
	XV64(0x58ed0bc2e7f3c8f), XV64(0x19b064f73c5b1a3d), XV64(0xb048030fee07cd8), XV64(0x173a347becc45a6a),
	XV64(0x189a71a58f41bc21), XV64(0x4a4c5ee9d659a93), XV64(0x161021295fdefc76), XV64(0xa2e95624dfadac4),
	XV64(0x950aa9598d530e0), XV64(0x156e1ede8af11652), XV64(0x7dafa19484a70b7), XV64(0x1be44e525a6e5605),
	XV64(0x14440b8c39ebb04e), XV64(0x87abfc72bcf96fc), XV64(0x1ace5b00e974f019), XV64(0x6f0ef4bfb50d6ab),
	XV64(0x1c3224ef432b2451), XV64(0xc90a4510f02e3), XV64(0x12b8746393b46406), XV64(0xe86c028819042b4),
	XV64(0x12685f6e215a4ff), XV64(0x1d1831bdf031824d), XV64(0xfacd57a328ae4a8), XV64(0x1392613120aec21a),
	XV64(0x10ec5ec6f581283e), XV64(0xcd2ea8de7a50e8c), XV64(0x1e660e4a251e6869), XV64(0x258ba01373a4edb),
	XV64(0xdf8ffdf54bfa890), XV64(0x11c64b94469b8e22), XV64(0x372af538420e8c7), XV64(0x1f4c1b189604ce75),
	XV64(0xb1da1785cfe791e), XV64(0x172315334eda5fac), XV64(0x597f1f48c613949), XV64(0x19a945bf9e451ffb),
	XV64(0x16090061fdc0f9b0), XV64(0xa37b42aefe4df02), XV64(0x188350ed2d5fb9e7), XV64(0x4bde4a63f7b9f55),
	XV64(0x7c3db51ea547571), XV64(0x1bfd6f1af87053c3), XV64(0x9498bdd3acb3526), XV64(0x15773f9628ef1394),
	XV64(0x1ad77a484b6af5df), XV64(0x6e9ce03594ed36d), XV64(0x145d2ac49bf5b588), XV64(0x8639e8f89d1933a),
	XV64(0x12a1552b31aa61c0), XV64(0xe9fe160238e4772), XV64(0x1c2b05a7e1352197), XV64(0x15b1ecf3110725),
	XV64(0xfb5f4329094e16e), XV64(0x138b407982b0c7dc), XV64(0x13fa4be400ba139), XV64(0x1d0110f5522f878b),
	XV64(0x1e7f2f0287006daf), XV64(0x2419b4995244b1d), XV64(0x10f57f8e579f2df8), XV64(0xccbcbc545bb0b4a),
	XV64(0x36b8e1b263eed01), XV64(0x1f553a50341acbb3), XV64(0xde1de97f6a1ad56), XV64(0x11df6adce4858be4),
	XV64(0xe9371c472814591), XV64(0x12adc58f60a56323), XV64(0x192148a21e05c6), XV64(0x1c279503b03a2374),
	XV64(0x1387d0ddd3bfc53f), XV64(0xfb96496c19be38d), XV64(0x1d0d805103208568), XV64(0x133341a1104a3da),
	XV64(0x24d0bedc42b49fe), XV64(0x1e73bfa6d60f6f4c), XV64(0xcc75b6114b409a9), XV64(0x10f9ef2a06902f1b),
	XV64(0x1f59aaf46515c950), XV64(0x3671ebf7731efe2), XV64(0x11d3fa78b58a8907), XV64(0xded4e33a7aeafb5),
	XV64(0x172f85971fd55d4f), XV64(0xb1131dc0df17bfd), XV64(0x19a5d51bcf4a1d18), XV64(0x59b6150dd6e3baa),
	XV64(0xa3b248ebeebdde1), XV64(0x160590c5accffb53), XV64(0x4b174026e749db6), XV64(0x188fc0497c50bb04),
	XV64(0x1bf1ffbea97f5120), XV64(0x7cf4bf5bb5b7792), XV64(0x157baf3279e01177), XV64(0x9451b796bc437c5),
	XV64(0x6e55ea70841d18e), XV64(0x1adbeaec1a65f73c), XV64(0x86f0e2bd8de91d9), XV64(0x1451ba60cafab76b),
	XV64(0x163b42f0b9fcf23c), XV64(0xa05f6bbabd8d48e), XV64(0x18b1127c6963b26b), XV64(0x48fa6377b4794d9),
	XV64(0xb2fe3e918c27292), XV64(0x171157a20ae65420), XV64(0x5a5b365c85d32c5), XV64(0x199b072eda791477),
	XV64(0x1ae538d90f56fe53), XV64(0x6db8c921d72d8e1), XV64(0x146f6855dfc9be04), XV64(0x851dc1ecded98b6),
	XV64(0x7f199c0ae687efd), XV64(0x1bcf2d8bbc4c584f), XV64(0x97bc94c7ef73eaa), XV64(0x15457d076cd31818),
	XV64(0xf87b6a3d4a8eae2), XV64(0x13b902e8c68ccc50), XV64(0x10de62f0437aab5), XV64(0x1d33526416138c07),
	XV64(0x129317ba75966a4c), XV64(0xeada3f167b24cfe), XV64(0x1c194736a5092a1b), XV64(0x27f37db72d0ca9),
	XV64(0x359cc8a6202e68d), XV64(0x1f6778c17026c03f), XV64(0xdd39c06b29da6da), XV64(0x11ed284da0b98068),
	XV64(0x1e4d6d93c33c6623), XV64(0x273d9d8d1184091), XV64(0x10c73d1f13a32674), XV64(0xcf98954018700c6),
	XV64(0x13b5924c9783ceb3), XV64(0xf8b260785a7e801), XV64(0x1d3fc2c0471c8ee4), XV64(0x101768b5538a856),
	XV64(0xea1335536bd4e1d), XV64(0x129f871e249968af), XV64(0x2b63d9e6220e4a), XV64(0x1c15d792f40628f8),
	XV64(0x1f6be8652129c2dc), XV64(0x3555c2e330de46e), XV64(0x11e1b8e9f1b6828b), XV64(0xddf0ca2e392a439),
	XV64(0x27f497c80174272), XV64(0x1e41fd37923364c0), XV64(0xcf519f050880225), XV64(0x10cbadbb42ac2497),
	XV64(0xa09661ffad7d66d), XV64(0x1637d254e8f3f0df), XV64(0x48336932a48963a), XV64(0x18bd82d8386cb088),
	XV64(0x171dc7065be956c3), XV64(0xb23734d49cd7071), XV64(0x1997978a8b761694), XV64(0x5a923c199523026),
	XV64(0x6d71c364c7dda02), XV64(0x1ae9a87d5e59fcb0), XV64(0x85d4cba9ce29a55), XV64(0x1463f8f18ec6bce7),
	XV64(0x1bc3bd2fed435aac), XV64(0x7fd0964ff677c1e), XV64(0x1549eda33ddc1afb), XV64(0x97759e82ff83c49),
	XV64(0x1d26e388e5028b22), XV64(0x11857c3f726ad90), XV64(0x13acb304359dcb75), XV64(0xf92074f27b9edc7),
	XV64(0x324291443c0b8c), XV64(0x1c0cf6da56182d3e), XV64(0xeb8121d94a34bdb), XV64(0x1286a65686876d69),
	XV64(0x11f899a153a8874d), XV64(0xdc62dea418ca1ff), XV64(0x1f72c92d8337c71a), XV64(0x34c7d669113e1a8),
	XV64(0xcec38b8f29607e3), XV64(0x10d28cf3e0b22151), XV64(0x2666834220947b4), XV64(0x1e58dc7f302d6106),
	XV64(0x49a17db885693fc), XV64(0x18a4a3909a72b54e), XV64(0xa10475758c9d3ab), XV64(0x162ef31c4aedf519),
	XV64(0x198eb6c229681352), XV64(0x5b002893b4c35e0), XV64(0x1704e64ef9f75305), XV64(0xb3a5205ebd375b7),
	XV64(0x8446df23efc9f93), XV64(0x147ad9b92cd8b921), XV64(0x6ce3d7eee63dfc4), XV64(0x1af08935fc47f976),
	XV64(0x1550cceb9fc21f3d), XV64(0x96e78a08de6398f), XV64(0x1bda9c674f5d5f6a), XV64(0x7e4282c5d7979d8),
	XV64(0x18a83334cb7db7ad), XV64(0x496877fd959911f), XV64(0x162263b81be2f7fa), XV64(0xa1cd7f309c6d148),
	XV64(0x5bc922d6a433703), XV64(0x19822666786711b1), XV64(0xb36c2a1badc7754), XV64(0x170876eaa8f851e6),
	XV64(0x1476491d7dd7bbc2), XV64(0x848fd566ff39d70), XV64(0x1afc1991ad48fb95), XV64(0x6c2addabf6cdd27),
	XV64(0x962e804dce93b6c), XV64(0x155c5c4fcecd1dde), XV64(0x7e8b8880c767b3b), XV64(0x1bd60cc31e525d89),
	XV64(0x114c767a629af73), XV64(0x1d2a732cb40d89c1), XV64(0xf9e97eb76b6ef24), XV64(0x13a023a06492c996),
	XV64(0x1c00667e07172fdd), XV64(0x3ed2351533096f), XV64(0x128a36f2d7886f8a), XV64(0xeb482b9c5ac4938),
	XV64(0xdcabd4e1083a31c), XV64(0x11f4090502a785ae), XV64(0x340edc2c01ce34b), XV64(0x1f7e5989d238c5f9),
	XV64(0x10de1c57b1bd23b2), XV64(0xce0a81ca3990500), XV64(0x1e544cdb612263e5), XV64(0x26af89073064557)
};

#endif /* if defined(XRABPLY_TYPE64) */

#if defined(XRABPLY_TYPE32)

#if !defined(XV32)
#define XV32(v) ((xply_word) v ## ULL)
#endif

#define XRAB_ROOTPOLY XV32(0xabd1f37b)

#define XRAB_SHIFT 23
#define XRAB_WNDSIZE 20

typedef unsigned XRABPLY_TYPE32 xply_word;

static const xply_word T[256] = {
	XV32(0x0), XV32(0xabd1f37b), XV32(0x57a3e6f6), XV32(0xfc72158d),
	XV32(0x4963e97), XV32(0xaf47cdec), XV32(0x5335d861), XV32(0xf8e42b1a),
	XV32(0x92c7d2e), XV32(0xa2fd8e55), XV32(0x5e8f9bd8), XV32(0xf55e68a3),
	XV32(0xdba43b9), XV32(0xa66bb0c2), XV32(0x5a19a54f), XV32(0xf1c85634),
	XV32(0x1258fa5c), XV32(0xb9890927), XV32(0x45fb1caa), XV32(0xee2aefd1),
	XV32(0x16cec4cb), XV32(0xbd1f37b0), XV32(0x416d223d), XV32(0xeabcd146),
	XV32(0x1b748772), XV32(0xb0a57409), XV32(0x4cd76184), XV32(0xe70692ff),
	XV32(0x1fe2b9e5), XV32(0xb4334a9e), XV32(0x48415f13), XV32(0xe390ac68),
	XV32(0x24b1f4b8), XV32(0x8f6007c3), XV32(0x7312124e), XV32(0xd8c3e135),
	XV32(0x2027ca2f), XV32(0x8bf63954), XV32(0x77842cd9), XV32(0xdc55dfa2),
	XV32(0x2d9d8996), XV32(0x864c7aed), XV32(0x7a3e6f60), XV32(0xd1ef9c1b),
	XV32(0x290bb701), XV32(0x82da447a), XV32(0x7ea851f7), XV32(0xd579a28c),
	XV32(0x36e90ee4), XV32(0x9d38fd9f), XV32(0x614ae812), XV32(0xca9b1b69),
	XV32(0x327f3073), XV32(0x99aec308), XV32(0x65dcd685), XV32(0xce0d25fe),
	XV32(0x3fc573ca), XV32(0x941480b1), XV32(0x6866953c), XV32(0xc3b76647),
	XV32(0x3b534d5d), XV32(0x9082be26), XV32(0x6cf0abab), XV32(0xc72158d0),
	XV32(0x4963e970), XV32(0xe2b21a0b), XV32(0x1ec00f86), XV32(0xb511fcfd),
	XV32(0x4df5d7e7), XV32(0xe624249c), XV32(0x1a563111), XV32(0xb187c26a),
	XV32(0x404f945e), XV32(0xeb9e6725), XV32(0x17ec72a8), XV32(0xbc3d81d3),
	XV32(0x44d9aac9), XV32(0xef0859b2), XV32(0x137a4c3f), XV32(0xb8abbf44),
	XV32(0x5b3b132c), XV32(0xf0eae057), XV32(0xc98f5da), XV32(0xa74906a1),
	XV32(0x5fad2dbb), XV32(0xf47cdec0), XV32(0x80ecb4d), XV32(0xa3df3836),
	XV32(0x52176e02), XV32(0xf9c69d79), XV32(0x5b488f4), XV32(0xae657b8f),
	XV32(0x56815095), XV32(0xfd50a3ee), XV32(0x122b663), XV32(0xaaf34518),
	XV32(0x6dd21dc8), XV32(0xc603eeb3), XV32(0x3a71fb3e), XV32(0x91a00845),
	XV32(0x6944235f), XV32(0xc295d024), XV32(0x3ee7c5a9), XV32(0x953636d2),
	XV32(0x64fe60e6), XV32(0xcf2f939d), XV32(0x335d8610), XV32(0x988c756b),
	XV32(0x60685e71), XV32(0xcbb9ad0a), XV32(0x37cbb887), XV32(0x9c1a4bfc),
	XV32(0x7f8ae794), XV32(0xd45b14ef), XV32(0x28290162), XV32(0x83f8f219),
	XV32(0x7b1cd903), XV32(0xd0cd2a78), XV32(0x2cbf3ff5), XV32(0x876ecc8e),
	XV32(0x76a69aba), XV32(0xdd7769c1), XV32(0x21057c4c), XV32(0x8ad48f37),
	XV32(0x7230a42d), XV32(0xd9e15756), XV32(0x259342db), XV32(0x8e42b1a0),
	XV32(0x3916219b), XV32(0x92c7d2e0), XV32(0x6eb5c76d), XV32(0xc5643416),
	XV32(0x3d801f0c), XV32(0x9651ec77), XV32(0x6a23f9fa), XV32(0xc1f20a81),
	XV32(0x303a5cb5), XV32(0x9bebafce), XV32(0x6799ba43), XV32(0xcc484938),
	XV32(0x34ac6222), XV32(0x9f7d9159), XV32(0x630f84d4), XV32(0xc8de77af),
	XV32(0x2b4edbc7), XV32(0x809f28bc), XV32(0x7ced3d31), XV32(0xd73cce4a),
	XV32(0x2fd8e550), XV32(0x8409162b), XV32(0x787b03a6), XV32(0xd3aaf0dd),
	XV32(0x2262a6e9), XV32(0x89b35592), XV32(0x75c1401f), XV32(0xde10b364),
	XV32(0x26f4987e), XV32(0x8d256b05), XV32(0x71577e88), XV32(0xda868df3),
	XV32(0x1da7d523), XV32(0xb6762658), XV32(0x4a0433d5), XV32(0xe1d5c0ae),
	XV32(0x1931ebb4), XV32(0xb2e018cf), XV32(0x4e920d42), XV32(0xe543fe39),
	XV32(0x148ba80d), XV32(0xbf5a5b76), XV32(0x43284efb), XV32(0xe8f9bd80),
	XV32(0x101d969a), XV32(0xbbcc65e1), XV32(0x47be706c), XV32(0xec6f8317),
	XV32(0xfff2f7f), XV32(0xa42edc04), XV32(0x585cc989), XV32(0xf38d3af2),
	XV32(0xb6911e8), XV32(0xa0b8e293), XV32(0x5ccaf71e), XV32(0xf71b0465),
	XV32(0x6d35251), XV32(0xad02a12a), XV32(0x5170b4a7), XV32(0xfaa147dc),
	XV32(0x2456cc6), XV32(0xa9949fbd), XV32(0x55e68a30), XV32(0xfe37794b),
	XV32(0x7075c8eb), XV32(0xdba43b90), XV32(0x27d62e1d), XV32(0x8c07dd66),
	XV32(0x74e3f67c), XV32(0xdf320507), XV32(0x2340108a), XV32(0x8891e3f1),
	XV32(0x7959b5c5), XV32(0xd28846be), XV32(0x2efa5333), XV32(0x852ba048),
	XV32(0x7dcf8b52), XV32(0xd61e7829), XV32(0x2a6c6da4), XV32(0x81bd9edf),
	XV32(0x622d32b7), XV32(0xc9fcc1cc), XV32(0x358ed441), XV32(0x9e5f273a),
	XV32(0x66bb0c20), XV32(0xcd6aff5b), XV32(0x3118ead6), XV32(0x9ac919ad),
	XV32(0x6b014f99), XV32(0xc0d0bce2), XV32(0x3ca2a96f), XV32(0x97735a14),
	XV32(0x6f97710e), XV32(0xc4468275), XV32(0x383497f8), XV32(0x93e56483),
	XV32(0x54c43c53), XV32(0xff15cf28), XV32(0x367daa5), XV32(0xa8b629de),
	XV32(0x505202c4), XV32(0xfb83f1bf), XV32(0x7f1e432), XV32(0xac201749),
	XV32(0x5de8417d), XV32(0xf639b206), XV32(0xa4ba78b), XV32(0xa19a54f0),
	XV32(0x597e7fea), XV32(0xf2af8c91), XV32(0xedd991c), XV32(0xa50c6a67),
	XV32(0x469cc60f), XV32(0xed4d3574), XV32(0x113f20f9), XV32(0xbaeed382),
	XV32(0x420af898), XV32(0xe9db0be3), XV32(0x15a91e6e), XV32(0xbe78ed15),
	XV32(0x4fb0bb21), XV32(0xe461485a), XV32(0x18135dd7), XV32(0xb3c2aeac),
	XV32(0x4b2685b6), XV32(0xe0f776cd), XV32(0x1c856340), XV32(0xb754903b)
};

static const xply_word U[256] = {
	XV32(0x0), XV32(0x5ce33923), XV32(0x1217813d), XV32(0x4ef4b81e),
	XV32(0x242f027a), XV32(0x78cc3b59), XV32(0x36388347), XV32(0x6adbba64),
	XV32(0x485e04f4), XV32(0x14bd3dd7), XV32(0x5a4985c9), XV32(0x6aabcea),
	XV32(0x6c71068e), XV32(0x30923fad), XV32(0x7e6687b3), XV32(0x2285be90),
	XV32(0x3b6dfa93), XV32(0x678ec3b0), XV32(0x297a7bae), XV32(0x7599428d),
	XV32(0x1f42f8e9), XV32(0x43a1c1ca), XV32(0xd5579d4), XV32(0x51b640f7),
	XV32(0x7333fe67), XV32(0x2fd0c744), XV32(0x61247f5a), XV32(0x3dc74679),
	XV32(0x571cfc1d), XV32(0xbffc53e), XV32(0x450b7d20), XV32(0x19e84403),
	XV32(0x76dbf526), XV32(0x2a38cc05), XV32(0x64cc741b), XV32(0x382f4d38),
	XV32(0x52f4f75c), XV32(0xe17ce7f), XV32(0x40e37661), XV32(0x1c004f42),
	XV32(0x3e85f1d2), XV32(0x6266c8f1), XV32(0x2c9270ef), XV32(0x707149cc),
	XV32(0x1aaaf3a8), XV32(0x4649ca8b), XV32(0x8bd7295), XV32(0x545e4bb6),
	XV32(0x4db60fb5), XV32(0x11553696), XV32(0x5fa18e88), XV32(0x342b7ab),
	XV32(0x69990dcf), XV32(0x357a34ec), XV32(0x7b8e8cf2), XV32(0x276db5d1),
	XV32(0x5e80b41), XV32(0x590b3262), XV32(0x17ff8a7c), XV32(0x4b1cb35f),
	XV32(0x21c7093b), XV32(0x7d243018), XV32(0x33d08806), XV32(0x6f33b125),
	XV32(0x46661937), XV32(0x1a852014), XV32(0x5471980a), XV32(0x892a129),
	XV32(0x62491b4d), XV32(0x3eaa226e), XV32(0x705e9a70), XV32(0x2cbda353),
	XV32(0xe381dc3), XV32(0x52db24e0), XV32(0x1c2f9cfe), XV32(0x40cca5dd),
	XV32(0x2a171fb9), XV32(0x76f4269a), XV32(0x38009e84), XV32(0x64e3a7a7),
	XV32(0x7d0be3a4), XV32(0x21e8da87), XV32(0x6f1c6299), XV32(0x33ff5bba),
	XV32(0x5924e1de), XV32(0x5c7d8fd), XV32(0x4b3360e3), XV32(0x17d059c0),
	XV32(0x3555e750), XV32(0x69b6de73), XV32(0x2742666d), XV32(0x7ba15f4e),
	XV32(0x117ae52a), XV32(0x4d99dc09), XV32(0x36d6417), XV32(0x5f8e5d34),
	XV32(0x30bdec11), XV32(0x6c5ed532), XV32(0x22aa6d2c), XV32(0x7e49540f),
	XV32(0x1492ee6b), XV32(0x4871d748), XV32(0x6856f56), XV32(0x5a665675),
	XV32(0x78e3e8e5), XV32(0x2400d1c6), XV32(0x6af469d8), XV32(0x361750fb),
	XV32(0x5cccea9f), XV32(0x2fd3bc), XV32(0x4edb6ba2), XV32(0x12385281),
	XV32(0xbd01682), XV32(0x57332fa1), XV32(0x19c797bf), XV32(0x4524ae9c),
	XV32(0x2fff14f8), XV32(0x731c2ddb), XV32(0x3de895c5), XV32(0x610bace6),
	XV32(0x438e1276), XV32(0x1f6d2b55), XV32(0x5199934b), XV32(0xd7aaa68),
	XV32(0x67a1100c), XV32(0x3b42292f), XV32(0x75b69131), XV32(0x2955a812),
	XV32(0x271dc115), XV32(0x7bfef836), XV32(0x350a4028), XV32(0x69e9790b),
	XV32(0x332c36f), XV32(0x5fd1fa4c), XV32(0x11254252), XV32(0x4dc67b71),
	XV32(0x6f43c5e1), XV32(0x33a0fcc2), XV32(0x7d5444dc), XV32(0x21b77dff),
	XV32(0x4b6cc79b), XV32(0x178ffeb8), XV32(0x597b46a6), XV32(0x5987f85),
	XV32(0x1c703b86), XV32(0x409302a5), XV32(0xe67babb), XV32(0x52848398),
	XV32(0x385f39fc), XV32(0x64bc00df), XV32(0x2a48b8c1), XV32(0x76ab81e2),
	XV32(0x542e3f72), XV32(0x8cd0651), XV32(0x4639be4f), XV32(0x1ada876c),
	XV32(0x70013d08), XV32(0x2ce2042b), XV32(0x6216bc35), XV32(0x3ef58516),
	XV32(0x51c63433), XV32(0xd250d10), XV32(0x43d1b50e), XV32(0x1f328c2d),
	XV32(0x75e93649), XV32(0x290a0f6a), XV32(0x67feb774), XV32(0x3b1d8e57),
	XV32(0x199830c7), XV32(0x457b09e4), XV32(0xb8fb1fa), XV32(0x576c88d9),
	XV32(0x3db732bd), XV32(0x61540b9e), XV32(0x2fa0b380), XV32(0x73438aa3),
	XV32(0x6aabcea0), XV32(0x3648f783), XV32(0x78bc4f9d), XV32(0x245f76be),
	XV32(0x4e84ccda), XV32(0x1267f5f9), XV32(0x5c934de7), XV32(0x7074c4),
	XV32(0x22f5ca54), XV32(0x7e16f377), XV32(0x30e24b69), XV32(0x6c01724a),
	XV32(0x6dac82e), XV32(0x5a39f10d), XV32(0x14cd4913), XV32(0x482e7030),
	XV32(0x617bd822), XV32(0x3d98e101), XV32(0x736c591f), XV32(0x2f8f603c),
	XV32(0x4554da58), XV32(0x19b7e37b), XV32(0x57435b65), XV32(0xba06246),
	XV32(0x2925dcd6), XV32(0x75c6e5f5), XV32(0x3b325deb), XV32(0x67d164c8),
	XV32(0xd0adeac), XV32(0x51e9e78f), XV32(0x1f1d5f91), XV32(0x43fe66b2),
	XV32(0x5a1622b1), XV32(0x6f51b92), XV32(0x4801a38c), XV32(0x14e29aaf),
	XV32(0x7e3920cb), XV32(0x22da19e8), XV32(0x6c2ea1f6), XV32(0x30cd98d5),
	XV32(0x12482645), XV32(0x4eab1f66), XV32(0x5fa778), XV32(0x5cbc9e5b),
	XV32(0x3667243f), XV32(0x6a841d1c), XV32(0x2470a502), XV32(0x78939c21),
	XV32(0x17a02d04), XV32(0x4b431427), XV32(0x5b7ac39), XV32(0x5954951a),
	XV32(0x338f2f7e), XV32(0x6f6c165d), XV32(0x2198ae43), XV32(0x7d7b9760),
	XV32(0x5ffe29f0), XV32(0x31d10d3), XV32(0x4de9a8cd), XV32(0x110a91ee),
	XV32(0x7bd12b8a), XV32(0x273212a9), XV32(0x69c6aab7), XV32(0x35259394),
	XV32(0x2ccdd797), XV32(0x702eeeb4), XV32(0x3eda56aa), XV32(0x62396f89),
	XV32(0x8e2d5ed), XV32(0x5401ecce), XV32(0x1af554d0), XV32(0x46166df3),
	XV32(0x6493d363), XV32(0x3870ea40), XV32(0x7684525e), XV32(0x2a676b7d),
	XV32(0x40bcd119), XV32(0x1c5fe83a), XV32(0x52ab5024), XV32(0xe486907)
};

#endif /* if defined(XRABPLY_TYPE32) */

/* END xrabply.c */





#define XRAB_SLIDE(v, c) do {					\
		if (++wpos == XRAB_WNDSIZE) wpos = 0;		\
		v ^= U[wbuf[wpos]];				\
		wbuf[wpos] = (c);				\
		v = ((v << 8) | (c)) ^ T[v >> XRAB_SHIFT];	\
	} while (0)


#define XRAB_MINCPYSIZE 12
#define XRAB_WBITS (sizeof(xply_word) * 8)



typedef struct s_xrabctx {
	long idxsize;
	long *idx;
	unsigned char const *data;
	long size;
} xrabctx_t;

typedef struct s_xrabcpyi {
	long src;
	long tgt;
	long len;
} xrabcpyi_t;

typedef struct s_xrabcpyi_arena {
	long cnt, size;
	xrabcpyi_t *acpy;
} xrabcpyi_arena_t;



static void xrab_init_cpyarena(xrabcpyi_arena_t *aca) {
	aca->cnt = aca->size = 0;
	aca->acpy = NULL;
}


static void xrab_free_cpyarena(xrabcpyi_arena_t *aca) {
	xdl_free(aca->acpy);
}


static int xrab_add_cpy(xrabcpyi_arena_t *aca, xrabcpyi_t const *rcpy) {
	long size;
	xrabcpyi_t *acpy;

	if (aca->cnt >= aca->size) {
		size = 2 * aca->size + 1024;
		if ((acpy = (xrabcpyi_t *)
		     xdl_realloc(aca->acpy, size * sizeof(xrabcpyi_t))) == NULL)
			return -1;
		aca->acpy = acpy;
		aca->size = size;
	}
	aca->acpy[aca->cnt++] = *rcpy;

	return 0;
}


static long xrab_cmnseq(unsigned char const *data, long start, long size) {
	unsigned char ch = data[start];
	unsigned char const *ptr, *top;

	for (ptr = data + start + 1, top = data + size; ptr < top && ch == *ptr; ptr++);

	return (long) (ptr - (data + start + 1));
}


static int xrab_build_ctx(unsigned char const *data, long size, xrabctx_t *ctx) {
	long i, isize, idxsize, seq, wpos = 0;
	xply_word fp = 0, mask;
	unsigned char ch;
	unsigned char const *ptr, *eot;
	long *idx;
	unsigned char wbuf[XRAB_WNDSIZE];
	long maxoffs[256];
	long maxseq[256];
	xply_word maxfp[256];

	memset(wbuf, 0, sizeof(wbuf));
	memset(maxseq, 0, sizeof(maxseq));
	isize = 2 * (size / XRAB_WNDSIZE);
	for (idxsize = 1; idxsize < isize; idxsize <<= 1);
	mask = (xply_word) (idxsize - 1);
	if ((idx = (long *) xdl_malloc(idxsize * sizeof(long))) == NULL)
		return -1;
	memset(idx, 0, idxsize * sizeof(long));
	for (i = 0; i + XRAB_WNDSIZE < size; i += XRAB_WNDSIZE) {
		/*
		 * Generate a brand new hash for the current window. Here we could
		 * try to perform pseudo-loop unroll by 4 blocks if necessary, and
		 * if we force XRAB_WNDSIZE to be a multiple of 4, we could reduce
		 * the branch occurence inside XRAB_SLIDE by a factor of 4.
		 */
		for (ptr = data + i, eot = ptr + XRAB_WNDSIZE; ptr < eot; ptr++)
			XRAB_SLIDE(fp, *ptr);

		/*
		 * Try to scan for single value scans, and store them in the
		 * array according to the longest one. Before we do a fast check
		 * to avoid calling xrab_cmnseq() when not necessary.
		 */
		if ((ch = data[i]) == data[i + XRAB_WNDSIZE - 1] &&
		    (seq = xrab_cmnseq(data, i, size)) > XRAB_WNDSIZE &&
		    seq > maxseq[ch]) {
			maxseq[ch] = seq;
			maxfp[ch] = fp;
			maxoffs[ch] = i + XRAB_WNDSIZE;
			seq = (seq / XRAB_WNDSIZE) * XRAB_WNDSIZE;
			i += seq - XRAB_WNDSIZE;
		} else
			idx[fp & mask] = i + XRAB_WNDSIZE;
	}

	/*
	 * Restore back the logest sequences by overwriting target hash buckets.
	 */
	for (i = 0; i < 256; i++)
		if (maxseq[i])
			idx[maxfp[i] & mask] = maxoffs[i];
	ctx->idxsize = idxsize;
	ctx->idx = idx;
	ctx->data = data;
	ctx->size = size;

	return 0;
}


static void xrab_free_ctx(xrabctx_t *ctx) {

	xdl_free(ctx->idx);
}


static int xrab_diff(unsigned char const *data, long size, xrabctx_t *ctx,
		     xrabcpyi_arena_t *aca) {
	long i, offs, ssize, src, tgt, esrc, etgt, wpos = 0;
	xply_word fp = 0, mask;
	long const *idx;
	unsigned char const *sdata;
	xrabcpyi_t rcpy;
	unsigned char wbuf[XRAB_WNDSIZE];

	xrab_init_cpyarena(aca);
	memset(wbuf, 0, sizeof(wbuf));
	for (i = 0; i < XRAB_WNDSIZE - 1 && i < size; i++)
		XRAB_SLIDE(fp, data[i]);
	idx = ctx->idx;
	sdata = ctx->data;
	ssize = ctx->size;
	mask = (xply_word) (ctx->idxsize - 1);
	while (i < size) {
		unsigned char ch = data[i++];

		XRAB_SLIDE(fp, ch);
		offs = idx[fp & mask];

		/*
		 * Fast check here to probabilistically reduce false positives
		 * that would trigger the slow path below.
		 */
		if (offs == 0 || ch != sdata[offs - 1])
			continue;

		/*
		 * Stretch the match both sides as far as possible.
		 */
		src = offs - 1;
		tgt = i - 1;
		for (; tgt > 0 && src > 0 && data[tgt - 1] == sdata[src - 1];
		     tgt--, src--);
		esrc = offs;
		etgt = i;
		for (; etgt < size && esrc < ssize && data[etgt] == sdata[esrc];
		     etgt++, esrc++);

		/*
		 * Avoid considering copies smaller than the XRAB_MINCPYSIZE
		 * threshold.
		 */
		if (etgt - tgt >= XRAB_MINCPYSIZE) {
			rcpy.src = src;
			rcpy.tgt = tgt;
			rcpy.len = etgt - tgt;
			if (xrab_add_cpy(aca, &rcpy) < 0) {
				xrab_free_cpyarena(aca);
				return -1;
			}

			/*
			 * Fill up the new window and exit with 'i' properly set on exit.
			 */
			for (i = etgt - XRAB_WNDSIZE; i < etgt; i++)
				XRAB_SLIDE(fp, data[i]);
		}
	}

	return 0;
}


static int xrab_tune_cpyarena(unsigned char const *data, long size, xrabctx_t *ctx,
			      xrabcpyi_arena_t *aca) {
	long i, cpos;
	xrabcpyi_t *rcpy;

	for (cpos = size, i = aca->cnt - 1; i >= 0; i--) {
		rcpy = aca->acpy + i;
		if (rcpy->tgt >= cpos)
			rcpy->len = 0;
		else if (rcpy->tgt + rcpy->len > cpos) {
			if ((rcpy->len = cpos - rcpy->tgt) >= XRAB_MINCPYSIZE)
				cpos = rcpy->tgt;
			else
				rcpy->len = 0;
		} else
			cpos = rcpy->tgt;
	}

	return 0;
}


int xdl_rabdiff_mb(mmbuffer_t *mmb1, mmbuffer_t *mmb2, xdemitcb_t *ecb) {
	long i, cpos, size;
	unsigned long fp;
	xrabcpyi_t *rcpy;
	xrabctx_t ctx;
	xrabcpyi_arena_t aca;
	mmbuffer_t mb[2];
	unsigned char cpybuf[32];

	fp = xdl_mmb_adler32(mmb1);
	if (xrab_build_ctx((unsigned char const *) mmb1->ptr, mmb1->size,
			   &ctx) < 0)
		return -1;
	if (xrab_diff((unsigned char const *) mmb2->ptr, mmb2->size, &ctx,
		      &aca) < 0) {
		xrab_free_ctx(&ctx);
		return -1;
	}
	xrab_tune_cpyarena((unsigned char const *) mmb2->ptr, mmb2->size, &ctx,
			   &aca);
	xrab_free_ctx(&ctx);

	/*
	 * Prepare and emit the binary patch file header. It will be used
	 * to verify that that file being patched matches in size and fingerprint
	 * the one that generated the patch.
	 */
	size = mmb1->size;
	XDL_LE32_PUT(cpybuf, fp);
	XDL_LE32_PUT(cpybuf + 4, size);

	mb[0].ptr = (char *) cpybuf;
	mb[0].size = 4 + 4;
	if (ecb->outf(ecb->priv, mb, 1) < 0) {
		xrab_free_cpyarena(&aca);
		return -1;
	}
	for (cpos = 0, i = 0; i < aca.cnt; i++) {
		rcpy = aca.acpy + i;
		if (rcpy->len == 0)
			continue;
		if (cpos < rcpy->tgt) {
			size = rcpy->tgt - cpos;
			if (size > 255) {
				cpybuf[0] = XDL_BDOP_INSB;
				XDL_LE32_PUT(cpybuf + 1, size);
				mb[0].ptr = (char *) cpybuf;
				mb[0].size = XDL_INSBOP_SIZE;
			} else {
				cpybuf[0] = XDL_BDOP_INS;
				cpybuf[1] = (unsigned char) size;
				mb[0].ptr = (char *) cpybuf;
				mb[0].size = 2;
			}
			mb[1].ptr = mmb2->ptr + cpos;
			mb[1].size = size;
			if (ecb->outf(ecb->priv, mb, 2) < 0) {
				xrab_free_cpyarena(&aca);
				return -1;
			}
			cpos = rcpy->tgt;
		}
		cpybuf[0] = XDL_BDOP_CPY;
		XDL_LE32_PUT(cpybuf + 1, rcpy->src);
		XDL_LE32_PUT(cpybuf + 5, rcpy->len);
		mb[0].ptr = (char *) cpybuf;
		mb[0].size = XDL_COPYOP_SIZE;
		if (ecb->outf(ecb->priv, mb, 1) < 0) {
			xrab_free_cpyarena(&aca);
			return -1;
		}
		cpos += rcpy->len;
	}
	xrab_free_cpyarena(&aca);
	if (cpos < mmb2->size) {
		size = mmb2->size - cpos;
		if (size > 255) {
			cpybuf[0] = XDL_BDOP_INSB;
			XDL_LE32_PUT(cpybuf + 1, size);
			mb[0].ptr = (char *) cpybuf;
			mb[0].size = XDL_INSBOP_SIZE;
		} else {
			cpybuf[0] = XDL_BDOP_INS;
			cpybuf[1] = (unsigned char) size;
			mb[0].ptr = (char *) cpybuf;
			mb[0].size = 2;
		}
		mb[1].ptr = mmb2->ptr + cpos;
		mb[1].size = size;
		if (ecb->outf(ecb->priv, mb, 2) < 0)
			return -1;
	}

	return 0;
}


int xdl_rabdiff(mmfile_t *mmf1, mmfile_t *mmf2, xdemitcb_t *ecb) {
	mmbuffer_t mmb1, mmb2;

	if (!xdl_mmfile_iscompact(mmf1) || !xdl_mmfile_iscompact(mmf2))
		return -1;
	if ((mmb1.ptr = (char *) xdl_mmfile_first(mmf1, &mmb1.size)) == NULL)
		mmb1.size = 0;
	if ((mmb2.ptr = (char *) xdl_mmfile_first(mmf2, &mmb2.size)) == NULL)
		mmb2.size = 0;

	return xdl_rabdiff_mb(&mmb1, &mmb2, ecb);
}

/* END xrabdiff.c */


/* BEGIN xutils.c */

#define XDL_GUESS_NLINES 256




long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	for (i = 1; n > 0; n >>= 2)
		i <<= 1;

	return i;
}


int xdl_emit_diffrec(char const *rec, long size, char const *pre, long psize,
		     xdemitcb_t *ecb) {
	int i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (char *) pre;
	mb[0].size = psize;
	mb[1].ptr = (char *) rec;
	mb[1].size = size;
	if (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (char *) "\n\\ No newline at end of file\n";
		mb[2].size = strlen(mb[2].ptr);
		i++;
	}
	if (ecb->outf(ecb->priv, mb, i) < 0) {

		return -1;
	}

	return 0;
}


int xdl_init_mmfile(mmfile_t *mmf, long bsize, unsigned long flags) {

	mmf->flags = flags;
	mmf->head = mmf->tail = NULL;
	mmf->bsize = bsize;
	mmf->fsize = 0;
	mmf->rcur = mmf->wcur = NULL;
	mmf->rpos = 0;

	return 0;
}


void xdl_free_mmfile(mmfile_t *mmf) {
	mmblock_t *cur, *tmp;

	for (cur = mmf->head; (tmp = cur) != NULL;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


int xdl_mmfile_iscompact(mmfile_t *mmf) {

	return mmf->head == mmf->tail;
}


int xdl_seek_mmfile(mmfile_t *mmf, long off) {
	long bsize;

	if (xdl_mmfile_first(mmf, &bsize)) {
		do {
			if (off < bsize) {
				mmf->rpos = off;
				return 0;
			}
			off -= bsize;
		} while (xdl_mmfile_next(mmf, &bsize));
	}

	return -1;
}


long xdl_read_mmfile(mmfile_t *mmf, void *data, long size) {
	long rsize, csize;
	char *ptr = (char*) data;
	mmblock_t *rcur;

	for (rsize = 0, rcur = mmf->rcur; rcur && rsize < size;) {
		if (mmf->rpos >= rcur->size) {
			if (!(mmf->rcur = rcur = rcur->next))
				break;
			mmf->rpos = 0;
		}
		csize = XDL_MIN(size - rsize, rcur->size - mmf->rpos);
		memcpy(ptr, rcur->ptr + mmf->rpos, csize);
		rsize += csize;
		ptr += csize;
		mmf->rpos += csize;
	}

	return rsize;
}


long xdl_write_mmfile(mmfile_t *mmf, void const *data, long size) {
	long wsize, bsize, csize;
	mmblock_t *wcur;

	for (wsize = 0; wsize < size;) {
		wcur = mmf->wcur;
		if (wcur && (wcur->flags & XDL_MMB_READONLY))
			return wsize;
		if (!wcur || wcur->size == wcur->bsize ||
		    (mmf->flags & XDL_MMF_ATOMIC && wcur->size + size > wcur->bsize)) {
			bsize = XDL_MAX(mmf->bsize, size);
			if (!(wcur = (mmblock_t *) xdl_malloc(sizeof(mmblock_t) + bsize))) {

				return wsize;
			}
			wcur->flags = 0;
			wcur->ptr = (char *) wcur + sizeof(mmblock_t);
			wcur->size = 0;
			wcur->bsize = bsize;
			wcur->next = NULL;
			if (!mmf->head)
				mmf->head = wcur;
			if (mmf->tail)
				mmf->tail->next = wcur;
			mmf->tail = wcur;
			mmf->wcur = wcur;
		}
		csize = XDL_MIN(size - wsize, wcur->bsize - wcur->size);
		memcpy(wcur->ptr + wcur->size, (char const *) data + wsize, csize);
		wsize += csize;
		wcur->size += csize;
		mmf->fsize += csize;
	}

	return size;
}


long xdl_writem_mmfile(mmfile_t *mmf, mmbuffer_t *mb, int nbuf) {
	int i;
	long size;
	char *data;

	for (i = 0, size = 0; i < nbuf; i++)
		size += mb[i].size;
	if (!(data = (char *) xdl_mmfile_writeallocate(mmf, size)))
		return -1;
	for (i = 0; i < nbuf; i++) {
		memcpy(data, mb[i].ptr, mb[i].size);
		data += mb[i].size;
	}

	return size;
}


void *xdl_mmfile_writeallocate(mmfile_t *mmf, long size) {
	long bsize;
	mmblock_t *wcur;
	char *blk;

	if (!(wcur = mmf->wcur) || wcur->size + size > wcur->bsize) {
		bsize = XDL_MAX(mmf->bsize, size);
		if (!(wcur = (mmblock_t *) xdl_malloc(sizeof(mmblock_t) + bsize))) {

			return NULL;
		}
		wcur->flags = 0;
		wcur->ptr = (char *) wcur + sizeof(mmblock_t);
		wcur->size = 0;
		wcur->bsize = bsize;
		wcur->next = NULL;
		if (!mmf->head)
			mmf->head = wcur;
		if (mmf->tail)
			mmf->tail->next = wcur;
		mmf->tail = wcur;
		mmf->wcur = wcur;
	}

	blk = wcur->ptr + wcur->size;
	wcur->size += size;
	mmf->fsize += size;

	return blk;
}


long xdl_mmfile_ptradd(mmfile_t *mmf, char *ptr, long size, unsigned long flags) {
	mmblock_t *wcur;

	if (!(wcur = (mmblock_t *) xdl_malloc(sizeof(mmblock_t)))) {

		return -1;
	}
	wcur->flags = flags;
	wcur->ptr = ptr;
	wcur->size = wcur->bsize = size;
	wcur->next = NULL;
	if (!mmf->head)
		mmf->head = wcur;
	if (mmf->tail)
		mmf->tail->next = wcur;
	mmf->tail = wcur;
	mmf->wcur = wcur;

	mmf->fsize += size;

	return size;
}


long xdl_copy_mmfile(mmfile_t *mmf, long size, xdemitcb_t *ecb) {
	long rsize, csize;
	mmblock_t *rcur;
	mmbuffer_t mb;

	for (rsize = 0, rcur = mmf->rcur; rcur && rsize < size;) {
		if (mmf->rpos >= rcur->size) {
			if (!(mmf->rcur = rcur = rcur->next))
				break;
			mmf->rpos = 0;
		}
		csize = XDL_MIN(size - rsize, rcur->size - mmf->rpos);
		mb.ptr = rcur->ptr + mmf->rpos;
		mb.size = csize;
		if (ecb->outf(ecb->priv, &mb, 1) < 0) {

			return rsize;
		}
		rsize += csize;
		mmf->rpos += csize;
	}

	return rsize;
}


void *xdl_mmfile_first(mmfile_t *mmf, long *size) {

	if (!(mmf->rcur = mmf->head))
		return NULL;

	*size = mmf->rcur->size;

	return mmf->rcur->ptr;
}


void *xdl_mmfile_next(mmfile_t *mmf, long *size) {

	if (!mmf->rcur || !(mmf->rcur = mmf->rcur->next))
		return NULL;

	*size = mmf->rcur->size;

	return mmf->rcur->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf) {

	return mmf->fsize;
}


int xdl_mmfile_cmp(mmfile_t *mmf1, mmfile_t *mmf2) {
	int cres;
	long size, bsize1, bsize2, size1, size2;
	char const *blk1, *cur1, *top1;
	char const *blk2, *cur2, *top2;

	if ((cur1 = blk1 = (char const*) xdl_mmfile_first(mmf1, &bsize1)) != NULL)
		top1 = blk1 + bsize1;
	if ((cur2 = blk2 = (char const*) xdl_mmfile_first(mmf2, &bsize2)) != NULL)
		top2 = blk2 + bsize2;
	if (!cur1) {
		if (!cur2 || xdl_mmfile_size(mmf2) == 0)
			return 0;
		return -*cur2;
	} else if (!cur2)
		return xdl_mmfile_size(mmf1) ? *cur1: 0;
	for (;;) {
		if (cur1 >= top1) {
			if ((cur1 = blk1 = (char const*) xdl_mmfile_next(mmf1, &bsize1)) != NULL)
				top1 = blk1 + bsize1;
		}
		if (cur2 >= top2) {
			if ((cur2 = blk2 = (char const*) xdl_mmfile_next(mmf2, &bsize2)) != NULL)
				top2 = blk2 + bsize2;
		}
		if (!cur1) {
			if (!cur2)
				break;
			return -*cur2;
		} else if (!cur2)
			return *cur1;
		size1 = top1 - cur1;
		size2 = top2 - cur2;
		size = XDL_MIN(size1, size2);
		if ((cres = memcmp(cur1, cur2, size)) != 0)
			return cres;
		cur1 += size;
		cur2 += size;
	}

	return 0;
}


int xdl_mmfile_compact(mmfile_t *mmfo, mmfile_t *mmfc, long bsize, unsigned long flags) {
	long fsize = xdl_mmfile_size(mmfo), size;
	char *data;
	char const *blk;

	if (xdl_init_mmfile(mmfc, bsize, flags) < 0) {

		return -1;
	}
	if (!(data = (char *) xdl_mmfile_writeallocate(mmfc, fsize))) {

		xdl_free_mmfile(mmfc);
		return -1;
	}
	if ((blk = (char const *) xdl_mmfile_first(mmfo, &size)) != NULL) {
		do {
			memcpy(data, blk, size);
			data += size;
		} while ((blk = (char const *) xdl_mmfile_next(mmfo, &size)) != NULL);
	}

	return 0;
}


int xdl_mmfile_outf(void *priv, mmbuffer_t *mb, int nbuf) {
	mmfile_t *mmf = (mmfile_t*) priv;

	if (xdl_writem_mmfile(mmf, mb, nbuf) < 0) {

		return -1;
	}

	return 0;
}


int xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = NULL;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = NULL;
	cha->scurr = 0;

	return 0;
}


void xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	for (cur = cha->head; (tmp = cur) != NULL;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


void *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	void *data;

	if (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		if (!(ancur = (chanode_t *) xdl_malloc(sizeof(chanode_t) + cha->nsize))) {

			return NULL;
		}
		ancur->icurr = 0;
		ancur->next = NULL;
		if (cha->tail)
			cha->tail->next = ancur;
		if (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (char *) ancur + sizeof(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	return data;
}


void *xdl_cha_first(chastore_t *cha) {
	chanode_t *sncur;

	if (!(cha->sncur = sncur = cha->head))
		return NULL;

	cha->scurr = 0;

	return (char *) sncur + sizeof(chanode_t) + cha->scurr;
}


void *xdl_cha_next(chastore_t *cha) {
	chanode_t *sncur;

	if (!(sncur = cha->sncur))
		return NULL;
	cha->scurr += cha->isize;
	if (cha->scurr == sncur->icurr) {
		if (!(sncur = cha->sncur = sncur->next))
			return NULL;
		cha->scurr = 0;
	}

	return (char *) sncur + sizeof(chanode_t) + cha->scurr;
}


long xdl_guess_lines(mmfile_t *mf) {
	long nl = 0, size, tsize = 0;
	char const *data, *cur, *top;

	if ((cur = data = (char const*) xdl_mmfile_first(mf, &size)) != NULL) {
		for (top = data + size; nl < XDL_GUESS_NLINES;) {
			if (cur >= top) {
				tsize += (long) (cur - data);
				if (!(cur = data = (char const*) xdl_mmfile_next(mf, &size)))
					break;
				top = data + size;
			}
			nl++;
			if (!(cur = (char const*) memchr(cur, '\n', top - cur)))
				cur = top;
			else
				cur++;
		}
		tsize += (long) (cur - data);
	}

	if (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	return nl + 1;
}


unsigned long xdl_hash_record(char const **data, char const *top) {
	unsigned long ha = 5381;
	char const *ptr = *data;

	for (; ptr < top && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*data = ptr < top ? ptr + 1: ptr;

	return ha;
}


unsigned int xdl_hashbits(unsigned int size) {
	unsigned int val = 1, bits = 0;

	for (; val < size && bits < CHAR_BIT * sizeof(unsigned int); val <<= 1, bits++);
	return bits ? bits: 1;
}


int xdl_num_out(char *out, long val) {
	char *ptr, *str = out;
	char buf[32];

	ptr = buf + sizeof(buf) - 1;
	*ptr = '\0';
	if (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	for (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	if (*ptr)
		for (; *ptr; ptr++, str++)
			*str = *ptr;
	else
		*str++ = '0';
	*str = '\0';

	return str - out;
}


long xdl_atol(char const *str, char const **next) {
	long val, base;
	char const *top;

	for (top = str; XDL_ISDIGIT(*top); top++);
	if (next)
		*next = top;
	for (val = 0, base = 1, top--; top >= str; top--, base *= 10)
		val += base * (long)(*top - '0');
	return val;
}


int xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2, xdemitcb_t *ecb) {
	int nb = 0;
	mmbuffer_t mb;
	char buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_num_out(buf + nb, c1 ? s1: s1 - 1);

	memcpy(buf + nb, ",", 1);
	nb += 1;

	nb += xdl_num_out(buf + nb, c1);

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_num_out(buf + nb, c2 ? s2: s2 - 1);

	memcpy(buf + nb, ",", 1);
	nb += 1;

	nb += xdl_num_out(buf + nb, c2);

	memcpy(buf + nb, " @@\n", 4);
	nb += 4;

	mb.ptr = buf;
	mb.size = nb;
	if (ecb->outf(ecb->priv, &mb, 1) < 0)
		return -1;

	return 0;
}

/* END xutils.c */



#endif /* LIBXDIFF_IMPLEMENTATION */
