/*
 * Copyright (C) 2014 Swift Navigation Inc.
 * Contact: Ian Horn <ian@swift-nav.com>
 *
 * This source is subject to the license found in the file 'LICENSE' which must
 * be be distributed together with this source. All other rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef LIBSWIFTNAV_AMB_KF_H
#define LIBSWIFTNAV_AMB_KF_H
#include "track.h"
#include "almanac.h"
#include "gpstime.h"
#include "common.h"
#include "single_diff.h"
#include "constants.h"

#define MAX_STATE_DIM (MAX_CHANNELS - 1)
#define MAX_OBS_DIM (2 * MAX_CHANNELS - 5)

typedef struct {
  u32 state_dim;
  u32 obs_dim;
  double amb_drift_var;
  double decor_mtx[MAX_OBS_DIM * MAX_OBS_DIM]; //the decorrelation matrix. takes raw measurements and decorrelates them
  double decor_obs_mtx[MAX_STATE_DIM * MAX_OBS_DIM]; //the observation matrix for decorrelated measurements
  double decor_obs_cov[MAX_OBS_DIM]; //the diagonal of the decorrelated observation covariance (for cholesky is ones)
  double null_basis_Q[(MAX_STATE_DIM - 3) * MAX_OBS_DIM];
  double state_mean[MAX_STATE_DIM];
  double state_cov_U[MAX_STATE_DIM * MAX_STATE_DIM];
  double state_cov_D[MAX_STATE_DIM];
} nkf_t;

double simple_amb_measurement(double carrier, double code);
// void predict_forward(nkf_t *kf);
void nkf_update(nkf_t *kf, double *measurements);

void assign_de_mtx(u8 num_sats, const sdiff_t *sats_with_ref_first,
                   const double ref_ecef[3], double *DE);

void assign_phase_obs_null_basis(u8 num_dds, double *DE_mtx, double *q);
void set_nkf(nkf_t *kf, double amb_drift_var, double phase_var, double code_var, double amb_init_var,
            u8 num_sdiffs, sdiff_t *sdiffs_with_ref_first, double *dd_measurements, double ref_ecef[3]);
void set_nkf_matrices(nkf_t *kf, double phase_var, double code_var,
                     u8 num_sdiffs, sdiff_t *sdiffs_with_ref_first, double ref_ecef[3]);
s32 find_index_of_element_in_u8s(const u32 num_elements, const u8 x, const u8 *list);
void rebase_nkf(nkf_t *kf, u8 num_sats, u8 *old_prns, u8 *new_prns);

void nkf_state_projection(nkf_t *kf,
                                    u8 num_old_non_ref_sats,
                                    u8 num_new_non_ref_sats,
                                    u8 *ndx_of_new_sat_in_old);
void nkf_state_inclusion(nkf_t *kf,
                         u8 num_old_non_ref_sats,
                         u8 num_new_non_ref_sats,
                         u8 *ndx_of_old_sat_in_new,
                         double *estimates,
                         double int_init_var);

void rebase_nkf(nkf_t *kf, u8 num_sats, u8 *old_prns, u8 *new_prns);
void rebase_covariance_udu(double *state_cov_U, double *state_cov_D, u8 num_sats, u8 *old_prns, u8 *new_prns);
void least_squares_solve_b(nkf_t *kf, const sdiff_t *sdiffs_with_ref_first,
         const double *dd_measurements, const double ref_ecef[3], double b[3]);

void rebase_mean_N(double *mean, const u8 num_sats, const u8 *old_prns, const u8 *new_prns);
void rebase_covariance_sigma(double *state_cov, const u8 num_sats, const u8 *old_prns, const u8 *new_prns);

#endif /* LIBSWIFTNAV_AMBFLOAT_KF_H */

