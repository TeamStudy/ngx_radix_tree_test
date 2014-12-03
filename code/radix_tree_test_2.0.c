#include "ngx_radix_tree.h"
#include "ngx_radix_tree_long.h"

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>//

#include <string.h>
#include <ext/hash_map>
using namespace __gnu_cxx;

#define TOTAL_TEST 1000000

PHONE *phonepack=NULL;

int secondsold, secondsnew, second;
int minisecondsold,minisecondsnew,minisecond;
struct timeval t;

ngx_radix_tree_t * radixTree = NULL;
ngx_radix_tree_t * radixTree_64 = NULL;

hash_map<int,int> phonemap;
pair< hash_map< int,int >::iterator,bool > ret;

unsigned int maxint=0;

void test_init()
{
	/*����1֮��10λ�������*/

	int iseed[10]={0,1,2,3,4,5,6,7,8,9};
	for(int j=0;j<TOTAL_TEST;j++)
	{
		phonepack[j].phonebook=0;
		phonepack[j].next=NULL;
		phonepack[j].value=NULL;
	}
	int phonecell;
	char phone[12];
	char add[10];

	int z=1;
	memset(phone, 0, sizeof(phone));
	sprintf(phone,"%d",z);
	srand((unsigned)time(NULL));
	for(int jj=0;jj<TOTAL_TEST;jj++)
	{
		for(int i=0;i<10;i++)
		{
			phonecell=iseed[rand()%10];
			sprintf(add,"%d",phonecell);
			strcat(phone,add);
		}
		phonepack[jj].phonebook=atoll(phone);
		memset(phone, 0, sizeof(phone));
		sprintf(phone,"%d",z);
	}
	/*���ǰ50������*/
	printf("********top 50 of the list********\n");
	for(int q=0;q<50;q++)
	{
		printf("%lld\n",phonepack[q].phonebook);
	}
	printf("**********************************\n");
}

void time_start()
{
	/*��ʼ��ʱ*/
	gettimeofday(&t, NULL);
	secondsold=t.tv_sec;
	minisecondsold=t.tv_usec/1000;
	printf("start time:%d:%d\n",t.tv_sec,t.tv_usec);
}
void time_end()
{
	/*������ʱ*/
	gettimeofday(&t, NULL);
	secondsnew=t.tv_sec;
	minisecondsnew=t.tv_usec/1000;
	printf("end time:%d:%d\n",t.tv_sec,t.tv_usec);

	second=secondsnew-secondsold;
	minisecond=minisecondsnew-minisecondsold;

	if(minisecond>=0)
		printf("time total:%d.%ds\n",second,minisecond);
	else
	{
		minisecond=1000-minisecondsold+minisecondsnew;
		second--;
		printf("time total:%d.%ds\n",second,minisecond);
	}
}
void test_radix32tree()
{
	int err=0;
	for(int num=0;num<TOTAL_TEST;num++)
	{
		if((num+1)%HUNDRED_THOUSAND==0)
		{
			printf("insert num: %d\n",num+1);
		}

		/*����������*/
		err=ngx_radix32tree_my_insert(radixTree, phonepack[num].phonebook, MASK_10,(ngx_uint_ptr_t)&phonepack[num],(VALUE_TYPE)(phonepack[num].value));
		if(err!=NGX_OK)
		{
			printf("insert error:%d  phonenum:%lld\n",err,phonepack[num].phonebook);
			break;
		}

	}
}
void test_radix64tree()
{
	int err=0;
		for(int num=0;num<TOTAL_TEST;num++)
		{
			if((num+1)%HUNDRED_THOUSAND==0)
			{
				printf("insert num: %d\n",num+1);
			}

			/*����������*/
			err=ngx_radix64tree_insert(radixTree_64, phonepack[num].phonebook, MASK_LONG_10,(unsigned int*)&(phonepack[num]));
			if(err!=NGX_OK)
			{
				printf("insert error:%d  phonenum:%lld\n",err,phonepack[num].phonebook);
				break;
			}

		}
}
void test_hash_map()
{
	/*hash_map����*/
	/*int errnum=1;
	ret=phonemap.insert(pair<int, int>(phonenum,phonenum));
	if(ret.second)
	{

	}
	else
	{
		printf("error!:%d  num:%d  ",ret.second,errnum);
		errnum++;
	}*/
}
int main()
{

	phonepack=(PHONE *)malloc(sizeof(PHONE )*TOTAL_TEST);
	test_init();//��ʼ����������

	BUFF_NODE *pool=NULL;
	radixTree=ngx_radix_tree_create(pool,-1);
	radixTree_64=ngx_radix_64tree_create(pool,-1);
	if(radixTree==NULL||radixTree_64==NULL)
	{
		printf("fail to creat tree!");
	}
	printf("complite!\n");

	time_start();

	test_radix32tree();
	//test_radix64tree();
	//test_hash_map();

	time_end();

	sleep(1000);//˯�ߣ����ڲ鿴����ռ���ڴ�

	return 0;

}

