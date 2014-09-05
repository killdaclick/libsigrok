/*
 * This file is part of the libsigrok project.
 *
 * Copyright (C) 2014 Bert Vermeulen <bert@biot.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include "protocol.h"

#define CH_IDX(x) (1 << x)

const char *pps_vendors[][2] = {
	{ "RIGOL TECHNOLOGIES", "Rigol" },
};

const char *get_vendor(const char *raw_vendor)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(pps_vendors); i++) {
		if (!strcasecmp(raw_vendor, pps_vendors[i][0]))
			return pps_vendors[i][1];
	}

	return raw_vendor;
}

/* Rigol DP800 series */
static const int32_t rigol_dp800_devopts[] = {
	SR_CONF_POWER_SUPPLY,
	SR_CONF_CONTINUOUS,
	SR_CONF_OVER_TEMPERATURE_PROTECTION,
};

static const int32_t rigol_dp800_devopts_cg[] = {
	SR_CONF_OUTPUT_REGULATION,
	SR_CONF_OVER_VOLTAGE_PROTECTION_ENABLED,
	SR_CONF_OVER_VOLTAGE_PROTECTION_ACTIVE,
	SR_CONF_OVER_VOLTAGE_PROTECTION_THRESHOLD,
	SR_CONF_OVER_CURRENT_PROTECTION_ENABLED,
	SR_CONF_OVER_CURRENT_PROTECTION_ACTIVE,
	SR_CONF_OVER_CURRENT_PROTECTION_THRESHOLD,
	SR_CONF_OUTPUT_VOLTAGE,
	SR_CONF_OUTPUT_VOLTAGE_MAX,
	SR_CONF_OUTPUT_CURRENT,
	SR_CONF_OUTPUT_CURRENT_MAX,
	SR_CONF_OUTPUT_ENABLED,
};

struct channel_spec rigol_dp831_ch[] = {
	{ "1", { 0, 8, 0.001 }, { 0, 5, 0.0003 } },
	{ "2", { 0, 30, 0.001 }, { 0, 2, 0.0001 } },
	{ "3", { 0, -30, 0.001 }, { 0, 2, 0.0001 } },
};

struct channel_spec rigol_dp832_ch[] = {
	{ "1", { 0, 30, 0.001 }, { 0, 3, 0.001 } },
	{ "2", { 0, 30, 0.001 }, { 0, 3, 0.001 } },
	{ "3", { 0, 5, 0.001 }, { 0, 3, 0.001 } },
};

struct channel_group_spec rigol_dp800_cg[] = {
	{ "1", CH_IDX(0), PPS_OVP | PPS_OCP },
	{ "2", CH_IDX(1), PPS_OVP | PPS_OCP },
	{ "3", CH_IDX(2), PPS_OVP | PPS_OCP },
};

struct scpi_command rigol_dp800_cmd[] = {
	{ SCPI_CMD_KEY_UNLOCK, "SYST:KLOCK OFF" },
	{ SCPI_CMD_GET_MEAS_VOLTAGE, ":MEAS:VOLT? CH%s" },
	{ SCPI_CMD_GET_MEAS_CURRENT, ":MEAS:CURR? CH%s" },
	{ SCPI_CMD_GET_MEAS_POWER, ":MEAS:POWE? CH%s" },
	{ SCPI_CMD_GET_VOLTAGE_MAX, ":SOUR%s:VOLT?" },
	{ SCPI_CMD_SET_VOLTAGE_MAX, ":SOUR%s:VOLT %.6f" },
	{ SCPI_CMD_GET_CURRENT_MAX, ":SOUR%s:CURR?" },
	{ SCPI_CMD_SET_CURRENT_MAX, ":SOUR%s:CURR %.6f" },
	{ SCPI_CMD_GET_OUTPUT_ENABLED, ":OUTP? CH%s" },
	{ SCPI_CMD_SET_OUTPUT_ENABLED, ":OUTP CH%s,%s" },
	{ SCPI_CMD_GET_OUTPUT_REGULATION, ":OUTP:MODE? CH%s" },
	{ SCPI_CMD_GET_OVER_TEMPERATURE_PROTECTION, ":SYST:OTP?" },
	{ SCPI_CMD_SET_OVER_TEMPERATURE_PROTECTION, ":SYST:OTP %s" },
	{ SCPI_CMD_GET_OVER_VOLTAGE_PROTECTION_ENABLED, ":OUTP:OVP? CH%s" },
	{ SCPI_CMD_SET_OVER_VOLTAGE_PROTECTION_ENABLED, ":OUTP:OVP CH%s,%s" },
	{ SCPI_CMD_GET_OVER_VOLTAGE_PROTECTION_ACTIVE, ":OUTP:OVP:QUES? CH%s" },
	{ SCPI_CMD_GET_OVER_VOLTAGE_PROTECTION_THRESHOLD, ":OUTP:OVP:VAL? CH%s" },
	{ SCPI_CMD_SET_OVER_VOLTAGE_PROTECTION_THRESHOLD, ":OUTP:OVP:VAL CH%s,%.6f" },
	{ SCPI_CMD_GET_OVER_CURRENT_PROTECTION_ENABLED, ":OUTP:OCP? CH%s" },
	{ SCPI_CMD_SET_OVER_CURRENT_PROTECTION_ENABLED, ":OUTP:OCP CH%s,%s" },
	{ SCPI_CMD_GET_OVER_CURRENT_PROTECTION_ACTIVE, ":OUTP:OCP:QUES? CH%s" },
	{ SCPI_CMD_GET_OVER_CURRENT_PROTECTION_THRESHOLD, ":OUTP:OCP:VAL? CH%s" },
	{ SCPI_CMD_SET_OVER_CURRENT_PROTECTION_THRESHOLD, ":OUTP:OCP:VAL CH%s,%.6f" },
};

SR_PRIV const struct scpi_pps pps_profiles[] = {
	/* Rigol DP800 series */
	{ "Rigol", "^DP831A$", PPS_OTP,
		ARRAY_AND_SIZE(rigol_dp800_devopts),
		ARRAY_AND_SIZE(rigol_dp800_devopts_cg),
		ARRAY_AND_SIZE(rigol_dp831_ch),
		ARRAY_AND_SIZE(rigol_dp800_cg),
		ARRAY_AND_SIZE(rigol_dp800_cmd),
	},
	{ "Rigol", "^(DP832|DP832A)$", PPS_OTP,
		ARRAY_AND_SIZE(rigol_dp800_devopts),
		ARRAY_AND_SIZE(rigol_dp800_devopts_cg),
		ARRAY_AND_SIZE(rigol_dp832_ch),
		ARRAY_AND_SIZE(rigol_dp800_cg),
		ARRAY_AND_SIZE(rigol_dp800_cmd),
	},
};
SR_PRIV unsigned int num_pps_profiles = ARRAY_SIZE(pps_profiles);

