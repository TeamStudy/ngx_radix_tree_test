
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */



#include "ngx_radix_tree.h"

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include <string.h>
#include <ext/hash_map>
using namespace __gnu_cxx;
//#include <sys/types.h>


//#include <arpa/inet.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <sys/socket.h>
//#include <pthread.h>
//#include <iostream>
//using namespace std;




ngx_radix_tree_t * ngx_palloc1(BUFF_NODE *pool, int size)
{
	ngx_radix_tree_t * temp=(ngx_radix_tree_t *)malloc(sizeof(ngx_radix_tree_t) );;

	return temp;
}
ngx_radix_tree_t * ngx_radix_tree_create(BUFF_NODE *pool, int preallocate)
{
	ngx_uint_t           key, mask, inc;
    ngx_radix_tree_t  *tree;

    tree = ngx_palloc1(pool, sizeof(ngx_radix_tree_t));
    if (tree == NULL) {
        return NULL;
    }

    tree->pool = pool;
    tree->free = NULL;
    tree->start = NULL;
    tree->size = 0;

    tree->root = ngx_radix_alloc(tree);
    if (tree->root == NULL) {
        return NULL;
    }

    tree->root->right = NULL;
    tree->root->left = NULL;
    tree->root->parent = NULL;
    tree->root->value = NGX_RADIX_NO_VALUE;

    if (preallocate == 0) {
        return tree;
    }

    /*
     * Preallocation of first nodes : 0, 1, 00, 01, 10, 11, 000, 001, etc.
     * increases TLB hits even if for first lookup iterations.
     * On 32-bit platforms the 7 preallocated bits takes continuous 4K,
     * 8 - 8K, 9 - 16K, etc.  On 64-bit platforms the 6 preallocated bits
     * takes continuous 4K, 7 - 8K, 8 - 16K, etc.  There is no sense to
     * to preallocate more than one page, because further preallocation
     * distributes the only bit per page.  Instead, a random insertion
     * may distribute several bits per page.
     *
     * Thus, by default we preallocate maximum
     *     6 bits on amd64 (64-bit platform and 4K pages)
     *     7 bits on i386 (32-bit platform and 4K pages)
     *     7 bits on sparc64 in 64-bit mode (8K pages)
     *     8 bits on sparc64 in 32-bit mode (8K pages)
     */
    ngx_uint_t  ngx_pagesize;
    if (preallocate == -1) {

            preallocate = 7;




    }

    mask = 0;
    inc = 0x00000001;

    while (preallocate--) {

        key = 0xffffffff;
        mask <<= 1;
        mask |= 0x00000001;

        do {
            if (ngx_radix32tree_insert(tree, key, mask, NGX_RADIX_NO_VALUE)
                != NGX_OK)
            {
                return NULL;
            }

            key -= inc;

        } while (key&mask);

      //  inc <<= 1;
    }

    return tree;
}


int ngx_radix32tree_insert(ngx_radix_tree_t *tree, ngx_uint_t key, ngx_uint_t mask, ngx_uint_ptr_t value)
{
	ngx_uint_t           bit;
    static int renum=1;
    static int re=1;
    ngx_radix_node_t  *node, *next;
    PHONE* phone_key=(PHONE*)value;
    bit = 0x00000001;

    node = tree->root;
    next = tree->root;

    while (bit & mask)
    {
        if (key & bit)
        {
            next = node->right;
        }
        else
        {
            next = node->left;
        }

        if (next == NULL)
        {
            break;
        }

        bit <<= 1;
        node = next;
    }

    if (next)
    {
        if (node->value != NGX_RADIX_NO_VALUE)
        {
        	unsigned int* key1=ngx_radix32tree_find(tree, key);
        	PHONE* key2=(PHONE*)key1;
        	//printf("last num:%lld\n",key2->phonebook);
        	if(key2->phonebook==phone_key->phonebook)
        	{
        		printf("repeat phone! num:%d \n",re);
        		re++;
        		return NGX_OK;
        	}
        	else
        	{
        		printf("repeat!  num:%d  phone:%lld\n",renum,phone_key->phonebook);
        		renum++;
        		if(key2->list==NULL)
        		{
        			key2->list=(PHONE* )malloc(sizeof(PHONE));
        			key2->list=phone_key;
        			//printf("first create list\n");
        		}
        		else if(key2->list->list==NULL)
        		{
        			key2->list->list=(PHONE* )malloc(sizeof(PHONE));
        			key2->list->list =phone_key;
        			//printf("second create list\n");
        		}
        		return NGX_OK;
        	}
        }

        node->value = value;
        return NGX_OK;
    }

    while (bit & mask)
    {
        next = ngx_radix_alloc(tree);
        if(next == NULL)
        {
        	//next = ngx_radix_alloc(tree);
            return NGX_ERROR;
        }

        next->right = NULL;
        next->left = NULL;
        next->parent = node;
        next->value = NGX_RADIX_NO_VALUE;

        if (key & bit)
        {
            node->right = next;
        }
        else
        {
            node->left = next;
        }

        bit <<= 1;
        node = next;
    }

    node->value = value;

    return NGX_OK;
}


int ngx_radix32tree_delete(ngx_radix_tree_t *tree, ngx_uint_t key, ngx_uint_t mask)
{
	ngx_uint_t           bit;
    ngx_radix_node_t  *node;

    bit = 0x00000001;
    node = tree->root;

    while (node && (bit & mask)) {
        if (key & bit) {
            node = node->right;

        } else {
            node = node->left;
        }

        bit <<= 1;
    }

    if (node == NULL) {
        return NGX_ERROR;
    }

    if (node->right || node->left) {
        if (node->value != NGX_RADIX_NO_VALUE) {
            node->value = NGX_RADIX_NO_VALUE;
            return NGX_OK;
        }

        return NGX_ERROR;
    }

    for ( ;; ) {
        if (node->parent->right == node) {
            node->parent->right = NULL;

        } else {
            node->parent->left = NULL;
        }

        node->right = tree->free;
        tree->free = node;

        node = node->parent;

        if (node->right || node->left) {
            break;
        }

        if (node->value != NGX_RADIX_NO_VALUE) {
            break;
        }

        if (node->parent == NULL) {
            break;
        }
    }

    return NGX_OK;
}


ngx_uint_ptr_t ngx_radix32tree_find(ngx_radix_tree_t *tree, ngx_uint_t key)
{
	ngx_uint_t           bit;
	ngx_uint_ptr_t          value;
    ngx_radix_node_t  *node;

    bit = 0x00000001;
    value = NGX_RADIX_NO_VALUE;
    node = tree->root;

    while (node) {
        if (node->value != NGX_RADIX_NO_VALUE) {
            value = node->value;
        }

        if (key & bit) {
            node = node->right;

        } else {
            node = node->left;
        }

        bit <<= 1;
    }

    return value;
}


#if (NGX_HAVE_INET6)

int
ngx_radix128tree_insert(ngx_radix_tree_t *tree, u_char *key, u_char *mask,
    unsigned int* value)
{
    u_char             bit;
    ngx_uint_t         i;
    ngx_radix_node_t  *node, *next;

    i = 0;
    bit = 0x80;

    node = tree->root;
    next = tree->root;

    while (bit & mask[i]) {
        if (key[i] & bit) {
            next = node->right;

        } else {
            next = node->left;
        }

        if (next == NULL) {
            break;
        }

        bit >>= 1;
        node = next;

        if (bit == 0) {
            if (++i == 16) {
                break;
            }

            bit = 0x80;
        }
    }

    if (next) {
        if (node->value != NGX_RADIX_NO_VALUE) {
            return NGX_BUSY;
        }

        node->value = value;
        return NGX_OK;
    }

    while (bit & mask[i]) {
        next = ngx_radix_alloc(tree);
        if (next == NULL) {
            return NGX_ERROR;
        }

        next->right = NULL;
        next->left = NULL;
        next->parent = node;
        next->value = NGX_RADIX_NO_VALUE;

        if (key[i] & bit) {
            node->right = next;

        } else {
            node->left = next;
        }

        bit >>= 1;
        node = next;

        if (bit == 0) {
            if (++i == 16) {
                break;
            }

            bit = 0x80;
        }
    }

    node->value = value;

    return NGX_OK;
}


int
ngx_radix128tree_delete(ngx_radix_tree_t *tree, u_char *key, u_char *mask)
{
    u_char             bit;
    ngx_uint_t         i;
    ngx_radix_node_t  *node;

    i = 0;
    bit = 0x80;
    node = tree->root;

    while (node && (bit & mask[i])) {
        if (key[i] & bit) {
            node = node->right;

        } else {
            node = node->left;
        }

        bit >>= 1;

        if (bit == 0) {
            if (++i == 16) {
                break;
            }

            bit = 0x80;
        }
    }

    if (node == NULL) {
        return NGX_ERROR;
    }

    if (node->right || node->left) {
        if (node->value != NGX_RADIX_NO_VALUE) {
            node->value = NGX_RADIX_NO_VALUE;
            return NGX_OK;
        }

        return NGX_ERROR;
    }

    for ( ;; ) {
        if (node->parent->right == node) {
            node->parent->right = NULL;

        } else {
            node->parent->left = NULL;
        }

        node->right = tree->free;
        tree->free = node;

        node = node->parent;

        if (node->right || node->left) {
            break;
        }

        if (node->value != NGX_RADIX_NO_VALUE) {
            break;
        }

        if (node->parent == NULL) {
            break;
        }
    }

    return NGX_OK;
}


unsigned int*
ngx_radix128tree_find(ngx_radix_tree_t *tree, u_char *key)
{
    u_char             bit;
    unsigned int*          value;
    ngx_uint_t         i;
    ngx_radix_node_t  *node;

    i = 0;
    bit = 0x80;
    value = NGX_RADIX_NO_VALUE;
    node = tree->root;

    while (node) {
        if (node->value != NGX_RADIX_NO_VALUE) {
            value = node->value;
        }

        if (key[i] & bit) {
            node = node->right;

        } else {
            node = node->left;
        }

        bit >>= 1;

        if (bit == 0) {
            i++;
            bit = 0x80;
        }
    }

    return value;
}

#endif



 ngx_radix_node_t * ngx_radix_alloc(ngx_radix_tree_t *tree)
{
    ngx_radix_node_t  *p;
    p=(ngx_radix_node_t *)malloc(sizeof(ngx_radix_node_t));


    return p;
}

