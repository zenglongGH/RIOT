/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     pkg_semtech-loramac
 * @file
 * @brief       Compatibility functions for controlling the radio driver
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Francisco Molina <francisco.molina@inria.cl>
 * @}
 */

#include "net/lora.h"
#include "net/netdev.h"

#include "sx127x.h"
#include "sx127x_internal.h"
#include "sx127x_netdev.h"

#include "semtech-loramac/board.h"

#include "radio/radio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

extern sx127x_t sx127x;

#define LORAMAC_RX_WINDOW_DURATION  (600UL * US_PER_MS)

/*
 * Radio driver functions implementation wrappers, the netdev2 object
 * is known within the scope of the function
 */
void SX127XInit(RadioEvents_t *events)
{
    (void) events;
    sx127x_init(&sx127x);
}

RadioState_t SX127XGetStatus(void)
{
    return (RadioState_t)sx127x_get_state(&sx127x);
}

void SX127XSetModem(RadioModems_t modem)
{
    sx127x_set_modem(&sx127x, (uint8_t)modem);
}

void SX127XSetChannel(uint32_t freq)
{
    sx127x_set_channel(&sx127x, freq);
}

bool SX127XIsChannelFree(RadioModems_t modem, uint32_t freq,
                         int16_t rssiThresh, uint32_t maxCarrierSenseTime )
{
    (void) modem;
    (void) maxCarrierSenseTime;
    return sx127x_is_channel_free(&sx127x, freq, rssiThresh);
}

uint32_t SX127XRandom(void)
{
    return sx127x_random(&sx127x);
}

void SX127XSetRxConfig(RadioModems_t modem, uint32_t bandwidth,
                       uint32_t spreading_factor, uint8_t coding_rate,
                       uint32_t bandwidthAfc, uint16_t preambleLen,
                       uint16_t symbTimeout, bool fixLen,
                       uint8_t payloadLen,
                       bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                       bool iqInverted, bool rxContinuous)
{
    (void) bandwidthAfc;
    (void) symbTimeout;
    (void) fixLen;
    sx127x_set_modem(&sx127x, modem);
    sx127x_set_bandwidth(&sx127x, bandwidth);
    sx127x_set_spreading_factor(&sx127x, spreading_factor);
    sx127x_set_coding_rate(&sx127x, coding_rate);
    sx127x_set_preamble_length(&sx127x, preambleLen);
    sx127x_set_fixed_header_len_mode(&sx127x, false);
    sx127x_set_payload_length(&sx127x, payloadLen);
    sx127x_set_crc(&sx127x, crcOn);
    sx127x_set_freq_hop(&sx127x, freqHopOn);
    sx127x_set_hop_period(&sx127x, hopPeriod);
    sx127x_set_iq_invert(&sx127x, iqInverted);
    sx127x_set_rx_timeout(&sx127x, LORAMAC_RX_WINDOW_DURATION);
    sx127x_set_rx_single(&sx127x, !rxContinuous);
    sx127x_set_rx(&sx127x);
}

void SX127XSetTxConfig(RadioModems_t modem, int8_t power, uint32_t fdev,
                       uint32_t bandwidth, uint32_t spreading_factor,
                       uint8_t coding_rate, uint16_t preambleLen,
                       bool fixLen, bool crcOn, bool freqHopOn,
                       uint8_t hopPeriod, bool iqInverted, uint32_t timeout)
{
    (void) fdev;
    (void) fixLen;
    sx127x_set_modem(&sx127x, modem);
    sx127x_set_freq_hop(&sx127x, freqHopOn);
    sx127x_set_bandwidth(&sx127x, bandwidth);
    sx127x_set_coding_rate(&sx127x, coding_rate);
    sx127x_set_spreading_factor(&sx127x, spreading_factor);
    sx127x_set_crc(&sx127x, crcOn);
    sx127x_set_freq_hop(&sx127x, freqHopOn);
    sx127x_set_hop_period(&sx127x, hopPeriod);
    sx127x_set_fixed_header_len_mode(&sx127x, false);
    sx127x_set_iq_invert(&sx127x, iqInverted);
    sx127x_set_payload_length(&sx127x, 0);
    sx127x_set_tx_power(&sx127x, power);
    sx127x_set_preamble_length(&sx127x, preambleLen);
    sx127x_set_rx_single(&sx127x, false);
    sx127x_set_tx_timeout(&sx127x, timeout * US_PER_MS); /* base unit us, LoRaMAC ms */
}

uint32_t SX127XGetTimeOnAir(RadioModems_t modem, uint8_t pktLen)
{
    (void) modem;
    return sx127x_get_time_on_air(&sx127x, pktLen);
}

void SX127XSend(uint8_t *buffer, uint8_t size)
{
    netdev_t *dev = (netdev_t *)&sx127x;
    struct iovec vec[1];
    vec[0].iov_base = buffer;
    vec[0].iov_len = size;
    dev->driver->send(dev, vec, 1);
}

void SX127XSetSleep(void)
{
    sx127x_set_sleep(&sx127x);
}

void SX127XSetStby(void)
{
    sx127x_set_standby(&sx127x);
}

void SX127XSetRx(uint32_t timeout)
{
    (void) timeout;
    sx127x_set_rx(&sx127x);
}

void SX127XStartCad(void)
{
    sx127x_start_cad(&sx127x);
}

int16_t SX127XReadRssi(RadioModems_t modem)
{
    sx127x_set_modem(&sx127x, (uint8_t)modem);
    return sx127x_read_rssi(&sx127x);
}

void SX127XWrite(uint8_t addr, uint8_t data)
{
    sx127x_reg_write(&sx127x, addr, data);
}

uint8_t SX127XRead(uint8_t addr)
{
    return sx127x_reg_read(&sx127x, addr);
}

void SX127XWriteBuffer(uint8_t addr, uint8_t *buffer, uint8_t size)
{
    sx127x_reg_write_burst(&sx127x, addr, buffer, size);
}

void SX127XReadBuffer(uint8_t addr, uint8_t *buffer, uint8_t size)
{
    sx127x_reg_read_burst(&sx127x, addr, buffer, size);
}

void SX127XSetMaxPayloadLength(RadioModems_t modem, uint8_t max)
{
    (void) modem;
    sx127x_set_max_payload_len(&sx127x, max);
}

bool SX127XCheckRfFrequency(uint32_t frequency)
{
    (void) frequency;
    /* Implement check. Currently all frequencies are supported */
    return true;
}

void SX127XSetTxContinuousWave(uint32_t freq, int8_t power, uint16_t time)
{
    (void) freq;
    (void) power;
    (void) time;
    /* TODO */
}

void SX127XSetPublicNetwork(bool enable)
{
    if (enable) {
        /* Use public network syncword */
        sx127x_set_syncword(&sx127x, LORA_SYNCWORD_PUBLIC);
    }
    else {
        /* Use private network syncword */
        sx127x_set_syncword(&sx127x, LORA_SYNCWORD_PRIVATE);
    }
}

/**
 * LoRa function callbacks
 */
const struct Radio_s Radio =
{
    SX127XInit,
    SX127XGetStatus,
    SX127XSetModem,
    SX127XSetChannel,
    SX127XIsChannelFree,
    SX127XRandom,
    SX127XSetRxConfig,
    SX127XSetTxConfig,
    SX127XCheckRfFrequency,
    SX127XGetTimeOnAir,
    SX127XSend,
    SX127XSetSleep,
    SX127XSetStby,
    SX127XSetRx,
    SX127XStartCad,
    SX127XSetTxContinuousWave,
    SX127XReadRssi,
    SX127XWrite,
    SX127XRead,
    SX127XWriteBuffer,
    SX127XReadBuffer,
    SX127XSetMaxPayloadLength,
    SX127XSetPublicNetwork
};
