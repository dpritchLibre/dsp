#ifndef DSP_BAYES_SRC_X_GEN_H
#define DSP_BAYES_SRC_X_GEN_H

#include "Rcpp.h"
#include "DayBlock.h"
#include "UProdBeta.h"
#include "UProdTau.h"
#include "WGen.h"
#include "XiGen.h"




class XGen {

public:

    class XMissDay;

    // storage for the X values
    Rcpp::IntegerVector& m_x_rcpp;
    Rcpp::IntegerVector::iterator m_vals;

    // information about the number of X missing for a given cycle
    const PregCyc* m_miss_cyc;
    const int m_n_miss_cyc;

    // tracks the indices of the missing values in X as well as whether
    // intercourse occured on the previous day
    const XMissDay* m_miss_day;

    // global probability used to sample missing values of intercourse for the
    // day before the fertile window
    const double m_cohort_sex_prob;

    // the regression coefficient in the missing intercourse prior probabilities
    // model coresponding to the previous day of intercourse
    const double m_sex_coef;


    XGen(Rcpp::IntegerVector& X_rcpp,
	 Rcpp::List& miss_cyc,
	 Rcpp::List& miss_day,
	 double cohort_sex_prob,
	 double sex_coef);
    ~XGen();

    void sample(const WGen& W,
		const XiGen& xi,
		const UProdBeta& ubeta,
		const UProdTau& utau);

    void sample_cycle(const PregCyc* miss_cyc,
		      const WGen& W,
		      const XiGen& xi,
		      const UProdBeta& ubeta,
		      const UProdTau& utau);

    double calc_prior_prob(const XMissDay* miss_day,
			   const UProdTau& utau,
			   const int prev_day_sex) const;

    static double calc_posterior_prob(const XMissDay* miss_day,
				      const UProdBeta& ubeta,
				      const double xi_i);

    static int sample_x_ijk(const XMissDay* miss_day,
			    const double prior_prob_yes,
			    const double posterior_prob_yes);

    int sample_day_before_fw_sex() const;

    // int calc_prior_probs(double prior_probs[][2],
    // 			 const PregCyc* curr_miss_cyc,
    // 			 const XMissDay* curr_miss_day,
    // 			 const UProdTau& utau) const;

    // static double XGen::calc_prior_prob(const XMissDay* miss_day,
    // 					const UProdTau& utau,
    // 					const int prev_day_sex);

    // static int calc_posterior_probs(double* posterior_probs,
    // 				    const PregCyc* curr_miss_cyc,
    // 				    const XMissDay* curr_miss_day,
    // 				    const double prior_probs[][2],
    // 				    const WGen& W,
    // 				    const XiGen& xi,
    // 				    const UProdBeta& ubeta,
    // 				    int day_before_fw_sex);

    // static int sample_x_perm(double* probs, int n_perms);

    // static int get_w_bitmap(const WGen& W,
    // 			    const PregCyc* curr_miss_cyc,
    // 			    const XMissDay* curr_miss_day);

    // void update_cyc_x(const PregCyc* curr_miss_cyc,
    // 		      const XMissDay* curr_miss_day,
    // 		      int t);

    // // static double calc_nonrand_sum_exp_ubeta(const PregCyc* curr_miss_cyc,
    // // 					     const XMissDay* curr_miss_day,
    // // 					     const UProdBeta& ubeta);

    // int* vals() { return m_vals; }
    // const int* vals() const { return m_vals; }
};




class XGen::XMissDay {

public:

    int idx;
    int prev;

    XMissDay() : idx(0), prev(0) {}

    XMissDay(int idx, int prev) :
	idx(idx),
	prev(prev) {
    }

    static XMissDay* list_to_arr(Rcpp::List& block_list);
};


#endif