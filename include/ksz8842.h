/* Bank select register offset is accessible in all banks to allow bank
   selection.
*/

#define REG_BANK_SEL_OFFSET     0x0E

/*
 * KS8841/KS8842 interface to Host by ISA bus.
 */

/* Bank 0 */

/* BAR */
#define REG_BASE_ADDR_BANK      0
#define REG_BASE_ADDR_OFFSET    0x00

/* BDAR */
#define REG_RX_WATERMARK_BANK   0
#define REG_RX_WATERMARK_OFFSET 0x04

#define RX_HIGH_WATERMARK_2KB   0x1000

/* BESR */
#define REG_BUS_ERROR_BANK      0
#define REG_BUS_ERROR_OFFSET    0x06

/* BBLR */
#define REG_BUS_BURST_BANK      0
#define REG_BUS_BURST_OFFSET    0x08

#define BURST_LENGTH_0          0x0000
#define BURST_LENGTH_4          0x3000
#define BURST_LENGTH_8          0x5000
#define BURST_LENGTH_16         0x7000

/* Bank 2 */

#define REG_ADDR_0_BANK         2
/* MARL */
#define REG_ADDR_0_OFFSET       0x00
#define REG_ADDR_1_OFFSET       0x01
/* MARM */
#define REG_ADDR_2_OFFSET       0x02
#define REG_ADDR_3_OFFSET       0x03
/* MARH */
#define REG_ADDR_4_OFFSET       0x04
#define REG_ADDR_5_OFFSET       0x05


/* Bank 3 */

/* OBCR */
#define REG_BUS_CTRL_BANK       3
#define REG_BUS_CTRL_OFFSET     0x00

/* EEPCR */
#define REG_EEPROM_CTRL_BANK    3
#define REG_EEPROM_CTRL_OFFSET  0x02

/* MBIR */
#define REG_MEM_INFO_BANK       3
#define REG_MEM_INFO_OFFSET     0x04

/* GCR */
#define REG_GLOBAL_CTRL_BANK    3
#define REG_GLOBAL_CTRL_OFFSET  0x06

/* WFCR */
#define REG_WOL_CTRL_BANK       3
#define REG_WOL_CTRL_OFFSET     0x0A

/* WF0 */
#define REG_WOL_FRAME_0_BANK    4
#define WOL_FRAME_CRC_OFFSET    0x00
#define WOL_FRAME_BYTE0_OFFSET  0x04
#define WOL_FRAME_BYTE2_OFFSET  0x08

/* WF1 */
#define REG_WOL_FRAME_1_BANK    5

/* WF2 */
#define REG_WOL_FRAME_2_BANK    6

/* WF3 */
#define REG_WOL_FRAME_3_BANK    7


/* Bank 16 */

/* TXCR */
#define REG_TX_CTRL_BANK        16
#define REG_TX_CTRL_OFFSET      0x00

#define TX_CTRL_ENABLE          0x0001
#define TX_CTRL_CRC_ENABLE      0x0002
#define TX_CTRL_PAD_ENABLE      0x0004
#define TX_CTRL_FLOW_ENABLE     0x0008
#define TX_CTRL_MAC_LOOPBACK    0x2000

/* TXSR */
#define REG_TX_STATUS_BANK      16
#define REG_TX_STATUS_OFFSET    0x02

#define TX_FRAME_ID_MASK        0x003F
#define TX_STAT_MAX_COL         0x1000
#define TX_STAT_LATE_COL        0x2000
#define TX_STAT_UNDERRUN        0x4000
#define TX_STAT_COMPLETE        0x8000

#ifdef EARLY_TRANSMIT
#define TX_STAT_ERRORS          ( TX_STAT_MAX_COL | TX_STAT_LATE_COL | TX_STAT_UNDERRUN )
#else
#define TX_STAT_ERRORS          ( TX_STAT_MAX_COL | TX_STAT_LATE_COL )
#endif

#define TX_CTRL_DEST_PORTS      0x0F00
#define TX_CTRL_INTERRUPT_ON    0x8000

#define TX_DEST_PORTS_SHIFT     8

#define TX_FRAME_ID_MAX         (( (TX_FRAME_ID_MASK + 1) / 2 ) - 1 )
#define TX_FRAME_ID_PORT_SHIFT  5

/* RXCR */
#define REG_RX_CTRL_BANK        16
#define REG_RX_CTRL_OFFSET      0x04

#define RX_CTRL_ENABLE          0x0001
#define RX_CTRL_MULTICAST       0x0004
#define RX_CTRL_STRIP_CRC       0x0008
#define RX_CTRL_PROMISCUOUS     0x0010
#define RX_CTRL_UNICAST         0x0020
#define RX_CTRL_ALL_MULTICAST   0x0040
#define RX_CTRL_BROADCAST       0x0080
#define RX_CTRL_BAD_PACKET      0x0200
#define RX_CTRL_FLOW_ENABLE     0x0400

/* TXMIR */
#define REG_TX_MEM_INFO_BANK    16
#define REG_TX_MEM_INFO_OFFSET  0x08

/* RXMIR */
#define REG_RX_MEM_INFO_BANK    16
#define REG_RX_MEM_INFO_OFFSET  0x0A

#define MEM_AVAILABLE_MASK      0x1FFF


/* Bank 17 */

/* TXQCR */
#define REG_TXQ_CMD_BANK        17
#define REG_TXQ_CMD_OFFSET      0x00

#define TXQ_CMD_ENQUEUE_PACKET  0x0001

/* RXQCR */
#define REG_RXQ_CMD_BANK        17
#define REG_RXQ_CMD_OFFSET      0x02

#define RXQ_CMD_FREE_PACKET     0x0001

/* TXFDPR */
#define REG_TX_ADDR_PTR_BANK    17
#define REG_TX_ADDR_PTR_OFFSET  0x04

/* RXFDPR */
#define REG_RX_ADDR_PTR_BANK    17
#define REG_RX_ADDR_PTR_OFFSET  0x06

#define ADDR_PTR_MASK           0x03FF
#define ADDR_PTR_AUTO_INC       0x4000

#define REG_DATA_BANK           17
/* QDRL */
#define REG_DATA_OFFSET         0x08
/* QDRH */
#define REG_DATA_HI_OFFSET      0x0A


/* Bank 18 */

/* IER */
#define REG_INT_MASK_BANK       18
#define REG_INT_MASK_OFFSET     0x00

#define INT_RX_ERROR            0x0080
#define INT_RX_STOPPED          0x0100
#define INT_TX_STOPPED          0x0200
#define INT_RX_EARLY            0x0400
#define INT_RX_OVERRUN          0x0800
#define INT_TX_UNDERRUN         0x1000
#define INT_RX                  0x2000
#define INT_TX                  0x4000
#define INT_PHY                 0x8000
#define INT_MASK                ( INT_RX | INT_TX )


/* ISR */
#define REG_INT_STATUS_BANK     18
#define REG_INT_STATUS_OFFSET   0x02

#ifdef SH_32BIT_ACCESS_ONLY
#define INT_STATUS( intr )  (( intr ) << 16 )
#endif

/* RXSR */
#define REG_RX_STATUS_BANK      18
#define REG_RX_STATUS_OFFSET    0x04

#define RX_BAD_CRC              0x0001
#define RX_TOO_SHORT            0x0002
#define RX_TOO_LONG             0x0004
#define RX_FRAME_ETHER          0x0008
#define RX_PHY_ERROR            0x0010
#define RX_UNICAST              0x0020
#define RX_MULTICAST            0x0040
#define RX_BROADCAST            0x0080
#define RX_SRC_PORTS            0x0F00
#define RX_VALID                0x8000
#define RX_ERRORS     ( RX_BAD_CRC | RX_TOO_LONG | RX_TOO_SHORT | RX_PHY_ERROR )


#define RX_SRC_PORTS_SHIFT      8

/* RXBC */
#define REG_RX_BYTE_CNT_BANK    18
#define REG_RX_BYTE_CNT_OFFSET  0x06

#define RX_BYTE_CNT_MASK        0x07FF

/* ETXR */
#define REG_EARLY_TX_BANK       18
#define REG_EARLY_TX_OFFSET     0x08

#define EARLY_TX_THRESHOLD      0x001F
#define EARLY_TX_ENABLE         0x0080
#define EARLY_TX_MULTIPLE       64

/* ERXR */
#define REG_EARLY_RX_BANK       18
#define REG_EARLY_RX_OFFSET     0x0A

#define EARLY_RX_THRESHOLD      0x001F
#define EARLY_RX_ENABLE         0x0080
#define EARLY_RX_MULTIPLE       64


/* Bank 19 */

#define REG_MULTICAST_BANK      19
/* MTR0 */
#define REG_MULTICAST_0_OFFSET  0x00
#define REG_MULTICAST_1_OFFSET  0x01
/* MTR1 */
#define REG_MULTICAST_2_OFFSET  0x02
#define REG_MULTICAST_3_OFFSET  0x03
/* MTR2 */
#define REG_MULTICAST_4_OFFSET  0x04
#define REG_MULTICAST_5_OFFSET  0x05
/* MTR3 */
#define REG_MULTICAST_6_OFFSET  0x06
#define REG_MULTICAST_7_OFFSET  0x07


#define REG_POWER_CNTL_BANK     19
/* PMCS */
#define REG_POWER_CNTL_OFFSET   0x08


/* -------------------------------------------------------------------------- */

/* KS884X ISA registers */

#define REG_SWITCH_CTRL_BANK        32

#define REG_SIDER_BANK              REG_SWITCH_CTRL_BANK
#define REG_SIDER_OFFSET            0x00

#define REG_CHIP_ID_OFFSET          REG_SIDER_OFFSET
#define REG_FAMILY_ID_OFFSET        ( REG_CHIP_ID_OFFSET + 1 )

#define REG_SGCR1_BANK              REG_SWITCH_CTRL_BANK
#define REG_SGCR1_OFFSET            0x02

#define REG_SWITCH_CTRL_1_OFFSET    REG_SGCR1_OFFSET
#define REG_SWITCH_CTRL_1_HI_OFFSET ( REG_SWITCH_CTRL_1_OFFSET + 1 )

#define REG_SGCR2_BANK              REG_SWITCH_CTRL_BANK
#define REG_SGCR2_OFFSET            0x04

#define REG_SWITCH_CTRL_2_OFFSET    REG_SGCR2_OFFSET
#define REG_SWITCH_CTRL_2_HI_OFFSET ( REG_SWITCH_CTRL_2_OFFSET + 1 )

#define REG_SGCR3_BANK              REG_SWITCH_CTRL_BANK
#define REG_SGCR3_OFFSET            0x06

#define REG_SWITCH_CTRL_3_OFFSET    REG_SGCR3_OFFSET
#define REG_SWITCH_CTRL_3_HI_OFFSET ( REG_SWITCH_CTRL_3_OFFSET + 1 )

#define REG_SGCR4_BANK              REG_SWITCH_CTRL_BANK
#define REG_SGCR4_OFFSET            0x08

#define REG_SWITCH_CTRL_4_OFFSET    REG_SGCR4_OFFSET

#define REG_SGCR5_BANK              REG_SWITCH_CTRL_BANK
#define REG_SGCR5_OFFSET            0x0A

#define REG_SWITCH_CTRL_5_OFFSET    REG_SGCR5_OFFSET


#define REG_SWITCH_802_1P_BANK      33

#define REG_SGCR6_BANK              REG_SWITCH_802_1P_BANK
#define REG_SGCR6_OFFSET            0x00

#define REG_SWITCH_CTRL_6_OFFSET    REG_SGCR6_OFFSET


#define REG_PHAR_BANK               34
#define REG_PHAR_OFFSET             0x00

#define REG_LBS21R_OFFSET           0x04

#define REG_LBRCTCER_OFFSET         0x08

#define REG_LBRCGR_OFFSET           0x0A


#define REG_CSCR_BANK               35
#define REG_CSCR_OFFSET             0x00

#define REG_PSWIR_OFFSET            0x02

#define REG_PC21R_OFFSET            0x04

#define REG_PC3R_OFFSET             0x06

#define REG_VMCRTCR_OFFSET          0x08


#define REG_S58R_BANK               36
#define REG_S58R_OFFSET             0x00

#define REG_MVI21R_OFFSET           0x04

#define REG_MM1V31R_OFFSET          0x06

#define REG_MMI32R_OFFSET           0x08


#define REG_LPVI21R_BANK            37
#define REG_LPVI21R_OFFSET          0x00

#define REG_LPM1V31R_OFFSET         0x04


#define REG_CSSR_BANK               38
#define REG_CSSR_OFFSET             0x00

#define REG_ASCTR_OFFSET            0x04

#define REG_MS21R_OFFSET            0x08

#define REG_LPS21R_OFFSET           0x0A


#define REG_MAC_ADDR_BANK           39

#define REG_MACAR1_BANK             REG_MAC_ADDR_BANK
#define REG_MACAR1_OFFSET           0x00
#define REG_MACAR2_OFFSET           0x02
#define REG_MACAR3_OFFSET           0x04

#define REG_MAC_ADDR_0_OFFSET       REG_MACAR1_OFFSET
#define REG_MAC_ADDR_1_OFFSET       ( REG_MAC_ADDR_0_OFFSET + 1 )
#define REG_MAC_ADDR_2_OFFSET       REG_MACAR2_OFFSET
#define REG_MAC_ADDR_3_OFFSET       ( REG_MAC_ADDR_2_OFFSET + 1 )
#define REG_MAC_ADDR_4_OFFSET       REG_MACAR3_OFFSET
#define REG_MAC_ADDR_5_OFFSET       ( REG_MAC_ADDR_4_OFFSET + 1 )


#define REG_TOS_PRIORITY_BANK       40
#define REG_TOS_PRIORITY_2_BANK     ( REG_TOS_PRIORITY_BANK + 1 )

#define REG_TOSR1_BANK              REG_TOS_PRIORITY_BANK
#define REG_TOSR1_OFFSET            0x00
#define REG_TOSR2_OFFSET            0x02
#define REG_TOSR3_OFFSET            0x04
#define REG_TOSR4_OFFSET            0x06
#define REG_TOSR5_OFFSET            0x08
#define REG_TOSR6_OFFSET            0x0A

#define REG_TOSR7_BANK              REG_TOS_PRIORITY_2_BANK
#define REG_TOSR7_OFFSET            0x00
#define REG_TOSR8_OFFSET            0x02

#define REG_TOS_1_OFFSET            REG_TOSR1_OFFSET
#define REG_TOS_2_OFFSET            REG_TOSR2_OFFSET
#define REG_TOS_3_OFFSET            REG_TOSR3_OFFSET
#define REG_TOS_4_OFFSET            REG_TOSR4_OFFSET
#define REG_TOS_5_OFFSET            REG_TOSR5_OFFSET
#define REG_TOS_6_OFFSET            REG_TOSR6_OFFSET

#define REG_TOS_7_OFFSET            REG_TOSR7_OFFSET
#define REG_TOS_8_OFFSET            REG_TOSR8_OFFSET


#define REG_IND_ACC_CTRL_BANK       42

#define REG_IACR_BANK               REG_IND_ACC_CTRL_BANK
#define REG_IACR_OFFSET             0x00

#define REG_IADR1_BANK              REG_IND_ACC_CTRL_BANK
#define REG_IADR1_OFFSET            0x02

#define REG_IADR2_BANK              REG_IND_ACC_CTRL_BANK
#define REG_IADR2_OFFSET            0x04

#define REG_IADR3_BANK              REG_IND_ACC_CTRL_BANK
#define REG_IADR3_OFFSET            0x06

#define REG_IADR4_BANK              REG_IND_ACC_CTRL_BANK
#define REG_IADR4_OFFSET            0x08

#define REG_IADR5_BANK              REG_IND_ACC_CTRL_BANK
#define REG_IADR5_OFFSET            0x0A

#define REG_ACC_CTRL_INDEX_OFFSET   REG_IACR_OFFSET
#define REG_ACC_CTRL_SEL_OFFSET     ( REG_ACC_CTRL_INDEX_OFFSET + 1 )

#define REG_ACC_DATA_0_OFFSET       REG_IADR4_OFFSET
#define REG_ACC_DATA_1_OFFSET       ( REG_ACC_DATA_0_OFFSET + 1 )
#define REG_ACC_DATA_2_OFFSET       REG_IADR5_OFFSET
#define REG_ACC_DATA_3_OFFSET       ( REG_ACC_DATA_2_OFFSET + 1 )
#define REG_ACC_DATA_4_OFFSET       REG_IADR2_OFFSET
#define REG_ACC_DATA_5_OFFSET       ( REG_ACC_DATA_4_OFFSET + 1 )
#define REG_ACC_DATA_6_OFFSET       REG_IADR3_OFFSET
#define REG_ACC_DATA_7_OFFSET       ( REG_ACC_DATA_6_OFFSET + 1 )
#define REG_ACC_DATA_8_OFFSET       REG_IADR1_OFFSET


#define REG_PHY_1_CTRL_BANK         45
#define REG_PHY_2_CTRL_BANK         46

#define PHY_BANK_INTERVAL           \
    ( REG_PHY_2_CTRL_BANK - REG_PHY_1_CTRL_BANK )

#define REG_P1MBCR_BANK             REG_PHY_1_CTRL_BANK
#define REG_P1MBCR_OFFSET           0x00

#define REG_P1MBSR_BANK             REG_PHY_1_CTRL_BANK
#define REG_P1MBSR_OFFSET           0x02

#define REG_PHY1ILR_BANK            REG_PHY_1_CTRL_BANK
#define REG_PHY1ILR_OFFSET          0x04

#define REG_PHY1LHR_BANK            REG_PHY_1_CTRL_BANK
#define REG_PHY1LHR_OFFSET          0x06

#define REG_P1ANAR_BANK             REG_PHY_1_CTRL_BANK
#define REG_P1ANAR_OFFSET           0x08

#define REG_P1ANLPR_BANK            REG_PHY_1_CTRL_BANK
#define REG_P1ANLPR_OFFSET          0x0A

#define REG_P2MBCR_BANK             REG_PHY_2_CTRL_BANK
#define REG_P2MBCR_OFFSET           0x00

#define REG_P2MBSR_BANK             REG_PHY_2_CTRL_BANK
#define REG_P2MBSR_OFFSET           0x02

#define REG_PHY2ILR_BANK            REG_PHY_2_CTRL_BANK
#define REG_PHY2ILR_OFFSET          0x04

#define REG_PHY2LHR_BANK            REG_PHY_2_CTRL_BANK
#define REG_PHY2LHR_OFFSET          0x06

#define REG_P2ANAR_BANK             REG_PHY_2_CTRL_BANK
#define REG_P2ANAR_OFFSET           0x08

#define REG_P2ANLPR_BANK            REG_PHY_2_CTRL_BANK
#define REG_P2ANLPR_OFFSET          0x0A


#define REG_PHY_SPECIAL_BANK        47

#define REG_P1VCT_BANK              REG_PHY_SPECIAL_BANK
#define REG_P1VCT_OFFSET            0x00

#define REG_P1PHYCTRL_BANK          REG_PHY_SPECIAL_BANK
#define REG_P1PHYCTRL_OFFSET        0x02

#define REG_P2VCT_BANK              REG_PHY_SPECIAL_BANK
#define REG_P2VCT_OFFSET            0x04

#define REG_P2PHYCTRL_BANK          REG_PHY_SPECIAL_BANK
#define REG_P2PHYCTRL_OFFSET        0x06


#define REG_PHY_CTRL_OFFSET         0x00
#define REG_PHY_STATUS_OFFSET       0x02
#define REG_PHY_ID_1_OFFSET         0x04
#define REG_PHY_ID_2_OFFSET         0x06
#define REG_PHY_AUTO_NEG_OFFSET     0x08
#define REG_PHY_REMOTE_CAP_OFFSET   0x0A

#define REG_PHY_LINK_MD_1_OFFSET    0x00
#define REG_PHY_PHY_CTRL_1_OFFSET   0x02
#define REG_PHY_LINK_MD_2_OFFSET    0x04
#define REG_PHY_PHY_CTRL_2_OFFSET   0x06

#define PHY_SPECIAL_INTERVAL        \
    ( REG_PHY_LINK_MD_2_OFFSET - REG_PHY_LINK_MD_1_OFFSET )


#define REG_PORT_1_CTRL_BANK        48
#define REG_PORT_1_LINK_CTRL_BANK   49
#define REG_PORT_1_LINK_STATUS_BANK 49

#define REG_PORT_2_CTRL_BANK        50
#define REG_PORT_2_LINK_CTRL_BANK   51
#define REG_PORT_2_LINK_STATUS_BANK 51

#define REG_PORT_3_CTRL_BANK        52
#define REG_PORT_3_LINK_CTRL_BANK   53
#define REG_PORT_3_LINK_STATUS_BANK 53

/* Port# Control Register */
#define REG_PORT_CTRL_BANK          REG_PORT_1_CTRL_BANK

/* Port# Link Control Register */
#define REG_PORT_LINK_CTRL_BANK     REG_PORT_1_LINK_CTRL_BANK
#define REG_PORT_LINK_STATUS_BANK   REG_PORT_1_LINK_STATUS_BANK

#define PORT_BANK_INTERVAL          \
    ( REG_PORT_2_CTRL_BANK - REG_PORT_1_CTRL_BANK )

#define REG_P1CR1_BANK              REG_PORT_1_CTRL_BANK
#define REG_P1CR1_OFFSET            0x00

#define REG_P1CR2_BANK              REG_PORT_1_CTRL_BANK
#define REG_P1CR2_OFFSET            0x02

#define REG_P1VIDCR_BANK            REG_PORT_1_CTRL_BANK
#define REG_P1VIDCR_OFFSET          0x04

#define REG_P1CR3_BANK              REG_PORT_1_CTRL_BANK
#define REG_P1CR3_OFFSET            0x06

#define REG_P1IRCR_BANK             REG_PORT_1_CTRL_BANK
#define REG_P1IRCR_OFFSET           0x08

#define REG_P1ERCR_BANK             REG_PORT_1_CTRL_BANK
#define REG_P1ERCR_OFFSET           0x0A

#define REG_P1SCSLMD_BANK           REG_PORT_1_LINK_CTRL_BANK
#define REG_P1SCSLMD_OFFSET         0x00

#define REG_P1CR4_BANK              REG_PORT_1_LINK_CTRL_BANK
#define REG_P1CR4_OFFSET            0x02

#define REG_P1SR_BANK               REG_PORT_1_LINK_STATUS_BANK
#define REG_P1SR_OFFSET             0x04

#define REG_P2CR1_BANK              REG_PORT_2_CTRL_BANK
#define REG_P2CR1_OFFSET            0x00

#define REG_P2CR2_BANK              REG_PORT_2_CTRL_BANK
#define REG_P2CR2_OFFSET            0x02

#define REG_P2VIDCR_BANK            REG_PORT_2_CTRL_BANK
#define REG_P2VIDCR_OFFSET          0x04

#define REG_P2CR3_BANK              REG_PORT_2_CTRL_BANK
#define REG_P2CR3_OFFSET            0x06

#define REG_P2IRCR_BANK             REG_PORT_2_CTRL_BANK
#define REG_P2IRCR_OFFSET           0x08

#define REG_P2ERCR_BANK             REG_PORT_2_CTRL_BANK
#define REG_P2ERCR_OFFSET           0x0A

#define REG_P2SCSLMD_BANK           REG_PORT_2_LINK_CTRL_BANK
#define REG_P2SCSLMD_OFFSET         0x00

#define REG_P2CR4_BANK              REG_PORT_2_LINK_CTRL_BANK
#define REG_P2CR4_OFFSET            0x02

#define REG_P2SR_BANK               REG_PORT_2_LINK_STATUS_BANK
#define REG_P2SR_OFFSET             0x04

#define REG_P3CR1_BANK              REG_PORT_3_CTRL_BANK
#define REG_P3CR1_OFFSET            0x00

#define REG_P3CR2_BANK              REG_PORT_3_CTRL_BANK
#define REG_P3CR2_OFFSET            0x02

#define REG_P3VIDCR_BANK            REG_PORT_3_CTRL_BANK
#define REG_P3VIDCR_OFFSET          0x04

#define REG_P3CR3_BANK              REG_PORT_3_CTRL_BANK
#define REG_P3CR3_OFFSET            0x06

#define REG_P3IRCR_BANK             REG_PORT_3_CTRL_BANK
#define REG_P3IRCR_OFFSET           0x08

#define REG_P3ERCR_BANK             REG_PORT_3_CTRL_BANK
#define REG_P3ERCR_OFFSET           0x0A

#define REG_P3SCSLMD_BANK           REG_PORT_3_LINK_CTRL_BANK
#define REG_P3SCSLMD_OFFSET         0x00

#define REG_P3CR4_BANK              REG_PORT_3_LINK_CTRL_BANK
#define REG_P3CR4_OFFSET            0x02

#define REG_P3SR_BANK               REG_PORT_3_LINK_STATUS_BANK
#define REG_P3SR_OFFSET             0x04

#define REG_PORT_CTRL_1_OFFSET      0x00

#define REG_PORT_CTRL_2_OFFSET      0x02
#define REG_PORT_CTRL_2_HI_OFFSET   0x03

#define REG_PORT_CTRL_VID_OFFSET    0x04

#define REG_PORT_CTRL_3_OFFSET      0x06

#define REG_PORT_IN_RATE_OFFSET     0x08
#define REG_PORT_OUT_RATE_OFFSET    0x0A

#define REG_PORT_LINK_MD_RESULT     0x00
#define REG_PORT_LINK_MD_CTRL       0x01

#define REG_PORT_CTRL_4_OFFSET      0x02

#define REG_PORT_STATUS_OFFSET      0x04
#define REG_PORT_STATUS_HI_OFFSET   0x05

#define KSZ8842_HEADER_SIZE			4
