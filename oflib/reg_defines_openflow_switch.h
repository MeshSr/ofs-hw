#ifndef REG_DEFINDS_OPENFLOW_SWITCH_H
#define REG_DEFINDS_OPENFLOW_SWITCH_H  1

#define ONS_DMA_SWITCH_ADDR     0x4b000000
#define ONS_DEVICE_ID_ADDR      0x4b000004
#define ONS_HW_TABLES_ADDR      0x4b00000c
#define ONS_HW_VERSION_ADDR     0x4b000008
#define ONS_HW_TABLE_SIZE_ADDR  0x4b000010

static uint32_t ons_get_hw_table_size()
{
    uint32_t val;
    rdReg(ONS_HW_TABLE_SIZE_ADDR, &val);

    return val;
}

static uint32_t ons_get_hw_tables()
{
    uint32_t val;
    rdReg(ONS_HW_TABLES_ADDR, &val);

    return val;
}

static uint32_t ons_get_device_id()
{
    uint32_t val;
    rdReg(ONS_DEVICE_ID_ADDR, &val);

    return val;
}

static uint32_t ons_get_hw_version()
{
    uint32_t val;
    rdReg(ONS_HW_VERSION_ADDR, &val);

    return val;
}

#define ONS_DEVICE_ID       ons_get_device_id()
#define ONS_HW_TABLES       ons_get_hw_tables()
#define ONS_HW_TABLE_SIZE   ons_get_hw_table_size()
#define ONS_HW_VERSION      ons_get_hw_version()
#define ONS_HW_VERSION_MAJOR    ((ONS_HW_VERSION >> 8) & 0xf)
#define ONS_HW_VERSION_MINOR    ONS_HW_VERSION & 0x0ff

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

#endif
