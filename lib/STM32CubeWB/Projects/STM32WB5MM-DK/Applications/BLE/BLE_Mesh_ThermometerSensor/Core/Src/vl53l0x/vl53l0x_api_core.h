/**
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef _VL53L0X_API_CORE_H_
#define _VL53L0X_API_CORE_H_

#include "vl53l0x_def.h"
#include "vl53l0x_platform.h"


#ifdef __cplusplus
extern "C" {
#endif


VL53L0X_Error VL53L0X_reverse_bytes(uint8_t *data, uint32_t size);

VL53L0X_Error VL53L0X_measurement_poll_for_completion(VL53L0X_DEV Dev);

uint8_t VL53L0X_encode_vcsel_period(uint8_t vcsel_period_pclks);

uint8_t VL53L0X_decode_vcsel_period(uint8_t vcsel_period_reg);

uint32_t VL53L0X_isqrt(uint32_t num);

uint32_t VL53L0X_quadrature_sum(uint32_t a, uint32_t b);

VL53L0X_Error VL53L0X_get_info_from_device(VL53L0X_DEV Dev, uint8_t option);

VL53L0X_Error VL53L0X_set_vcsel_pulse_period(VL53L0X_DEV Dev,
	VL53L0X_VcselPeriod VcselPeriodType, uint8_t VCSELPulsePeriodPCLK);

VL53L0X_Error VL53L0X_get_vcsel_pulse_period(VL53L0X_DEV Dev,
	VL53L0X_VcselPeriod VcselPeriodType, uint8_t *pVCSELPulsePeriodPCLK);

uint32_t VL53L0X_decode_timeout(uint16_t encoded_timeout);

VL53L0X_Error get_sequence_step_timeout(VL53L0X_DEV Dev,
			VL53L0X_SequenceStepId SequenceStepId,
			uint32_t *pTimeOutMicroSecs);

VL53L0X_Error set_sequence_step_timeout(VL53L0X_DEV Dev,
			VL53L0X_SequenceStepId SequenceStepId,
			uint32_t TimeOutMicroSecs);

VL53L0X_Error VL53L0X_set_measurement_timing_budget_micro_seconds(VL53L0X_DEV Dev,
	uint32_t MeasurementTimingBudgetMicroSeconds);

VL53L0X_Error VL53L0X_get_measurement_timing_budget_micro_seconds(VL53L0X_DEV Dev,
		uint32_t *pMeasurementTimingBudgetMicroSeconds);

VL53L0X_Error VL53L0X_load_tuning_settings(VL53L0X_DEV Dev,
		uint8_t *pTuningSettingBuffer);

VL53L0X_Error VL53L0X_calc_sigma_estimate(VL53L0X_DEV Dev,
		VL53L0X_RangingMeasurementData_t *pRangingMeasurementData,
		FixPoint1616_t *pSigmaEstimate, uint32_t *pDmax_mm);

VL53L0X_Error VL53L0X_get_total_xtalk_rate(VL53L0X_DEV Dev,
	VL53L0X_RangingMeasurementData_t *pRangingMeasurementData,
	FixPoint1616_t *ptotal_xtalk_rate_mcps);

VL53L0X_Error VL53L0X_get_total_signal_rate(VL53L0X_DEV Dev,
	VL53L0X_RangingMeasurementData_t *pRangingMeasurementData,
	FixPoint1616_t *ptotal_signal_rate_mcps);

VL53L0X_Error VL53L0X_get_pal_range_status(VL53L0X_DEV Dev,
		 uint8_t DeviceRangeStatus,
		 FixPoint1616_t SignalRate,
		 uint16_t EffectiveSpadRtnCount,
		 VL53L0X_RangingMeasurementData_t *pRangingMeasurementData,
		 uint8_t *pPalRangeStatus);

VL53L0X_Error VL53L0X_device_read_strobe(VL53L0X_DEV Dev);

VL53L0X_Error VL53L0X_calc_dmax(
	VL53L0X_DEV Dev,
	FixPoint1616_t totalSignalRate_mcps,
	FixPoint1616_t totalCorrSignalRate_mcps,
	FixPoint1616_t pwMult,
	uint32_t sigmaEstimateP1,
	FixPoint1616_t sigmaEstimateP2,
	uint32_t peakVcselDuration_us,
	uint32_t *pdmax_mm);

uint32_t VL53L0X_calc_timeout_mclks(VL53L0X_DEV Dev,
	uint32_t timeout_period_us, uint8_t vcsel_period_pclks);

uint16_t VL53L0X_encode_timeout(uint32_t timeout_macro_clks);

uint32_t VL53L0X_calc_macro_period_ps(VL53L0X_DEV Dev, uint8_t vcsel_period_pclks);

uint32_t VL53L0X_calc_timeout_us(VL53L0X_DEV Dev,
		uint16_t timeout_period_mclks,
		uint8_t vcsel_period_pclks);

#ifdef __cplusplus
}
#endif

#endif /* _VL53L0X_API_CORE_H_ */
