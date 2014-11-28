
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




ngx_radix_tree_t * ngx_palloc_tree(BUFF_NODE *pool, int size)
{
	ngx_radix_tree_t * temp=(ngx_radix_tree_t *)malloc(sizeof(ngx_radix_tree_t) );;

	return temp;
}
ngx_radix_tree_t * ngx_radix_tree_create(BUFF_NODE *pool, int preallocate)
{
	ngx_uint_t           key, mask, inc;
    ngx_radix_tree_t  *tree;

    tree = ngx_palloc_tree(pool, sizeof(ngx_radix_tree_t));
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
            if (ngx_radix32tree_init_insert(tree, key, mask, NGX_RADIX_NO_VALUE)
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
ngx_int_t ngx_radix32tree_init_insert(ngx_radix_tree_t *tree, ngx_uint_t key, ngx_uint_t mask, ngx_uint_ptr_t ptr_struct)
{
	ngx_uint_t           bit;
    static int renum=1;
    static int re=1;
    ngx_radix_node_t  *node, *next;

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
        	return NGX_BUSY;
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

ngx_int_t ngx_radix32tree_insert(ngx_radix_tree_t *tree, ngx_64_int key_long, ngx_uint_t mask,ngx_uint_ptr_t ptr_struct)
{
	ngx_uint_t key = key_long % MY_INT_MAX;
	ngx_uint_t bit;
    static int renum=1;
    static int re=1;
    ngx_radix_node_t  *node, *next;

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
        	PHONE* key_old=(PHONE*)(node->value);
        	PHONE* key_new=(PHONE*)ptr_struct;
        	if(key_old->phonebook==key_long)
        	{
        		printf("repeat phone! num:%d \n",re);
        		re++;
        		node->value = ptr_struct;//重复则替换
        		return NGX_OK;
        	}
        	else
        	{
        		printf("repeat!  num:%d  phone:%lld\n",renum,key_long);
        		renum++;
        		if(key_old->next==NULL)
        		{
        			key_old->next=(PHONE*)malloc(sizeof(PHONE));
        			key_old->next=key_new;
        			//printf("first create list\n");
        		}
        		else if(key_old->next->next==NULL)
        		{
        			key_old->next->next=(PHONE*)malloc(sizeof(PHONE));
        			key_old->next->next=key_new;
        			//printf("second create list\n");
        		}
        		return NGX_OK;
        	}
        }

        node->value = ptr_struct;
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

    node->value = ptr_struct;

    return NGX_OK;
}

/*信息插入函数*/
/*返回值：  插入成功： NGX_OK   插入失败：NGX_ERROR*/
ngx_int_t ngx_radix32tree_my_insert(ngx_radix_tree_t *tree,ngx_64_int key_long_with1, ngx_uint_t mask,ngx_uint_ptr_t ptr_struct, VALUE_TYPE value)
{
	ngx_64_int key_long = phone_11_to_10(key_long_with1);

	ngx_int_t err = ngx_radix32tree_insert(tree, key_long, mask,ptr_struct);
	return err;
}

ngx_64_int phone_11_to_10(ngx_64_int key_long_with1)
{
	char phone_old[11];
	sprintf(phone_old,"%lld",key_long_with1);
	char phone_new[10];
	strncpy(phone_new,phone_old+1,10);
	ngx_64_int key_long = atoll(phone_new);
	return key_long;
}


ngx_int_t ngx_radix32tree_delete(ngx_radix_tree_t *tree, ngx_64_int key_long, ngx_uint_t mask)
{
	ngx_uint_t key = key_long % MY_INT_MAX;
	ngx_uint_t           bit;
    ngx_radix_node_t  *node;

    bit = 0x00000001;
    node = tree->root;

    while (node && (bit & mask))
    {
        if (key & bit)
        {
            node = node->right;
        }
        else
        {
            node = node->left;
        }

        bit <<= 1;
    }

    if (node == NULL)
    {
        return NGX_ERROR;
    }
    /*前N层找到了有效节点*/
    if (node->right || node->left)
    {
        if (node->value != NGX_RADIX_NO_VALUE)
        {
        	PHONE* phone=(PHONE*)(node->value);
        	ngx_64_int key_long_exist = phone_11_to_10(phone->phonebook);
        	if(key_long_exist==key_long)
        	{
        		if(phone->next)
        		{
        			node->value=phone->next;
        			free(phone);
        			phone=NULL;
        			return NGX_OK;
        		}
        		else
        		{
        			node->value = NGX_RADIX_NO_VALUE;
        		    return NGX_OK;
        		}
        	}
        	else
        	{
        		PHONE* phone_front=phone;
        		phone = phone->next;
        		if(phone)
        		{
        			ngx_64_int key_long_exit = phone_11_to_10(phone->phonebook);
        			if(key_long_exit==key_long)
        			{
        				phone_front->next=phone->next;
        				free(phone);
        				phone=NULL;
        				return NGX_OK;
        			}
        			else
        			{
        				if(phone->next)
        				{
        					free(phone->next);
        					phone->next = NULL;
        					return NGX_OK;
        				}
        				return NGX_ERROR;
        			}
        		}
        		return NGX_ERROR;
        	}
        }

        return NGX_ERROR;
    }
    /*最后一层找到了有效节点*/
    if(!node->right&&!node->left&&node->value!=NGX_RADIX_NO_VALUE)
    {
    	PHONE* phone=(PHONE*)(node->value);
    	ngx_64_int key_long_exit = phone_11_to_10(phone->phonebook);
    	if(key_long_exit==key_long)
    	{
    		if(phone->next)
    		{
    			node->value=phone->next;
    			free(phone);
    			phone=NULL;
    			return NGX_OK;
    		}
    		else
    		{
    		    for ( ;; )
    		    {
    		        if (node->parent->right == node)
    		        {
    		            node->parent->right = NULL;
    		        }
    		        else
    		        {
    		            node->parent->left = NULL;
    		        }

    		        tree->free = node;
    		        node = node->parent;
    		        free(tree->free);
    		        tree->free=NUll;

    		        if (node->right || node->left)
    		        {
    		            break;
    		        }

    		        if (node->value != NGX_RADIX_NO_VALUE)
    		        {
    		            break;
    		        }

    		        if (node->parent == NULL)
    		        {
    		            break;
    		        }
    		    }
    		    return NGX_OK;
    		}
    	}
    	else
    	{
    		PHONE* phone_front=phone;
    		phone = phone->next;
    		if(phone)
    		{
    			ngx_64_int key_long_exit = phone_11_to_10(phone->phonebook);
    			if(key_long_exit==key_long)
    			{
    				phone_front->next=phone->next;
    				free(phone);
    				phone=NULL;
    				return NGX_OK;
    			}
    			else
    			{
    				if(phone->next)
    				{
    					free(phone->next);
    					phone->next = NULL;
    					return NGX_OK;
    				}
    				return NGX_ERROR;
    			}
    		}
    		return NGX_ERROR;
    	}
    }
    return NGX_ERROR;
}

/*信息删除函数*/
/*返回值： 找到并删除了：NGX_OK   没找到：NGX_ERROR*/
ngx_int_t ngx_radix32tree_my_delete(ngx_radix_tree_t *tree,ngx_64_int key_long_with1, ngx_uint_t mask)
{
	ngx_64_int key_long = phone_11_to_10(key_long_with1);
	ngx_int_t err = ngx_radix32tree_delete(tree,key_long, mask);
	return err;
}

ngx_uint_ptr_t ngx_radix32tree_find(ngx_radix_tree_t *tree, ngx_64_int key_long)
{
	ngx_uint_t key = key_long % MY_INT_MAX;
	ngx_uint_t           bit;
	ngx_uint_ptr_t           ptr_struct;
    ngx_radix_node_t  *node;

    bit = 0x00000001;
    ptr_struct = NGX_RADIX_NO_VALUE;
    node = tree->root;

    while (node)
    {
        if (node->value != NGX_RADIX_NO_VALUE)
        {
        	PHONE* phone=(PHONE*)(node->value);
        	while(phone)
        	{
        		ngx_64_int key_long_exist = phone_11_to_10(phone->phonebook);
        		if(key_long_exist == key_long)
        		{
        			ptr_struct = (ngx_uint_ptr_t)phone;
        		}
        		phone=phone->next;
        	}

        }

        if (key & bit)
        {
            node = node->right;

        }
        else
        {
            node = node->left;
        }

        bit <<= 1;
    }

    return ptr_struct;
}

/*信息查找函数*/
/*返回值：没找到：NO_VALUE 找到了：VALUE_TYPE(用户自定义结构体)   */
VALUE_TYPE ngx_radix32tree_my_find(ngx_radix_tree_t *tree, ngx_64_int key_long_with1)
{
	VALUE_TYPE goal=NO_VALUE;
	ngx_64_int key_long = phone_11_to_10(key_long_with1);
	ngx_uint_ptr_t ptr_struct = ngx_radix32tree_find(tree, key_long);
	if(ptr_struct!=NGX_RADIX_NO_VALUE)
	{
		PHONE* phone_goal = (PHONE*)ptr_struct;
		goal = phone_goal->value;
	}
	return goal;
}

 ngx_radix_node_t * ngx_radix_alloc(ngx_radix_tree_t *tree)
{
    ngx_radix_node_t  *p;
    p=(ngx_radix_node_t *)malloc(sizeof(ngx_radix_node_t));

    return p;
}

