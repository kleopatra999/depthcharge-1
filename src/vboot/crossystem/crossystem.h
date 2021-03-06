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
 */

#ifndef __VBOOT_CROSSYSTEM_CROSSYSTEM_H__
#define __VBOOT_CROSSYSTEM_CROSSYSTEM_H__

// Setup the crossystem data. This should be done as late as possible to
// ensure the data used is up to date.
int crossystem_setup(void);

#endif /* __VBOOT_CROSSYSTEM_CROSSYSTEM_H__ */
