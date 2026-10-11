#include <assert.h>
#include <stdio.h>
#include "lin_master.h"

int main(void) {
    uint8_t data[3] = {1, 2, 3};
    lin_frame_t frame = {0};
    assert(lin_encode_frame(&frame, 0x12, data, 3, true) == LIN_OK);
    assert(lin_validate_frame(&frame, true) == LIN_OK);
    frame.pid ^= 0x40;
    assert(lin_validate_frame(&frame, true) == LIN_ERR_PID);

    lin_master_t master;
    lin_master_init(&master, 2000);
    assert(lin_master_add_slave(&master, (lin_slave_t){.node_id=0x10,.kind=LIN_SLAVE_BODY,.present=true,.response_delay_us=300}) == LIN_OK);
    assert(lin_master_add_slave(&master, (lin_slave_t){.node_id=0x11,.kind=LIN_SLAVE_THERMAL,.present=true,.response_delay_us=400}) == LIN_OK);
    assert(lin_master_add_slave(&master, (lin_slave_t){.node_id=0x12,.kind=LIN_SLAVE_DOOR,.present=false,.response_delay_us=400}) == LIN_OK);
    assert(lin_master_add_slot(&master, (lin_schedule_slot_t){0x10,LIN_SLOT_UNCONDITIONAL,10,3,0}) == LIN_OK);
    assert(lin_master_add_slot(&master, (lin_schedule_slot_t){0x11,LIN_SLOT_UNCONDITIONAL,10,3,1}) == LIN_OK);
    assert(lin_master_add_slot(&master, (lin_schedule_slot_t){0x12,LIN_SLOT_UNCONDITIONAL,10,3,2}) == LIN_OK);
    assert(lin_master_add_slot(&master, (lin_schedule_slot_t){0x3C,LIN_SLOT_DIAGNOSTIC,20,8,0}) == LIN_OK);

    lin_frame_t tx = {0}, rx = {0};
    bool received = false;
    assert(lin_master_tick(&master, &tx, &rx, &received) == LIN_OK && received);
    assert(lin_master_tick(&master, &tx, &rx, &received) == LIN_OK && received);
    assert(lin_master_tick(&master, &tx, &rx, &received) == LIN_ERR_TIMEOUT && !received);
    assert(master.stats.timeouts == 1 && master.stats.recovered_slots == 1);
    assert(lin_master_tick(&master, &tx, &rx, &received) == LIN_OK && !received);
    assert(tx.id == 0x3C);
    assert(lin_validate_frame(&tx, false) == LIN_OK);

    lin_frame_t diag = {0};
    assert(lin_master_diagnostic_request(&master, 0, 0x22, data, 2, &diag) == LIN_OK);
    assert(diag.id == 0x3C && diag.dlc == 5);
    assert(diag.data[0] == 0x10 && diag.data[1] == 3 && diag.data[2] == 0x22);
    assert(lin_validate_frame(&diag, false) == LIN_OK);
    puts("LIN tests passed.");
    return 0;
}
