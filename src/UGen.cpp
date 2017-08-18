#include <cmath>
#include "Rcpp.h"

#include "CoefGen.h"
#include "UGen.h"
#include "UProdBeta.h"
#include "UProdTau.h"
#include "WGen.h"
#include "XiGen.h"

#define IN_NON_PREG_CYC -1
#define REF_CELL     -1




UGen::UGen() :
    m_col_start(0),
    m_col_end(0),
    m_n_categs(0),
    m_is_ref_cell_coding(0),
    m_miss_block(0),
    m_w_idx(0) {
}



// void UGen::sample() {

//     m_curr_block = m_miss_block;
//     m_curr_day = m_miss_day;

//     for ( ; m_curr_block < m_end_block; ++m_curr_block) {

// 	sample_block();
//     }

// }




// void UGen::sample_block() {

//     // calc p(W | ubeta) perms

//     // calc p(X | utau) perms

//     // calc p(U) perms

//     // sample missing covariate

//     // update miss_block

//     // update ubeta

//     // update utau

//     // -------------------------------

//     double* posterior_w_probs[m_n_categs];
//     double* posterior_x_probs[m_n_categs];

//     calc_posterior_w_probs(posterior_w_probs, W, xi, coefs, ubeta, miss_block);


// }





// void calc_posterior_x_probs() {


// }



void UGen::calc_posterior_w_probs(double* posterior_w_probs,
				  const WGen& W,
				  const XiGen& xi,
				  const CoefGen coefs,
				  const UProdBeta ubeta,
				  const UMissBlock* const miss_block) const {

    // each element of `categ_exp_ubeta_arrs` is used to point to values of
    // `exp(U * beta)` affected by the missing covariate and corresponding to a
    // particular choice of covariate category
    const double* categ_exp_ubeta_arrs[m_n_categs];

    // storage used for values of `exp(U * beta)` affected by the missing
    // covariate and for various choices of covariate categories.  We need one
    // less than the number of categories b/c we already know the values for the
    // current chioce of category.
    double alt_exp_ubeta_vals[ miss_block->n_days * (m_n_categs - 1) ];

    // calculate p(W | U) for the various choices of missing covariate and store
    // in `posterior_w_probs`
    calc_ubeta_possibs(alt_exp_ubeta_vals, categ_exp_ubeta_arrs, coefs, ubeta, miss_block);
    calc_posterior_w_from_ubetas(posterior_w_probs, W, xi, miss_block, categ_exp_ubeta_arrs);
}




void UGen::calc_ubeta_possibs(double* alt_exp_ubeta_vals,
			      const double** categ_exp_ubeta_arrs,
			      const CoefGen& coefs,
			      const UProdBeta& ubeta,
			      const UMissBlock* const miss_block) const {

    const int curr_beg_idx = miss_block->beg_idx;
    const int curr_n_days  = miss_block->n_days;
    const int curr_u_col   = miss_block->u_col;

    // const double* const coefs_vals     = coefs.vals();
    const double* coefs_vals     = coefs.vals();
    const double* const ubeta_exp_vals = ubeta.exp_vals();

    // each iteration obtains `exp(U * beta)` corresponding to a value of 1 for
    // the dummy coding in the j-th variable for the observations that are
    // affected by the missing covariate.
    for (int j = m_col_start; j < m_col_end; ++j) {

	// case: the j-th column is the one for which the current `U` has a
	// value of 1.  No need to calculate `exp(U * beta)` for this choice of
	// `U`.
	if (j == curr_u_col) {
	    *categ_exp_ubeta_arrs++ = ubeta_exp_vals + curr_beg_idx;
	}

	// case: the j-th column is not the one for which the current `U` has a
	// value of 1.  Need to calculate the value `exp(U * beta)` for the
	// modified choice of `U`.
	else {

	    // the exponentiated difference between beta_j and the beta value
	    // corresponding to the current value of `U`.  The current value is
	    // implicitely 0 if the current category for the missing covariate
	    // corresponds to the reference cell.
	    const double exp_beta_diff = (curr_u_col == REF_CELL) ?
		std::exp(coefs_vals[j]) :
		std::exp(coefs_vals[j] - coefs_vals[curr_u_col]);

	    // step through the observations affected by the missing covariate
	    // and calclate `exp(U * beta)` for the modified choice of `U`
	    for (int r = 0 ; r < curr_n_days; ++r) {
		alt_exp_ubeta_vals[r] = ubeta_exp_vals[curr_beg_idx + r] * exp_beta_diff;
	    }

	    // point to the modified values of `exp(U * beta)` corresponding to
	    // a 1 for the current column
	    *categ_exp_ubeta_arrs++ = alt_exp_ubeta_vals;
	    // point to the next free location in the storage for the modified
	    // values
	    alt_exp_ubeta_vals += curr_n_days;
	}
    }

    // case: the categorical variable corresponding to the current group of
    // columns uses reference-cell coding, so we have to obtain the values of
    // `exp(U * beta)` corresponding to the reference cell
    if (m_is_ref_cell_coding) {

	// case: the currrent value of `U` corresponds to the reference cell
	if (curr_u_col == REF_CELL) {
	    *categ_exp_ubeta_arrs = ubeta_exp_vals + curr_beg_idx;
	}

	// case: the reference cell is not the one for which the previous value
	// of `U` has a value of 1.  Need to calculate the value `exp(U * beta)`
	// for the modified choice of `U`.
	else {

	    // the exponentiated difference between the beta coefficient
	    // corresponding to the reference cell and the beta coefficient
	    // corresponding to column that currently has a value of 1.
	    const double exp_beta_diff = std::exp(- coefs_vals[curr_u_col]);

	    // step through the observations affected by the missing covariate
	    // and calclate `U * beta` for the modified choice of `U`
	    for (int r = 0 ; r < curr_n_days; ++r) {
		alt_exp_ubeta_vals[r] = ubeta_exp_vals[curr_beg_idx + r] * exp_beta_diff;
	    }

	    // point to the modified values of `exp(U * beta)` corresponding to
	    // a 1 for the current column
	    *categ_exp_ubeta_arrs = alt_exp_ubeta_vals;
	}
    }
}




void UGen::calc_posterior_w_from_ubetas(double* posterior_w_probs,
					const WGen& W,
					const XiGen& xi,
					const UMissBlock* const miss_block,
					const double* const * categ_exp_ubeta_arrs) const {

    const int curr_beg_idx = miss_block->beg_idx;
    const int curr_n_days  = miss_block->n_days;
    const double xi_i      = xi.vals()[miss_block->subj_idx];
    const int* w_vals      = W.vals();

    // each iteration calculates the `p(W | U, data)` for the current missing
    // covariate
    for (int j = 0; j < m_n_categs; ++j) {

	// `curr_exp_ubeta_vals` stores the values of `exp(U * beta)` for the
	// choice of U corresponding to the j-th category
	const double* curr_exp_ubeta_vals = categ_exp_ubeta_arrs[j];
	double log_dpois_sum       = 0.0;

	// each iteration adds the value of `p(W_ijk | U, data)` for the for the
	// choice of U corresponding to the j-th category to `log_dpois_sum`.
	for (int r = 0; r < curr_n_days; ++r) {

	    // the mean value for the Poisson distribution of `W_ijk`, and the
	    // index in `W.vals()` corresponding to observation ijk
	    double mean_val = xi_i * curr_exp_ubeta_vals[r];
	    int curr_w_idx  = m_w_idx[curr_beg_idx + r];

	    // the value of `p(W_ijk | U, data)`.  If `curr_w_idx` has the flag
	    // value indicating that observation ijk corresponds to a day in a
	    // non-pregnancy cycle, then `W_ijk` has a value of 0.
	    log_dpois_sum += (curr_w_idx == IN_NON_PREG_CYC) ?
		-mean_val :
		R::dpois(w_vals[curr_w_idx], mean_val, 1);
	}

	// exponentiate and store the result
	*posterior_w_probs = std::exp(log_dpois_sum);
    }
}




// void calc_utau_possibs(double** categ_exp_utau_arrs,
// 		       double* alt_exp_utau_vals,
// 		       const UProdTau& utau,
// 		       const UMissBlock* const miss_block) const {

//     const int curr_beg_idx         = miss_block->beg_idx;
//     const int curr_n_days          = miss_block->n_days;
//     const int curr_u_col           = miss_block->u_col;
//     const double* const coefs_vals = coefs.vals();

//     // each iteration obtains `exp(U * beta)` corresponding to a value of 1 for
//     // the dummy coding in the j-th variable for the observations that are
//     // affected by the missing covariate.
//     for (int j = m_col_start; j < m_col_end; ++j) {

// 	// case: the j-th column is the one for which the current `U` has a
// 	// value of 1.  No need to calculate `exp(U * beta)` for this choice of
// 	// `U`.
// 	if (j == curr_u_col) {
// 	    *categ_exp_utau_arrs++ = utau_exp_vals + curr_beg_idx;
// 	}

// 	// case: the j-th column is not the one for which the current `U` has a
// 	// value of 1.  Need to calculate the value `exp(U * beta)` for the
// 	// modified choice of `U`.
// 	else {

// 	    // the exponentiated difference between beta_j and the beta value
// 	    // corresponding to the current value of `U`.  The current value is
// 	    // implicitely 0 if the current category for the missing covariate
// 	    // corresponds to the reference cell.
// 	    const double exp_beta_diff = (curr_u_col == REF_CELL) ?
// 		std::exp(coefs_vals[j]) :
// 		std::exp(coefs_vals[j] - coefs_vals[curr_u_col]);

// 	    // step through the observations affected by the missing covariate
// 	    // and calclate `exp(U * beta)` for the modified choice of `U`
// 	    for (int r = 0 ; r < curr_n_days; ++r) {
// 		alt_exp_utau_vals[r] = exp_utau_vals[curr_beg_idx + r] * exp_beta_diff;
// 	    }

// 	    // point to the modified values of `exp(U * beta)` corresponding to
// 	    // a 1 for the current column
// 	    *categ_exp_utau_arrs++ = alt_exp_utau_vals;
// 	    // point to the next free location in the storage for the modified
// 	    // values
// 	    alt_exp_utau_vals += r;
// 	}
//     }

//     // case: the categorical variable corresponding to the current group of
//     // columns uses reference-cell coding, so we have to obtain the values of
//     // `exp(U * beta)` corresponding to the reference cell
//     if (is_ref_cell_coding) {

// 	// case: the currrent value of `U` corresponds to the reference cell
// 	if (curr_u_col == REF_CELL) {
// 	    *categ_exp_utau_arrs = utau_exp_vals + curr_beg_idx;
// 	}

// 	// case: the reference cell is not the one for which the previous value
// 	// of `U` has a value of 1.  Need to calculate the value `exp(U * beta)`
// 	// for the modified choice of `U`.
// 	else {

// 	    // the exponentiated difference between the beta coefficient
// 	    // corresponding to the reference cell and the beta coefficient
// 	    // corresponding to column that currently has a value of 1.
// 	    const double exp_beta_diff = std::exp(- coefs_vals[curr_u_col]);

// 	    // step through the observations affected by the missing covariate
// 	    // and calclate `U * beta` for the modified choice of `U`
// 	    for (int r = 0 ; r < curr_n_days; ++r) {
// 		alt_exp_utau_vals[r] = exp_utau_vals[beg_idx + r] * exp_beta_diff;
// 	    }

// 	    // point to the modified values of `exp(U * beta)` corresponding to
// 	    // a 1 for the current column
// 	    *categ_exp_utau_arrs = alt_exp_utau_vals;
// 	}
//     }
// }