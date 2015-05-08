/*
ONet OpenFlow Switch hardware flow table definition.
*/
#ifndef HW_TABLE_DEFINE_H
#define HW_TABLE_DEFINE_H
#include <stdint.h>

#define NF2_OF_ENTRY_WORD_LEN	   3
#define NF2_OF_MASK_WORD_LEN	   3
#define NF2_OF_ACTION_WORD_LEN	10

#pragma pack(1)

typedef union match_len8 {
	uint8_t icmpv4_type;
	uint8_t icmpv4_code;
	uint8_t ip_tos;       /*the low 6 bit represent ip_dscp, the high 2 bit represent ip_ecn*/
	uint8_t ip_proto; 
	uint8_t vlan_pcp;
} match_len8;

typedef union match_len16 {
	uint16_t in_port;
	uint16_t arp_op;
	uint16_t in_phy_port;
	uint16_t eth_type;
	uint16_t vlan_id;  /*the low 12 bit represent vlan_vid ,the high 3 bit represent vlan priority*/
	uint16_t tcp_src;
	uint16_t tcp_dst;
	uint16_t udp_src;
	uint16_t udp_dst;
	uint16_t sctp_src;
	uint16_t sctp_dst;
} match_len16;

typedef union match_len32 {
	uint32_t ip_src;
	uint32_t ip_dst;	
	uint32_t arp_spa;
	uint32_t arp_tpa;
} match_len32;

typedef union match_len48 {
	uint8_t eth_src[6];
	uint8_t eth_dst[6];
	uint8_t arp_sha[6];
	uint8_t arp_pha[6];
} match_len48;
	
typedef struct match_type_1 {
	match_len8 match_8_1;
	match_len8 match_8_2;
	match_len16 match_16;
	match_len32 match_32;
	uint32_t metadata;
} match_type_1;

typedef struct match_type_2 {
	match_len8 match_8_1;
	match_len8 match_8_2;
	match_len48 match_48;
	uint32_t metadata;
} match_type_2;

typedef struct match_type_3 {
	match_len16 match_16_1;
	match_len16 match_16_2;
	match_len32 match_32; 
	uint32_t metadata;
} match_type_3;	

typedef struct match_type_4 {
	match_len32 match_32_1;
	match_len32 match_32_2;
	uint32_t metadata;
} match_type_4;	
	
typedef struct match_type_5 {
	match_len16 match_16;
	match_len48 match_48;	
	uint32_t metadata;
} match_type_5;	

typedef union hw_flow_match {
	match_type_1 match_field_type_1;
	match_type_2 match_field_type_2;
	match_type_3 match_field_type_3;
	match_type_4 match_field_type_4;
	match_type_5 match_field_type_5;
	uint32_t raw[NF2_OF_ENTRY_WORD_LEN];
} hw_flow_match;

typedef hw_flow_match hw_flow_mask;

typedef struct hw_entry_action {
	uint8_t forward_bitmask;
	uint8_t nf2_action_flag[3];
	uint16_t set_vlan;
	uint16_t add_vlan;
	uint8_t eth_src[6];
	uint8_t eth_dst[6];
	uint32_t ip_src;
	uint32_t ip_dst;
	uint16_t transp_src;
	uint16_t transp_dst;
    uint8_t meter_and_n_table;
    uint8_t ip_tos;
	uint16_t queue_and_metadata;
	uint8_t reserved[4];
} hw_entry_action;

typedef union hw_flow_action {
	hw_entry_action action;
	uint32_t raw[NF2_OF_ACTION_WORD_LEN];
} hw_flow_action;

typedef struct hw_flow_entry {
	hw_flow_match flow_match;
	hw_flow_mask flow_mask;
	hw_flow_action flow_action;
	int prio;	
} hw_flow_entry;

typedef struct Entry_Node {  
    hw_flow_entry flow_entry;
    struct Entry_Node * prev;
    struct Entry_Node * next;
} Entry_Node, *PNode;  

typedef struct hw_table_list {  
    PNode head;
    PNode tail;
    int size;  
} hw_table_list;  

#pragma pack()

#endif
