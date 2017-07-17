// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// dsp_sampler
void dsp_sampler(Rcpp::NumericMatrix U, Rcpp::List preg_cyc, Rcpp::IntegerVector w_days_idx, Rcpp::IntegerVector w_cyc_idx, int fw_len, Rcpp::NumericVector xi_initial, Rcpp::List subj_days, Rcpp::List gamma_specs, Rcpp::NumericVector phi_hyper);
RcppExport SEXP dspBayes_dsp_sampler(SEXP USEXP, SEXP preg_cycSEXP, SEXP w_days_idxSEXP, SEXP w_cyc_idxSEXP, SEXP fw_lenSEXP, SEXP xi_initialSEXP, SEXP subj_daysSEXP, SEXP gamma_specsSEXP, SEXP phi_hyperSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type U(USEXP);
    Rcpp::traits::input_parameter< Rcpp::List >::type preg_cyc(preg_cycSEXP);
    Rcpp::traits::input_parameter< Rcpp::IntegerVector >::type w_days_idx(w_days_idxSEXP);
    Rcpp::traits::input_parameter< Rcpp::IntegerVector >::type w_cyc_idx(w_cyc_idxSEXP);
    Rcpp::traits::input_parameter< int >::type fw_len(fw_lenSEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericVector >::type xi_initial(xi_initialSEXP);
    Rcpp::traits::input_parameter< Rcpp::List >::type subj_days(subj_daysSEXP);
    Rcpp::traits::input_parameter< Rcpp::List >::type gamma_specs(gamma_specsSEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericVector >::type phi_hyper(phi_hyperSEXP);
    dsp_sampler(U, preg_cyc, w_days_idx, w_cyc_idx, fw_len, xi_initial, subj_days, gamma_specs, phi_hyper);
    return R_NilValue;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"dspBayes_dsp_sampler", (DL_FUNC) &dspBayes_dsp_sampler, 9},
    {NULL, NULL, 0}
};

RcppExport void R_init_dspBayes(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
