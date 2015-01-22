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

int move_entry(uint8_t tableid, int row, nf2_of_entry_wrap_t0 entry, nf2_of_mask_wrap_t0 mask, nf2_of_action_wrap action)
{
	int i;
	unsigned int val;
	int h, t, l;
	if (tableid == 0) {
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; i++) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), entry.raw[i]);			
		}		
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; i++) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), mask.raw[i]);			
		}		
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; i++) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), action.raw[i]);			
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, (row));
		return 0;
	}
	return 0;
}

int add_entry(uint8_t tableid, int row)
{
	int i;
	unsigned int val;
	extern nf2_of_entry_wrap_t0 flow_entry_t0;
	extern nf2_of_mask_wrap_t0 flow_mask_t0;	
	extern nf2_of_action_wrap flow_action_t0;	
	if (tableid == 0) {

		for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; i++) {

			wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t0.raw[i]);			
		}
		for (i = 0; i < NF2_OF_MASK_WORD_LEN; i++) {
			wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t0.raw[i]);
		}		
		for (i = 0; i < NF2_OF_ACTION_WORD_LEN; i++) {
			wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG + (4 * i), flow_action_t0.raw[i]);			
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, row);
		return 0;
	}
	return 0;
}

int del_entry(uint8_t tableid,int row)
{
	int i;
	unsigned int val;	
	if(tableid == 0) {

		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; i++) {

			wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG + (4 * i), 0);	
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; i++) {
			wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG + (4 * i), 0);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; i++) {
			wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG + (4 * i),0);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, row);
		return 0;
	}
	return 0;
}

int add_entry_table(int row)
{
	printf("add default entries to table 0,prio:%d\n", row);
	int i, j;
	unsigned int val;
	int h,t,l;
	extern nf2_of_entry_wrap_t0 flow_entry_t0;
	extern nf2_of_mask_wrap_t0 flow_mask_t0;
	extern nf2_of_action_wrap flow_action_t0;
	if (row == 0) {
		flow_entry_t0.entry.src_port = 0;
		flow_mask_t0.entry.src_port = 0;
		flow_mask_t0.entry.eth_dst[0] = 0xff;
		flow_mask_t0.entry.eth_dst[1] = 0xff;
		flow_mask_t0.entry.eth_dst[2] = 0xff;
		flow_mask_t0.entry.eth_dst[3] = 0xff;
		flow_mask_t0.entry.eth_dst[4] = 0xff;
		flow_mask_t0.entry.eth_dst[5] = 0xff;
		flow_action_t0.action.nf2_action_flag |= 0x0001;
		flow_action_t0.action.forward_bitmask = 0x0002;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; i++) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; i++) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; i++) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 *row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 *row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 *row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG,row);
		return 0;
	}
	if(row == 1) {
		flow_entry_t0.entry.src_port=2;
		flow_mask_t0.entry.src_port=0;
		flow_mask_t0.entry.eth_dst[0]=0xff;
		flow_mask_t0.entry.eth_dst[1]=0xff;
		flow_mask_t0.entry.eth_dst[2]=0xff;
		flow_mask_t0.entry.eth_dst[3]=0xff;
		flow_mask_t0.entry.eth_dst[4]=0xff;
		flow_mask_t0.entry.eth_dst[5]=0xff;
		flow_action_t0.action.nf2_action_flag |= 0x0001;
		flow_action_t0.action.forward_bitmask = 0x0008;
		for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; i++) {

			h = wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG + (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i < NF2_OF_MASK_WORD_LEN; i++) {
			t = wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG + (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i < NF2_OF_ACTION_WORD_LEN; i++) {
			l = wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG + (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, row);
		return 0;
	}
	if(row == 2) {
		flow_entry_t0.entry.src_port = 4;
		flow_mask_t0.entry.src_port = 0;
		flow_mask_t0.entry.eth_dst[0] = 0xff;
		flow_mask_t0.entry.eth_dst[1] = 0xff;
		flow_mask_t0.entry.eth_dst[2] = 0xff;
		flow_mask_t0.entry.eth_dst[3] = 0xff;
		flow_mask_t0.entry.eth_dst[4] = 0xff;
		flow_mask_t0.entry.eth_dst[5] = 0xff;
		flow_action_t0.action.nf2_action_flag |= 0x0001;
		flow_action_t0.action.forward_bitmask = 0x0020;
		for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; i++) {

			h = wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG + (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; i++) {
			t = wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG + (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; i++) {
			l = wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG + (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, row);
		return 0;
	}
	if(row ==3) {
		flow_entry_t0.entry.src_port = 6;
		flow_mask_t0.entry.src_port = 0;
		flow_mask_t0.entry.eth_dst[0] = 0xff;
		flow_mask_t0.entry.eth_dst[1] = 0xff;
		flow_mask_t0.entry.eth_dst[2] = 0xff;
		flow_mask_t0.entry.eth_dst[3] = 0xff;
		flow_mask_t0.entry.eth_dst[4] = 0xff;
		flow_mask_t0.entry.eth_dst[5] = 0xff;
		flow_action_t0.action.nf2_action_flag |= 0x0001;
		flow_action_t0.action.forward_bitmask = 0x0080;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; i++) {

			h = wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG + (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; i++) {
			t = wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG + (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; i++) {
			l = wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG + (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, row);
		return 0;
	}
	if(row == 4) {
		flow_entry_t0.entry.src_port=1;
		flow_mask_t0.entry.src_port=0;
		flow_mask_t0.entry.eth_dst[0] = 0xff;
		flow_mask_t0.entry.eth_dst[1] = 0xff;
		flow_mask_t0.entry.eth_dst[2] = 0xff;
		flow_mask_t0.entry.eth_dst[3] = 0xff;
		flow_mask_t0.entry.eth_dst[4] = 0xff;
		flow_mask_t0.entry.eth_dst[5] = 0xff;
		flow_action_t0.action.nf2_action_flag |= 0x0001;
		flow_action_t0.action.forward_bitmask = 0x0001;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; i++) {

			h = wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG + (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; i++) {
			t = wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG + (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; i++) {
			l = wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG + (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, row);
		return 0;
	}
	if(row == 5) {
		flow_entry_t0.entry.src_port = 3;
		flow_mask_t0.entry.src_port = 0;
		flow_mask_t0.entry.eth_dst[0] = 0xff;
		flow_mask_t0.entry.eth_dst[1] = 0xff;
		flow_mask_t0.entry.eth_dst[2] = 0xff;
		flow_mask_t0.entry.eth_dst[3] = 0xff;
		flow_mask_t0.entry.eth_dst[4] = 0xff;
		flow_mask_t0.entry.eth_dst[5] = 0xff;
		flow_action_t0.action.nf2_action_flag |= 0x0001;
		flow_action_t0.action.forward_bitmask = 0x0004;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; i++) {

			h = wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG + (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; i++) {
			t = wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG + (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; i++) {
			l = wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG + (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, row);
		return 0;
	}
	if(row == 6) {
		flow_entry_t0.entry.src_port = 5;
		flow_mask_t0.entry.src_port = 0;
		flow_mask_t0.entry.eth_dst[0] = 0xff;
		flow_mask_t0.entry.eth_dst[1] = 0xff;
		flow_mask_t0.entry.eth_dst[2] = 0xff;
		flow_mask_t0.entry.eth_dst[3] = 0xff;
		flow_mask_t0.entry.eth_dst[4] = 0xff;
		flow_mask_t0.entry.eth_dst[5] = 0xff;
		flow_action_t0.action.nf2_action_flag |= 0x0001;
		flow_action_t0.action.forward_bitmask = 0x0010;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; i++) {

			h = wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG + (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; i++) {
			t = wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG + (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; i++) {
			l = wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG + (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, row);
		return 0;
	}
	if(row == 7) {
		flow_entry_t0.entry.src_port = 7;
		flow_mask_t0.entry.src_port = 0;
		flow_mask_t0.entry.eth_dst[0] = 0xff;
		flow_mask_t0.entry.eth_dst[1] = 0xff;
		flow_mask_t0.entry.eth_dst[2] = 0xff;
		flow_mask_t0.entry.eth_dst[3] = 0xff;
		flow_mask_t0.entry.eth_dst[4] = 0xff;
		flow_mask_t0.entry.eth_dst[5] = 0xff;
		flow_action_t0.action.nf2_action_flag |= 0x0001;
		flow_action_t0.action.forward_bitmask = 0x0040;
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; i++) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG + (4 * i), flow_entry_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; i++) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG + (4 * i), flow_mask_t0.raw[i]);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; i++) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG + (4 * i), flow_action_t0.raw[i]);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, row);
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
		for (i = 0; i <NF2_OF_ENTRY_WORD_LEN; i++) {

			h=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_0_REG+ (4 * i),0);
		}
		for (i = 0; i <NF2_OF_MASK_WORD_LEN; i++) {
			t=wt(T0_OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_0_REG+ (4 * i),0);
		}
		for (i = 0; i <NF2_OF_ACTION_WORD_LEN; i++) {
			l=wt(T0_OPENFLOW_WILDCARD_LOOKUP_ACTION_0_REG+ (4 * i),0);
		}
		val = 0;
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_LAST_SEEN_TS_0_REG + (4 * row), val);
		wt(T0_OPENFLOW_WILDCARD_LOOKUP_WRITE_ADDR_REG, row);
		return 0;
	}
	
	return 0;
}

unsigned hw_entry_idle_timeout_lookup(uint8_t tableid,int row)
{
	printf("look up entry if it is idle time out!\n");
	printf("The table:%d 's %d th entry is idle time out?", tableid, row);
	int i;
	unsigned val = 0;
	int h, t, l;
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

int add_meter(int meter_id, unsigned int kbps){
	clear_meter(meter_id);
	unsigned int inc, interval;
	unsigned int meter_addr_base;
	unsigned int a, b, temp;
	a = kbps;
	b = 8000 * 125;
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
	interval = (unsigned int)(8000 * 125 / a);
	meter_addr_base = meter_addr_lookup(meter_id);
	wt(meter_addr_base + 4, interval);
	wt(meter_addr_base + 8, inc);
	wt(meter_addr_base, 1); //enable
	printf("add meter:%d ok! kbps:%d interval:%d inc:%d\n", meter_id, kbps, interval,inc);
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
	int i, linenum, count = 0;
	unsigned char * map_base;
	unsigned long PHYADDR, length;
	unsigned char val_str[4];
	/* get pagesize on the specific device */
	int pagesize = getpagesize();
	/* turn a strig into a unsigned long integer */
	PHYADDR = reg;	
	length = 1;	
	/* open a descriptor to /dev/mem */
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(fd == -1) {  
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
	}

	/* get the within-page offset */
	int pgoffset = PHYADDR & 0x00000fff;
	
	val_str[0] = map_base[pgoffset];        
	val_str[1] = map_base[pgoffset + 1];        
	val_str[2] = map_base[pgoffset + 2];        
	val_str[3] = map_base[pgoffset + 3];        
	*val = val_str[0]|(val_str[1] << 8)|(val_str[2] << 16)|(val_str[3] << 24);        

	/* close file descriptor */
	close(fd);
	/* release map */	
	munmap(map_base, pagesize);
        
	return 0;
}
int wt(unsigned reg, unsigned val)
{
	int argc = 3;
	if(argc != 3) {
		printf("insufficient arguments\n");
		return -1;
	}
	int fd;
	unsigned char * map_base;
	unsigned long PHYADDR,newval;
	int pagesize = getpagesize();	
	PHYADDR = reg;
	newval = val;
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(fd == -1) {  
		printf("open /dev/mem failed\n");
		return -1;  
	}
	map_base = mmap(NULL, pagesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PHYADDR & 0xfffff000);
	if(map_base == MAP_FAILED) {
		perror("mmap");
	}

	int pgoffset = PHYADDR & 0x00000fff;
	unsigned int c0, c1, c2, c3;

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

	close(fd);
	munmap(map_base, pagesize);

	return 0;
}



int  hw_table_list_init(hw_table_list *plist)  
{  
    if (plist != NULL) {           
		plist->head = NULL;  
		plist->tail = NULL;  
		plist->size = 0;                  
    }  
    return 0;  
}  

int hw_table_list_tail_insert(hw_table_list *plist, PNode s)  
{  
	if (plist->head == NULL) {
		plist->head = plist->tail = s;
		plist->tail->next = NULL;       
		plist->size = 1;
    } else {
		s->next = NULL;
        s->prev = plist->tail;
        if(plist->tail)
           plist->tail->next = s;
        else
           plist->head = s;
        plist->tail = s;
		plist->size++;
    }   
    return 0;  
}  
int hw_table_list_insert(hw_table_list *plist, PNode s)  
{  
    int num = 0;
    PNode cur = plist->head;

	if (plist->head == NULL) {
		plist->head = plist->tail = s;
		plist->tail->next = NULL;       
		plist->size = 1;
		num++;
    } else {
		if (plist->tail->hw_entry.prio >= s->hw_entry.prio) {
			hw_table_list_tail_insert(plist, s);
			num = plist->size;
		} else {
		   	while (cur) {
				num++;
				if (cur->hw_entry.prio < s->hw_entry.prio) {
					if (cur == plist->head) {
						plist->head->prev = s;
						s->next = plist->head;
						plist->head = s;
						s->prev = NULL;	
					} else {	
						s->prev = cur->prev;
						cur->prev->next = s;
						s->next = cur;
					}
					plist->size++;
					break;
				}
				cur = cur->next;
			}
		}
    }
    
    return num;  
} 
 
int hw_table_list_compare(hw_table_list *plist, PNode s)
{
	PNode p;
	p = plist->head;        
	int i = 0;
	if ((plist->size == 1)
	    && (p->hw_entry.prio == s->hw_entry.prio)
	    && (p->hw_entry.flow_entry.raw[0] == s->hw_entry.flow_entry.raw[0])
	    && (p->hw_entry.flow_entry.raw[1] == s->hw_entry.flow_entry.raw[1])) {
		i++;
		return i;
	} else { 
		while (p->next != NULL) {
			i++;
			if ((p->hw_entry.prio == s->hw_entry.prio)
				&& (p->hw_entry.flow_entry.raw[0] == s->hw_entry.flow_entry.raw[0])
				&& (p->hw_entry.flow_entry.raw[1] == s->hw_entry.flow_entry.raw[1])) {
				return i;
			}			
			p = p->next;
		}
		if (p == plist->tail) {
			i++;
			if ((p->hw_entry.prio == s->hw_entry.prio)
				&& (p->hw_entry.flow_entry.raw[0] == s->hw_entry.flow_entry.raw[0])
				&& (p->hw_entry.flow_entry.raw[1] == s->hw_entry.flow_entry.raw[1])) {
				return i;
			}		
		}
		return 0;
	}	
}

int hw_table_list_replace(hw_table_list *plist,PNode s, int num)
{
	int i;
	PNode p = plist->head;
	for(i = 1; i < num; i++) {		
		p = p->next;
	}
	memcpy(&(p->hw_entry), &(s->hw_entry), sizeof(hw_table_t0));

	return 0;
}

int hw_table_list_clear(hw_table_list *plist)
{
	plist->head = NULL;  
	plist->tail = NULL;  
	plist->size = 0;  

	return 0;
}
