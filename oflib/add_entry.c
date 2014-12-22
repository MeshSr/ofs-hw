#include<stdio.h>  
#include<unistd.h>  
#include<sys/mman.h>
#include<sys/types.h>  
#include<sys/stat.h>  
#include<fcntl.h>
#include<stdlib.h>
#include <stdint.h>
#include "entry.h"
#include "reg_defines_openflow_switch.h"
#include "hw_table_define.h"

int add_entry(uint8_t tableid,int row)
{
	int i;
	unsigned int val;
	int h,t,l;
	extern nf2_of_entry_wrap_t0 flow_entry_t0;
	extern nf2_of_entry_wrap_t1 flow_entry_t1;
	extern nf2_of_mask_wrap_t0 flow_mask_t0;
	extern nf2_of_mask_wrap_t1 flow_mask_t1;
	extern nf2_of_action_wrap flow_action_t0;
	extern nf2_of_action_wrap flow_action_t1;
	if(tableid==0)
	{

		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t0.raw[i]);
			printf("write entry1 ok!!\n");
			printf("%d:%x\n",i,flow_entry_t0.raw[i]);
		}

		printf("\n"); 
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t0.raw[i]);
			printf("write mask1 ok!!\n");
			printf("%d:%x\n",i,flow_mask_t0.raw[i]);
		}

		printf("\n"); 
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), flow_action_t0.raw[i]);
			printf("write action1 ok!!\n");
			printf("%d:%x\n",i,flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * (row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * (row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * (row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, (row));
		return 0;
	}
	if(tableid==1)
	{

		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T1_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t1.raw[i]);
			printf("write entry2 ok!!\n");
			printf("%d:%x\n",i,flow_entry_t1.raw[i]);
		}

		printf("\n"); 
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T1_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t1.raw[i]);
			printf("write mask2 ok!!\n");
			printf("%d:%x\n",i,flow_mask_t1.raw[i]);
		}

		printf("\n"); 
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T1_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), flow_action_t1.raw[i]);
			printf("write action2 ok!!\n");
			printf("%d:%x\n",i,flow_action_t1.raw[i]);
		}
		// wt(OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,row);
		val=0;
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * (row)), val);
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 *(row)), val);
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 *( row)), val);
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,(row));
		return 0;
	}
	return 0;
}
int del_entry(uint8_t tableid,int row)
{
	int i;
	unsigned int val;
	int h,t,l;
	extern nf2_of_entry_wrap_t0 flow_entry_t0;
	extern nf2_of_entry_wrap_t1 flow_entry_t1;
	extern nf2_of_mask_wrap_t0 flow_mask_t0;
	extern nf2_of_mask_wrap_t1 flow_mask_t1;
	extern nf2_of_action_wrap flow_action_t0;
	extern nf2_of_action_wrap flow_action_t1;
	if(tableid==0)
	{

		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), 0);
			printf("write entry1 ok!!\n");
			printf("%d:%x\n",i,flow_entry_t0.raw[i]);
		}

		printf("\n"); 
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), 0);
			printf("write mask1 ok!!\n");
			printf("%d:%x\n",i,flow_mask_t0.raw[i]);
		}

		printf("\n"); 
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i),0);
			printf("write action1 ok!!\n");
			printf("%d:%x\n",i,flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * (row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * (row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * (row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, (row));
		return 0;
	}
	if(tableid==1)
	{

		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T1_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), 0);
			printf("write entry2 ok!!\n");
			printf("%d:%x\n",i,flow_entry_t1.raw[i]);
		}

		printf("\n"); 
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T1_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), 0);
			printf("write mask2 ok!!\n");
			printf("%d:%x\n",i,flow_mask_t1.raw[i]);
		}

		printf("\n"); 
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T1_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), 0);
			printf("write action2 ok!!\n");
			printf("%d:%x\n",i,flow_action_t1.raw[i]);
		}
		// wt(OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,row);
		val=0;
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * (row)), val);
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 *(row)), val);
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 *( row)), val);
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,(row));
		return 0;
	}
	return 0;
}

int add_entry_output(uint8_t tableid)
{
	int i,j;
	unsigned int val;
	int h,t,l;
	extern nf2_of_entry_wrap_t1 flow_entry_t1;
	extern nf2_of_mask_wrap_t1 flow_mask_t1;
	extern nf2_of_action_wrap flow_action_t1;
	if(tableid==1)
	{
		for(j=0;j<3;j++)
		{
			flow_mask_t1.raw[j]=0xffffffff;
			printf("mask:%d:%x\n",j,flow_mask_t1.raw[j]);
		}

		flow_action_t1.action.nf2_action_flag |=0x0001;
		flow_action_t1.action.forward_bitmask=0x02;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T1_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t1.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T1_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t1.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T1_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), flow_action_t1.raw[i]);
		}
		val=0;
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * (TABLE_SIZE-1)), val);
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * (TABLE_SIZE-1)), val);
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 *(TABLE_SIZE-1)), val);
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, (TABLE_SIZE-1));
		flow_action_t1.action.nf2_action_flag=0;
		flow_action_t1.action.forward_bitmask=0;
		return 0;
	}
	return 0;
}


int add_entry_table(int row)
{
	printf("add default entries to table 0\n");
	int i,j;
	unsigned int val;
	int h,t,l;
	extern nf2_of_entry_wrap_t0 flow_entry_t0;
	extern nf2_of_mask_wrap_t0 flow_mask_t0;
	extern nf2_of_action_wrap flow_action_t0;
	if(row==12)
	{
		flow_entry_t0.entry.src_port=0;
		flow_mask_t0.entry.src_port=0;
		flow_mask_t0.entry.eth_dst[0]=0xff;
		flow_mask_t0.entry.eth_dst[1]=0xff;
		flow_mask_t0.entry.eth_dst[2]=0xff;
		flow_mask_t0.entry.eth_dst[3]=0xff;
		flow_mask_t0.entry.eth_dst[4]=0xff;
		flow_mask_t0.entry.eth_dst[5]=0xff;
		flow_action_t0.action.nf2_action_flag |=0x0001;
		flow_action_t0.action.forward_bitmask=0x0002;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,(row));
		return 0;
	}
	if(row==13)
	{
		flow_entry_t0.entry.src_port=2;
		flow_mask_t0.entry.src_port=0;
		flow_mask_t0.entry.eth_dst[0]=0xff;
		flow_mask_t0.entry.eth_dst[1]=0xff;
		flow_mask_t0.entry.eth_dst[2]=0xff;
		flow_mask_t0.entry.eth_dst[3]=0xff;
		flow_mask_t0.entry.eth_dst[4]=0xff;
		flow_mask_t0.entry.eth_dst[5]=0xff;
		flow_action_t0.action.nf2_action_flag |=0x0001;
		flow_action_t0.action.forward_bitmask=0x0008;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,(row));
		return 0;
	}
	if(row==14)
	{
		flow_entry_t0.entry.src_port=4;
		flow_mask_t0.entry.src_port=0;
		flow_mask_t0.entry.eth_dst[0]=0xff;
		flow_mask_t0.entry.eth_dst[1]=0xff;
		flow_mask_t0.entry.eth_dst[2]=0xff;
		flow_mask_t0.entry.eth_dst[3]=0xff;
		flow_mask_t0.entry.eth_dst[4]=0xff;
		flow_mask_t0.entry.eth_dst[5]=0xff;
		flow_action_t0.action.nf2_action_flag |=0x0001;
		flow_action_t0.action.forward_bitmask=0x0020;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,(row));
		return 0;
	}
	if(row==7)
	{
		flow_entry_t0.entry.src_port=6;
		flow_mask_t0.entry.src_port=0;
		flow_mask_t0.entry.eth_dst[0]=0xff;
		flow_mask_t0.entry.eth_dst[1]=0xff;
		flow_mask_t0.entry.eth_dst[2]=0xff;
		flow_mask_t0.entry.eth_dst[3]=0xff;
		flow_mask_t0.entry.eth_dst[4]=0xff;
		flow_mask_t0.entry.eth_dst[5]=0xff;
		flow_action_t0.action.nf2_action_flag |=0x0001;
		flow_action_t0.action.forward_bitmask=0x0080;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,(row));
		return 0;
	}
	if(row==8)
	{
		flow_entry_t0.entry.src_port=1;
		flow_mask_t0.entry.src_port=0;
		flow_mask_t0.entry.eth_dst[0]=0xff;
		flow_mask_t0.entry.eth_dst[1]=0xff;
		flow_mask_t0.entry.eth_dst[2]=0xff;
		flow_mask_t0.entry.eth_dst[3]=0xff;
		flow_mask_t0.entry.eth_dst[4]=0xff;
		flow_mask_t0.entry.eth_dst[5]=0xff;
		flow_action_t0.action.nf2_action_flag |=0x0001;
		flow_action_t0.action.forward_bitmask=0x0001;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,(row));
		return 0;
	}
	if(row==9)
	{
		flow_entry_t0.entry.src_port=3;
		flow_mask_t0.entry.src_port=0;
		flow_mask_t0.entry.eth_dst[0]=0xff;
		flow_mask_t0.entry.eth_dst[1]=0xff;
		flow_mask_t0.entry.eth_dst[2]=0xff;
		flow_mask_t0.entry.eth_dst[3]=0xff;
		flow_mask_t0.entry.eth_dst[4]=0xff;
		flow_mask_t0.entry.eth_dst[5]=0xff;
		flow_action_t0.action.nf2_action_flag |=0x0001;
		flow_action_t0.action.forward_bitmask=0x0004;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,(row));
		return 0;
	}
	if(row==10)
	{
		flow_entry_t0.entry.src_port=5;
		flow_mask_t0.entry.src_port=0;
		flow_mask_t0.entry.eth_dst[0]=0xff;
		flow_mask_t0.entry.eth_dst[1]=0xff;
		flow_mask_t0.entry.eth_dst[2]=0xff;
		flow_mask_t0.entry.eth_dst[3]=0xff;
		flow_mask_t0.entry.eth_dst[4]=0xff;
		flow_mask_t0.entry.eth_dst[5]=0xff;
		flow_action_t0.action.nf2_action_flag |=0x0001;
		flow_action_t0.action.forward_bitmask=0x0010;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,(row));
		return 0;
	}
	if(row==11)
	{
		flow_entry_t0.entry.src_port=7;
		flow_mask_t0.entry.src_port=0;
		flow_mask_t0.entry.eth_dst[0]=0xff;
		flow_mask_t0.entry.eth_dst[1]=0xff;
		flow_mask_t0.entry.eth_dst[2]=0xff;
		flow_mask_t0.entry.eth_dst[3]=0xff;
		flow_mask_t0.entry.eth_dst[4]=0xff;
		flow_mask_t0.entry.eth_dst[5]=0xff;
		flow_action_t0.action.nf2_action_flag |=0x0001;
		flow_action_t0.action.forward_bitmask=0x0040;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 *(row)), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,(row));
		return 0;
	}
	return 0;
}


int hw_entry_hard_timeout(uint8_t tableid,int row)
{
	printf("The table:%d 's %d th entry is hard time out!\n",tableid,row);
	int i;
	unsigned int val;
	int h,t,l;
	if(tableid==0)
	{

		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i),0);
		}

		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i),0);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i),0);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, row);
		return 0;
	}
	if(tableid==1)
	{

		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; ++i) {

			h=wt(T1_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i),0);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; ++i) {
			t=wt(T1_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i),0);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; ++i) {
			l=wt(T1_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i),0);
		}
		// wt(OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,row);
		val=0;
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T1_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, row);
		return 0;
	}
	return 0;
}
unsigned hw_entry_idle_timeout_lookup(uint8_t tableid,int row)
{
	printf("look up entry if it is idle time out!\n");
	printf("The table:%d 's %d th entry is idle time out?",tableid,row);
	int i;
	unsigned val = 0;
	int h,t,l;
	if(tableid==0)
	{
		switch(row)
		{
			case 0: 
				rdReg(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG, &val);
				break;
			case 1:
				rdReg(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_1_REG, &val);
				break;
			case 2:
				rdReg(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_2_REG, &val);
				break;
			case 3:
				rdReg(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_3_REG, &val);
				break;
			case 4:
				rdReg(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_4_REG, &val);
				break;
			case 5:
				rdReg(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_5_REG, &val);
				break;
			case 6:
				rdReg(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_6_REG, &val);
				break;
			case 7:
				rdReg(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_7_REG, &val);
				break;

		}

		return val;
	}
	if(tableid==1)
	{


		switch(row)
		{
			case 0: 
				rdReg(T1_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG, &val);
				break;
			case 1:
				rdReg(T1_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_1_REG, &val);
				break;
			case 2:
				rdReg(T1_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_2_REG, &val);
				break;
			case 3:
				rdReg(T1_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_3_REG, &val);
				break;
			case 4:
				rdReg(T1_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_4_REG, &val);
				break;
			case 5:
				rdReg(T1_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_5_REG, &val);
				break;
			case 6:
				rdReg(T1_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_6_REG, &val);
				break;
			case 7:
				rdReg(T1_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_7_REG, &val);
				break;

		}

		return val;
	}
	return 0;
}

unsigned int meter_addr_lookup(int meter_id){
	unsigned int meter_addr;
	switch(meter_id){
		case 0: 
			meter_addr = RATE_LIMIT_0_CTRL_REG;
			break;
		case 1: 
			meter_addr = RATE_LIMIT_1_CTRL_REG;
			break;
		case 2: 
			meter_addr = RATE_LIMIT_2_CTRL_REG;
			break;
		case 3: 
			meter_addr = RATE_LIMIT_3_CTRL_REG;
			break; 
		default: meter_addr = RATE_LIMIT_1_CTRL_REG;
	}
	return meter_addr;
}
int add_meter(int meter_id,unsigned int kbps){
	clear_meter(meter_id);
	unsigned int inc, interval;
	unsigned int meter_addr_base;
	unsigned int a, b, temp;
	printf("add_meter begin!\n");
	printf("kbps=%d\n",kbps);
	a = kbps;
	b = 8000*75;
	if(a < b){
		temp = a;
		a = b;
		b = temp;
	}
	while(b!=0){
		temp = a % b;
		a = b;
		b = temp;
	}
	inc = (unsigned int)(kbps / a);
	interval = (unsigned int)(8000 * 75 / a);
	meter_addr_base = meter_addr_lookup(meter_id);
	printf("meter_id=%d\n",meter_id);
	printf("interval=%d\n",interval);
	printf("inc=%d\n",inc);
	wt(meter_addr_base + 4, interval);
	wt(meter_addr_base + 8, inc);
	wt(meter_addr_base, 1); //enable
	return 0;
}
clear_meter(int meter_id){
	unsigned int meter_addr_base;

	meter_addr_base = meter_addr_lookup(meter_id);
	wt(meter_addr_base + 4, 1);
	wt(meter_addr_base + 8, 1);
	wt(meter_addr_base, 0); //disable
}
int rdReg(unsigned reg, unsigned *val)
{

	int fd;
	int i, linenum, count=0;
	unsigned char * map_base;
	unsigned long PHYADDR, length;
   unsigned char val_str[4];
	/* get pagesize on the specific device */
	int  pagesize = getpagesize();

	/* turn a strig into a unsigned long integer */
	PHYADDR = reg;
	//printf("PHYADDR = 0x%08x\n", PHYADDR);
	length = 1;
	//printf("length = %d words\n", length);

	/* open a descriptor to /dev/mem */
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(fd == -1)  
	{  
		printf("open /dev/mem failed\n");
		return -1;  
	}

	/* mmap() maps the physical address into virtual address, including IO and hardware registers
	* the start address is NULL, so kernel will locate it at any possible place.
	* with read and write permission
	* the offset is aligned with a memory page
	*/
	map_base = mmap(NULL, pagesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PHYADDR & 0xfffff000);
	if(map_base == MAP_FAILED) {
		perror("mmap");
	};/*
	else
		printf("map_base = 0x%08X\n", map_base);
	printf("\n");*/

	/* get the within-page offset */
	int pgoffset = PHYADDR & 0x00000fff;
	
	/* print 4 registers in a line */
	while(length>4) {
		linenum=4;
		length-=4;
		printf("%08X: ", (PHYADDR + count));
		for(i=0;i<4*linenum;i=i+4) {
			printf("%02X%02X%02X%02X    ",  (unsigned int)map_base[pgoffset + count + i+3],
							(unsigned int)map_base[pgoffset + count + i+2],
							(unsigned int)map_base[pgoffset + count + i+1],
							(unsigned int)map_base[pgoffset + count + i]
							);
		}
		printf("\n");
		count+=16;
	}
	//printf("%08X: ", (PHYADDR + count));
	for(i=0;i<4*length;i=i+4) {
		sprintf(val_str, "%02X%02X%02X%02X    ",  (unsigned int)map_base[pgoffset + count + i+3],
						(unsigned int)map_base[pgoffset + count + i+2],
						(unsigned int)map_base[pgoffset + count + i+1],
						(unsigned int)map_base[pgoffset + count + i]
						);
      *val = strtoul(val_str,NULL,16);
      //printf("%08X: ", *val);
	}
	//printf("\n");


	/* close file descriptor */
	close(fd);
	/* release map */
	munmap(map_base, pagesize);
	return 0;
}
int wt(unsigned reg,unsigned val)
{
int argc=3;
	if(argc != 3) {
		printf("insufficient arguments\n");
		return -1;
	}
	int fd;
	unsigned char * map_base;
	unsigned long PHYADDR,newval;
	int  pagesize = getpagesize();
	//PHYADDR = strtoul(reg,NULL,16);
        PHYADDR =reg;
	printf("PHYADDR = 0x%08X\n",PHYADDR);
	//newval = strtoul(val,NULL,16);
        newval =val;
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(fd == -1)  
	{  
		printf("open /dev/mem failed\n");
		return -1;  
	}
	map_base = mmap(NULL, pagesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PHYADDR & 0xfffff000);
	if(map_base == MAP_FAILED) {
		perror("mmap");
	}
	else
		printf("mmap = 0x%08X\n", map_base);
	int pgoffset = PHYADDR & 0x00000fff;
	unsigned int c0,c1,c2,c3;
	/* seperate 4 bytes of a 32-bit register value */
	c0 = newval & 0x000000ff;
	c1 = (newval & 0x0000ff00) / 0x100;
	c2 = (newval & 0x00ff0000) / 0x10000;
	c3 = (newval & 0xff000000) / 0x1000000;
	/* write into registers */
	map_base[pgoffset] = (unsigned char)c0;
	map_base[pgoffset + 1] = (unsigned char)c1;
	map_base[pgoffset + 2] = (unsigned char)c2;
	map_base[pgoffset + 3] = (unsigned char)c3;
	printf("\nValue at 0x%08X: 0x%02X%02X%02X%02X\n", PHYADDR,
					(unsigned int)map_base[pgoffset + 3],
					(unsigned int)map_base[pgoffset + 2],
					(unsigned int)map_base[pgoffset + 1],
					(unsigned int)map_base[pgoffset]
					);
	
	close(fd);
	munmap(map_base, pagesize);
	return 0;
}
