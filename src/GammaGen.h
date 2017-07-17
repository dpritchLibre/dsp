#ifndef DSP_BAYES_GAMMA_GEN_H_
#define DSP_BAYES_GAMMA_GEN_H_

#include "Rcpp.h"
#include "WGen.h"
#include "XiGen.h"




class GammaGen {

public:

    // current value of beta_h and exp(beta_h), i.e. gamma_h
    double m_beta_val;
    double m_gam_val;

    // gamma_h hyperparameters
    const double m_hyp_a;
    const double m_hyp_b;
    const double m_hyp_p;
    const double m_bnd_l;
    const double m_bnd_u;

    // points to the beginning of the data for U_h
    const double* m_Uh;

    // number of observations in the data
    const int m_n_days;


    GammaGen(const Rcpp::NumericMatrix& U, const Rcpp::NumericVector& coef_specs);
    virtual ~GammaGen();

    static GammaGen** create_arr(const Rcpp::NumericMatrix& U, const Rcpp::List& gamma_specs);
    // virtual double samp_gam(const std::vector<double>& U_prod_beta,
    // 			    const std::vector<double>& W);

    virtual void sample(const WGen& W, const XiGen& xi, UProdBeta& u_prod_beta);

    // virtual void set_ar_param(double prev_gam_val);


};




class GammaCateg : public GammaGen {

public:

    // whether h-th variable is truncated or not
    const bool m_bnd_l_is_zero;
    const bool m_bnd_u_is_inf;
    const bool m_is_trunc;

    // the log of the constant terms in the expression for d2 as defined in
    // `GammaCateg::calc_p_tilde`.
    const double m_log_d2_const_terms;


    GammaCateg(const Rcpp::NumericMatrix& U, const Rcpp::NumericVector& gamma_specs);

    void sample(WGen& W, const XiGen& xi, UProdBeta& u_prod_beta, const double* X);
    double calc_a_tilde(WGen& W);
    double calc_b_tilde(UProdBeta& u_prod_beta, const XiGen& xi, const double* X);
    double calc_p_tilde(double a_tilde, double b_tilde);
    double sample_gamma(double a_tilde, double b_tilde, double p_tilde);
    void add_uh_prod_beta_h(UProdBeta& u_prod_beta_no_h);
    double log_dgamma_norm_const(double a, double b);
    double log_dgamma_trunc_const(double a, double b);
    double init_log_d2_const_terms();
    double calc_log_d2_const_terms();
};




/* class GammaContAux : public GammaGen { */

/* public: */

/*     // // whether M == 1 (i.e no effect in the model) */
/*     // bool M_is_one; */


/* }; */




/* class GammaContMetr : public GammaGen { */

/* public: */

/*     // Metropolis-Hastings tuning parameter.  Specifies the probability of */
/*     // selecting 1 as the proposal value. */
/*     double mh_pi; */

/* }; */




/* class GammaMeanShrink : public GammaGen { */

/* public: */



/* }; */


#endif
