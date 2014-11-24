
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_RADIX_TREE_H_INCLUDED_
#define _NGX_RADIX_TREE_H_INCLUDED_






#define NGX_RADIX_NO_VALUE   (unsigned int *) -1
#define HAVE_VALUE   (unsigned int *) 1

//���붨��
#define MASK_1	0x0000000f	/*1λ����0x0000000f*/
#define MASK_2	0x000000ff	/*2λ����0x000000ff*/
#define MASK_3	0x00000fff	/*3λ����0x00000fff*/
#define MASK_4	0x0000ffff	/*4λ����0x0000ffff*/
#define MASK_5	0x0000ffff	/*5λ����0x0000ffff*/
#define MASK_6	0x000fffff	/*6λ����0x000fffff*/
#define MASK_7	0x00ffffff	/*7λ����0x00ffffff*/
#define MASK_8	0x0fffffff	/*8λ����0x0fffffff*/
#define MASK_9	0xffffffff	/*9λ����0xffffffff*/
#define MASK_10	0xffffffff	/*10λ����1000000000~4294967295��0xffffffff*/
#define TOTAL_TEST 1000000
#define HUNDRED_THOUSAND 100000
#define MY_INT_MAX 4200000000U


#define  NGX_OK          0
#define  NGX_ERROR      -1
#define  NGX_BUSY       -3

typedef int        ngx_int_t;
typedef unsigned int       ngx_uint_t;
typedef unsigned int*       ngx_uint_ptr_t;
typedef int*        ngx_flag_t;
typedef unsigned int  ngx_pagesize;

typedef struct ngx_radix_node_s  ngx_radix_node_t;

struct ngx_radix_node_s {
    ngx_radix_node_t  *right;
    ngx_radix_node_t  *left;
    ngx_radix_node_t  *parent;
    ngx_uint_ptr_t          value;
};
/*PHONE���ڲ��Դ洢����long long������룬phonebookΪ��ţ�tempΪ�洢��ŵ����飬stateΪ��������Ԫ�ظ���*/
typedef struct PHONE
{

	long long phonebook;
	struct PHONE* list;
}PHONE;
/*PHONESTORE����ʵ�ʴ洢����char����洢��ţ�phonebookΪ��ţ�tempΪ�洢��ŵ�ָ�����飬stateΪ��������Ԫ�ظ���*/
typedef struct
{

	char phonebook[10];
	char** temp;
	short state;
}PHONESTORE;
typedef struct BUFF_NODE
{
	unsigned char* s_data_cell;
	struct BUFF_NODE* s_next;
}BUFF_NODE;
typedef struct {
    ngx_radix_node_t  *root;
    BUFF_NODE        *pool;
    ngx_radix_node_t  *free;
    char              *start;
    ngx_uint_t             size;
} ngx_radix_tree_t;
ngx_radix_node_t *ngx_radix_alloc(ngx_radix_tree_t *tree);
ngx_radix_tree_t * ngx_palloc1(BUFF_NODE *pool, int size);

ngx_radix_tree_t *ngx_radix_tree_create(BUFF_NODE *pool,
    ngx_int_t preallocate);

ngx_int_t ngx_radix32tree_insert(ngx_radix_tree_t *tree,
		ngx_uint_t key, ngx_uint_t mask, ngx_uint_ptr_t value);
ngx_int_t ngx_radix32tree_delete(ngx_radix_tree_t *tree,
		ngx_uint_t key, ngx_uint_t mask);
ngx_uint_ptr_t ngx_radix32tree_find(ngx_radix_tree_t *tree, ngx_uint_t key);

#if (NGX_HAVE_INET6)
ngx_int_t ngx_radix128tree_insert(ngx_radix_tree_t *tree,
    u_char *key, u_char *mask, unsigned int* value);
ngx_int_t ngx_radix128tree_delete(ngx_radix_tree_t *tree,
    u_char *key, u_char *mask);
unsigned int* ngx_radix128tree_find(ngx_radix_tree_t *tree, u_char *key);
#endif


#endif /* _NGX_RADIX_TREE_H_INCLUDED_ */
