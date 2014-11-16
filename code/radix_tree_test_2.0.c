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

int main()
{
	struct timeval tm;
	int iseed[10]={0,1,2,3,4,5,6,7,8,9};

	PHONESTORE *phonepack=(PHONESTORE *)malloc(sizeof(PHONESTORE )*1000000);
	/*初始化测试数组*/
	for(int j=0;j<1000000;j++)
	{
		memset(phonepack[j].phonebook, 0, sizeof(phonepack[j].phonebook));
		phonepack[j].temp=NULL;
		phonepack[j].state=0;
	}
	int phonecell;
	char phone[10];
	char add[100];

	memset(phone, 0, sizeof(phone));
	int z=1;

	hash_map<int,int> phonemap;
	/*生成1之后10位随机数字*/
	srand((unsigned)time(NULL));
	for(int jj=0;jj<1000000;jj++)
	{
		for(int i=0;i<10;i++)
			{
	        	phonecell=iseed[rand()%10];
	        	sprintf(add,"%d",phonecell);
	        	strcat(phone,add);
	        }
		memcpy(phonepack[jj].phonebook,phone,10);
	    //phonepack[jj].phonebook=atoll(phone);
	    memset(phone, 0, sizeof(phone));
	    //sprintf(phone,"%d",z);
	}
	/*输出前50个看看*/
	printf("********top 50 of the list********\n");
	for(int q=0;q<50;q++)
	{
		printf("%s\n",phonepack[q].phonebook);
	}
	printf("**********************************\n");
	int *pool=(int *)malloc(sizeof(int));
	ngx_radix_tree_t * radixTree = NULL;
	radixTree=ngx_radix_tree_create(pool,-1);
	if(radixTree==NULL)
	{
		printf("fail to creat tree!");
	}
	int secondsold,secondsnew,second;
	int minisecondsold,minisecondsnew,minisecond;
	struct timeval t;
	/*开始计时*/
	gettimeofday(&t, NULL);
	secondsold=t.tv_sec;
	minisecondsold=t.tv_usec/1000;
	printf("start time:%d:%d\n",t.tv_sec,t.tv_usec);

	int err=0;
	pair< hash_map< int,int >::iterator,bool > ret;
	int errnum=1;
	int renum=1;
	/*方便操作*/
	PHONE *phonepack_long=(PHONE *)malloc(sizeof(PHONE ));
	phonepack_long->phonebook=0;
	phonepack_long->temp=NULL;
	phonepack_long->state=0;
	for(int num=0;num<1000000;num++)
	{
		if((num+1)%100000==0)
		{printf("insert num: %d\n",num+1);}
		phonepack_long->phonebook=atoll(phonepack[num].phonebook);
		int phonenum=(phonepack_long->phonebook)%4200000000UL;

		/*基数树插入*/
		err=ngx_radix32tree_insert(radixTree, phonenum, MASK_10,(unsigned int*)&(phonepack[num]));
		if(err!=NGX_OK)
		{
			if(err==NGX_BUSY)
			{
				printf("repeat!  num:%d  phone:%s\n",renum,phonepack[num].phonebook);
				renum++;
				unsigned int* key1=ngx_radix32tree_find(radixTree, phonenum);
				PHONESTORE* key2=(PHONESTORE*)key1;
				if(key2->state>0)
				{

					if(key2->temp==NULL)
					{
						char** localtemp =(char** )malloc(sizeof(char*)*3);
						key2->temp=localtemp;
						memset(key2->temp, 0, sizeof(key2->temp));
						key2->temp[0]=phonepack[num].phonebook;
						//printf("first creat list\n");
					}
					else
					{
						int x=1;
						while(key2->temp[x]!=0)
						{
							x++;
						}
						key2->temp[x]=phonepack[num].phonebook;
						phonepack[num].state++;
						//printf("%d creat list\n",x);
					}
				}
				continue;
			}
			else
			{
				printf("insert error:%d  num:%d  phonenum:%s\n",err,errnum,phonepack[num].phonebook);
				errnum++;
				break;
			}
		}
		else
		{
			phonepack[num].state=1;
		}


		/*hash_map插入*/
		/*ret=phonemap.insert(pair<int, int>(phonenum,phonenum));
		if(ret.second)
		{

		}
		else
		{
			printf("error!:%d  num:%d  ",ret.second,errnum);
			errnum++;
		}*/
	}
	/*结束计时*/
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

	/*睡眠，用于查看程序占用内存*/
	sleep(1000);
	return 0;

}

