/**/
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <sys/mman.h>
#include <time.h>  

#include <linux/types.h>

#define EOS_DMA_MAX_LEN 0xa000

/*************************************************************************
* 函数名  ： diff_data
* 负责人  ：罗雄豹
* 创建日期：20160621
* 函数功能：比较数据函数
* 输入参数：len，数据长度，rd_buf， wr_buf比较的数组       
* 输出参数：
* 返回值：	OK: 成功      
* 调用关系：                            
*************************************************************************/
int diff_data(int len, int *rd_buf, int *wr_buf)
{
     int i;
	 int diff = 0;
	 for (i = 0; i < len; i++)
	 {
	     if(rd_buf[i] != wr_buf[i])
	     {
	         diff = 1;
			 return diff;
	     }
	 }
	 return diff;
}


/*************************************************************************
* 函数名  ： str_to_hex
* 负责人  ：罗雄豹
* 创建日期：20150520
* 函数功能：字符串转换为16进制数据
* 输入参数：
       				
       			str-字符串buffer
* 输出参数：无
* 返回值：	无
       			介绍函数功能
*************************************************************************/
 unsigned long long str_to_hex(char *str)
{
    char buf[64];
	char off[64];
	char *pstr;
    unsigned long long  sum = 0;
    int len;
	int i;
    strcpy(buf, str);
    if (buf[1] == 'x' || buf[1] == 'X')
	{
        pstr = &buf[2];
        strcpy(off, pstr);
        strcpy(buf, off);
    }
    len = strlen(buf);
    for (i = 0; i<len; i++)
	{
        if (buf[i] >= '0' && buf[i] <= '9')
    	{
            buf[i] -= '0';
    	}
        else if (buf[i] >= 'A' && buf[i] <= 'F')
    	{
            buf[i] = buf[i] - '0' - 7;
    	}
        else
    	{
            buf[i] = buf[i] - '0' - 39;
    	}
    }
    for (i = 0; i < len; i++)
	{
        sum = sum * 16 + buf[i];
    }
    return sum;
}


/*************************************************************************
* 函数名  ： main
* 负责人  ：罗雄豹
* 创建日期：20160621
* 函数功能：main函数
* 输入参数：
* 输出参数：
* 返回值：	OK: 成功      
* 调用关系：                            
*************************************************************************/
int main(int argc, char *argv[])
{
	int fd = 0;
	int i = 0;
	unsigned long addr = 0;
//	u32_dma_addr  addr;
	int len = 16;
//	unsigned long long data[4][2] = {{0x4044a0b000,252},{0x4044a06000,252},{0x4044b03000,252},{0x4044b84000,4000}};
//	unsigned long long data[4][2] = {{0x80000000,252},{0x80100000,252},{0x81000000,252},{0x82000000,4000}};
	unsigned long long data[4][2] = {{0x73000000,16},{0x73000020,16}};


	struct timeval tpstart;
	struct timeval tpend;    
	float timeuse;
	float timeuse_min = 0.01;
	float timeuse_max = 0.000001;
	float timeuse_mid  = 0;
	int err_times = 0;
	int times;
	
	int ret;
	int rev_buffer[EOS_DMA_MAX_LEN];
	int wr_buffer[EOS_DMA_MAX_LEN];

	int channel = 0;

printf("%s %s\n", __DATE__,__TIME__);
	if (argc <= 1)
	{
		printf("arg:addr1  len addr2 channel times\n");
		return 0;
	}

	
	fd = eos_open_dma_channel("/dev/dma_control0_channel0");
	if(fd == -1)
	{
		return -1;
	}
	if (argc >= 2)
	{
#if 1
	    data[0][0] = (unsigned long long)str_to_hex(argv[1]);
	    //data[2][0] = (unsigned long)str_to_hex(argv[1]);

#endif		//data[0][0] *= 4;
		printf("read  1 offset  0x%llx ", str_to_hex(argv[1]));
	}
	if (argc >= 3)
	{
#if 1
	    len = (unsigned int)strtoul(argv[2], NULL, 0);//atoi(argv[2]);
	    data[0][1] =  1 * (unsigned int)strtoul(argv[2], NULL, 0);//atoi(argv[2]);
	    //data[2][1] =  1 * (unsigned int)strtoul(argv[2], NULL, 0);//atoi(argv[2]);
	    data[1][1]  = data[0][1];
		//data[3][1]  = data[0][1];

		len = len;
#else	



		len = 0;
		for (i = 0;i < 4; i++)
		{	
			len += data[i][1];
		}
		if (len > EOS_DMA_MAX_LEN)
		{
			len = EOS_DMA_MAX_LEN;
		}	
		else
		{	
			len /=1;
		}
#endif
 		printf("read len %d(0x%x) Bytes\n", len, data[0][1]);
	}	
	if (argc > 4)
	{
	  //data[0][1] = data[0][1] ;
	 //data[1][0] = 4 * (unsigned int)strtoul(argv[4], NULL, 0);
	 //data[1][1]  = data[0][1];
#if 1
	   data[1][0] = (unsigned long long)str_to_hex(argv[3]);
	   //data[3][0] = (unsigned long)str_to_hex(argv[3]);
#else
            data[0][0] = (unsigned long long)str_to_hex(argv[1]);
            data[1][0] = (unsigned long long)str_to_hex(argv[1]);
	    data[2][0] = (unsigned long long)str_to_hex(argv[2]);
            data[3][0] = (unsigned long long)str_to_hex(argv[3]);
		printf("addr1 0x%llx,addr2 0x%llx,addr3 0x%llx,addr4 0x%llx\n", data[0][0],data[1][0],data[2][0],data[3][0]);
		printf("len1  0x%llx,len2  0x%llx,len3  0x%llx,len4  0x%llx\n", data[0][1],data[1][1],data[2][1],data[3][1]);
#endif		
		//data[0][0] *= 4;
		printf("read 2 offset  0x%llx\n", str_to_hex(argv[3]));	 
	}
	else
	{
	 //data[0][1] = data[0][1] ;
	 //data[1][0] = 4 * 0x14000; 
	 //data[1][1]  = data[0][1];	
	}

	if (argc > 5)
	{
	       channel = (unsigned long)str_to_hex(argv[4]);
		printf("channel= %d\n", channel);
	}
        printf("eos_init_dma_arg\n");
	eos_init_dma_arg_channel(fd, data, 1, channel);
        printf("eos_init_dma_arg\n");

	eos_get_dma_buffer_addr_channel(fd, &addr, channel);
        printf("eos_get_dma_buffer_addr\n");
        //eos_print_dma_data(fd, &addr);
        printf("addr is 0x%lx\n",addr);
	

		eos_start_dma_transfer_channel(fd, channel);
        printf("eos_start_dma_transfer\n");	
			//eos_start_dma_transfer(fd);
        //printf("eos_start_dma_transfer==2\n");	
	//memcpy(wr_buffer, (unsigned int *)addr, 4);
        printf("2\n");
        ret = eos_get_dma_status_channel(fd, channel);
         printf("3\n");
while ((ret == 1))
{
ret = eos_get_dma_status_channel(fd, channel);		    			
}		 
	if(ret == 1)
	{
		printf("this is busy = %d\n", ret);
		sleep(1);
	}
	printf("DMA get data  0:");
	memcpy(wr_buffer, (unsigned int  *)(addr), sizeof(len));
	printf("DMA get data:");
	if ((len > 0) && (len < EOS_DMA_MAX_LEN))
    {
        for(i = 0;i < len/4; i++)
        {
       	    if (i % 4 == 0)
       	    {
    			 printf("\n0x%04x :", 16 * (i / 4));
       	    }
        		
    			printf(" %08x",*((unsigned int *)(addr) + i));
    
        		
        }
    }
	printf("\n");


#if 0
int add_times = 0;


if (argc > 4)
{
	times = (unsigned int)strtoul(argv[5], NULL, 0);
	printf("test times %d", times);
	for (i = 0; i < times; i++)
        {
//     	gettimeofday(&tpstart, NULL);	
        	eos_init_dma_arg_channel(fd,data, 2, channel);
        	eos_start_dma_transfer_channel(fd, channel);//Read_Fpga_Dma(fd,rd_buffer, offset, len);
       	//eos_start_dma_transfer(fd);
        	//eos_get_dma_buffer_addr(fd,&addr);
	       ret = eos_get_dma_status_channel(fd, channel);
		add_times = 0;
#if 1
		//while ((ret == 1) && 1 /*(100 > add_times)*/)
		while ((ret == 1) &&  (100 > add_times))
		{
		    ret = eos_get_dma_status_channel(fd, channel);
		    usleep(10);
			add_times++;
		}
		if (100 < add_times)
		printf("=%d=[%d]\n", add_times,  i);	
        	gettimeofday(&tpend, NULL); 	
        	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec;		
        	timeuse = timeuse / 1000000;   
		memset(rev_buffer, 0, sizeof(len));
		memcpy(rev_buffer, (unsigned int *)(addr), sizeof(len));
        	ret = diff_data(len, wr_buffer, rev_buffer);
		if (timeuse_min > timeuse)
		{
		    timeuse_min = timeuse;
		}
		if (timeuse_max < timeuse)
		{
		    timeuse_max = timeuse;
		}
		timeuse_mid +=  timeuse;
		if (ret == 1)
              err_times++;
#else	
		memset(rev_buffer, 0, sizeof(len));
		memcpy(rev_buffer, (unsigned int *)(addr), sizeof(len));
		ret = diff_data(len/4, wr_buffer, rev_buffer);
		if (ret == 1)
              err_times++;

		usleep(500);
#endif   	
        }
	printf("\n");
	 printf("err times %d  min_time =%f max_time =%f mid_time =%f \n",
	 	err_times, timeuse_min, timeuse_max,timeuse_mid / times);
	
}
#endif
close(fd);
	
}
