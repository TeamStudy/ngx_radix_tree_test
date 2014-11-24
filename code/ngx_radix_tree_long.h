
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_RADIX_TREE_LONG_H_INCLUDED_
#define _NGX_RADIX_TREE_LONG_H_INCLUDED_
#include "ngx_radix_tree.h"


//���붨��
#define MASK_LONG_1		0x000000000000000fLL		/*1λ��*/
#define MASK_LONG_2		0x00000000000000ffLL		/*2λ��*/
#define MASK_LONG_3		0x0000000000000fffLL		/*3λ��*/
#define MASK_LONG_4		0x000000000000ffffLL		/*4λ��*/
#define MASK_LONG_5		0x000000000000ffffLL		/*5λ��*/
#define MASK_LONG_6		0x00000000000fffffLL		/*6λ��*/
#define MASK_LONG_7		0x0000000000ffffffLL		/*7λ��*/
#define MASK_LONG_8		0x000000000fffffffLL		/*8λ��*/
#define MASK_LONG_9		0x00000000ffffffffLL		/*9λ��*/
#define MASK_LONG_10	0x0000000fffffffffLL		/*10λ��*/

typedef unsigned long long ngx_64_int;

ngx_radix_tree_t *ngx_radix_64tree_create(BUFF_NODE *pool,
    int preallocate);

int ngx_radix64tree_insert(ngx_radix_tree_t *tree,
		ngx_64_int key, ngx_64_int mask, unsigned int* value);
int ngx_radix64tree_delete(ngx_radix_tree_t *tree,
		ngx_64_int key, ngx_64_int mask);
unsigned int* ngx_radix64tree_find(ngx_radix_tree_t *tree, ngx_64_int key);




#endif /* _NGX_RADIX_TREE_H_INCLUDED_ */