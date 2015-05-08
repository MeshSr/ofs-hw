#define HW_TABLE_SIZE                                     16
/*header parse*/
#define OPENFLOW_WILDCARD_LOOKUP_CMP_FLAG_REG             0X48000050
/* flow entry */
#define OPENFLOW_WILDCARD_LOOKUP_CMP_DATA_REG             0x48000040
#define OPENFLOW_WILDCARD_LOOKUP_CMP_MASK_REG             0x48000030
#define OPENFLOW_WILDCARD_LOOKUP_CMP_ACTION_REG           0X48000000


/*meter rate_limiter*/
//#define RATE_LIMIT_CTRL_REG             0x49000000
#define RATE_LIMIT_TOKEN_INTERVAL_REG   0x49000000
#define RATE_LIMIT_TOKEN_INC_REG        0x49000004

/*mac statistics counters*/
#define PORT_RX_PKT_REG 					0X43C00290
#define PORT_RX_BYTE_REG 					0X43C00200
#define PORT_RX_CRC_ERROR_REG               0X43C00298
/*entry stats counter*/
#define OPENFLOW_WILDCARD_LOOKUP_BYTES_HIT_REG          0x48000060
#define OPENFLOW_WILDCARD_LOOKUP_PKTS_HIT_REG           0x48000064
/*table stats counter*/
#define TABLE_MATCH_COUNT_REG           	0x48000068
#define TABLE_LOOKUP_COUNT_REG           	0x4800006c

/*meter stats counter*/
#define METER_PASS_PKT_COUNTER              0x49000020
#define METER_PASS_BYTE_COUNTER             0x49000024

/*queue stats counter*/
#define QUEUE_TRANSMIT_PKT_COUNT_REG         0x4a000040
#define QUEUE_TRANSMIT_BYTE_COUNT_REG        0x4a000044
#define QUEUE_DROP_PKT_COUNT_REG            0x4a000048
#define QUEUE_DROP_BYTE_COUNT_REG           0x4a00004c

