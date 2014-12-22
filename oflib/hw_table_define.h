/*
ONet OpenFlow Switch hardware flow table definition.
*/
#include <stdint.h>

#define NF2_OF_ENTRY_WORD_LEN	   2
#define NF2_OF_MASK_WORD_LEN	   2
#define NF2_OF_ACTION_WORD_LEN	10

#pragma pack(1)
struct nf2_of_entry_t0 {
   uint16_t src_port;
	uint8_t eth_dst[6];
};
struct nf2_of_entry_t1 {
	uint32_t ip_dst;
   uint16_t transp_src;
	uint16_t transp_dst;
};
typedef union nf2_of_entry_wrap_t0 {
	struct nf2_of_entry_t0 entry;
	uint32_t raw[NF2_OF_ENTRY_WORD_LEN];
} nf2_of_entry_wrap_t0;

typedef union nf2_of_entry_wrap_t1 {
	struct nf2_of_entry_t1 entry;
	uint32_t raw[NF2_OF_ENTRY_WORD_LEN];
} nf2_of_entry_wrap_t1;

typedef nf2_of_entry_wrap_t0 nf2_of_mask_wrap_t0;
typedef nf2_of_entry_wrap_t1 nf2_of_mask_wrap_t1;

struct nf2_of_action {
	uint16_t forward_bitmask;
	uint16_t nf2_action_flag;
	uint16_t vlan_id;
	uint8_t vlan_pcp;
	uint8_t eth_src[6];
	uint8_t eth_dst[6];
	uint32_t ip_src;
	uint32_t ip_dst;
	uint8_t ip_tos;
	uint16_t transp_src;
	uint16_t transp_dst;
   uint8_t meter_id;
   uint8_t next_table_id;
	uint8_t reserved[6];
};

typedef union nf2_of_action_wrap {
	struct nf2_of_action action;
	uint32_t raw[NF2_OF_ACTION_WORD_LEN];
} nf2_of_action_wrap;
#pragma pack()