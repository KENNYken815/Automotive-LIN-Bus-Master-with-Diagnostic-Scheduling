#include "lin_master.h"
#include "lin_sim.h"
#include <string.h>

void lin_master_init(lin_master_t *m, uint32_t timeout) {
    if (!m) return;
    memset(m, 0, sizeof(*m));
    m->timeout_us = timeout;
    lin_schedule_init(&m->schedule);
}

lin_status_t lin_master_add_slave(lin_master_t *m, lin_slave_t slave) {
    if (!m) return LIN_ERR_ARGUMENT;
    if (m->slave_count >= LIN_MAX_SLAVES) return LIN_ERR_CAPACITY;
    m->slaves[m->slave_count++] = slave;
    return LIN_OK;
}

lin_status_t lin_master_add_slot(lin_master_t *m, lin_schedule_slot_t slot) {
    if (!m) return LIN_ERR_ARGUMENT;
    if (slot.type == LIN_SLOT_DIAGNOSTIC) {
        if (slot.frame_id != 0x3C && slot.frame_id != 0x3D) return LIN_ERR_ARGUMENT;
    } else if (slot.slave_index >= LIN_MAX_SLAVES) return LIN_ERR_ARGUMENT;
    return lin_schedule_add(&m->schedule, slot);
}

lin_status_t lin_master_tick(lin_master_t *m, lin_frame_t *tx,
                             lin_frame_t *rx, bool *received) {
    if (!m || !tx || !rx || !received) return LIN_ERR_ARGUMENT;
    const lin_schedule_slot_t *slot = lin_schedule_current(&m->schedule);
    if (!slot) return LIN_ERR_NO_SLOT;

    m->now_us += (uint64_t)slot->delay_ms * 1000ULL;
    uint8_t command[LIN_MAX_DATA] = {0};
    lin_status_t status;

    if (slot->type == LIN_SLOT_DIAGNOSTIC) {
        /* Example diagnostic request: NAD, PCI length, SID, and DID. */
        command[0] = 0x7E; command[1] = 0x03;
        command[2] = 0x22; command[3] = 0xF1; command[4] = 0x90;
        status = lin_encode_frame(tx, slot->frame_id, command, 5, false);
        m->stats.diagnostic_requests++;
    } else {
        status = lin_encode_frame(tx, slot->frame_id, command, 0, true);
    }
    if (status != LIN_OK) return status;

    tx->timestamp_us = m->now_us;
    m->stats.frames_sent++;
    *received = false;

    if (slot->type == LIN_SLOT_DIAGNOSTIC) {
        /* Frame construction is modeled; no diagnostic slave is simulated. */
        status = LIN_OK;
    } else if (slot->slave_index < m->slave_count) {
        lin_slave_t *slave = &m->slaves[slot->slave_index];
        lin_sim_respond(slave, tx, rx, received, m->now_us);
        if (*received) {
            if (rx->timestamp_us < m->now_us ||
                rx->timestamp_us - m->now_us > m->timeout_us) {
                *received = false;
                m->stats.timeouts++;
                status = LIN_ERR_TIMEOUT;
            } else {
                status = lin_validate_frame(rx, true);
                if (status == LIN_OK && rx->dlc != slot->expected_dlc)
                    status = LIN_ERR_CHECKSUM;
                if (status == LIN_OK) m->stats.frames_received++;
                else if (status == LIN_ERR_CHECKSUM) m->stats.checksum_errors++;
                else if (status == LIN_ERR_PID) m->stats.pid_errors++;
            }
        } else {
            m->stats.timeouts++;
            status = LIN_ERR_TIMEOUT;
        }
    } else {
        m->stats.timeouts++;
        status = LIN_ERR_TIMEOUT;
    }

    if (status == LIN_ERR_TIMEOUT) m->stats.recovered_slots++;
    lin_schedule_advance(&m->schedule, m->now_us);
    return status;
}

lin_status_t lin_master_diagnostic_request(lin_master_t *m, uint8_t slave_index,
                                           uint8_t service, const uint8_t *data,
                                           uint8_t length, lin_frame_t *tx) {
    if (!m || !tx || slave_index >= m->slave_count ||
        length > 5 || (!data && length)) return LIN_ERR_ARGUMENT;

    /* LIN diagnostic payload: NAD, PCI length, SID, then service data. */
    uint8_t payload[LIN_MAX_DATA] = {0};
    payload[0] = m->slaves[slave_index].node_id;
    payload[1] = (uint8_t)(length + 1u);
    payload[2] = service;
    for (uint8_t i = 0; i < length; ++i) payload[3u + i] = data[i];

    lin_status_t status = lin_encode_frame(tx, 0x3C, payload,
                                           (uint8_t)(length + 3u), false);
    if (status == LIN_OK) m->stats.diagnostic_requests++;
    return status;
}
