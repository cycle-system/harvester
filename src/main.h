/*!
 * @file main.h
 * @brief Header file for main file in ChibiosRT.
 *
 * @{
 */

#include "hal.h"
#include "stdlib.h"

/**
 * @brief FSM States
 */
enum fsm_states {
    st_init = 0,
    st_measure = 1,
    st_send_to_cu = 2,
    st_wait_ack = 3,
    st_store_in_bd = 4,
    st_wait_bd_confirmation = 5
};
