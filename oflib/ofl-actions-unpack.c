/* Copyright (c) 2011, TrafficLab, Ericsson Research, Hungary
 * Copyright (c) 2012, CPqD, Brazil
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

#include <netinet/in.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "ofl.h"
#include "ofl-utils.h"
#include "ofl-actions.h"
#include "ofl-structs.h"
#include "ofl-messages.h"
#include "ofl-print.h"
#include "ofl-packets.h"
#include "ofl-log.h"
#include "openflow/openflow.h"
#include "oxm-match.h"
#include "hw_table_define.h"
#define LOG_MODULE ofl_act_u
OFL_LOG_INIT(LOG_MODULE)

extern nf2_of_action_wrap flow_action_t0;
extern nf2_of_action_wrap flow_action_t1;
extern table_id;
extern nf2_action_wrap naction;
extern meter;
ofl_err
ofl_actions_unpack(struct ofp_action_header *src, size_t *len, struct ofl_action_header **dst, struct ofl_exp *exp) {
    /*patch by meshsr*/	
    int i=0;    
    for(i=0;i<6;i++)
    {
        flow_action_t0.action.reserved[i]=0;
        flow_action_t1.action.reserved[i]=0;
    } 
    if (*len < sizeof(struct ofp_action_header)) {
        OFL_LOG_WARN(LOG_MODULE, "Received action is too short (%zu).", *len);
        return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_LEN);
    }

    if (*len < ntohs(src->len)) {
        OFL_LOG_WARN(LOG_MODULE, "Received action has invalid length (set to %u, but only %zu received).", ntohs(src->len), *len);
        return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_LEN);
    }

    if ((ntohs(src->len) % 8) != 0) {
        OFL_LOG_WARN(LOG_MODULE, "Received action length is not a multiple of 64 bits (%u).", ntohs(src->len));
        return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_LEN);
    }

    switch (ntohs(src->type)) {
	    case OFPAT_OUTPUT: {
	        struct ofp_action_output *sa;
	        struct ofl_action_output *da;

	        if (*len < sizeof(struct ofp_action_output)) {
		        OFL_LOG_WARN(LOG_MODULE, "Received OUTPUT action has invalid length (%zu).", *len);
		        return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_LEN);
	        }

	        sa = (struct ofp_action_output *)src;

	        if (ntohl(sa->port) < 0 ||
			        (ntohl(sa->port) > OFPP_MAX && ntohl(sa->port) < OFPP_IN_PORT) ||
			        ntohl(sa->port) == OFPP_ANY) {
		        if (OFL_LOG_IS_WARN_ENABLED(LOG_MODULE)) {
			        char *ps = ofl_port_to_string(ntohl(sa->port));
			        OFL_LOG_WARN(LOG_MODULE, "Received OUTPUT action has invalid port (%s).", ps);
			        free(ps);
		        }
		        return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_OUT_PORT);
	        }

	        da = (struct ofl_action_output *)malloc(sizeof(struct ofl_action_output));
	        da->port = ntohl(sa->port);
	        da->max_len = ntohs(sa->max_len);
	        /*patch by meshsr*/				       
	        if(table_id==0)
		         flow_action_t0.action.nf2_action_flag=flow_action_t0.action.nf2_action_flag|0x0001;
	        if(table_id==1)
		        flow_action_t1.action.nf2_action_flag=flow_action_t1.action.nf2_action_flag|0x0001;				      
	        //forward_bitmask.
	        if(table_id==0)
	        {
		        flow_action_t0.action.forward_bitmask=0x01<<(2*(da->port-1));
	        }
	        if(table_id==1)
	        {
		        flow_action_t1.action.forward_bitmask=0x01<<(2*(da->port-1));
	        }

	        *len -= sizeof(struct ofp_action_output);
	        *dst = (struct ofl_action_header *)da;
	        break;
        }
	    case OFPAT_COPY_TTL_OUT: {
		    //ofp_action_header length was already checked
		    *len -= sizeof(struct ofp_action_header);
		    *dst = (struct ofl_action_header *)malloc(sizeof(struct ofl_action_header));
		    break;
	    }
	    case OFPAT_COPY_TTL_IN: {
		    //ofp_action_header length was already checked
		    *len -= sizeof(struct ofp_action_header);
		    *dst = (struct ofl_action_header *)malloc(sizeof(struct ofl_action_header));
		    break;
	    }
	    case OFPAT_SET_MPLS_TTL: {
		    struct ofp_action_mpls_ttl *sa;
		    struct ofl_action_mpls_ttl *da;

		    if (*len < sizeof(struct ofp_action_mpls_ttl)) {
			    OFL_LOG_WARN(LOG_MODULE, "Received SET_MPLS_TTL action has invalid length (%zu).", *len);
			    return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_LEN);
		    }

		    sa = (struct ofp_action_mpls_ttl *)src;

		    da = (struct ofl_action_mpls_ttl *)malloc(sizeof(struct ofl_action_mpls_ttl));
		    da->mpls_ttl = sa->mpls_ttl;	
		    *len -= sizeof(struct ofp_action_mpls_ttl);
		    *dst = (struct ofl_action_header *)da;
		    break;
	    }
	    case OFPAT_DEC_MPLS_TTL: {
		    //ofp_action_header length was already checked
		    *len -= sizeof(struct ofp_action_mpls_ttl);
		    *dst = (struct ofl_action_header *)malloc(sizeof(struct ofl_action_header));
		    break;
	    }
	    case OFPAT_PUSH_VLAN: 
	    case OFPAT_PUSH_PBB:
	    case OFPAT_PUSH_MPLS: {
			struct ofp_action_push *sa;
			struct ofl_action_push *da;

			if (*len < sizeof(struct ofp_action_push)) {
		        OFL_LOG_WARN(LOG_MODULE, "Received PUSH_VLAN/MPLS/PBB action has invalid length (%zu).", *len);
				return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_LEN);
			}

			sa = (struct ofp_action_push *)src;

			if (((ntohs(src->type) == OFPAT_PUSH_VLAN) &&
				(ntohs(sa->ethertype) != ETH_TYPE_VLAN &&
				   ntohs(sa->ethertype) != ETH_TYPE_VLAN_PBB)) ||
				((ntohs(src->type) == OFPAT_PUSH_MPLS) &&
				(ntohs(sa->ethertype) != ETH_TYPE_MPLS &&
				   ntohs(sa->ethertype) != ETH_TYPE_MPLS_MCAST)) ||
				((ntohs(src->type) == OFPAT_PUSH_PBB) &&
				(ntohs(sa->ethertype) != ETH_TYPE_PBB))) {
				OFL_LOG_WARN(LOG_MODULE, "Received PUSH_VLAN/MPLS/PBB has invalid eth type. (%u)", ntohs(sa->ethertype));
				return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT);
			}
			da = (struct ofl_action_push *)malloc(sizeof(struct ofl_action_push));
			da->ethertype = ntohs(sa->ethertype);

			*len -= sizeof(struct ofp_action_push);
			*dst = (struct ofl_action_header *)da;
		    break;
		}
	    case OFPAT_POP_VLAN: 
	    case OFPAT_POP_PBB: {
			//ofp_action_header length was already checked
			//hw_table_forward_bitmask.
			if(table_id==0)
				flow_action_t0.action.nf2_action_flag=flow_action_t0.action.nf2_action_flag|0x0008;
			if(table_id==1)
				flow_action_t1.action.nf2_action_flag=flow_action_t1.action.nf2_action_flag|0x0008;
			//end hw_table_forward_bitmask.
			*len -= sizeof(struct ofp_action_header);
			*dst = (struct ofl_action_header *)malloc(sizeof(struct ofl_action_header));
			break;
		}
	    case OFPAT_POP_MPLS: {
			struct ofp_action_pop_mpls *sa;
			struct ofl_action_pop_mpls *da;

			if (*len < sizeof(struct ofp_action_pop_mpls)) {
			    OFL_LOG_WARN(LOG_MODULE, "Received POP_MPLS action has invalid length (%zu).", *len);
			    return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_LEN);
		    }

			sa = (struct ofp_action_pop_mpls *)src;

			da = (struct ofl_action_pop_mpls *)malloc(sizeof(struct ofl_action_pop_mpls));
			da->ethertype = ntohs(sa->ethertype);

			*len -= sizeof(struct ofp_action_pop_mpls);
			*dst = (struct ofl_action_header *)da;
			break;
		}
	    case OFPAT_SET_QUEUE: {
			struct ofp_action_set_queue *sa;
			struct ofl_action_set_queue *da;

			if (*len < sizeof(struct ofp_action_set_queue)) {
				 OFL_LOG_WARN(LOG_MODULE, "Received SET_QUEUE action has invalid length (%zu).", *len);
				 return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_LEN);
			}

			sa = (struct ofp_action_set_queue *)src;

			da = (struct ofl_action_set_queue *)malloc(sizeof(struct ofl_action_set_queue));
			da->queue_id = ntohl(sa->queue_id);
			/*patch by meshsr*/
			*len -= sizeof(struct ofp_action_set_queue);
		    *dst = (struct ofl_action_header *)da;
			break;
		}
	    case OFPAT_GROUP: {
	        struct ofp_action_group *sa;
	        struct ofl_action_group *da;

	        if (*len < sizeof(struct ofp_action_group)) {
		        OFL_LOG_WARN(LOG_MODULE, "Received GROUP action has invalid length (%zu).", *len);
		        return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_LEN);
	        }

	        sa = (struct ofp_action_group *)src;

	        if (ntohl(sa->group_id) > OFPG_MAX) {
		        if (OFL_LOG_IS_WARN_ENABLED(LOG_MODULE)) {
			        char *gs = ofl_group_to_string(ntohl(sa->group_id));
			        OFL_LOG_WARN(LOG_MODULE, "Received GROUP action has invalid group id (%s).", gs);
			        free(gs);
		        }
		        return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_OUT_GROUP);
	        }

	        da = (struct ofl_action_group *)malloc(sizeof(struct ofl_action_group));
	        da->group_id = ntohl(sa->group_id);
	        /*patch by meshsr*/        
	        *len -= sizeof(struct ofp_action_group);
	        *dst = (struct ofl_action_header *)da;
	        break;
        }
	    case OFPAT_SET_NW_TTL: {
		    struct ofp_action_nw_ttl *sa;
		    struct ofl_action_set_nw_ttl *da;

		    if (*len < sizeof(struct ofp_action_nw_ttl)) {
			    OFL_LOG_WARN(LOG_MODULE, "Received SET_NW_TTL action has invalid length (%zu).", *len);
			    return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_LEN);
		    }

		    sa = (struct ofp_action_nw_ttl *)src;

		    da = (struct ofl_action_set_nw_ttl *)malloc(sizeof(struct ofl_action_set_nw_ttl));
		    da->nw_ttl = sa->nw_ttl;
		    *len -= sizeof(struct ofp_action_nw_ttl);
		    *dst = (struct ofl_action_header *)da;
		    break;
	    }
	    case OFPAT_DEC_NW_TTL: {
		    //ofp_action_header length was already checked
		    *len -= sizeof(struct ofp_action_header);
		    *dst = (struct ofl_action_header *)malloc(sizeof(struct ofl_action_header));
		    break;
	    }
	    case OFPAT_SET_FIELD: {
			struct ofp_action_set_field *sa;
			struct ofl_action_set_field *da;
			uint8_t *value;

			sa = (struct ofp_action_set_field*) src;
			da = (struct ofl_action_set_field *)malloc(sizeof(struct ofl_action_set_field));
			da->field = (struct ofl_match_tlv*) malloc(sizeof(struct ofl_match_tlv));

			memcpy(&da->field->header,sa->field,4);
			da->field->header = ntohl(da->field->header);
			value = (uint8_t *) src + sizeof (struct ofp_action_set_field);
			da->field->value = malloc(OXM_LENGTH(da->field->header));
			/*TODO: need to check if other fields are valid */
			if(da->field->header == OXM_OF_IN_PORT || da->field->header == OXM_OF_IN_PHY_PORT
					|| da->field->header == OXM_OF_METADATA){
				return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_SET_TYPE);
			}
			uint8_t temp0=0,temp1=0,temp9=0,temp10=0,temp13=0,temp15=0;
			uint16_t temp2=0,temp3=0,temp11=0,temp12=0;
			uint32_t temp4=0,temp5=0,temp14=0;
			uint8_t *a=&temp0,*b=&temp1,*c=&temp9,*e=&temp4;
			uint16_t *d=&temp2;
			uint8_t eth_src[6];
			uint8_t eth_dst[6];	
                                  uint16_t transp_src;
			uint16_t transp_dst;
                                  uint32_t ip_src;
		 	uint32_t ip_dst;  
			switch(OXM_LENGTH(da->field->header)){
				case 1:
				case 6:
				case 16:
					 if(OXM_LENGTH(da->field->header)==1)
					 {
						  switch(OXM_FIELD(da->field->header))
						  {
							   case 8:{									 
								    memcpy(a, value, OXM_LENGTH(da->field->header));
								    //hw_table_forward_bitmask.
								    if(table_id==0)
										 flow_action_t0.action.nf2_action_flag=flow_action_t0.action.nf2_action_flag|0x0100;
								    if(table_id==1)
										 flow_action_t1.action.nf2_action_flag=flow_action_t1.action.nf2_action_flag|0x0100;
								    //end hw_table_forward_bitmask.
								    break;
							   }
							   case 9:{									
									memcpy(b, value, OXM_LENGTH(da->field->header));
									break;
							   }
							   case 7:{									 
									memcpy(c, value, OXM_LENGTH(da->field->header));
									//hw_table_action_vlan_pcp
									if(table_id==0)
									{
										 flow_action_t0.action.vlan_pcp=(*c);
									}
									if(table_id==1)
									{
										 flow_action_t1.action.vlan_pcp=(*c);
									}									    
									//hw_table_forward_bitmask.
									if(table_id==0)
										 flow_action_t0.action.nf2_action_flag=flow_action_t0.action.nf2_action_flag|0x0004;
									if(table_id==1)
										 flow_action_t1.action.nf2_action_flag=flow_action_t1.action.nf2_action_flag|0x0004;
									//end hw_table_forward_bitmask.
									break;
							   }
						  }						 
						  //hw_table_action_ip_tos
						  if(table_id==0)
						  {
							  flow_action_t0.action.ip_tos=(*a);
						  }
						  if(table_id==1)
						  {
							  flow_action_t1.action.ip_tos=(*a);
						  }
					 }
					 if(OXM_LENGTH(da->field->header)==6)
					 {

						  switch(OXM_FIELD(da->field->header))
						  {
							  case 4:{
								   memcpy(&(eth_src), value, OXM_LENGTH(da->field->header));
								   int d=0;
								   for(d=0;d<3;d++)
								   {
										int temp=0;
										temp=eth_src[d];
										eth_src[d]=eth_src[5-d];
										eth_src[5-d]=temp;
								   }
								   //hw_table action->eth_src
								   for(d=0;d<6;d++)
								   {
										 if(table_id==0)
										 {
											 flow_action_t0.action.eth_src[d]=eth_src[d];
										 }
										 if(table_id==1)
										 {
											 flow_action_t1.action.eth_src[d]=eth_src[d];
										 }

								   }									 
								   //end hw_table action.
								   //hw_table_forward_bitmask.
								   if(table_id==0)
									    flow_action_t0.action.nf2_action_flag=flow_action_t0.action.nf2_action_flag|0x0010;
								   if(table_id==1)
									    flow_action_t1.action.nf2_action_flag=flow_action_t1.action.nf2_action_flag|0x0010;
								   //end hw_table_forward_bitmask.
								   break;
							  }
							  case 3:{
									 memcpy(&(eth_dst), value, OXM_LENGTH(da->field->header));

									 int s=0;
									 for(s=0;s<3;s++)
									 {
										 int tempt=0;
										 tempt=eth_dst[s];
										 eth_dst[s]=eth_dst[5-s];
										 eth_dst[5-s]=tempt;}
									 //hw_table action->eth_dst
									 for(s=0;s<6;s++)
									 {
										 if(table_id==0)
										 {
											 flow_action_t0.action.eth_dst[s]=eth_dst[s];
										 }
										 if(table_id==1)
										 {
											 flow_action_t1.action.eth_dst[s]=eth_dst[s];
										 }

									 }
									 //end hw_table action
									 //hw_table_forward_bitmask.
									 if(table_id==0)
										 flow_action_t0.action.nf2_action_flag=flow_action_t0.action.nf2_action_flag|0x0020;
									 if(table_id==1)
										 flow_action_t1.action.nf2_action_flag=flow_action_t1.action.nf2_action_flag|0x0020;
									 //end hw_table_forward_bitmask.
									 break;
							  }
						  }
					 }
					 memcpy(da->field->value , value, OXM_LENGTH(da->field->header));
					 break;

				case 2:{
					 uint16_t v = ntohs(*((uint16_t*) value));
					 if(OXM_LENGTH(da->field->header)==2)
					 {
						  switch(OXM_FIELD(da->field->header))
						  {
								case 13:{
									 memcpy(&(transp_src),&v, OXM_LENGTH(da->field->header));
									 //hw_table_transp		
									 if(table_id==0)
									 {
										  flow_action_t0.action.transp_src=transp_src;
									 }
									 if(table_id==1)
								     {
									      flow_action_t1.action.transp_src=transp_src;
									 }
									 //end hw_table_transp
									 //hw_table_forward_bitmask.
									 if(table_id==0)
										  flow_action_t0.action.nf2_action_flag=flow_action_t0.action.nf2_action_flag|0x0200;
									 if(table_id==1)
									      flow_action_t1.action.nf2_action_flag=flow_action_t1.action.nf2_action_flag|0x0200;
									 //end hw_table_forward_bitmask.
									 break;
								}
								case 14:{
									 memcpy(&(transp_dst),&v, OXM_LENGTH(da->field->header));
									 //hw_table_transp	
									 if(table_id==0)
									 {
										 flow_action_t0.action.transp_dst=transp_dst;
									 }
									 if(table_id==1)
									 {
										 flow_action_t1.action.transp_dst=transp_dst;
									 }
									 //end hw_table_transp
									 //hw_table_forward_bitmask.
									 if(table_id==0)
										 flow_action_t0.action.nf2_action_flag=flow_action_t0.action.nf2_action_flag|0x0400;
									 if(table_id==1)
										 flow_action_t1.action.nf2_action_flag=flow_action_t1.action.nf2_action_flag|0x0400;
									 //end hw_table_forward_bitmask.
									 break;
								}
								case 6:{

									 memcpy(d, value, OXM_LENGTH(da->field->header));

									 //hw_table_action_vlan_id
									 if(table_id==0)
									 {
										 flow_action_t0.action.vlan_id=(*d);
									 }
									 if(table_id==1)
									 {
										 flow_action_t1.action.vlan_id=(*d);
									 }
									 //end       
									 //hw_table_forward_bitmask.
									 if(table_id==0)
									   	 flow_action_t0.action.nf2_action_flag=flow_action_t0.action.nf2_action_flag|0x0002;
									 if(table_id==1)
										 flow_action_t1.action.nf2_action_flag=flow_action_t1.action.nf2_action_flag|0x0002;
									 //end hw_table_forward_bitmask.
									 break;
								}
						  }

					 }
					 memcpy(da->field->value , &v, OXM_LENGTH(da->field->header));
					 break;
				}
				case 4:{
					 uint32_t v; 
					 uint8_t field = OXM_FIELD(da->field->header);					
					 if( field != 11 && field != 12 && field != 22 && field != 23)  
						 v = htonl(*((uint32_t*) value));
					 else v = *((uint32_t*) value);
					 if(field==11)
					 {

						 memcpy(&(ip_src),&v, OXM_LENGTH(da->field->header));
						 //hw_table_ip
						 if(table_id==0)
						 {
							 flow_action_t0.action.ip_src=ip_src;
						 }
						 if(table_id==1)
						 {
							 flow_action_t1.action.ip_src=ip_src;
						 }
						 //end hw_table_ip
						 //hw_table_forward_bitmask.
						 if(table_id==0)
							 flow_action_t0.action.nf2_action_flag=flow_action_t0.action.nf2_action_flag|0x0040;
						 if(table_id==1)
							 flow_action_t1.action.nf2_action_flag=flow_action_t1.action.nf2_action_flag|0x0040;
						 //end hw_table_forward_bitmask.
					 }
					 if(field==12)
					 {

						  memcpy(&(ip_dst),&v, OXM_LENGTH(da->field->header));
						  //hw_table_ip	
						  if(table_id==0)
						  {
							 flow_action_t0.action.ip_dst=ip_dst;
						  }
						  if(table_id==1)
						  {
							  flow_action_t1.action.ip_dst=ip_dst;
						  }
						  //end hw_table_ip
						  //hw_table_forward_bitmask.
						  if(table_id==0)
							  flow_action_t0.action.nf2_action_flag=flow_action_t0.action.nf2_action_flag|0x0080;
						  if(table_id==1)
							  flow_action_t1.action.nf2_action_flag=flow_action_t1.action.nf2_action_flag|0x0080;
						  //end hw_table_forward_bitmask.
					 }
					 if(field==34)
					 {						  
					 }

					 memcpy(da->field->value , &v, OXM_LENGTH(da->field->header));
					 break;
				}
				case 8:{
					 uint64_t v = hton64(*((uint64_t*) value));
					 memcpy(da->field->value , &v, OXM_LENGTH(da->field->header));
					 break;
				}                
			}			
			*len -= ROUND_UP(ntohs(src->len),8);
			*dst = (struct ofl_action_header *)da;
			break;
		}
	    case OFPAT_EXPERIMENTER: {
		     ofl_err error;

		     if (*len < sizeof(struct ofp_action_experimenter_header)) {
			     OFL_LOG_WARN(LOG_MODULE, "Received EXPERIMENTER action has invalid length (%zu).", *len);
			     return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_LEN);
		     }

		     if (exp == NULL || exp->act == NULL || exp->act->unpack == NULL) {
			     OFL_LOG_WARN(LOG_MODULE, "Received EXPERIMENTER action, but no callback is given.");
			     return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_EXPERIMENTER);
		     }
		     error = exp->act->unpack(src, len, dst);
		     if (error) {
			     return error;
		     }
		     break;
	    }
	    default: {
			     OFL_LOG_WARN(LOG_MODULE, "Received unknown action type (%u).", ntohs(src->type));
			     return ofl_error(OFPET_BAD_ACTION, OFPBAC_BAD_TYPE);
		     }
    }
    (*dst)->type = (enum ofp_action_type)ntohs(src->type);

    return 0;
}
