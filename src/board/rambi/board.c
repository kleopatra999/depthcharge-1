/*
 * Copyright 2013 Google Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <pci.h>

#include "base/init_funcs.h"
#include "drivers/ec/cros/lpc.h"
#include "drivers/flash/flash.h"
#include "drivers/flash/memmapped.h"
#include "drivers/gpio/baytrail.h"
#include "drivers/gpio/sysinfo.h"
#include "drivers/power/pch.h"
#include "drivers/storage/ahci.h"
#include "drivers/storage/blockdev.h"
#include "drivers/storage/sdhci.h"
#include "drivers/tpm/lpc.h"
#include "vboot/util/flag.h"


static int board_setup(void)
{
        if (sysinfo_install_flags())
		return 1;

	/* ECRW GPIO: SCGPIO59 */
	PchGpio *ec_in_rw = new_baytrail_gpio_input(59 / 32,
						    59 % 32);
	if (!ec_in_rw || flag_install(FLAG_ECINRW, &ec_in_rw->ops))
		return 1;

	CrosEcLpcBus *cros_ec_lpc_bus = new_cros_ec_lpc_bus();
	if (!cros_ec_lpc_bus)
		return 1;
	cros_ec_set_bus(&cros_ec_lpc_bus->ops);

	MemMappedFlash *flash = new_mem_mapped_flash(0xff800000, 0x800000);
	if (!flash || flash_set_ops(&flash->ops))
		return 1;

	/* TODO(shawnn): Init I2S audio codec here for FW beep */

	/* TODO(shawnn): Remove AHCI controller once we have boards
	 * with SATA removed */
	AhciCtrlr *ahci = new_ahci_ctrlr(PCI_DEV(0, 0x13, 0));
	if (!ahci)
		return 1;
	list_insert_after(&ahci->ctrlr.list_node, &fixed_block_dev_controllers);

	if (power_set_ops(&pch_power_ops))
		return 1;

	LpcTpm *tpm = new_lpc_tpm((void *)0xfed40000);
	if (!tpm || tpm_set_ops(&tpm->ops))
		return 1;

	/*
	 * For now initting memory structures for port zero (eMMC) only.
	 * Device numbers of the other two SCC ports are 17 and 18.
	 *
	 * Clock frequencies for the eMMC port are 400 KHz min (used during
	 * initialization) and 200 MHz max. Firmware does not run at 200 MHz
	 * (aka HS mode), it is limited to 52 MHz.
	 */
	SdhciHost *emmc = new_pci_sdhci_host(PCI_DEV(0, 23, 0), 0,
					     400 * 1000, 200 * 1000 * 1000);

	if (emmc)
		list_insert_after(&emmc->mmc_ctrlr.ctrlr.list_node,
				  &fixed_block_dev_controllers);

	return 0;
}

INIT_FUNC(board_setup);