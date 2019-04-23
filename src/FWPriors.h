#ifndef DSP_BAYES_FW_PRIORS_H_
#define DSP_BAYES_FW_PRIORS_H_

#include <vector>
#include "MHCont.h"
// #include "GammaFWDay.h"

// note that CoefGen.h included at the bottom of the file
class CoefGen;
class MDay;
class Mu;
class Nu;
class Delta;




class MDay {
public:

    // storage for previous values
    Rcpp::IntegerVector m_vals_rcpp;
    Rcpp::IntegerVector::iterator m_vals;

    // number of fertile window days
    int m_n_days_fw;

    // tracks whether we wish to save the samples of  to return to the user
    bool m_record_status;

    MDay(int n_samp, int n_days_fw, bool record_status);
    double val() const { return *m_vals; }
    // double val() const { return 2; }

    // void sample();
    void sample(const CoefGen& coefs,
                const Mu& mu,
                const Nu& nu,
                const Delta& delta);

    double calc_mday_log_lik(int proposal_peak_idx,
                             const CoefGen& coefs,
                             const Mu& mu,
                             const Nu& nu,
                             const Delta& delta);
};




class Mu : public MHCont {
public:

    // hyperparameters for mu
    const double m_alpha_0_minus_1;
    const double m_beta_0;

    // current value and log-value
    double m_mu_val;
    double m_log_mu_val;

    Mu(int n_samp, bool record_status, double proposal_dispersion);
    void sample(const CoefGen& coefs, const MDay& mday, const Nu& nu, const Delta& delta);

    double calc_log_r(const CoefGen& coefs,
                      const MDay& mday,
                      const Nu& nu,
                      const Delta& delta,
                      double proposal_val,
                      double log_proposal_val) const;

    double calc_log_lik_gamma_term(const CoefGen& coefs,
                                   const MDay& mday,
                                   const Nu& nu,
                                   const Delta& delta,
                                   double proposal_val,
                                   double log_proposal_val) const;

    double calc_log_lik_mu_term(double proposal_val, double log_proposal_val) const;

    // double val() const { return 0.44; }  // CRITICAL: remove this!!
};




class Nu : public MHCont {
public:

    // hyperparameters for nu
    const double m_alpha_0_minus_1;
    const double m_beta_0;

    // current value and log-value
    double m_nu_val;
    double m_log_nu_val;

    Nu(int n_samp, bool record_status, double proposal_dispersion);
    void sample(const CoefGen& coefs, const MDay& mday, const Mu& mu, const Delta& delta);

    double calc_log_r(const CoefGen& coefs,
                      const MDay& mday,
                      const Mu& mu,
                      const Delta& delta,
                      double proposal_val,
                      double log_proposal_val) const;

    double calc_log_lik_gamma_term(const CoefGen& coefs,
                                   const MDay& mday,
                                   const Mu& mu,
                                   const Delta& delta,
                                   double proposal_val,
                                   double log_proposal_val) const;

    double calc_log_lik_nu_term(double proposal_val, double log_proposal_val) const;
};




class Delta : public MHCont {
public:

    // hyperparameters for nu
    const double m_alpha_0_minus_1;
    const double m_beta_0_minus_1;

    // current value and log-value
    double m_delta_val;
    double m_log_delta_val;

    Delta(int n_samp, bool record_status, double proposal_dispersion);
    void sample(const CoefGen& coefs, const MDay& mday, const Mu& mu, const Nu& nu);

    double calc_log_r(const CoefGen& coefs,
                      const MDay& mday,
                      const Mu& mu,
                      const Nu& nu,
                      double proposal_val,
                      double log_proposal_val) const;

    double calc_log_lik_gamma_term(const CoefGen& coefs,
                                   const MDay& mday,
                                   const Mu& mu,
                                   const Nu& nu,
                                   double proposal_val,
                                   double log_proposal_val) const;

    double calc_log_lik_nu_term(double proposal_val, double log_proposal_val) const;

    // double val() const { return 0.5; }  // CRITICAL: remove this!!
};





class FWPriors {
public:

    MDay m_mday;
    Mu m_mu;
    Nu m_nu;
    Delta m_delta;

    FWPriors(const Rcpp::List& fw_prior_specs, int n_samp, int n_days_fw, bool record_status) :
        m_mday  {MDay(n_samp, n_days_fw, record_status)},
        m_mu    {build_mu(fw_prior_specs, n_samp)},
        m_nu    {build_nu(fw_prior_specs, n_samp)},
        m_delta {build_delta(fw_prior_specs, n_samp)}
    {}

    void sample(const CoefGen& coefs) { // FIXME
        m_mday.sample(coefs, m_mu, m_nu, m_delta);
        m_mu.sample(coefs, m_mday, m_nu, m_delta);
        m_nu.sample(coefs, m_mday, m_mu, m_delta);
        m_delta.sample(coefs, m_mday, m_mu, m_nu);
    }

    // FIXME.  Everything here and below is hardcoded.
    Delta build_delta(const Rcpp::List& fw_prior_specs, int n_samp) { // FIXME
        // Rcpp::List mu_specs {fw_prior_specs["mu_specs"]};
        Delta out {Delta(n_samp, true, 0.7)};
        return out;
    }

    // FIXME
    Mu build_mu(const Rcpp::List& fw_prior_specs, int n_samp) { // FIXME
        // Rcpp::List mu_specs {fw_prior_specs["mu_specs"]};
        Mu out {Mu(n_samp, true, 0.9)};
        return out;
    }

    // FIXME
    Nu build_nu(const Rcpp::List& fw_prior_specs, int n_samp) { // FIXME
        // Rcpp::List mu_specs {fw_prior_specs["mu_specs"]};
        Nu out {Nu(n_samp, true, 15.0)};
        return out;
    }
};


#include "CoefGen.h"

#endif
