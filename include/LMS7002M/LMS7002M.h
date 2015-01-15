///
/// \file LMS7002M/LMS7002M.h
///
/// Main interface header for LMS7002M C driver.
///
/// \copyright
/// Copyright (c) 2014-2015 Fairwaves, Inc.
/// Copyright (c) 2014-2015 Rice University
/// SPDX-License-Identifier: Apache-2.0
/// http://www.apache.org/licenses/LICENSE-2.0
///

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <LMS7002M/LMS7002M_regs.h>

/*!
 * API export macro to declare linkage for driver functions:
 * We want to support including implementation headers for private use
 * or building this into a library -- so define LMS7002M_API accordingly.
 */
#ifndef LMS7002M_API
#define LMS7002M_API static inline
#endif

//! direction constants
typedef enum
{
    LMS_TX = 1,
    LMS_RX = 2,
} LMS7002M_dir_t;

//! port number constants
typedef enum
{
    LMS_PORT1 = 1,
    LMS_PORT2 = 2,
} LMS7002M_port_t;

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Function typedef for a function that implements SPI register transactions.
 * The handle is the same pointer that the handle passed into the driver instance.
 * Example: the data may be a pointer to a /dev/spiXX file descriptor
 *
 * The readback option supplied by the driver specifies whether or not
 * it requires the result of the spi transaction to be returned.
 * The implementor of this function can use the readback parameter
 * to implement non-blocking spi transactions (as an optimization).
 *
 * \param handle handle provided data
 * \param data the 32-bit write data
 * \param readback true to readback
 * \return the 32-bit readback data
 */
typedef uint32_t (*LMS7002M_spi_transact_t)(void *handle, const uint32_t data, const bool readback);

//! The opaque instance of the LMS7002M instance
struct LMS7002M_struct;

//! Helpful typedef for the LMS7002M driver instance
typedef struct LMS7002M_struct LMS7002M_t;

/*!
 * Create an instance of the LMS7002M driver.
 * This call does not reset or initialize the LMS7002M.
 * See LMS7002M_init(...) and LMS7002M_reset(...).
 *
 * \param transact the SPI transaction function
 * \param handle arbitrary handle data for transact
 * \return a new instance of the LMS7002M driver
 */
LMS7002M_API LMS7002M_t *LMS7002M_create(LMS7002M_spi_transact_t transact, void *handle);

/*!
 * Destroy an instance of the LMS7002M driver.
 * This call simply fees the instance data,
 * it does not shutdown or have any effects on the chip.
 * Use the LMS7002M_power_down(...) call before destroy().
 *
 * \param self an instance of the LMS7002M driver
 */
LMS7002M_API void LMS7002M_destroy(LMS7002M_t *self);

/*!
 * Perform a SPI write transaction on the given device.
 * This call can be used directly to access SPI registers,
 * rather than indirectly through the high level driver calls.
 * \param self an instance of the LMS7002M driver
 * \param addr the 16 bit register address
 * \param value the 16 bit register value
 */
LMS7002M_API void LMS7002M_spi_write(LMS7002M_t *self, const int addr, const int value);

/*!
 * Perform a SPI read transaction on the given device.
 * This call can be used directly to access SPI registers,
 * rather than indirectly through the high level driver calls.
 * \param self an instance of the LMS7002M driver
 * \param addr the 16 bit register address
 * \return the 16 bit register value
 */
LMS7002M_API int LMS7002M_spi_read(LMS7002M_t *self, const int addr);

/*!
 * Write a spi register using values from the regs structure.
 * \param self an instance of the LMS7002M driver
 * \param addr the 16 bit register address
 */
LMS7002M_API void LMS7002M_regs_spi_write(LMS7002M_t *self, const int addr);

/*!
 * Read a spi register, filling in the fields in the regs structure.
 * \param self an instance of the LMS7002M driver
 * \param addr the 16 bit register address
 */
LMS7002M_API void LMS7002M_regs_spi_read(LMS7002M_t *self, const int addr);

/*!
 * Get access to the registers structure and unpacked fields.
 * Use LMS7002M_regs_spi_write()/LMS7002M_regs_spi_read()
 * to sync the fields in this structure with the device.
 * \param self an instance of the LMS7002M driver
 * \return the pointer to the unpacked LMS7002M fields
 */
LMS7002M_API LMS7002M_regs_t *LMS7002M_regs(LMS7002M_t *self);

/*!
 * Perform all soft and hard resets available.
 * Call this first to put the LMS7002M into a known state.
 * \param self an instance of the LMS7002M driver
 */
LMS7002M_API void LMS7002M_reset(LMS7002M_t *self);

/*!
 * Put all available hardware into disable/power-down mode.
 * Call this last before destroying the LMS7002M instance.
 * \param self an instance of the LMS7002M driver
 */
LMS7002M_API void LMS7002M_power_down(LMS7002M_t *self);

/*!
 * Configure the muxing and clocking on a lime light port.
 * This sets the data mode and direction for the DIQ pins,
 * and selects the appropriate clock and stream muxes.
 * This call is not compatible with JESD207 operation.
 *
 * The mclkDiv must be 1 for no divider, or an even value.
 * Odd divider values besides 1 (bypass) are not allowed.
 *
 * \param self an instance of the LMS7002M driver
 * \param portNo the lime light data port 1 or 2
 * \param direction the direction LMS_TX or LMS_RX
 * \param mclkDiv the output clock divider ratio
 */
LMS7002M_API void LMS7002M_configure_lml_port(LMS7002M_t *self, const LMS7002M_port_t portNo, const LMS7002M_dir_t direction, const int mclkDiv);

/*!
 * Invert the feedback clock used with the transmit pins.
 * This call inverts both FCLK1 and FCLK2 (only one of which is used).
 * \param self an instance of the LMS7002M driver
 * \param invert true to invert the clock
 */
LMS7002M_API void LMS7002M_invert_fclk(LMS7002M_t *self, const bool invert);

/*!
 * Enable digital loopback inside the lime light.
 * This call also applies the tx fifo write clock to the rx fifo.
 * To undo the effect of this loopback, call LMS7002M_configure_lml_port().
 * \param self an instance of the LMS7002M driver
 */
LMS7002M_API void LMS7002M_setup_digital_loopback(LMS7002M_t *self);

/*!
 * Configure the ADC/DAC clocking given the reference and the desired rate.
 * This is a helper function that may make certain non-ideal assumptions,
 * for example this calculation will always make use of fractional-N tuning.
 * Also, this function does not directly set the clock muxing (see CGEN section).
 * \param self an instance of the LMS7002M driver
 * \param fref the reference clock frequency in Hz
 * \param fout the desired data clock frequency in Hz
 * \return 0 for success or error code on failure
 */
LMS7002M_API int LMS7002M_set_data_clock(LMS7002M_t *self, const double fref, const double fout);

#ifdef __cplusplus
}
#endif
