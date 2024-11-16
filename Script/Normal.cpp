#include <immintrin.h>
#include <array>
#include <limits>
#include <cmath>
#include "Normal.h"

#if defined(_MSC_VER)
#define FORCEINLINE inline
#elif defined(__GNUC__) || defined(__clang__)
#define FORCEINLINE __attribute__((alwaysinline))inline
#endif

#undef __AVX2__

namespace simd {
	FORCEINLINE auto _set2(double x) { return _mm_set1_pd(x); }
	FORCEINLINE auto _add(__m128d x, __m128d y) { return _mm_add_pd(x, y); }
	FORCEINLINE auto _mul(__m128d x, __m128d y) { return _mm_mul_pd(x, y); }
	FORCEINLINE auto _div1(__m128d x, __m128d y) { return _mm_div_sd(x, y); }
	template<typename T> FORCEINLINE auto _load2(const T* px) { return _mm_loadu_pd((const double*)px); }
	template<typename T> FORCEINLINE auto _fmadd(T x, T y, T z) { return _add(_mul(x, y), z); }
	template<int imm8> FORCEINLINE auto _permute(__m128d x) { return _mm_permute_pd(x, imm8); }
}

namespace wichura_detail {
	constexpr auto sqr(double x) { return x * x; }
	constexpr auto inv_sqrt2 = 0.70710678118654752440084436210485;
	constexpr auto split1 = 0.425;
	constexpr auto split2 = 5.0;
	constexpr auto const2 = 1.6;

	constexpr auto a = std::array<double, 8>{
		3.3871328727963666080, 1.3314166789178437745E2,
			1.9715909503065514427E3, 1.3731693765509461125E4,
			4.5921953931549871457E4, 6.7265770927008700853E4,
			3.3430575583588128105E4, 2.5090809287301226727E3,
	};

	constexpr auto b = std::array<double, 8>{
		1.0, 4.2313330701600911252E1,
			6.8718700749205790830E2, 5.3941960214247511077E3,
			2.1213794301586595867E4, 3.9307895800092710610E4,
			2.8729085735721942674E4, 5.2264952788528545610E3
	};

	constexpr auto c = std::array<double, 8>{
		1.42343711074968357734E0, 4.63033784615654529590E0,
			5.76949722146069140550E0, 3.64784832476320460504E0,
			1.27045825245236838258E0, 2.41780725177450611770E-1,
			2.27238449892691845833E-2, 7.74545014278341407640E-4,
	};

	constexpr auto d = std::array<double, 8>{
		1.0, 2.05319162663775882187E0,
			1.67638483018380384940E0, 6.89767334985100004550E-1,
			1.48103976427480074590E-1, 1.51986665636164571966E-2,
			5.47593808499534494600E-4, 1.05075007164441684324E-9
	};

	constexpr auto e = std::array<double, 8>{
		6.65790464350110377720E0, 5.46378491116411436990E0,
			1.78482653991729133580E0, 2.96560571828504891230E-1,
			2.65321895265761230930E-2, 1.24266094738807843860E-3,
			2.71155556874348757815E-5, 2.01033439929228813265E-7,
	};

	constexpr auto f = std::array<double, 8>{
		1.0, 5.99832206555887937690E-1,
			1.36929880922735805310E-1, 1.48753612908506148525E-2,
			7.86869131145613259100E-4, 1.84631831751005468180E-5,
			1.42151175831644588870E-7, 2.04426310338993978564E-15,
	};

	alignas(64) constexpr auto ab = std::array<std::array<double, 2>, 8>{
		std::array<double, 2>({ a[0],b[0] }), std::array<double, 2>({ a[1],b[1] }), std::array<double, 2>({ a[2],b[2] }), std::array<double, 2>({ a[3],b[3] }),
			std::array<double, 2>({ a[4],b[4] }), std::array<double, 2>({ a[5],b[5] }), std::array<double, 2>({ a[6],b[6] }), std::array<double, 2>({ a[7],b[7] }),
	};

	alignas(64) constexpr auto cd = std::array<std::array<double, 2>, 8>{
		std::array<double, 2>({ c[0], d[0] }), std::array<double, 2>({ c[1], d[1] }), std::array<double, 2>({ c[2], d[2] }), std::array<double, 2>({ c[3], d[3] }),
			std::array<double, 2>({ c[4], d[4] }), std::array<double, 2>({ c[5], d[5] }), std::array<double, 2>({ c[6], d[6] }), std::array<double, 2>({ c[7], d[7] }),
	};

	alignas(64) constexpr auto ef = std::array<std::array<double, 2>, 8>{
		std::array<double, 2>({ e[0], f[0] }), std::array<double, 2>({ e[1], f[1] }), std::array<double, 2>({ e[2], f[2] }), std::array<double, 2>({ e[3], f[3] }),
			std::array<double, 2>({ e[4], f[4] }), std::array<double, 2>({ e[5], f[5] }), std::array<double, 2>({ e[6], f[6] }), std::array<double, 2>({ e[7], f[7] }),
	};

	inline auto rat_8x8(const std::array<std::array<double, 2>, 8>& pq, double x)
	{
		using namespace simd;
		auto x1 = _set2(x);
		auto D = _fmadd(_load2(&pq[1]), x1, _load2(&pq[0]));
		auto C = _fmadd(_load2(&pq[3]), x1, _load2(&pq[2]));
		auto B = _fmadd(_load2(&pq[5]), x1, _load2(&pq[4]));
		auto A = _fmadd(_load2(&pq[7]), x1, _load2(&pq[6]));
		auto x2 = _mul(x1, x1);
		A = _fmadd(A, x2, B);
		C = _fmadd(C, x2, D);
		A = _fmadd(A, _mul(x2, x2), C);
#ifdef __AVX__
		auto r = _div1(A, _permute<3>(A));
#else
		auto r = _div1(A, _mm_shuffle_pd(A, A, 1));
#endif
		return _mm_cvtsd_f64(r);
	}

	inline double tail_impl(double r)
	{
		if (r < std::numeric_limits<double>::min())
		{
			if (r > 0)
				return rat_8x8(ef, std::sqrt(-std::log(r)) - split2);
			return (r = 0) ? std::numeric_limits<double>::infinity() : std::numeric_limits<double>::quiet_NaN();
		}
		auto w = std::sqrt(-std::log(r));
		return (w > split2) ? rat_8x8(ef, w - split2) : rat_8x8(cd, w - const2);
	}

	inline double norm_cdf_inv_impl(double r)
	{
		if (r >= .5 - split1)
		{
			auto q = .5 - r;
			return q * rat_8x8(ab, sqr(split1) - sqr(q));
		}
		return tail_impl(r);
	}

	inline double erf_inv_impl(double r)
	{
		return norm_cdf_inv_impl(.5 - r);
	}
}

namespace wichura
{
	double norm_cdf_inv(double p)
	{
		using namespace wichura_detail;
		return (p < .5)
			? -norm_cdf_inv_impl(p)
			: norm_cdf_inv_impl(1 - p);
	}

	double erfc_inv(double p)
	{
		using namespace wichura_detail;
		return -inv_sqrt2 * norm_cdf_inv(.5 * p);
	}

	double erf_inv(double p)
	{
		using namespace wichura_detail;
		return (p < 0)
			? -inv_sqrt2 * erf_inv_impl(-.5 * p)
			: inv_sqrt2 * erf_inv_impl(.5 * p);
	}
}

namespace CML {
	static const double MAXLOG = 7.09782712893383996732E2;     /* log(2**1024) */
	static const double Mtools_nep[] = {
		2.46196981473530512524E-10, 5.64189564831068821977E-1, 7.46321056442269912687E0,
		4.86371970985681366614E1, 1.96520832956077098242E2, 5.26445194995477358631E2,
		9.34528527171957607540E2, 1.02755188689515710272E3, 5.57535335369399327526E2 };
	static const double Mtools_neq[] = {/* 1.00000000000000000000E0,*/
		1.32281951154744992508E1, 8.67072140885989742329E1, 3.54937778887819891062E2,
		9.75708501743205489753E2, 1.82390916687909736289E3, 2.24633760818710981792E3,
		1.65666309194161350182E3, 5.57535340817727675546E2 };
	static const double Mtools_ner[] = {
		5.64189583547755073984E-1, 1.27536670759978104416E0, 5.01905042251180477414E0,
		6.16021097993053585195E0, 7.40974269950448939160E0, 2.97886665372100240670E0 };
	static const double Mtools_nes[] = {/* 1.00000000000000000000E0,*/
		2.26052863220117276590E0, 9.39603524938001434673E0, 1.20489539808096656605E1,
		1.70814450747565897222E1, 9.60896809063285878198E0, 3.36907645100081516050E0 };
	static const double Mtools_T[] = {
		9.60497373987051638749E0, 9.00260197203842689217E1, 2.23200534594684319226E3,
		7.00332514112805075473E3, 5.55923013010394962768E4 };
	static const double Mtools_U[] = {
		3.35617141647503099647E1, 5.21357949780152679795E2, 4.59432382970980127987E3,
		2.26290000613890934246E4, 4.92673942608635921086E4 };
	static const double Mtools_nip0[5] = {
		-5.99633501014107895267E1, 9.80010754185999661536E1,-5.66762857469070293439E1,
		1.39312609387279679503E1,-1.23916583867381258016E0, };
	static const double Mtools_niq0[8] = {/* 1.00000000000000000000E0,*/
		1.95448858338141759834E0, 4.67627912898881538453E0, 8.63602421390890590575E1,
		-2.25462687854119370527E2, 2.00260212380060660359E2,-8.20372256168333339912E1,
		1.59056225126211695515E1,-1.18331621121330003142E0, };
	static const double Mtools_nip1[9] = {
		4.05544892305962419923E0, 3.15251094599893866154E1, 5.71628192246421288162E1,
		4.40805073893200834700E1, 1.46849561928858024014E1, 2.18663306850790267539E0,
		-1.40256079171354495875E-1,-3.50424626827848203418E-2,-8.57456785154685413611E-4, };
	static const double Mtools_nip2[9] = {
		3.23774891776946035970E0,6.91522889068984211695E0,3.93881025292474443415E0,
		1.33303460815807542389E0,2.01485389549179081538E-1,1.23716634817820021358E-2,
		3.01581553508235416007E-4,2.65806974686737550832E-6,6.23974539184983293730E-9, };
	static const double Mtools_niq1[8] = {/*  1.00000000000000000000E0,*/
		1.57799883256466749731E1, 4.53907635128879210584E1, 4.13172038254672030440E1,
		1.50425385692907503408E1, 2.50464946208309415979E0,-1.42182922854787788574E-1,
		-3.80806407691578277194E-2,-9.33259480895457427372E-4, };
	static const double Mtools_niq2[8] = {/*  1.00000000000000000000E0,*/
		6.02427039364742014255E0,3.67983563856160859403E0,1.37702099489081330271E0,
		2.16236993594496635890E-1,1.34204006088543189037E-2,3.28014464682127739104E-4,
		2.89247864745380683936E-6,6.79019408009981274425E-9, };

	inline double polevl(const double& x, const double* coef, size_t N) {
		double ans;
		size_t i;
		const double* p;
		p = coef;
		ans = *p++;
		i = N;
		do {
			ans = ans * x + *p++;
		} while (--i);
		return ans;
	}

	inline double p1evl(const double& x, const double* coef, size_t N) {
		double ans;
		const double* p;
		size_t i;

		p = coef;
		ans = x + *p++;
		i = N - 1;
		do {
			ans = ans * x + *p++;
		} while (--i);
		return ans;
	}

	const double NormalDistribution::normalPDFMaxX = 37.67712072049519011237021844671; // = sqrt(2*ln(2**1024))

	double NormalDistribution::erfc_Cephes(const double& a, double* pdf) {
		double p, q, x, y, z;
		if (a < 0.0) {
			x = -a;
		}
		else {
			x = a;
		}
		if (x < 1.0) {
			return 1.0 - erf_Cephes(a, pdf);
		}
		z = -a * a;
		if (z < -MAXLOG) {
			if (pdf) {
				*pdf = 0.0;
			}
			return 0.0;
		}
		z = exp(z);
		if (pdf) {
			*pdf = ONE_OVER_SQRT_TWO_PI * z;
		}
		if (x < 8.0) {
			p = polevl(x, Mtools_nep, 8);
			q = p1evl(x, Mtools_neq, 8);
		}
		else {
			p = polevl(x, Mtools_ner, 5);
			q = p1evl(x, Mtools_nes, 6);
		}
		y = (z * p) / q;
		if (a < 0.0) {
			y = 2.0 - y;
		}
		return y;
	}

	double NormalDistribution::erf_Cephes(const double& x, double* pdf) {
		double y, z;
		if (fabs(x) > 1.0) {
			return 1.0 - erfc_Cephes(x, pdf);
		}
		z = x * x;
		y = x * polevl(z, Mtools_T, 4) / p1evl(z, Mtools_U, 5);
		if (pdf) {
			*pdf = ONE_OVER_SQRT_TWO_PI * exp(-x * x);
		}
		return y;
	}

	double NormalDistribution::CDF_Cephes(const double& a, double* pdf) {
		double x, y, z;
		x = a * ONE_OVER_SQRT_TWO;
		z = fabs(x);
		if (z < 1.0) {
			y = 0.5 + 0.5 * erf_Cephes(x, pdf);
		}
		else {
			y = 0.5 * erfc_Cephes(z, pdf);
			if (x > 0) {
				y = 1.0 - y;
			}
		}
		return y;
	}

	// The Inverse cumulative normal distribution function
	// by Peter J. Acklam, University of Oslo, Statistics Division.
	//
	// URL: http://www.math.uio.no/~jacklam/notes/invnorm
	//

	namespace Acklam_detail {
		constexpr auto a = std::array<double, 6>{
			2.506628277459239e+00, -3.066479806614716e+01,
				1.383577518672690e+02, -2.759285104469687e+02,
				2.209460984245205e+02, -3.969683028665376e+01,
		};

		constexpr auto b = std::array<double, 6>{
			1.0, -1.328068155288572e+01,
				6.680131188771972e+01, -1.556989798598866e+02,
				1.615858368580409e+02, -5.447609879822406e+01
		};

		constexpr auto c = std::array<double, 6>{
			2.938163982698783e+00, 4.374664141464968e+00,
				-2.549732539343734e+00, -2.400758277161838e+00,
				-3.223964580411365e-01, -7.784894002430293e-03,
		};

		constexpr auto d = std::array<double, 6>{
			1.0, 3.754408661907416e+00,
				2.445134137142996e+00, 3.224671290700398e-01,
				7.784695709041462e-03, 0.0,
		};

		alignas(64) constexpr auto ab = std::array<std::array<double, 2>, 6>{
			std::array<double, 2>({ a[0],b[0] }), std::array<double, 2>({ a[1],b[1] }), std::array<double, 2>({ a[2],b[2] }),
				std::array<double, 2>({ a[3],b[3] }), std::array<double, 2>({ a[4],b[4] }), std::array<double, 2>({ a[5],b[5] }),
		};

		alignas(64) constexpr auto cd = std::array<std::array<double, 2>, 6>{
			std::array<double, 2>({ c[0], d[0] }), std::array<double, 2>({ c[1], d[1] }), std::array<double, 2>({ c[2], d[2] }),
				std::array<double, 2>({ c[3], d[3] }), std::array<double, 2>({ c[4], d[4] }), std::array<double, 2>({ c[5], d[5] }),
		};

		inline auto rat_6x6(const std::array<std::array<double, 2>, 6>& pq, double x)
		{
			using namespace simd;
			auto x1 = _set2(x);
			auto C = _fmadd(_load2(&pq[1]), x1, _load2(&pq[0]));
			auto B = _fmadd(_load2(&pq[3]), x1, _load2(&pq[2]));
			auto A = _fmadd(_load2(&pq[5]), x1, _load2(&pq[4]));
			auto x2 = _mul(x1, x1);
			A = _fmadd(A, x2, B);
			A = _fmadd(A, x2, C);

			/*auto x1 = _mm_set1_pd(x);
			auto C = _mm_fmadd_pd(_mm_loadu_pd((const double*)&pq[1]), x1, _mm_loadu_pd((const double*)&pq[0]));
			auto B = _mm_fmadd_pd(_mm_loadu_pd((const double*)&pq[3]), x1, _mm_loadu_pd((const double*)&pq[2]));
			auto A = _mm_fmadd_pd(_mm_loadu_pd((const double*)&pq[5]), x1, _mm_loadu_pd((const double*)&pq[4]));
			auto x2 = _mm_mul_pd(x1, x1);
			A = _mm_fmadd_pd(A, x2, B);
			A = _mm_fmadd_pd(A, x2, C);*/
#ifdef __AVX__
			auto r = _div1(A, _permute<3>(A));
#else
			auto r = _div1(A, _mm_shuffle_pd(A, A, 1));
#endif
			return _mm_cvtsd_f64(r);
		}
	}

	double NormalDistribution::InverseCDF_Acklam(const double& u) {

		if (u <= 0.0 || u >= 1.0) {
			throw "u must be within (0,1) for inverse cumulative normal to work";
		}

		// Limits of the approximation region. 
		const double
			u_low = 0.02425,
			u_high = 1.0 - u_low;

		double z, r;

		// Rational approximation for the lower region. ( 0 < u < u_low )
		if (u < u_low) {
			z = sqrt(-2.0 * log(u));
			z = Acklam_detail::rat_6x6(Acklam_detail::cd, z);
		}
		// Rational approximation for the central region. ( u_low <= u <= u_high )
		else if (u <= u_high) {
			z = u - 0.5;
			r = z * z;
			z = Acklam_detail::rat_6x6(Acklam_detail::ab, r) * z;
		}
		// Rational approximation for the upper region. ( u_high < u < 1 )
		else {
			z = sqrt(-2.0 * log(1.0 - u));
			z = -Acklam_detail::rat_6x6(Acklam_detail::cd, z);
		}

//#define REFINE_INVERSECUMULATIVENORMAL_TO_FULL_MACHINE_PRECISION_USING_HALLEYS_METHOD
#ifdef REFINE_INVERSECUMULATIVENORMAL_TO_FULL_MACHINE_PRECISION_USING_HALLEYS_METHOD
		// The relative error of the approximation has absolute value less
		// than 1.15e-9.  One iteration of Halley's rational method (third
		// order) gives full machine precision.
		double p;
		r = CDF_Cephes(z, &p);
		r = (r - u) / p;	//	f(z)/df(z)
		z -= r / (1 + 0.5 * z * r);							//	Halley's method
#endif
		return z;
	}

	//
	// Example: The following program:
	//
	//  #include <stdio.h>
	//  #include "NormalDistribution.h"
	//  int main (long , char *[]) {
	//   double x=.335, y = CumulativeNormal(x), z = InverseCumulativeNormal(y);
	//   printf("%.16f\n",y);
	//   printf("%.16f\n",z);
	//  }
	//
	// produces the following output.:
	//
	//  0.6311874568912148
	//  0.3349999997032685
	//
	//
	// Uncommenting the line
	//
	//   //#define REFINE_INVERSECUMULATIVENORMAL_TO_FULL_MACHINE_PRECISION_USING_HALLEYS_METHOD
	//
	// above, and rerunning the program, produces
	//
	//  0.6311874568912148
	//  0.3350000000000000
	//

	double NormalDistribution::InverseCDF_Cephes(const double& y0) {
		double x, y, z, y2, x0, x1;
		int code;
		if (y0 <= 0.0 || y0 >= 1.0) {
			throw "Argument must be in (0,1) for inverse cumulative normal to work";
		}

		code = 1;
		y = y0;
		if (y > (1.0 - 0.13533528323661269189)) { /* 0.135... = exp(-2) */
			y = 1.0 - y;
			code = 0;
		}
		if (y > 0.13533528323661269189) {
			y = y - 0.5;
			y2 = y * y;
			x = y + y * (y2 * polevl(y2, Mtools_nip0, 4) / p1evl(y2, Mtools_niq0, 8));
			x = x * SQRT_TWO_PI;
			return x;
		}
		x = sqrt(-2.0 * log(y));
		x0 = x - log(x) / x;
		z = 1.0 / x;
		if (x < 8.0) { /* y > exp( -32 ) = 1.2664165549e-14 */
			x1 = z * polevl(z, Mtools_nip1, 8) / p1evl(z, Mtools_niq1, 8);
		}
		else {
			x1 = z * polevl(z, Mtools_nip2, 8) / p1evl(z, Mtools_niq2, 8);
		}
		x = x0 - x1;
		if (code != 0) {
			x = -x;
		}
		return x;
	}
}
