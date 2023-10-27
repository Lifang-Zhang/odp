/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2023 Nokia
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <odp/api/ml.h>
#include <odp/api/ml_quantize.h>

int odp_ml_capability(odp_ml_capability_t *capa)
{
	memset(capa, 0, sizeof(odp_ml_capability_t));

	return -1;
}
