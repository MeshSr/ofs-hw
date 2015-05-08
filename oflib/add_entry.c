#include <stdio.h>  
#include <unistd.h>  
#include <sys/mman.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include "reg_defines_openflow_switch.h"
#include "hw_table_define.h"
int clear_entry_counter(uint8_t tableid, int row)
{ 
	wt(OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_REG | (tableid << 0x14) | (row << 0x08), 0);
	wt(OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_REG | (tableid << 0x14) | (row << 0x08), 0);
	printf("reset the pkt count and byte count of the flow entry! table_id:%d prio:%d\n", tableid, row);

	return 0;
}
int add_entry(uint8_t tableid, int row, PNode hw_entry)
{
	int i;
	for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; ++i) {
		wt((OPENFLOW_WILDCARD_LOOKUP_CMP_DATA_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_match.raw[i] );		
	}
	for (i = 0; i < NF2_OF_MASK_WORD_LEN; ++i) {
		wt((OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_mask.raw[i]);		
	}
	for (i = 0; i < NF2_OF_ACTION_WORD_LEN; ++i) {
		wt((OPENFLOW_WILDCARD_LOOKUP_CMP_ACTION_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_action.raw[i]);		
	}
	return 0;
}
int mod_entry(uint8_t tableid, int row, PNode hw_entry)
{
	int i;
	for (i = 0; i < NF2_OF_ACTION_WORD_LEN; ++i) {
		wt((OPENFLOW_WILDCARD_LOOKUP_CMP_ACTION_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_action.raw[i]);	
	}
	return 0;
}
int del_entry(uint8_t tableid, int row)
{
	int i;	
	for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; ++i) {
		wt((OPENFLOW_WILDCARD_LOOKUP_CMP_DATA_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), 0 );		
	}
	for (i = 0; i < NF2_OF_MASK_WORD_LEN; ++i) {
		wt((OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), 0);	
	}
	for (i = 0; i < NF2_OF_ACTION_WORD_LEN; ++i) {
		wt((OPENFLOW_WILDCARD_LOOKUP_CMP_ACTION_REG | (tableid << 0x14) | (row << 0x08))+ (4 * i), 0);	
	}
	clear_entry_counter(tableid, row);

	return 0;
}

int inport_to_dma(uint8_t tableid,int row)
{
	printf("add default entries to table :%d,prority:%d\n", tableid, row);
	int i, j;
	PNode hw_entry;
	hw_entry = (PNode)malloc(sizeof(Entry_Node));

	if (tableid == 0) {
		if (row == 0) {
			hw_entry->flow_entry.flow_match.raw[0] = 0;
			hw_entry->flow_entry.flow_mask.raw[0] = 0xffff0000;
			hw_entry->flow_entry.flow_mask.raw[1] = 0xffffffff;
			hw_entry->flow_entry.flow_mask.raw[2] = 0xffffffff;
			hw_entry->flow_entry.flow_action.action.meter_and_n_table |= 1 << 0x4;
			hw_entry->flow_entry.flow_action.action.queue_and_metadata |= 0x1 << 0x4;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[0] |= 0x00;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[1] |= 0x10|0x40;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[2] |= 0x00;
			for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_DATA_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_match.raw[i]);
			}
			for (i = 0; i < NF2_OF_MASK_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_mask.raw[i]);
			}
			for (i = 0; i < NF2_OF_ACTION_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_ACTION_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_action.raw[i]);
			}
			
			return 0;
		}
		if (row == 1) {
			hw_entry->flow_entry.flow_match.raw[0] = 2;
			hw_entry->flow_entry.flow_mask.raw[0] = 0xffff0000;
			hw_entry->flow_entry.flow_mask.raw[1] = 0xffffffff;
			hw_entry->flow_entry.flow_mask.raw[2] = 0xffffffff;
			hw_entry->flow_entry.flow_action.action.meter_and_n_table |= 1 << 0x4;
			hw_entry->flow_entry.flow_action.action.queue_and_metadata |= 0x2 << 0x4;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[0] |= 0x00;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[1] |= 0x10|0x40;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[2] |= 0x00;
			for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_DATA_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_match.raw[i]);
			}
			for (i = 0; i < NF2_OF_MASK_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_mask.raw[i]);
			}
			for (i = 0; i < NF2_OF_ACTION_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_ACTION_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_action.raw[i]);
			}
			
			return 0;	
		}
		if (row == 2) {
			hw_entry->flow_entry.flow_match.raw[0] = 4;
			hw_entry->flow_entry.flow_mask.raw[0] = 0xffff0000;
			hw_entry->flow_entry.flow_mask.raw[1] = 0xffffffff;
			hw_entry->flow_entry.flow_mask.raw[2] = 0xffffffff;
			hw_entry->flow_entry.flow_action.action.meter_and_n_table |= 1 << 0x4;
			hw_entry->flow_entry.flow_action.action.queue_and_metadata |= 0x3 << 0x4;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[0] |= 0x00;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[1] |= 0x10|0x40;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[2] |= 0x00;
			for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_DATA_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_match.raw[i]);
			}
			for (i = 0; i < NF2_OF_MASK_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_mask.raw[i]);
			}
			for (i = 0; i < NF2_OF_ACTION_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_ACTION_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_action.raw[i]);
			}
	
			return 0;
		}
		if (row == 3) {
			hw_entry->flow_entry.flow_match.raw[0] = 6;
			hw_entry->flow_entry.flow_mask.raw[0] = 0xffff0000;
			hw_entry->flow_entry.flow_mask.raw[1] = 0xffffffff;
			hw_entry->flow_entry.flow_mask.raw[2] = 0xffffffff;
			hw_entry->flow_entry.flow_action.action.meter_and_n_table |= 1 << 0x4;
			hw_entry->flow_entry.flow_action.action.queue_and_metadata |= 0x4 << 0x4;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[0] |= 0x00;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[1] |= 0x10|0x40;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[2] |= 0x00;
			for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_DATA_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_match.raw[i]);
			}
			for (i = 0; i < NF2_OF_MASK_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_mask.raw[i]);
			}
			for (i = 0; i < NF2_OF_ACTION_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_ACTION_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_action.raw[i]);
			}
			
			return 0;
		}
	}
	if (tableid == 2) {
		if (row == 0) {
			hw_entry->flow_entry.flow_match.raw[2] = 0x1;
			hw_entry->flow_entry.flow_mask.raw[0] = 0xffffffff;
			hw_entry->flow_entry.flow_mask.raw[1] = 0xffffffff;
			hw_entry->flow_entry.flow_mask.raw[2] = 0xffffff00;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[0] |= 0x01;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[1] |= 0x00;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[2] |= 0x00;
			hw_entry->flow_entry.flow_action.action.forward_bitmask = 0x02;
			for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_DATA_REG | (tableid<<0x14) | (row<<0x08)) + (4 * i), hw_entry->flow_entry.flow_match.raw[i]);
			}
			for (i = 0; i < NF2_OF_MASK_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_REG | (tableid<<0x14) | (row<<0x08)) + (4 * i), hw_entry->flow_entry.flow_mask.raw[i]);
			}
			for (i = 0; i < NF2_OF_ACTION_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_ACTION_REG | (tableid<<0x14) | (row<<0x08)) + (4 * i), hw_entry->flow_entry.flow_action.raw[i]);
			}
	
			return 0;
		}
		if (row == 1) {
			hw_entry->flow_entry.flow_match.raw[2] = 0x2;
			hw_entry->flow_entry.flow_mask.raw[0] = 0xffffffff;
			hw_entry->flow_entry.flow_mask.raw[1] = 0xffffffff;
			hw_entry->flow_entry.flow_mask.raw[2] = 0xffffff00;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[0] |= 0x01;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[1] |= 0x00;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[2] |= 0x00;
			hw_entry->flow_entry.flow_action.action.forward_bitmask = 0x08;          
			for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_DATA_REG | (tableid<<0x14) | (row<<0x08)) + (4 * i), hw_entry->flow_entry.flow_match.raw[i]);
			}
			for (i = 0; i < NF2_OF_MASK_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_REG | (tableid<<0x14) | (row<<0x08)) + (4 * i), hw_entry->flow_entry.flow_mask.raw[i]);
			}
			for (i = 0; i < NF2_OF_ACTION_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_ACTION_REG | (tableid<<0x14) | (row<<0x08)) + (4 * i), hw_entry->flow_entry.flow_action.raw[i]);
			}
			
			return 0;
		}
		if (row == 2) {
			hw_entry->flow_entry.flow_match.raw[2] = 0x3;
			hw_entry->flow_entry.flow_mask.raw[0] = 0xffffffff;
			hw_entry->flow_entry.flow_mask.raw[1] = 0xffffffff;
			hw_entry->flow_entry.flow_mask.raw[2] = 0xffffff00;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[0] |= 0x01;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[1] |= 0x00;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[2] |= 0x00;
			hw_entry->flow_entry.flow_action.action.forward_bitmask = 0x20;
			for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_DATA_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_match.raw[i]);
			}
			for (i = 0; i < NF2_OF_MASK_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_mask.raw[i]);
			}
			for (i = 0; i < NF2_OF_ACTION_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_ACTION_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_action.raw[i]);
			}
			
			return 0;
		}
		if(row == 3) {
			hw_entry->flow_entry.flow_match.raw[2] = 0x4;
			hw_entry->flow_entry.flow_mask.raw[0] = 0xffffffff;
			hw_entry->flow_entry.flow_mask.raw[1] = 0xffffffff;
			hw_entry->flow_entry.flow_mask.raw[2] = 0xffffff00;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[0] |= 0x01;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[1] |= 0x00;
			hw_entry->flow_entry.flow_action.action.nf2_action_flag[2] |= 0x00;
			hw_entry->flow_entry.flow_action.action.forward_bitmask = 0x80;
			for (i = 0; i < NF2_OF_ENTRY_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_DATA_REG | (tableid << 0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_match.raw[i]);
			}
			for (i = 0; i < NF2_OF_MASK_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_REG | (tableid<<0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_mask.raw[i]);
			}
			for (i = 0; i < NF2_OF_ACTION_WORD_LEN; ++i) {
				wt((OPENFLOW_WILDCARD_LOOKUP_CMP_ACTION_REG | (tableid<<0x14) | (row << 0x08)) + (4 * i), hw_entry->flow_entry.flow_action.raw[i]);
			}
			
			return 0;
		}
	}
	free(hw_entry);

	return 0;
}
int mod_meter (int meter_id, unsigned int kbps) {
	clear_meter(meter_id);
	unsigned int inc, interval;
    unsigned int meter_addr_base;
	unsigned int a, b, temp;
	printf("mod_meter begin!\n");
	printf("rate=%d\n", kbps);
	a = kbps;
	b = 8000*125;
	if (a < b) {
		temp = a;
		a = b;
		b = temp;
	}
	while (b != 0) {
		temp = a % b;
		a = b;
		b = temp;
	}
	inc = (unsigned int)(kbps / a);
	interval = (unsigned int)(8000 * 125 / a);
	meter_addr_base = RATE_LIMIT_TOKEN_INTERVAL_REG|(meter_id<<0x08);
	printf("meter_id=%d\n", meter_id);
	printf("interval=%d\n", interval);
	printf("inc=%d\n",inc);
	wt(meter_addr_base, interval);
	wt(meter_addr_base + 4, inc);
	return 0;
}
void clear_meter(int meter_id) {
	unsigned int meter_addr_base;
	meter_addr_base = RATE_LIMIT_TOKEN_INTERVAL_REG|(meter_id<<0x08);

	wt(meter_addr_base, 1);
	wt(meter_addr_base + 4, 1);
}

int rdReg(unsigned reg, unsigned *val)
{

	int fd;
	int i, linenum, count=0;
	unsigned int * map_base;
	unsigned int PHYADDR, length;
	unsigned char val_str[4];
	/* get pagesize on the specific device */
	int  pagesize = getpagesize();
	/* turn a strig into a unsigned long integer */
	PHYADDR = reg;
	length = 1;
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
	};
	/* get the within-page offset */
	int pgoffset = (PHYADDR & 0x00000fff) >> 2;
	*val = map_base[pgoffset];     

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
	unsigned int * map_base;
	unsigned int PHYADDR,newval;
	int  pagesize = getpagesize();
	PHYADDR =reg;
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
	int pgoffset = (PHYADDR & 0x00000fff) >> 2;
	/* write into registers */
	map_base[pgoffset] = newval;
	printf("Value at 0x%08X: 0x%08X\n", PHYADDR,(unsigned int)map_base[pgoffset]);

	close(fd);
	munmap(map_base, pagesize);
	return 0;
}

int  hw_table_list_init(hw_table_list *plist)  
{  
	plist->head = NULL;  
	plist->tail = NULL;  
	plist->size = 0;                  
 
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
		if (plist->tail->flow_entry.prio >= s->flow_entry.prio) {
			hw_table_list_tail_insert(plist, s);
			num = plist->size;
		} else {
		   	while (cur) {
				num++;
				if (cur->flow_entry.prio < s->flow_entry.prio) {
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

int judge_mask(PNode hw_mask1, PNode hw_mask2)
{
	int i = 0, flag = 0;
	if (hw_mask1->flow_entry.flow_mask.raw[0] == hw_mask2->flow_entry.flow_mask.raw[0] 
		&& hw_mask1->flow_entry.flow_mask.raw[1] == hw_mask2->flow_entry.flow_mask.raw[1]
		&& hw_mask1->flow_entry.flow_mask.raw[2] == hw_mask2->flow_entry.flow_mask.raw[2]) {
				flag = 1;
	}
	else
		flag = 0;
	return flag;
}

int judge_entry(PNode hw_match1, PNode hw_match2)
{
	int i = 0, flag = 0;
	if ((hw_match1->flow_entry.flow_match.raw[0] | hw_match2->flow_entry.flow_mask.raw[0]) 
			== (hw_match2->flow_entry.flow_match.raw[0] | hw_match2->flow_entry.flow_mask.raw[0]) 
		&& (hw_match1->flow_entry.flow_match.raw[1] | hw_match2->flow_entry.flow_mask.raw[1]) 
			== (hw_match2->flow_entry.flow_match.raw[1] | hw_match2->flow_entry.flow_mask.raw[1]) 
		&& (hw_match1->flow_entry.flow_match.raw[2]|hw_match2->flow_entry.flow_mask.raw[2]) 
			== (hw_match2->flow_entry.flow_match.raw[2] | hw_match2->flow_entry.flow_mask.raw[2]) ) {
		flag = 1;
	}
	else
		flag = 0;
	return flag;
}
int judge_entry_strict(PNode hw_match1, PNode hw_match2)
{
	int i = 0, flag = 0;
	if (hw_match1->flow_entry.flow_match.raw[0] == hw_match2->flow_entry.flow_match.raw[0] 
		&& hw_match1->flow_entry.flow_match.raw[1] == hw_match2->flow_entry.flow_match.raw[1] 
		&& hw_match1->flow_entry.flow_match.raw[2] == hw_match2->flow_entry.flow_match.raw[2] ) {
		flag = 1;
	}
	else
		flag = 0;
	return flag;
}
 
int hw_table_list_compare(hw_table_list *plist, PNode s)
{
	PNode p;
	p = plist->head;        
	int i = 0;
	if (plist->size == 1 
		&& p->flow_entry.prio == s->flow_entry.prio 
		&& judge_mask(p, s)
		&& judge_entry(p, s)) {
		i++;
		return i;
	}
	else { 
		while (p->next != NULL) {
			i++;
			if (p->flow_entry.prio == s->flow_entry.prio 
				&& judge_mask(p, s)
				&& judge_entry(p, s)) {
				return i;
			}			
			p = p->next;
		}
		if (p == plist->tail) {                        
			i++;
			if (p->flow_entry.prio == s->flow_entry.prio 
				&& judge_mask(p, s)
				&& judge_entry(p, s)) {
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
	memcpy(&(p->flow_entry), &(s->flow_entry), sizeof(hw_flow_entry));

	return 0;
}

int hw_table_list_clear(hw_table_list *plist)
{
	plist->head = NULL;  
	plist->tail = NULL;  
	plist->size = 0;  

	return 0;
}
