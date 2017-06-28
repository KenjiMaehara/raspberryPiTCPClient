#ifndef _CC1100_H
#define _CC1100_H

#include "cc1100_globals.h"
#include <mytype.h>
typedef struct _cc1101_reg_
{
	u8 addr;
	u8 data;
}reg_t;

// other
void micro_delay(register unsigned int n);

// real functions
void cc1100_init(void (*mili)(u16 msec),
				u8 (*write_single)(u8 byte),
				u8 (*read_single)(void),
				u8 (*write)(u8* data, u16 len),
				void (*read)(u8* data, u16 len),
				void (*select)(void),
				void (*deselect)(void),
				u16 (*read_somi)(void));

void cc1100_reinit(void);

u8 cc1100_read_reg(u8 addr);

void cc1100_write_reg(u8 addr, u8 value);

void cc1100_fifo_put(u8* buffer, u16 length);

void cc1100_fifo_get(u8* buffer, u16 length);

u8 cc1100_read_status(u8 addr);

u8 cc1100_strobe_cmd(u8 cmd);

u8 cc1100_sleep_strobe_cmd(u8 cmd);

// status byte
#define cc1100_status() \
    cc1100_strobe_cmd(CC1100_STROBE_SNOP)

// commands macros

#define cc1100_cmd_reset() \
    cc1100_strobe_cmd(CC1100_STROBE_SRES)

#define cc1100_cmd_xoff() \
    cc1100_strobe_cmd(CC1100_STROBE_SXOFF)

void cc1100_cmd_calibrate(void);

#define cc1100_cmd_rx() \
    cc1100_strobe_cmd(CC1100_STROBE_SRX)

#define cc1100_cmd_tx() \
    cc1100_strobe_cmd(CC1100_STROBE_STX)

void cc1100_cmd_idle(void);

#define cc1100_cmd_wor() \
    cc1100_strobe_cmd(CC1100_STROBE_SWOR)

#define cc1100_cmd_pwd() \
    cc1100_strobe_cmd(CC1100_STROBE_SPWD)


/*add 20100113*/
#define cc1100_sleep_cmd_pwd() \
	cc1100_sleep_strobe_cmd(CC1100_STROBE_SPWD)


#define cc1100_cmd_flush_rx() \
    cc1100_strobe_cmd(CC1100_STROBE_SFRX)

#define cc1100_cmd_flush_tx() \
    cc1100_strobe_cmd(CC1100_STROBE_SFTX)

#define cc1100_cmd_reset_wor() \
    cc1100_strobe_cmd(CC1100_STROBE_SWORRST)

#define cc1100_cmd_nop() \
    cc1100_strobe_cmd(CC1100_STROBE_SNOP)


// Power Table Config
#define cc1100_cfg_patable(table, length) \
    cc1100_write_reg(CC1100_PATABLE_ADDR | CC1100_ACCESS_WRITE_BURST, (table)[0])

#define CC1100_GDOx_RX_FIFO           0x00  /* assert above threshold, deassert when below         */
#define CC1100_GDOx_RX_FIFO_EOP       0x01  /* assert above threshold or EOP, deassert when empty  */
#define CC1100_GDOx_TX_FIFO           0x02  /* assert above threshold, deassert when below         */
#define CC1100_GDOx_TX_THR_FULL       0x03  /* asserts TX FIFO full. De-asserts when below thr     */
#define CC1100_GDOx_RX_OVER           0x04  /* asserts when RX overflow, deassert when flushed     */
#define CC1100_GDOx_TX_UNDER          0x05  /* asserts when RX underflow, deassert when flushed    */
#define CC1100_GDOx_SYNC_WORD         0x06  /* assert SYNC sent/recv, deasserts on EOP             */
                                            /* In RX, de-assert on overflow or bad address         */
                                            /* In TX, de-assert on underflow                       */
#define CC1100_GDOx_RX_OK             0x07  /* assert when RX PKT with CRC ok, de-assert on 1byte  */
                                            /* read from RX Fifo                                   */
#define CC1100_GDOx_PREAMB_OK         0x08  /* assert when preamble quality reached : PQI/PQT ok   */
#define CC1100_GDOx_CCA               0x09  /* Clear channel assessment. High when RSSI level is   */
                                            /* below threshold (dependent on the current CCA_MODE) */
#define cc1100_cfg_gdo0(cfg) \
    cc1100_write_reg(CC1100_REG_IOCFG0, cfg)

#define cc1100_cfg_gdo2(cfg) \
    cc1100_write_reg(CC1100_REG_IOCFG2, cfg)

#define cc1100_cfg_fifo_thr(cfg) \
  cc1100_write_reg(CC1100_REG_FIFOTHR, ((cfg)&0x0F))

#define cc1100_cfg_packet_length(cfg) \
    cc1100_write_reg(CC1100_REG_PKTLEN, (cfg))

#define cc1100_cfg_pqt(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_PKTCTRL1); \
  reg = (reg & 0x1F) | (((cfg) << 5) & 0xE0); \
  cc1100_write_reg(CC1100_REG_PKTCTRL1, reg); \
} while (0) 

#define CC1100_CRC_AUTOFLUSH_ENABLE  0x1
#define CC1100_CRC_AUTOFLUSH_DISABLE 0x0

#define cc1100_cfg_crc_autoflush(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_PKTCTRL1); \
  reg = (reg & 0xF7) | (((cfg) << 3) & 0x08); \
  cc1100_write_reg(CC1100_REG_PKTCTRL1, reg); \
} while (0)

#define CC1100_APPEND_STATUS_ENABLE  0x1
#define CC1100_APPEND_STATUS_DISABLE 0x0

#define cc1100_cfg_append_status(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_PKTCTRL1); \
  reg = (reg & 0xFB) | (((cfg) << 2) & 0x04); \
  cc1100_write_reg(CC1100_REG_PKTCTRL1, reg); \
} while (0)

#define CC1100_ADDR_NO_CHECK                 0x0
#define CC1100_ADDR_CHECK_NO_BROADCAST       0x1
#define CC1100_ADDR_CHECK_BROADCAST_0        0x2
#define CC1100_ADDR_CHECK_NO_BROADCAST_0_255 0x3

#define cc1100_cfg_adr_check(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_PKTCTRL1); \
  reg = (reg & 0xFC) | (((cfg) << 0) & 0x03); \
  cc1100_write_reg(CC1100_REG_PKTCTRL1, reg); \
} while (0)

#define CC1100_DATA_WHITENING_ENABLE  0x1
#define CC1100_DATA_WHITENING_DISABLE 0x0

#define cc1100_cfg_white_data(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_PKTCTRL0); \
  reg = (reg & 0xBF) | (((cfg) << 6) & 0x40); \
  cc1100_write_reg(CC1100_REG_PKTCTRL0, reg); \
} while (0)


#define CC1100_CRC_CALCULATION_ENABLE  0x1
#define CC1100_CRC_CALCULATION_DISABLE 0x0

#define cc1100_cfg_crc_en(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_PKTCTRL0); \
  reg = (reg & 0xFB) | (((cfg) << 2) & 0x04); \
  cc1100_write_reg(CC1100_REG_PKTCTRL0, reg); \
} while (0)

#define CC1100_PACKET_LENGTH_FIXED    0x0
#define CC1100_PACKET_LENGTH_VARIABLE 0x1
#define CC1100_PACKET_LENGTH_INFINITE 0x2

#define cc1100_cfg_length_config(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_PKTCTRL0); \
  reg = (reg & 0xFC) | (((cfg) << 0) & 0x03); \
  cc1100_write_reg(CC1100_REG_PKTCTRL0, reg); \
} while (0)

#define cc1100_cfg_device_addr(cfg) \
    cc1100_write_reg(CC1100_REG_ADDR, (cfg))

#define cc1100_cfg_chan(cfg) \
    cc1100_write_reg(CC1100_REG_CHANNR, (cfg))

#define cc1100_cfg_freq_if(cfg) \
    cc1100_write_reg(CC1100_REG_FSCTRL1, ((cfg) & 0x1F))

#define cc1100_cfg_freq(cfg) do { \
  u8 reg; \
  reg = (u8) ( ((cfg)>>16)&0xFF ); \
  cc1100_write_reg(CC1100_REG_FREQ2, reg); \
  reg = (u8) ( ((cfg)>>8)&0xFF ); \
  cc1100_write_reg(CC1100_REG_FREQ1, reg); \
  reg = (u8) ( (cfg)&0xFF ); \
  cc1100_write_reg(CC1100_REG_FREQ0, reg); \
} while (0)

#define cc1100_cfg_chanbw_e(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MDMCFG4); \
  reg = (reg & 0x3F) | (((cfg) << 6) & 0xC0); \
  cc1100_write_reg(CC1100_REG_MDMCFG4, reg); \
} while (0)

#define cc1100_cfg_chanbw_m(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MDMCFG4); \
  reg = (reg & 0xCF) | (((cfg)<<4) & 0x30); \
  cc1100_write_reg(CC1100_REG_MDMCFG4, reg); \
} while (0)

#define cc1100_cfg_drate_e(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MDMCFG4); \
  reg = (reg & 0xF0) | (((cfg)) & 0x0F); \
  cc1100_write_reg(CC1100_REG_MDMCFG4, reg); \
} while (0)

#define cc1100_cfg_drate_m(cfg) \
  cc1100_write_reg(CC1100_REG_MDMCFG3, (cfg))

#define CC1100_MODULATION_2FSK 0x00
#define CC1100_MODULATION_GFSK 0x01
#define CC1100_MODULATION_ASK  0x03
#define CC1100_MODULATION_MSK  0x07

#define cc1100_cfg_mod_format(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MDMCFG2); \
  reg = (reg & 0x8F) | (((cfg) << 4) & 0x70); \
  cc1100_write_reg(CC1100_REG_MDMCFG2, reg); \
} while (0)

#define CC1100_MANCHESTER_ENABLE  0x1
#define CC1100_MANCHESTER_DISABLE 0x0

#define cc1100_cfg_manchester_en(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MDMCFG2); \
  reg = (reg & 0xF7) | (((cfg) << 3) & 0x08); \
  cc1100_write_reg(CC1100_REG_MDMCFG2, reg); \
} while (0)


#define CC1100_SYNCMODE_NO_PREAMB      0x0
#define CC1100_SYNCMODE_15_16          0x1
#define CC1100_SYNCMODE_16_16          0x2
#define CC1100_SYNCMODE_30_32          0x3
#define CC1100_SYNCMODE_NO_PREAMB_CS   0x4
#define CC1100_SYNCMODE_15_16_CS       0x5
#define CC1100_SYNCMODE_16_16_CS       0x6
#define CC1100_SYNCMODE_30_32_CS       0x7

#define cc1100_cfg_sync_mode(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MDMCFG2); \
  reg = (reg & 0xF8) | (((cfg) << 0) & 0x07); \
  cc1100_write_reg(CC1100_REG_MDMCFG2, reg); \
} while (0)

#define CC1100_FEC_ENABLE  0x1
#define CC1100_FEC_DISABLE 0x0

#define cc1100_cfg_fec_en(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MDMCFG1); \
  reg = (reg & 0x7F) | (((cfg) << 7) & 0x80); \
  cc1100_write_reg(CC1100_REG_MDMCFG1, reg); \
} while (0)

#define cc1100_cfg_num_preamble(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MDMCFG1); \
  reg = (reg & 0x8F) | (((cfg) << 4) & 0x70); \
  cc1100_write_reg(CC1100_REG_MDMCFG1, reg); \
} while (0)

#define cc1100_cfg_chanspc_e(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MDMCFG1); \
  reg = (reg & 0xFE) | (((cfg) << 0) & 0x01); \
  cc1100_write_reg(CC1100_REG_MDMCFG1, reg); \
} while (0)

#define cc1100_cfg_chanspc_m(cfg) \
    cc1100_write_reg(CC1100_REG_MDMCFG0, (cfg))

#define CC1100_RX_TIME_RSSI_ENABLE  0x1
#define CC1100_RX_TIME_RSSI_DISABLE 0x0

#define cc1100_cfg_rx_time_rssi(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MCSM2); \
  reg = (reg & 0xEF) | (((cfg) << 4) & 0x10); \
  cc1100_write_reg(CC1100_REG_MCSM2, reg); \
} while (0)

#define cc1100_cfg_rx_time(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MCSM2); \
  reg = (reg & 0xF8) | (((cfg) << 0) & 0x07); \
  cc1100_write_reg(CC1100_REG_MCSM2, reg); \
} while (0)

#define CC1100_CCA_MODE_ALWAYS      0x0
#define CC1100_CCA_MODE_RSSI        0x1
#define CC1100_CCA_MODE_PKT_RX      0x2
#define CC1100_CCA_MODE_RSSI_PKT_RX 0x3

#define cc1100_cfg_cca_mode(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MCSM1); \
  reg = (reg & 0xCF) | (((cfg) << 4) & 0x30); \
  cc1100_write_reg(CC1100_REG_MCSM1, reg); \
} while (0)

#define CC1100_RXOFF_MODE_IDLE     0x00
#define CC1100_RXOFF_MODE_FSTXON   0x01 /* freq synth on, ready to Tx */
#define CC1100_RXOFF_MODE_TX       0x02 
#define CC1100_RXOFF_MODE_STAY_RX  0x03

#define cc1100_cfg_rxoff_mode(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MCSM1); \
  reg = (reg & 0xF3) | (((cfg) << 2) & 0x0C); \
  cc1100_write_reg(CC1100_REG_MCSM1, reg); \
} while (0)

#define CC1100_TXOFF_MODE_IDLE     0x00
#define CC1100_TXOFF_MODE_FSTXON   0x01 /* freq synth on, ready to Tx */
#define CC1100_TXOFF_MODE_STAY_TX  0x02
#define CC1100_TXOFF_MODE_RX       0x03

#define cc1100_cfg_txoff_mode(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MCSM1); \
  reg = (reg & 0xFC) | (((cfg) << 0) & 0x03); \
  cc1100_write_reg(CC1100_REG_MCSM1, reg); \
} while (0)


#define CC1100_AUTOCAL_NEVER             0x00
#define CC1100_AUTOCAL_IDLE_TO_TX_RX     0x01
#define CC1100_AUTOCAL_TX_RX_TO_IDLE     0x02
#define CC1100_AUTOCAL_4TH_TX_RX_TO_IDLE 0x03

#define cc1100_cfg_fs_autocal(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_MCSM0); \
  reg = (reg & 0xCF) | (((cfg) << 4) & 0x30); \
  cc1100_write_reg(CC1100_REG_MCSM0, reg); \
} while (0)

#define cc1100_cfg_carrier_sense_rel_thr(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_AGCCTRL1); \
  reg = (reg & 0xCF) | (((cfg) << 4) & 0x30); \
  cc1100_write_reg(CC1100_REG_AGCCTRL1, reg); \
} while (0)

#define cc1100_cfg_carrier_sense_abs_thr(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_AGCCTRL1); \
  reg = (reg & 0xF0) | (((cfg) << 0) & 0x0F); \
  cc1100_write_reg(CC1100_REG_AGCCTRL1, reg); \
} while (0)

#define cc1100_cfg_event0(cfg) do { \
  u8 reg; \
  reg = (u8)((cfg >> 8) & 0xFF); \
  cc1100_write_reg(CC1100_REG_WOREVT1, reg); \
  reg = (u8)((cfg) & 0xFF); \
  cc1100_write_reg(CC1100_REG_WOREVT0, reg); \
} while (0)

#define CC1100_RC_OSC_ENABLE  0x0
#define CC1100_RC_OSC_DISABLE 0x1

#define cc1100_cfg_rc_pd(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_WORCTRL); \
  reg = (reg & 0x7F) | (((cfg) << 7) & 0x80); \
  cc1100_write_reg(CC1100_REG_WORCTRL, reg); \
} while (0)

#define cc1100_cfg_event1(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_WORCTRL); \
  reg = (reg & 0x8F) | (((cfg) << 4) & 0x70); \
  cc1100_write_reg(CC1100_REG_WORCTRL, reg); \
} while (0)

#define cc1100_cfg_wor_res(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_WORCTRL); \
  reg = (reg & 0xFC) | (((cfg) << 0) & 0x03); \
  cc1100_write_reg(CC1100_REG_WORCTRL, reg); \
} while (0)

#define cc1100_cfg_pa_power(cfg) do { \
  u8 reg; \
  reg = cc1100_read_reg(CC1100_REG_FREND0); \
  reg = (reg & 0xF8) | (((cfg) << 0) & 0x07); \
  cc1100_write_reg(CC1100_REG_FREND0, reg); \
} while (0)

// Status Registers access
#define cc1100_status_crc_lqi() \
    cc1100_read_status(CC1100_REG_LQI)

#define cc1100_status_rssi() \
    cc1100_read_status(CC1100_REG_RSSI)

#define cc1100_status_marcstate() \
    cc1100_read_status(CC1100_REG_MARCSTATE)

#define cc1100_status_wortime1() \
    cc1100_read_status(CC1100_REG_WORTIME1)

#define cc1100_status_wortime0() \
    cc1100_read_status(CC1100_REG_WORTIME0)

#define cc1100_status_pktstatus() \
    cc1100_read_status(CC1100_REG_PKTSTATUS)

#define cc1100_status_txbytes() \
    cc1100_read_status(CC1100_REG_TXBYTES)

#define cc1100_status_rxbytes() \
    cc1100_read_status(CC1100_REG_RXBYTES)


// GDOx int config & access


#define cc1100_gdo0_int_enable() \
    GDO0_INT_ENABLE()

#define cc1100_gdo0_int_disable() \
    GDO0_INT_DISABLE()

#define cc1100_gdo0_int_clear() \
    GDO0_INT_CLEAR()

#define cc1100_gdo0_int_set_falling_edge() \
    GDO0_INT_SET_FALLING()

#define cc1100_gdo0_int_set_rising_edge() \
    GDO0_INT_SET_RISING()

#define cc1100_gdo0_read() \
    GDO0_READ()

void cc1100_gdo0_register_callback(u16 (*cb)(void));

#define cc1100_gdo2_int_enable() \
    GDO2_INT_ENABLE()

#define cc1100_gdo2_int_disable() \
    GDO2_INT_DISABLE()

#define cc1100_gdo2_int_clear() \
    GDO2_INT_CLEAR()

#define cc1100_gdo2_int_set_falling_edge() \
    GDO2_INT_SET_FALLING()

#define cc1100_gdo2_int_set_rising_edge() \
    GDO2_INT_SET_RISING()

#define cc1100_gdo2_read() \
    GDO2_READ()


//void cc1100_gdo2_register_callback(u16 (*cb)(void));



// my function
void cc1101_init_reg(void);
void cc1101_rtx_reg_set(u8 rtx);
void cc1101_8PATABLE_write_reg(void);
u8 cc1101_rx(u8 * data,u8 * rssi, u8 * lqi);

#endif

