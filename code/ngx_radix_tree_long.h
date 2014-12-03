
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_RADIX_TREE_LONG_H_INCLUDED_
#define _NGX_RADIX_TREE_LONG_H_INCLUDED_
#include "ngx_radix_tree.h"


//掩码定义
#define MASK_LONG_1    0x000000000000000fULL		/*1位数*/
#define MASK_LONG_2    0x00000000000000ffULL		/*2位数*/
#define MASK_LONG_3    0x0000000000000fffULL		/*3位数*/
#define MASK_LONG_4    0x000000000000ffffULL		/*4位数*/
#define MASK_LONG_5    0x000000000000ffffULL		/*5位数*/
#define MASK_LONG_6    0x00000000000fffffULL		/*6位数*/
#define MASK_LONG_7    0x0000000000ffffffULL		/*7位数*/
#define MASK_LONG_8    0x000000000fffffffULL		/*8位数*/
#define MASK_LONG_9    0x00000000ffffffffULL		/*9位数*/
#define MASK_LONG_10   0x0000000fffffffffULL		/*10位数*/

typedef unsigned long long ngx_64_int;

ngx_radix_tree_t *ngx_radix_64tree_create(BUFF_NODE *pool,
    int preallocate);

int ngx_radix64tree_insert(ngx_radix_tree_t *tree,
		ngx_64_int key, ngx_64_int mask, ngx_uint_ptr_t ptr_struct, VALUE_TYPE value);
int ngx_radix64tree_insert_init(ngx_radix_tree_t *tree,
		ngx_64_int key, ngx_64_int mask, ngx_uint_ptr_t ptr_struct);
int ngx_radix64tree_delete(ngx_radix_tree_t *tree,
		ngx_64_int key, ngx_64_int mask);
unsigned int* ngx_radix64tree_find(ngx_radix_tree_t *tree, ngx_64_int key);




#endif /* _NGX_RADIX_TREE_H_INCLUDED_ */
