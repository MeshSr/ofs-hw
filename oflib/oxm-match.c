/* Copyright (c) 2012, CPqD, Brazil
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Ericsson Research nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */
/*
 *  * Copyright (c) 2010 Nicira Networks.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#include <config.h>

#include "oxm-match.h"

#include <netinet/icmp6.h>
#include "hmap.h"
#include "hash.h"
#include "ofp.h"
#include "ofpbuf.h"
#include "byte-order.h"
#include "packets.h"
#include "ofpbuf.h"
#include "oflib/ofl-structs.h"
#include "oflib/ofl-utils.h"
#include "oflib/ofl-print.h"
#include "unaligned.h"
#include "byte-order.h"
#include "../include/openflow/openflow.h"
#include "ofl-messages.h"
#include "hw_table_define.h"
#include "reg_defines_openflow_switch.h"
#define LOG_MODULE VLM_oxm_match
#include "vlog.h"

/*patch by meshsr*/
extern PNode hw_entry;
uint32_t hw_in_port;
extern table_id;
extern row;
int match_type_flag_0 =5;
int match_type_flag_1 =3;
int match_type_flag_2 =5; 
int hw_match_type_flag_0;
int hw_match_type_flag_1;
int hw_match_type_flag_2;
int match_type_flag;
int hw_match_type_flag;
/*
value = position[x][y]
x represent match type
	0 ip_tos
	1 ip_proto
	2 in_port
	3 eth_type
	4 transport src
	5 transport_dst
	6 vlan_id 
	7 ip_src
	8 ip_dst
	9 mac_src
	10 mac_dst
y represent the header parse 
value represent the match field's position in every header parse	
*/
int match_type_field_positon[11][5] ={
	{0,0,0,0,2},    /* 0 ip_tos */
	{0,0,0,0,1},    /* 1 ip_proto */
	{0,0,0,0,0},	/* 2 in_port */
	{0,0,0,0,0},	/* 3 eth_type */
	{0,1,0,0,0},	/* 4 transport src */
	{0,2,0,0,0},	/* 5 transport_dst */
	{0,0,0,0,0},	/* 6 vlan_id */
	{0,0,0,0,0},	/* 7 ip_src */
	{0,0,0,0,0},	/* 8 ip_dst */
	{0,0,0,0,0},	/* 9 mac_src */
	{0,0,0,0,0},	/* 10 mac_dst */
};
static struct vlog_rate_limit rl = VLOG_RATE_LIMIT_INIT(1, 5);

/* Possible masks for TLV OXM_ETH_DST_W. */
static const uint8_t eth_all_0s[ETH_ADDR_LEN]
    = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t eth_all_1s[ETH_ADDR_LEN]
    = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const uint8_t eth_mcast_1[ETH_ADDR_LEN]
    = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t eth_mcast_0[ETH_ADDR_LEN]
    = {0xfe, 0xff, 0xff, 0xff, 0xff, 0xff};

struct oxm_field all_fields[NUM_OXM_FIELDS] = {
#define DEFINE_FIELD(HEADER, DL_TYPES, NW_PROTO, MASKABLE)     \
    { HMAP_NODE_NULL_INITIALIZER, OFI_OXM_##HEADER, OXM_##HEADER, \
        DL_CONVERT DL_TYPES, NW_PROTO, MASKABLE },
#define DL_CONVERT(T1, T2) { CONSTANT_HTONS(T1), CONSTANT_HTONS(T2) }
#include "oxm-match.def"
};

/* Hash table of 'oxm_fields'. */
static struct hmap all_oxm_fields = HMAP_INITIALIZER(&all_oxm_fields);

static void
oxm_init(void)
{
    if (hmap_is_empty(&all_oxm_fields)) {
        int i;

        for (i = 0; i < NUM_OXM_FIELDS; i++) {
            struct oxm_field *f = &all_fields[i];
            hmap_insert(&all_oxm_fields, &f->hmap_node,
                        hash_int(f->header, 0));
        }

        /* Verify that the header values are unique (duplicate "case" values
         * cause a compile error). */
        switch (0) {
#define DEFINE_FIELD(HEADER, DL_TYPE, NW_PROTO, MASKABLE)  \
        case OXM_##HEADER: break;
#include "oxm-match.def"
        }
    }
}

struct oxm_field *
oxm_field_lookup(uint32_t header)
{
    struct oxm_field *f;

    oxm_init();
    HMAP_FOR_EACH_WITH_HASH(f, struct oxm_field, hmap_node, hash_int(header, 0),
                            &all_oxm_fields) {
        if (f->header == header) {
            return f;
        }
    }
    return NULL;
}


struct ofl_match_tlv *
oxm_match_lookup(uint32_t header, const struct ofl_match *omt)
{
    struct ofl_match_tlv *f;
    HMAP_FOR_EACH_WITH_HASH(f, struct ofl_match_tlv, hmap_node, hash_int(header, 0),
    					    &omt->match_fields) {
        if (f->header == header) {
            return f;
        }
    }
    return NULL;
}


static bool
check_present_prereq(const struct ofl_match *match, uint32_t header){

    struct ofl_match_tlv *omt;

    /* Check for header */
    HMAP_FOR_EACH_WITH_HASH (omt, struct ofl_match_tlv, hmap_node, hash_int(header, 0),
          &match->match_fields) {
         return true;
    }
    return false;
}

bool
oxm_prereqs_ok(const struct oxm_field *field, const struct ofl_match *rule)
{

    struct ofl_match_tlv *omt = NULL;

    /*Check ICMP type*/
    if (field->header == OXM_OF_IPV6_ND_SLL || field->header == OXM_OF_IPV6_ND_TARGET ){
        bool found =  false;
        HMAP_FOR_EACH_WITH_HASH (omt, struct ofl_match_tlv, hmap_node, hash_int(OXM_OF_ICMPV6_TYPE, 0),
              &rule->match_fields) {
            if (*omt->value != ICMPV6_NEIGHSOL){
                return false;
            }
            found = true;
        }
        if(!found)
            return false;
    }
    /*Check ICMP type*/
    if (field->header == OXM_OF_IPV6_ND_TLL || field->header == OXM_OF_IPV6_ND_TARGET){
        bool found =  false;
        HMAP_FOR_EACH_WITH_HASH (omt, struct ofl_match_tlv, hmap_node, hash_int(OXM_OF_ICMPV6_TYPE, 0),
              &rule->match_fields) {
            if (*omt->value != ICMPV6_NEIGHADV){
                return false;
            }
            found = true;
        }
        if(!found)
            return false;
    }

    /*Check for IP_PROTO */
    if (field->nw_proto){
        bool found =  false;
        HMAP_FOR_EACH_WITH_HASH (omt, struct ofl_match_tlv, hmap_node, hash_int(OXM_OF_IP_PROTO, 0),
            &rule->match_fields) {
            uint8_t ip_proto;
            memcpy(&ip_proto,omt->value, sizeof(uint8_t));
            if (field->nw_proto != ip_proto)
                return false;
            found = true;
        }
        if(!found)
            return false;
    }

    /* Check for eth_type */
    if (!field->dl_type[0])
        return true;
    else {
        HMAP_FOR_EACH_WITH_HASH (omt, struct ofl_match_tlv, hmap_node, hash_int(OXM_OF_ETH_TYPE, 0),
              &rule->match_fields) {
              uint16_t eth_type;
              memcpy(&eth_type, omt->value, sizeof(uint16_t));
              if (field->dl_type[0] == htons(eth_type)) {
                return true;
              } else if (field->dl_type[1] && field->dl_type[1] ==  htons(eth_type)) {
                return true;
              }
        }
    }

    return false;
}

static bool
check_oxm_dup(struct ofl_match *match,const struct oxm_field *om){

    struct ofl_match_tlv *t;
    HMAP_FOR_EACH_WITH_HASH(t, struct ofl_match_tlv, hmap_node ,hash_int(om->header, 0),
                             &match->match_fields) {
        return true;
    }
    return false;

}

static uint8_t* get_oxm_value(struct ofl_match *m, uint32_t header){

     struct ofl_match_tlv *t;
     HMAP_FOR_EACH_WITH_HASH (t, struct ofl_match_tlv, hmap_node, hash_int(header, 0),
          &m->match_fields) {
         return t->value;
     }

     return NULL;
}

static int
parse_oxm_entry(struct ofl_match *match, const struct oxm_field *f,
	const void *value, const void *mask){
	int i;
	if (table_id == 0) {
		match_type_flag = match_type_flag_0;
		hw_match_type_flag = hw_match_type_flag_0;		
	}
	if (table_id == 1) {
		match_type_flag = match_type_flag_1;
		hw_match_type_flag = hw_match_type_flag_1;	
	}
	if (table_id == 2) {
		match_type_flag = match_type_flag_2;	
		hw_match_type_flag = hw_match_type_flag_2;
	}
	switch (f->index) {
		case OFI_OXM_OF_IN_PORT: {
			uint32_t *in_port = (uint32_t*) value;
			hw_in_port=htonl(*in_port);
			unsigned port_num;
			unsigned port_num_phy, port_num_mask;
			switch (match_type_flag) {
				case 1:{
					hw_entry->flow_entry.flow_match.match_field_type_1.match_16.in_port = (uint32_t)((htonl(*in_port)-1)*2);
					hw_entry->flow_entry.flow_mask.match_field_type_1.match_16.in_port = 0x0000;		
					break;
				}
				case 3:{
					if(match_type_field_positon[2][hw_match_type_flag] == 1) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_1.in_port = (uint32_t)((htonl(*in_port)-1)*2);
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_1.in_port = 0x0000;
					}
					if(match_type_field_positon[2][hw_match_type_flag] == 2) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_2.in_port = (uint32_t)((htonl(*in_port)-1)*2);
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_2.in_port = 0x0000;
					}
					break;
				}
				case 5:{
					hw_entry->flow_entry.flow_match.match_field_type_5.match_16.in_port = (uint32_t)((htonl(*in_port)-1)*2);
					hw_entry->flow_entry.flow_mask.match_field_type_5.match_16.in_port = 0x0000;
					break;
				}
			}
			ofl_structs_match_put32(match, f->header, ntohl(*in_port));
			return 0;
		}
		case OFI_OXM_OF_IN_PHY_PORT:{
			/* Check for inport presence */
			if (check_present_prereq(match,OXM_OF_IN_PORT))
				ofl_structs_match_put32(match, f->header, ntohl(*((uint32_t*) value)));
			else return ofp_mkerr(OFPET_BAD_MATCH, OFPBMC_BAD_PREREQ);

	    }
		case OFI_OXM_OF_METADATA:{
			/*patch by meshsr*/                 
			hw_entry->flow_entry.flow_match.raw[2] = htonl(*((uint32_t*)value+1))&0x000000ff;
			hw_entry->flow_entry.flow_mask.raw[2] = 0xffffff00;
			if (row == 0) {
				if (table_id == 0) {
					hw_match_type_flag_0 = htonl(*((uint32_t*)value));
					if (hw_match_type_flag_0 == 0
						||hw_match_type_flag_0 == 2
						||hw_match_type_flag_0 == 3)
						match_type_flag_0 = 5;
					if (hw_match_type_flag_0 == 1)
						match_type_flag_0 = 3;
					if (hw_match_type_flag_0 == 4)
						match_type_flag_0 = 1;
					wt((OPENFLOW_WILDCARD_LOOKUP_CMP_FLAG_REG|(table_id<<0x14)), hw_match_type_flag_0);
				}
				if (table_id == 1) {
					hw_match_type_flag_1 = htonl(*((uint32_t*)value));
					if (hw_match_type_flag_1 == 0
						||hw_match_type_flag_1 == 2
						||hw_match_type_flag_1 == 3)
						match_type_flag_1 = 5;
					if (hw_match_type_flag_1 == 1)
						match_type_flag_1 = 3;
					if (hw_match_type_flag_1 == 4)
						match_type_flag_1 = 1;
					wt((OPENFLOW_WILDCARD_LOOKUP_CMP_FLAG_REG|(table_id<<0x14)), hw_match_type_flag_1);
				}
				if (table_id == 2) {
					hw_match_type_flag_2 = htonl(*((uint32_t*)value));
					if (hw_match_type_flag_2 == 0
						||hw_match_type_flag_2 == 2
						||hw_match_type_flag_2 == 3)
						match_type_flag_2 = 5;
					if (hw_match_type_flag_2 == 1)
						match_type_flag_2 = 3;
					if (hw_match_type_flag_2 == 4)
						match_type_flag_2 = 1;
					wt((OPENFLOW_WILDCARD_LOOKUP_CMP_FLAG_REG|(table_id<<0x14)), hw_match_type_flag_2);
				}
			}
			ofl_structs_match_put64(match, f->header, ntoh64(*((uint64_t*) value))&0x00000000000000ff);
			return 0;
		}
		case OFI_OXM_OF_METADATA_W:{
 			/*patch by meshsr*/	
			ofl_structs_match_put64m(match, f->header, ntoh64(*((uint64_t*) value)), ntoh64(*((uint64_t*) mask)));
			return 0;
	    }
	    /* Ethernet header. */
		case OFI_OXM_OF_ETH_DST:
		case OFI_OXM_OF_ETH_SRC:{
			switch (match_type_flag) {
				case 2:{
					if ((f->index) == OFI_OXM_OF_ETH_SRC) { 
						for (i = 0;i < 6;i++) {
							hw_entry->flow_entry.flow_match.match_field_type_2.match_48.eth_src[5-i] = *((uint8_t*)value+i);
							hw_entry->flow_entry.flow_mask.match_field_type_2.match_48.eth_src[i] = 0;
						}
					}
					if ((f->index) == OFI_OXM_OF_ETH_DST) { 
						for(i = 0;i < 6;i++) {
							hw_entry->flow_entry.flow_match.match_field_type_2.match_48.eth_dst[5-i] = *((uint8_t*)value+i);
							hw_entry->flow_entry.flow_mask.match_field_type_2.match_48.eth_dst[i] = 0;
						}
					}
					break;
				}
				case 5:{
					if ((f->index) == OFI_OXM_OF_ETH_SRC) { 
						for (i = 0;i < 6;i++) {
							hw_entry->flow_entry.flow_match.match_field_type_5.match_48.eth_src[5-i] = *((uint8_t*)value+i);
							hw_entry->flow_entry.flow_mask.match_field_type_5.match_48.eth_src[i] = 0;
						}
					}
					if ((f->index) == OFI_OXM_OF_ETH_DST) { 
						for (i = 0;i < 6;i++) {
							hw_entry->flow_entry.flow_match.match_field_type_5.match_48.eth_dst[5-i] = *((uint8_t*)value+i);
							hw_entry->flow_entry.flow_mask.match_field_type_5.match_48.eth_dst[i] = 0;
						}
					}
					break;				
				}		
			}
			ofl_structs_match_put_eth(match, f->header, (uint8_t* )value);
			return 0;
		}
		case OFI_OXM_OF_ETH_DST_W:
		case OFI_OXM_OF_ETH_SRC_W:{
			ofl_structs_match_put_eth_m(match, f->header, (uint8_t* )value, (uint8_t* )mask );
			return 0;
	    }
		case OFI_OXM_OF_ETH_TYPE:{
			uint16_t* eth_type = (uint16_t*) value;
			switch (match_type_flag) {
				case 1:{
					hw_entry->flow_entry.flow_match.match_field_type_1.match_16.eth_type = ntohs(*eth_type);
					hw_entry->flow_entry.flow_mask.match_field_type_1.match_16.eth_type = 0x0000;		
					break;
				}
				case 3:{
					if(match_type_field_positon[3][hw_match_type_flag] == 1) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_1.eth_type = ntohs(*eth_type);
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_1.eth_type = 0x0000;
					}
					if(match_type_field_positon[3][hw_match_type_flag] == 2) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_2.eth_type = ntohs(*eth_type);
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_2.eth_type = 0x0000;
					}
					break;
				}
				case 5:{
					hw_entry->flow_entry.flow_match.match_field_type_5.match_16.eth_type = ntohs(*eth_type);
					hw_entry->flow_entry.flow_mask.match_field_type_5.match_16.eth_type = 0x0000;
					break;
				}
			}				
			ofl_structs_match_put16(match, f->header, ntohs(*eth_type));
			return 0;
		}
		/* 802.1Q header. */
		case OFI_OXM_OF_VLAN_VID:{
			uint16_t* vlan_id = (uint16_t*) value;
			if (ntohs(*vlan_id) > OFPVID_PRESENT+VLAN_VID_MAX) {
				return ofp_mkerr(OFPET_BAD_MATCH, OFPBMC_BAD_VALUE);
			} else {				
				switch (match_type_flag) {
					case 1:{
						hw_entry->flow_entry.flow_match.match_field_type_1.match_16.vlan_id = ntohs(*vlan_id);
						hw_entry->flow_entry.flow_mask.match_field_type_1.match_16.vlan_id = 0xf000;		
						break;
					}
					case 3:{
						if(match_type_field_positon[6][hw_match_type_flag] == 1) {
							hw_entry->flow_entry.flow_match.match_field_type_3.match_16_1.vlan_id = ntohs(*vlan_id);
							hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_1.vlan_id = 0xf000;
							
						}
						if(match_type_field_positon[6][hw_match_type_flag] == 2) {
							hw_entry->flow_entry.flow_match.match_field_type_3.match_16_2.vlan_id = ntohs(*vlan_id);
							hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_2.vlan_id = 0xf000;
						}
						break;
					}
					case 5:{
						hw_entry->flow_entry.flow_match.match_field_type_5.match_16.vlan_id = ntohs(*vlan_id);
						hw_entry->flow_entry.flow_mask.match_field_type_5.match_16.vlan_id = 0xf000;
						break;
					}
				}			
				ofl_structs_match_put16(match, f->header, ntohs(*vlan_id));
				return 0;
			}
		}
		case OFI_OXM_OF_VLAN_VID_W:{
			uint16_t* vlan_id = (uint16_t*) value;
			uint16_t* vlan_mask = (uint16_t*) mask;

			if (ntohs(*vlan_id) > OFPVID_PRESENT+VLAN_VID_MAX)
				return ofp_mkerr(OFPET_BAD_MATCH, OFPBMC_BAD_VALUE);
			else {			   
				ofl_structs_match_put16m(match, f->header, ntohs(*vlan_id), ntohs(*vlan_mask));
				return 0;
			}
	    }
		case OFI_OXM_OF_VLAN_PCP:{
			 /* Check for VLAN_VID presence */
			if (check_present_prereq(match,OXM_OF_VLAN_VID)){
				uint8_t *p = get_oxm_value(match,OXM_OF_VLAN_VID);
				if (*(uint16_t*) p != OFPVID_NONE ){
					uint8_t *v = (uint8_t*) value;		
					switch (match_type_flag) {
						case 1:{
							hw_entry->flow_entry.flow_match.match_field_type_1.match_16.vlan_id = 
								(hw_entry->flow_entry.flow_match.match_field_type_1.match_16.vlan_id & 0x0fff) | ((*v) << 0xd);
							hw_entry->flow_entry.flow_mask.match_field_type_1.match_16.vlan_id = 0x1000;		
							break;
						}
						case 3:{
							if(match_type_field_positon[6][hw_match_type_flag] == 1) {
								hw_entry->flow_entry.flow_match.match_field_type_3.match_16_1.vlan_id = 
									(hw_entry->flow_entry.flow_match.match_field_type_3.match_16_1.vlan_id & 0x0fff) | ((*v) << 0xd);
								hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_1.vlan_id = 0x1000;
							
							}
							if(match_type_field_positon[6][hw_match_type_flag] == 2) {
								hw_entry->flow_entry.flow_match.match_field_type_3.match_16_2.vlan_id = 
									(hw_entry->flow_entry.flow_match.match_field_type_3.match_16_2.vlan_id & 0x0fff) | ((*v) << 0xd);
								hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_2.vlan_id = 0x1000;
							}
							break;
						}
						case 5:{
							hw_entry->flow_entry.flow_match.match_field_type_5.match_16.vlan_id = 
								(hw_entry->flow_entry.flow_match.match_field_type_5.match_16.vlan_id & 0x0fff) | ((*v) << 0xd);
							hw_entry->flow_entry.flow_mask.match_field_type_5.match_16.vlan_id = 0x1000;
							break;
						}
					}					 
					ofl_structs_match_put8(match, f->header, *v);
				}
				return 0;
			}
			else
			{				
				return ofp_mkerr(OFPET_BAD_MATCH, OFPBMC_BAD_PREREQ);
			}
		}
		/* IP header. */
		case OFI_OXM_OF_IP_DSCP:{
			uint8_t *v = (uint8_t*) value;
			if (*v & 0xc0) {
			 	return ofp_mkerr(OFPET_BAD_MATCH, OFPBMC_BAD_VALUE);
			}
			else{	
				switch (match_type_flag) {
					case 1:{
						if (match_type_field_positon[0][hw_match_type_flag] == 1) {
							hw_entry->flow_entry.flow_match.match_field_type_1.match_8_1.ip_tos = *v;
							hw_entry->flow_entry.flow_mask.match_field_type_1.match_8_1.ip_tos = 0xc0;
						} else {
							hw_entry->flow_entry.flow_match.match_field_type_1.match_8_2.ip_tos = *v;
							hw_entry->flow_entry.flow_mask.match_field_type_1.match_8_2.ip_tos = 0xc0;
						}	
						break;
					}
					case 2:{
						if (match_type_field_positon[0][hw_match_type_flag]==1) {
							hw_entry->flow_entry.flow_match.match_field_type_1.match_8_1.ip_tos = *v;
							hw_entry->flow_entry.flow_mask.match_field_type_1.match_8_1.ip_tos = 0xc0;
						} else {
							hw_entry->flow_entry.flow_match.match_field_type_1.match_8_2.ip_tos = *v;
							hw_entry->flow_entry.flow_mask.match_field_type_1.match_8_2.ip_tos = 0xc0;
						}	
						break;
					}
				}								
			 	ofl_structs_match_put8(match, f->header, *v);
				return 0;
			}
		}
		case OFI_OXM_OF_IP_ECN:
		case OFI_OXM_OF_IP_PROTO:{
			uint8_t *v = (uint8_t*) value;
			switch (match_type_flag) {
				case 1:{
					if (match_type_field_positon[1][hw_match_type_flag] == 1) {
						hw_entry->flow_entry.flow_match.match_field_type_1.match_8_1.ip_tos = *v;
						hw_entry->flow_entry.flow_mask.match_field_type_1.match_8_1.ip_tos = 0x00;
					} 
					if (match_type_field_positon[1][hw_match_type_flag] == 2) {
						hw_entry->flow_entry.flow_match.match_field_type_1.match_8_2.ip_tos = *v;
						hw_entry->flow_entry.flow_mask.match_field_type_1.match_8_2.ip_tos = 0x00;
					}	
					break;
				}
				case 2:{
					if (match_type_field_positon[1][hw_match_type_flag]==1) {
						hw_entry->flow_entry.flow_match.match_field_type_1.match_8_1.ip_tos = *v;
						hw_entry->flow_entry.flow_mask.match_field_type_1.match_8_1.ip_tos = 0x00;
					} 
					if (match_type_field_positon[1][hw_match_type_flag] == 2) {
						hw_entry->flow_entry.flow_match.match_field_type_1.match_8_2.ip_tos = *v;
						hw_entry->flow_entry.flow_mask.match_field_type_1.match_8_2.ip_tos = 0x00;
					}	
					break;
				}
			}				
			ofl_structs_match_put8(match, f->header, *v);
			return 0;
		}
		/* IP addresses in IP and ARP headers. */
		case OFI_OXM_OF_IPV4_SRC:
			 
		case OFI_OXM_OF_IPV4_DST:
			 
			 
		case OFI_OXM_OF_ARP_TPA:
		case OFI_OXM_OF_ARP_SPA:{					 
			 ofl_structs_match_put32(match, f->header, *((uint32_t*) value));
			 return 0;
		}
		case OFI_OXM_OF_IPV4_DST_W:			 
		case OFI_OXM_OF_IPV4_SRC_W:
		case OFI_OXM_OF_ARP_SPA_W:
		case OFI_OXM_OF_ARP_TPA_W:{
			switch (match_type_flag) {
				case 1:{
					if (((f->index) == OFI_OXM_OF_IPV4_DST)||((f->index) == OFI_OXM_OF_IPV4_DST_W)) {
						hw_entry->flow_entry.flow_match.match_field_type_1.match_32.ip_dst = htonl(*((uint32_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_1.match_32.ip_dst = *((uint32_t*) mask);
					}
					if ((f->index) == OFI_OXM_OF_IPV4_SRC_W) {
						hw_entry->flow_entry.flow_match.match_field_type_1.match_32.ip_src = htonl(*((uint32_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_1.match_32.ip_src = *((uint32_t*) mask);
					}
					break;
				}
				case 3:{
					if (((f->index) == OFI_OXM_OF_IPV4_DST)||((f->index) == OFI_OXM_OF_IPV4_DST_W)) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_32.ip_dst = htonl(*((uint32_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_32.ip_dst = *((uint32_t*) mask);
					}
					if ((f->index) == OFI_OXM_OF_IPV4_SRC_W) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_32.ip_src = htonl(*((uint32_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_32.ip_src = *((uint32_t*) mask);
					}
					break;
				}
				case 4:{
					if ((f->index) == OFI_OXM_OF_IPV4_DST_W) {
						if (match_type_field_positon[8][hw_match_type_flag] == 1) {
							hw_entry->flow_entry.flow_match.match_field_type_4.match_32_1.ip_dst = htonl(*((uint32_t*) value));
							hw_entry->flow_entry.flow_mask.match_field_type_4.match_32_1.ip_dst = *((uint32_t*) mask);
						}
						if (match_type_field_positon[8][hw_match_type_flag] == 2) {
							hw_entry->flow_entry.flow_match.match_field_type_4.match_32_2.ip_dst = htonl(*((uint32_t*) value));
							hw_entry->flow_entry.flow_mask.match_field_type_4.match_32_2.ip_dst = *((uint32_t*) mask);
						}
					}
					if ((f->index) == OFI_OXM_OF_IPV4_SRC_W) {
						if (match_type_field_positon[7][hw_match_type_flag] == 1) {
							hw_entry->flow_entry.flow_match.match_field_type_4.match_32_1.ip_src = htonl(*((uint32_t*) value));
							hw_entry->flow_entry.flow_mask.match_field_type_4.match_32_1.ip_src = *((uint32_t*) mask);
						}
						if (match_type_field_positon[7][hw_match_type_flag] == 2) {
							hw_entry->flow_entry.flow_match.match_field_type_4.match_32_2.ip_src = htonl(*((uint32_t*) value));
							hw_entry->flow_entry.flow_mask.match_field_type_4.match_32_2.ip_src = *((uint32_t*) mask);
						}
					}
					
				}
			}
			ofl_structs_match_put32m(match, f->header, *((uint32_t*) value), *((uint32_t*) mask));
			return 0;
		}
		case OFI_OXM_OF_ARP_SHA:
		case OFI_OXM_OF_ARP_THA:
					 ofl_structs_match_put_eth(match, f->header,(uint8_t* )value);
					 return 0;

		case OFI_OXM_OF_ARP_SHA_W:
		case OFI_OXM_OF_ARP_THA_W:
					 ofl_structs_match_put_eth_m(match, f->header,(uint8_t* )value, (uint8_t* )mask );
					 return 0;

					 /* IPv6 addresses. */
		case OFI_OXM_OF_IPV6_SRC:
		case OFI_OXM_OF_IPV6_DST:{
						 ofl_structs_match_put_ipv6(match, f->header,(uint8_t* ) value);
						 return 0;
					 }
		case OFI_OXM_OF_IPV6_SRC_W:
		case OFI_OXM_OF_IPV6_DST_W:{
						   ofl_structs_match_put_ipv6m(match, f->header,(uint8_t* ) value,(uint8_t* ) mask);
						   return 0;
					   }
		case OFI_OXM_OF_IPV6_FLABEL:{
						    ofl_structs_match_put32(match, f->header, ntohl(*((uint32_t*) value)));
						    return 0;
					    }
		case OFI_OXM_OF_IPV6_FLABEL_W:{
						      ofl_structs_match_put32m(match, f->header, ntohl(*((uint32_t*) value)), ntohl(*((uint32_t*) mask)));
						      return 0;
					      }
					      /* TCP header. */
		case OFI_OXM_OF_TCP_SRC:
			/*by meshsr*/
			switch (match_type_flag) {
				case 1:{
					hw_entry->flow_entry.flow_match.match_field_type_1.match_16.tcp_src = ntohs(*((uint16_t*) value));
					hw_entry->flow_entry.flow_mask.match_field_type_1.match_16.tcp_src = 0x0000;		
					break;
				}
				case 3:{
					if (match_type_field_positon[4][hw_match_type_flag] == 1) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_1.tcp_src = ntohs(*((uint16_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_1.tcp_src = 0x0000;
						
					}
					if (match_type_field_positon[4][hw_match_type_flag] == 2) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_2.tcp_src = ntohs(*((uint16_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_2.tcp_src = 0x0000;
					}
					break;
				}
				case 5:{
					hw_entry->flow_entry.flow_match.match_field_type_5.match_16.tcp_src = ntohs(*((uint16_t*) value));
					hw_entry->flow_entry.flow_mask.match_field_type_5.match_16.tcp_src = 0x0000;
					break;
				}
			}
			ofl_structs_match_put16(match, f->header, ntohs(*((uint16_t*) value)));
			return 0;
			 
		case OFI_OXM_OF_TCP_DST:
			/*by meshsr*/
			switch (match_type_flag) {
				case 1:{
					hw_entry->flow_entry.flow_match.match_field_type_1.match_16.tcp_dst = ntohs(*((uint16_t*) value));
					hw_entry->flow_entry.flow_mask.match_field_type_1.match_16.tcp_dst = 0x0000;		
					break;
				}
				case 3:{
					if (match_type_field_positon[5][hw_match_type_flag] == 1) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_1.tcp_dst = ntohs(*((uint16_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_1.tcp_dst = 0x0000;
						
					}
					if (match_type_field_positon[5][hw_match_type_flag] == 2) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_2.tcp_dst = ntohs(*((uint16_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_2.tcp_dst = 0x0000;
					}
					break;
				}
				case 5:{
					hw_entry->flow_entry.flow_match.match_field_type_5.match_16.tcp_dst = ntohs(*((uint16_t*) value));
					hw_entry->flow_entry.flow_mask.match_field_type_5.match_16.tcp_dst = 0x0000;
					break;
				}
			}
			ofl_structs_match_put16(match, f->header, ntohs(*((uint16_t*) value)));
			return 0;
					      /* UDP header. */
		case OFI_OXM_OF_UDP_SRC:
			switch (match_type_flag) {
				case 1:{
					hw_entry->flow_entry.flow_match.match_field_type_1.match_16.udp_src = ntohs(*((uint16_t*) value));
					hw_entry->flow_entry.flow_mask.match_field_type_1.match_16.udp_src = 0x0000;		
					break;
				}
				case 3:{
					if (match_type_field_positon[4][hw_match_type_flag] == 1) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_1.udp_src = ntohs(*((uint16_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_1.udp_src = 0x0000;
					
					}
					if (match_type_field_positon[4][hw_match_type_flag] == 2) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_2.udp_src = ntohs(*((uint16_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_2.udp_src = 0x0000;
					}
					break;
				}
				case 5:{
					hw_entry->flow_entry.flow_match.match_field_type_5.match_16.udp_src = ntohs(*((uint16_t*) value));
					hw_entry->flow_entry.flow_mask.match_field_type_5.match_16.udp_src = 0x0000;
					break;
				}
			}
			ofl_structs_match_put16(match, f->header, ntohs(*((uint16_t*) value)));
			return 0;
		case OFI_OXM_OF_UDP_DST:
			switch (match_type_flag) {
				case 1:{
					hw_entry->flow_entry.flow_match.match_field_type_1.match_16.udp_dst = ntohs(*((uint16_t*) value));
					hw_entry->flow_entry.flow_mask.match_field_type_1.match_16.udp_dst = 0x0000;		
					break;
				}
				case 3:{
					if (match_type_field_positon[5][hw_match_type_flag] == 1) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_1.udp_dst = ntohs(*((uint16_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_1.udp_dst = 0x0000;
						
					}
					if (match_type_field_positon[5][hw_match_type_flag] == 2) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_2.udp_dst = ntohs(*((uint16_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_2.udp_dst = 0x0000;
					}
					break;
				}
				case 5:{
					hw_entry->flow_entry.flow_match.match_field_type_5.match_16.udp_dst = ntohs(*((uint16_t*) value));
					hw_entry->flow_entry.flow_mask.match_field_type_5.match_16.udp_dst = 0x0000;
					break;
				}
			}
			ofl_structs_match_put16(match, f->header, ntohs(*((uint16_t*) value)));
			return 0;
					      /* SCTP header. */
		case OFI_OXM_OF_SCTP_SRC:
			switch (match_type_flag) {
				case 1:{
					hw_entry->flow_entry.flow_match.match_field_type_1.match_16.sctp_src = ntohs(*((uint16_t*) value));
					hw_entry->flow_entry.flow_mask.match_field_type_1.match_16.sctp_src = 0x0000;		
					break;
				}
				case 3:{
					if (match_type_field_positon[4][hw_match_type_flag] == 1) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_1.sctp_src = ntohs(*((uint16_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_1.sctp_src = 0x0000;
						
					}
					if (match_type_field_positon[4][hw_match_type_flag] == 2) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_2.sctp_src = ntohs(*((uint16_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_2.sctp_src = 0x0000;
					}
					break;
				}
				case 5:{
					hw_entry->flow_entry.flow_match.match_field_type_5.match_16.sctp_src = ntohs(*((uint16_t*) value));
					hw_entry->flow_entry.flow_mask.match_field_type_5.match_16.sctp_src = 0x0000;
					break;
				}
			}
			ofl_structs_match_put16(match, f->header, ntohs(*((uint16_t*) value)));
			return 0;
		case OFI_OXM_OF_SCTP_DST:{
			switch (match_type_flag) {
				case 1:{
					hw_entry->flow_entry.flow_match.match_field_type_1.match_16.sctp_dst = ntohs(*((uint16_t*) value));
					hw_entry->flow_entry.flow_mask.match_field_type_1.match_16.sctp_dst = 0x0000;		
					break;
				}
				case 3:{
					if (match_type_field_positon[5][hw_match_type_flag] == 1) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_1.sctp_dst = ntohs(*((uint16_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_1.sctp_dst = 0x0000;
						
					}
					if (match_type_field_positon[5][hw_match_type_flag] == 2) {
						hw_entry->flow_entry.flow_match.match_field_type_3.match_16_2.sctp_dst = ntohs(*((uint16_t*) value));
						hw_entry->flow_entry.flow_mask.match_field_type_3.match_16_2.sctp_dst = 0x0000;
					}
					break;
				}
				case 5:{
					hw_entry->flow_entry.flow_match.match_field_type_5.match_16.sctp_dst = ntohs(*((uint16_t*) value));
					hw_entry->flow_entry.flow_mask.match_field_type_5.match_16.sctp_dst = 0x0000;
					break;
				}
			}
			ofl_structs_match_put16(match, f->header, ntohs(*((uint16_t*) value)));
			return 0;
		}
		/* ICMP header. */
		case OFI_OXM_OF_ICMPV4_TYPE:
		case OFI_OXM_OF_ICMPV4_CODE:
					      /* ICMPv6 header. */
		case OFI_OXM_OF_ICMPV6_TYPE:
		case OFI_OXM_OF_ICMPV6_CODE:{
						    uint8_t *v = (uint8_t*) value;
						    ofl_structs_match_put8(match, f->header, *v);
						    return 0;
					    }
					    /* IPv6 Neighbor Discovery. */
		case OFI_OXM_OF_IPV6_ND_TARGET:
					    ofl_structs_match_put_ipv6(match, f->header,(uint8_t* ) value);
					    return 0;
		case OFI_OXM_OF_IPV6_ND_SLL:
		case OFI_OXM_OF_IPV6_ND_TLL:
					    ofl_structs_match_put_eth(match, f->header,(uint8_t* )value);
					    return 0;
					    /* ARP header. */
		case OFI_OXM_OF_ARP_OP:{
					       ofl_structs_match_put16(match, f->header, ntohs(*((uint16_t*) value)));
					       return 0;
				       }
		case OFI_OXM_OF_MPLS_LABEL:
				       {					    
					       ofl_structs_match_put32(match, f->header, ntohl(*((uint32_t*) value)));
					       return 0;
				       }
		case OFI_OXM_OF_MPLS_TC:{
						uint8_t *v = (uint8_t*) value;
						ofl_structs_match_put8(match, f->header, *v);
						return 0;
					}
		case OFI_OXM_OF_MPLS_BOS:{
						 uint8_t *v = (uint8_t*) value;
						 ofl_structs_match_put8(match, f->header, *v);
						 return 0;
					 }
		case OFI_OXM_OF_PBB_ISID:
					 ofl_structs_match_put32(match, f->header, ntohl(*((uint32_t*) value)));
					 return 0;
		case OFI_OXM_OF_PBB_ISID_W:
					 ofl_structs_match_put32m(match, f->header, ntohl(*((uint32_t*) value)), ntohl(*((uint32_t*) mask)));
					 return 0;
		case OFI_OXM_OF_TUNNEL_ID:{
						  ofl_structs_match_put64(match, f->header, *((uint64_t*) value));
						  return 0;
					  }
		case OFI_OXM_OF_TUNNEL_ID_W:{
						    ofl_structs_match_put64m(match, f->header,*((uint64_t*) value),*((uint64_t*) mask));
						    return 0;
					    }
		case OFI_OXM_OF_IPV6_EXTHDR:
					    ofl_structs_match_put16(match, f->header, ntohs(*((uint16_t*) value)));
					    return 0;
		case OFI_OXM_OF_IPV6_EXTHDR_W:
					    ofl_structs_match_put16m(match, f->header, ntohs(*((uint16_t*) value)),ntohs(*((uint16_t*) mask)));
					    return 0;
		case NUM_OXM_FIELDS:
					    NOT_REACHED();
	}
	NOT_REACHED();
}
 /*hmap_insert(match_dst, &f->hmap_node,
                hash_int(f->header, 0));               */


/* oxm_pull_match() and helpers. */


/* Puts the match in a hash_map structure */
int
oxm_pull_match(struct ofpbuf *buf, struct ofl_match * match_dst, int match_len)
{

    uint32_t header;
    uint8_t *p;
	int i;
    p = ofpbuf_try_pull(buf, match_len);
    if (!p) {
        VLOG_DBG_RL(LOG_MODULE,&rl, "oxm_match length %u, rounded up to a "
                    "multiple of 8, is longer than space in message (max "
                    "length %d)", match_len, buf->size);

        return ofp_mkerr(OFPET_BAD_MATCH, OFPBRC_BAD_LEN);
    }

    /* Initialize the match hashmap */
    ofl_structs_match_init(match_dst);

    while ((header = oxm_entry_ok(p, match_len)) != 0) {
	
        unsigned length = OXM_LENGTH(header);
        const struct oxm_field *f;
        int error;
        f = oxm_field_lookup(header);
        if (!f) {
            error = ofp_mkerr(OFPET_BAD_MATCH, OFPBMC_BAD_FIELD);
        }
        else if (OXM_HASMASK(header) && !f->maskable){
            error = ofp_mkerr(OFPET_BAD_MATCH, OFPBMC_BAD_MASK);
        }
        else if (!oxm_prereqs_ok(f, match_dst)) {
            error = ofp_mkerr(OFPET_BAD_MATCH, OFPBMC_BAD_PREREQ);
        }
        else if (check_oxm_dup(match_dst,f)){
            error = ofp_mkerr(OFPET_BAD_MATCH, OFPBMC_DUP_FIELD);
        }
        else {
            /* 'hasmask' and 'length' are known to be correct at this point
             * because they are included in 'header' and oxm_field_lookup()
             * checked them already. */
            error = parse_oxm_entry(match_dst, f, p + 4, p + 4 + length / 2);
        }
        if (error) {
            VLOG_DBG_RL(LOG_MODULE,&rl, "bad oxm_entry with vendor=%"PRIu32", "
                        "field=%"PRIu32", hasmask=%"PRIu32", type=%"PRIu32" "
                        "(error %x)",
                        OXM_VENDOR(header), OXM_FIELD(header),
                        OXM_HASMASK(header), OXM_TYPE(header),
                        error);
            return error;
        }
        p += 4 + length;
        match_len -= 4 + length;
    }
    return match_len ? ofp_mkerr(OFPET_BAD_MATCH, OFPBMC_BAD_LEN) : 0;
}


uint32_t
oxm_entry_ok(const void *p, unsigned int match_len)
{
    unsigned int payload_len;
    uint32_t header;

    if (match_len <= 4) {
        if (match_len) {
            VLOG_DBG(LOG_MODULE,"oxm_match ends with partial oxm_header");
        }
        return 0;
    }

    memcpy(&header, p, 4);
    header = ntohl(header);
    payload_len = OXM_LENGTH(header);
    if (!payload_len) {
        VLOG_DBG(LOG_MODULE, "oxm_entry %08"PRIx32" has invalid payload "
                    "length 0", header);
        return 0;
    }
    if (match_len < payload_len + 4) {
        VLOG_DBG(LOG_MODULE, "%"PRIu32"-byte oxm_entry but only "
                    "%u bytes left in ox_match", payload_len + 4, match_len);
        VLOG_DBG(LOG_MODULE, "Header ==  %d"
                    ,  OXM_FIELD(header));
        return 0;
    }
    return header;
}

/* oxm_put_match() and helpers.
 *
 * 'put' functions whose names end in 'w' add a wildcarded field.
 * 'put' functions whose names end in 'm' add a field that might be wildcarded.
 * Other 'put' functions add exact-match fields.
 */

static void
oxm_put_header(struct ofpbuf *buf, uint32_t header)
{
    uint32_t n_header = htonl(header);
    ofpbuf_put(buf, &n_header, sizeof n_header);

}

static void
oxm_put_8(struct ofpbuf *buf, uint32_t header, uint8_t value)
{
    oxm_put_header(buf, header);
    ofpbuf_put(buf, &value, sizeof value);
}

static void
oxm_put_8w(struct ofpbuf *buf, uint32_t header, uint8_t value, uint16_t mask){

    oxm_put_header(buf, header);
    ofpbuf_put(buf, &value, sizeof value);
    ofpbuf_put(buf, &mask, sizeof mask);

}

static void
oxm_put_16(struct ofpbuf *buf, uint32_t header, uint16_t value)
{
    oxm_put_header(buf, header);
    ofpbuf_put(buf, &value, sizeof value);
}

static void
oxm_put_16w(struct ofpbuf *buf, uint32_t header, uint16_t value, uint16_t mask)
{
   oxm_put_header(buf, header);
   ofpbuf_put(buf, &value, sizeof value);
   ofpbuf_put(buf, &mask, sizeof mask);
}

static void
oxm_put_32(struct ofpbuf *buf, uint32_t header, uint32_t value)
{
    oxm_put_header(buf, header);
    ofpbuf_put(buf, &value, sizeof value);
}

static void
oxm_put_32w(struct ofpbuf *buf, uint32_t header, uint32_t value, uint32_t mask)
{
    oxm_put_header(buf, header);
    ofpbuf_put(buf, &value, sizeof value);
    ofpbuf_put(buf, &mask, sizeof mask);
}

static void
oxm_put_64(struct ofpbuf *buf, uint32_t header, uint64_t value)
{
    oxm_put_header(buf, header);
    ofpbuf_put(buf, &value, sizeof value);
}

static void
oxm_put_64w(struct ofpbuf *buf, uint32_t header, uint64_t value, uint64_t mask)
{
    oxm_put_header(buf, header);
    ofpbuf_put(buf, &value, sizeof value);
    ofpbuf_put(buf, &mask, sizeof mask);
}

static void
oxm_put_eth(struct ofpbuf *buf, uint32_t header,
            const uint8_t value[ETH_ADDR_LEN])
{
    oxm_put_header(buf, header);
    ofpbuf_put(buf, value, ETH_ADDR_LEN);

}

static void
oxm_put_ethm(struct ofpbuf *buf, uint32_t header,
            const uint8_t value[ETH_ADDR_LEN], const uint8_t mask[ETH_ADDR_LEN])
{
    oxm_put_header(buf, header);
    ofpbuf_put(buf, value, ETH_ADDR_LEN);
    ofpbuf_put(buf, mask, ETH_ADDR_LEN);
}

static void oxm_put_ipv6(struct ofpbuf *buf, uint32_t header,
                    uint8_t value[IPv6_ADDR_LEN]){
     oxm_put_header(buf, header);
     ofpbuf_put(buf, value, IPv6_ADDR_LEN);
}

static void oxm_put_ipv6m(struct ofpbuf *buf, uint32_t header,
                    uint8_t value[ETH_ADDR_LEN], uint8_t mask[ETH_ADDR_LEN]){
    oxm_put_header(buf, header);
    ofpbuf_put(buf, value, IPv6_ADDR_LEN);
    ofpbuf_put(buf, mask, IPv6_ADDR_LEN);
}

/* TODO: put the ethernet destiny address handling possible masks
static void
oxm_put_eth_dst(struct ofpbuf *b,
                uint32_t wc, const uint8_t value[ETH_ADDR_LEN])
{
    switch (wc & (bufWW_DL_DST | FWW_ETH_MCAST)) {
    case FWW_DL_DST | FWW_ETH_MCAST:
        break;
    case FWW_DL_DST:
        oxm_put_header(b, oxM_OF_ETH_DST_W);
        ofpbuf_put(b, value, ETH_ADDR_LEN);
        ofpbuf_put(b, eth_mcast_1, ETH_ADDR_LEN);
        break;
    case FWW_ETH_MCAST:
        oxm_put_header(b, oxM_OF_ETH_DST_W);
        ofpbuf_put(b, value, ETH_ADDR_LEN);
        ofpbuf_put(b, eth_mcast_0, ETH_ADDR_LEN);
        break;
    case 0:
        oxm_put_eth(b, oxM_OF_ETH_DST, value);
        break;
    }
}*/

static bool
is_requisite(uint32_t header){
    if(header == OXM_OF_IN_PORT || header == OXM_OF_ETH_TYPE
        || header == OXM_OF_VLAN_VID || header == OXM_OF_IP_PROTO) {
        return true;
    }
    return false;
}

/* Puts the match in the buffer */
int oxm_put_match(struct ofpbuf *buf, struct ofl_match *omt){

    struct ofl_match_tlv *oft;
    int start_len = buf->size;
    int match_len;


    /* We put all pre-requisites fields first */
    /* In port present */
    HMAP_FOR_EACH_WITH_HASH(oft, struct ofl_match_tlv, hmap_node, hash_int(OXM_OF_IN_PORT, 0),
          &omt->match_fields) {
        uint32_t value;
        memcpy(&value, oft->value,sizeof(uint32_t));
        oxm_put_32(buf,oft->header, htonl(value));
    }

    /* L2 Pre-requisites */

    /* Ethernet type */
    HMAP_FOR_EACH_WITH_HASH(oft, struct ofl_match_tlv, hmap_node, hash_int(OXM_OF_ETH_TYPE, 0),
          &omt->match_fields) {
        uint16_t value;
        memcpy(&value, oft->value,sizeof(uint16_t));
        oxm_put_16(buf,oft->header, htons(value));
    }

     /* VLAN ID */
    HMAP_FOR_EACH_WITH_HASH(oft, struct ofl_match_tlv, hmap_node, hash_int(OXM_OF_VLAN_VID, 0),
          &omt->match_fields) {
         uint16_t value;
         memcpy(&value, oft->value,sizeof(uint16_t));
         oxm_put_16(buf,oft->header, htons(value));
    }

    /* L3 Pre-requisites */
     HMAP_FOR_EACH_WITH_HASH(oft, struct ofl_match_tlv, hmap_node, hash_int(OXM_OF_IP_PROTO, 0),
          &omt->match_fields) {
         uint8_t value;
         memcpy(&value, oft->value,sizeof(uint8_t));
         oxm_put_8(buf,oft->header, value);
    }

    /* Loop through the remaining fields */
    HMAP_FOR_EACH(oft, struct ofl_match_tlv, hmap_node, &omt->match_fields){

        if (is_requisite(oft->header))
            /*We already inserted  fields that are pre requisites to others */
             continue;
        else {
            uint8_t length = OXM_LENGTH(oft->header) ;
            bool has_mask =false;
            if (OXM_HASMASK(oft->header)){
               length = length / 2;
               has_mask = true;
            }
            switch (length){
                case (sizeof(uint8_t)):{
                    uint8_t value;
                    memcpy(&value, oft->value,sizeof(uint8_t));
                    if(!has_mask)
                        oxm_put_8(buf,oft->header, value);
                    else {
                        uint8_t mask;
                        memcpy(&mask,oft->value + length ,sizeof(uint8_t));
                        oxm_put_8w(buf, oft->header,value,mask);
                    }
                    break;
                  }
                case (sizeof(uint16_t)):{
                    uint16_t value;
                    memcpy(&value, oft->value,sizeof(uint16_t));
                    if(!has_mask)
                        oxm_put_16(buf,oft->header, htons(value));
                    else {
                        uint16_t mask;
                        memcpy(&mask,oft->value + length ,sizeof(uint16_t));
                        oxm_put_16w(buf, oft->header,htons(value),htons(mask));
                    }
                    break;
                }
                case (sizeof(uint32_t)):{
                    uint32_t value;
                    memcpy(&value, oft->value,sizeof(uint32_t));
					if(!has_mask)
						if (oft->header == OXM_OF_IPV4_DST || oft->header == OXM_OF_IPV4_SRC
							||oft->header == OXM_OF_ARP_SPA || oft->header == OXM_OF_ARP_TPA)
							oxm_put_32(buf,oft->header, value);
						else
							oxm_put_32(buf,oft->header, htonl(value));
                    else {
                         uint32_t mask;
                         memcpy(&mask,oft->value + length ,sizeof(uint32_t));
						 if (oft->header == OXM_OF_IPV4_DST_W|| oft->header == OXM_OF_IPV4_SRC_W
							||oft->header == OXM_OF_ARP_SPA_W || oft->header == OXM_OF_ARP_TPA_W){
                            oxm_put_32w(buf, oft->header, value, mask);
                            }
						 else {
							oxm_put_32w(buf, oft->header, htonl(value),htonl(mask));
                         }
                    }
                      break;

                }
                case (sizeof(uint64_t)):{
                     uint64_t value;
                     memcpy(&value, oft->value,sizeof(uint64_t));
                     if(!has_mask)
                         oxm_put_64(buf,oft->header, hton64(value));
                     else {
                         uint64_t mask;
                         memcpy(&mask,oft->value + length ,sizeof(uint64_t));
                         oxm_put_64w(buf, oft->header,hton64(value),hton64(mask));
                     }
                     break;
                }
                case (ETH_ADDR_LEN):{
                     uint8_t value[ETH_ADDR_LEN];
                     memcpy(&value, oft->value,ETH_ADDR_LEN);
                     if(!has_mask)
                         oxm_put_eth(buf,oft->header, value);
                     else {
                         uint8_t mask[ETH_ADDR_LEN];
                         memcpy(&mask,oft->value + length ,ETH_ADDR_LEN);
                         oxm_put_ethm(buf, oft->header,value,mask);
                      }
                      break;
                   }
               case (IPv6_ADDR_LEN):{
                     uint8_t value[IPv6_ADDR_LEN];
                     memcpy(value, oft->value,IPv6_ADDR_LEN);
                     if(!has_mask)
                         oxm_put_ipv6(buf,oft->header, value);
                     else {
                         uint8_t mask[IPv6_ADDR_LEN];
                         memcpy(&mask,oft->value + length ,IPv6_ADDR_LEN);
                         oxm_put_ipv6m(buf, oft->header,value,mask);
                      }
                      break;
                   }
            }
        }
    }

    match_len = buf->size - start_len;
    ofpbuf_put_zeros(buf, ROUND_UP(match_len - 4, 8) - (match_len -4));
    return match_len;
}



