#pragma once

#include <math.h>

#ifdef ONE_OVER_SQRT_TWO
#undef ONE_OVER_SQRT_TWO
#endif
#ifdef ONE_OVER_SQRT_TWO_PI
#undef ONE_OVER_SQRT_TWO_PI
#endif
#ifdef SQRT_TWO_PI
#undef SQRT_TWO_PI
#endif

#define ONE_OVER_SQRT_TWO     0.7071067811865475244008443621048490392848359376887
#define ONE_OVER_SQRT_TWO_PI  0.3989422804014326779399460599343818684758586311649
#define SQRT_TWO_PI           2.506628274631000502415765284811045253006986740610

namespace wichura {
	double norm_cdf_inv(double p);
	double erfc_inv(double p);
	double erf_inv(double p);
}

namespace CML {
	class NormalDistribution {
	public:
		static double PDF(const double& x);
		static const double normalPDFMaxX;

		static double erfc_Cephes(const double& z, double* pdf);
		static double erf_Cephes(const double& z, double* pdf);
		static double CDF_Cephes(const double& z, double* pdf);
		static double InverseCDF_Acklam(const double& x);
		static double InverseCDF_Cephes(const double& x);
	};

	inline double NormalDistribution::PDF(const double& x) {
		return ONE_OVER_SQRT_TWO_PI * exp(-0.5 * (x * x));
	}
}
