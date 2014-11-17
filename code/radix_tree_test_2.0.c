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

PHONE *phonepack=NULL;
int secondsold, secondsnew, second;
int minisecondsold,minisecondsnew,minisecond;
struct timeval t;
ngx_radix_tree_t * radixTree = NULL;

void test_init()
{
	/*����1֮��10λ�������*/

	int iseed[10]={0,1,2,3,4,5,6,7,8,9};
	for(int j=0;j<MILLION;j++)
	{
		phonepack[j].phonebook=0;
		phonepack[j].list=NULL;
	}
	int phonecell;
	char phone[10];
	char add[10];

	memset(phone, 0, sizeof(phone));
	//int z=1;
	srand((unsigned)time(NULL));
	for(int jj=0;jj<MILLION;jj++)
	{
		for(int i=0;i<10;i++)
		{
			phonecell=iseed[rand()%10];
			sprintf(add,"%d",phonecell);
			strcat(phone,add);
		}
		phonepack[jj].phonebook=atoll(phone);
		memset(phone, 0, sizeof(phone));
		//sprintf(phone,"%d",z);
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

	if(minisecond>0)
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
	for(int num=0;num<MILLION;num++)
	{
		if((num+1)%HUNDRED_THOUSAND==0)
		{
			printf("insert num: %d\n",num+1);
		}

		int phonenum=(phonepack[num].phonebook)%INT_MAX;

		/*����������*/
		err=ngx_radix32tree_insert(radixTree, phonenum, MASK_10,(unsigned int*)&(phonepack[num]));
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

	phonepack=(PHONE *)malloc(sizeof(PHONE )*MILLION);
	test_init();//��ʼ����������
	hash_map<int,int> phonemap;
	pair< hash_map< int,int >::iterator,bool > ret;

	BUFF_NODE *pool=NULL;
	radixTree=ngx_radix_tree_create(pool,-1);
	if(radixTree==NULL)
	{
		printf("fail to creat tree!");
	}

	time_start();

	test_radix32tree();
	//test_hash_map();

	time_end();

	sleep(1000);//˯�ߣ����ڲ鿴����ռ���ڴ�

	return 0;

}

