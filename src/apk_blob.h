/* apk_blob.h - Alpine Package Keeper (APK)
 *
 * Copyright (C) 2005-2008 Natanael Copa <n@tanael.org>
 * Copyright (C) 2008 Timo Teräs <timo.teras@iki.fi>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation. See http://www.gnu.org/ for details.
 */

#ifndef APK_BLOB_H
#define APK_BLOB_H

#include <string.h>
#include <openssl/evp.h>

#include "apk_defines.h"

struct apk_blob {
	unsigned int len;
	char *ptr;
};
typedef struct apk_blob apk_blob_t;
typedef int (*apk_blob_cb)(void *ctx, apk_blob_t blob);

#define APK_CHECKSUM_NONE	0
#define APK_CHECKSUM_MD5	16
#define APK_CHECKSUM_SHA1	20

/* Internal cointainer for MD5 or SHA1 */
struct apk_checksum {
	unsigned char data[20];
	unsigned char type;
};

static inline const EVP_MD *apk_default_checksum(void)
{
	return EVP_sha1();
}

static inline const EVP_MD *apk_get_digest(int type)
{
	switch (type) {
	case APK_CHECKSUM_MD5:
		return EVP_md5();
	case APK_CHECKSUM_SHA1:
		return EVP_sha1();
	}
	return EVP_md_null();
}

#define APK_BLOB_IS_NULL(blob)		((blob).ptr == NULL)

#define APK_BLOB_NULL			((apk_blob_t){0, NULL})
#define APK_BLOB_BUF(buf)		((apk_blob_t){sizeof(buf), (char *)(buf)})
#define APK_BLOB_CSUM(csum)		((apk_blob_t){(csum).type, (char *)(csum).data})
#define APK_BLOB_STRUCT(s)		((apk_blob_t){sizeof(s), (char*)&(s)})
#define APK_BLOB_PTR_LEN(beg,len)	((apk_blob_t){(len), (beg)})
#define APK_BLOB_PTR_PTR(beg,end)	APK_BLOB_PTR_LEN((beg),(end)-(beg)+1)

static inline apk_blob_t APK_BLOB_STR(const char *str)
{
	if (str == NULL)
		return APK_BLOB_NULL;
	return ((apk_blob_t){strlen(str), (void *)(str)});
}

char *apk_blob_cstr(apk_blob_t str);
int apk_blob_spn(apk_blob_t blob, const char *accept, apk_blob_t *l, apk_blob_t *r);
int apk_blob_cspn(apk_blob_t blob, const char *reject, apk_blob_t *l, apk_blob_t *r);
int apk_blob_split(apk_blob_t blob, apk_blob_t split, apk_blob_t *l, apk_blob_t *r);
int apk_blob_rsplit(apk_blob_t blob, char split, apk_blob_t *l, apk_blob_t *r);
unsigned long apk_blob_hash_seed(apk_blob_t, unsigned long seed);
unsigned long apk_blob_hash(apk_blob_t str);
int apk_blob_compare(apk_blob_t a, apk_blob_t b);
int apk_blob_for_each_segment(apk_blob_t blob, const char *split,
			      apk_blob_cb cb, void *ctx);

static inline void apk_blob_checksum(apk_blob_t b, const EVP_MD *md, struct apk_checksum *csum)
{
	csum->type = EVP_MD_size(md);
	EVP_Digest(b.ptr, b.len, csum->data, NULL, md, NULL);
}
static inline const int apk_checksum_compare(const struct apk_checksum *a,
					     const struct apk_checksum *b)
{
	return apk_blob_compare(APK_BLOB_PTR_LEN((char *) a->data, a->type),
				APK_BLOB_PTR_LEN((char *) b->data, b->type));
}

void apk_blob_push_blob(apk_blob_t *to, apk_blob_t literal);
void apk_blob_push_uint(apk_blob_t *to, unsigned int value, int radix);
void apk_blob_push_csum(apk_blob_t *to, struct apk_checksum *csum);
void apk_blob_push_base64(apk_blob_t *to, apk_blob_t binary);
void apk_blob_push_hexdump(apk_blob_t *to, apk_blob_t binary);

void apk_blob_pull_char(apk_blob_t *b, int expected);
unsigned int apk_blob_pull_uint(apk_blob_t *b, int radix);
void apk_blob_pull_csum(apk_blob_t *b, struct apk_checksum *csum);
void apk_blob_pull_base64(apk_blob_t *b, apk_blob_t to);
void apk_blob_pull_hexdump(apk_blob_t *b, apk_blob_t to);

#endif
