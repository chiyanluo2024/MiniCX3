#include "pch.h"
#include <numeric>
#include "ScriptInterface.h"

class ScriptTest : public testing::Test {
public:
	ScriptTest() { }
	~ScriptTest() { }
	void SetUp() override { }
	void TearDown() override { }
};

static const double eps = 1e-14;

TEST_F(ScriptTest, TestBasics) {
	auto check = [](int testNo, const std::vector<double>& res, const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z) {
		if (y.size() != 1) {
			EXPECT_EQ(x.size(), y.size());
		}
		if (z.size() != 1) {
			EXPECT_EQ(x.size(), z.size());
		}
		size_t n1 = std::max(x.size(), y.size());
		size_t n2 = std::max(x.size(), z.size());
		size_t ntotal = n1 * 10 + n2 * 2 + x.size() + 1;
		EXPECT_EQ(res.size(), ntotal);
		size_t ind = 0;
		double val(0.0);
		for (size_t i = 0; i < n1; ++i) {
			val = x[x.size() == 1 ? 0 : i] + y[y.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < n1; ++i) {
			val = x[x.size() == 1 ? 0 : i] - y[y.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < n1; ++i) {
			val = x[x.size() == 1 ? 0 : i] * y[y.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < n1; ++i) {
			val = x[x.size() == 1 ? 0 : i] / y[y.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < n1; ++i) {
			val = x[x.size() == 1 ? 0 : i] == y[y.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < n1; ++i) {
			double val = x[x.size() == 1 ? 0 : i] != y[y.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < n1; ++i) {
			val = x[x.size() == 1 ? 0 : i] > y[y.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < n1; ++i) {
			val = x[x.size() == 1 ? 0 : i] >= y[y.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < n1; ++i) {
			val = x[x.size() == 1 ? 0 : i] < y[y.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < n1; ++i) {
			double val = x[x.size() == 1 ? 0 : i] <= y[y.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < n2; ++i) {
			val = x[x.size() == 1 ? 0 : i] && z[z.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < x.size(); ++i) {
			val = x[x.size() == 1 ? 0 : i] || z[z.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < n2; ++i) {
			val = !x[x.size() == 1 ? 0 : i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		val = double(x.size());
		EXPECT_NEAR(res[ind], val, eps);
		++ind;
		};
	std::vector<std::string> in_name({ "x", "y", "z" });
	std::vector<std::vector<double> > in_value({ {1, 2, 3}, {3, 2, 1}, {0} });
	std::string script = "x + y & x - y & x * y & x / y & x == y & x != y & x > y & x >= y & x < y & x <= y & x && z & x || z & !x & #x";
	std::vector<double> value1, value2;
	client::ScriptInterface s(script, in_name, in_value);
	s.run(value1);
	check(0, value1, in_value[0], in_value[1], in_value[2]);
	std::vector<double> x2({ 3.5, 0.0, 1 });
	std::vector<double> y2({ 0.5, 1.5, 1.0 });
	std::vector<double> z2({ 1 });
	// constant updates
	s.update("x", x2);
	s.update("y", y2);
	s.update("z", z2);
	s.run(value2);
	check(1, value2, x2, y2, z2);
}

TEST_F(ScriptTest, TestUnary) {
	auto check = [](int testNo, const std::vector<double>& res, const std::vector<double>& x) {
		double val(0.0);
		size_t ind = 0;

		val = std::accumulate(x.begin(), x.end(), 0.0);
		EXPECT_NEAR(res[ind], val, eps);
		++ind;

		val = std::accumulate(x.begin(), x.end(), 1.0, [](const double& a, const double& b) { return a * b; });
		EXPECT_NEAR(res[ind], val, eps);
		++ind;

		val = std::accumulate(x.begin(), x.end(), std::numeric_limits<double>::infinity(), [](const double& a, const double& b) { return std::min(a, b); });
		EXPECT_NEAR(res[ind], val, eps);
		++ind;

		val = std::accumulate(x.begin(), x.end(), -std::numeric_limits<double>::infinity(), [](const double& a, const double& b) { return std::max(a, b); });
		EXPECT_NEAR(res[ind], val, eps);
		++ind;

		for (size_t i = 0; i < x.size(); ++i) {
			val = std::max(x[i], 0.0);
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < x.size(); ++i) {
			val = std::min(x[i], 0.0);
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < x.size(); ++i) {
			val = x[i] * x[i];
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < x.size(); ++i) {
			val = sqrt(fabs(x[i]));
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < x.size(); ++i) {
			val = exp(x[i]);
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		for (size_t i = 0; i < x.size(); ++i) {
			val = std::log(fabs(x[i]));
			EXPECT_NEAR(res[ind], val, eps);
			++ind;
		}
		};
	std::vector<std::string> in_name({ "x" });
	std::vector<std::vector<double> > in_value({ {-0.2, 0.4} });
	std::string script = "$ax = abs(x); sum(x) & prod(x) & min(x) & max(x) & plus(x) & minus(x) & sqr(x) & sqrt(ax) & exp(x) & log(ax)";
	std::vector<double> value;
	client::ScriptInterface s(script, in_name, in_value);
	s.run(value);
	check(0, value, in_value[0]);
	// test retrieve
	s.retrieve("ax", value);
	auto check0 = [](int testNo, const std::vector<double>& value, const std::vector<double>& value2) {
		EXPECT_EQ(value.size(), value2.size());
		for (size_t i = 0; i < value.size(); ++i) {
			double expected = fabs(value2[i]);
			EXPECT_NEAR(value[i], expected, eps);
		}
		};
	check0(1, value, in_value[0]);
}

TEST_F(ScriptTest, TestBinary) {
	auto check = [](int testNo, const std::vector<double>& res, const std::vector<double>& saved) {
		EXPECT_EQ(res.size(), saved.size());
		for (size_t i = 0; i < res.size(); ++i) {
			EXPECT_NEAR(res[i], saved[i], eps);
		}
		};
	std::vector<std::string> in_name({ "x", "y" });
	std::vector<std::vector<double> > in_value({ {5, 0.5, 3.}, {1, 2, 4} });
	std::vector<double> value;
	std::string script = "emax(x, y) & emin(x, y)";
	client::ScriptInterface s(script, in_name, in_value);
	s.run(value);
	std::vector<double> expected({ 5, 2, 4, 1, 0.5, 3. });
	check(0, value, expected);
	s.update("y", std::vector<double>({ 1.0 }));
	s.run(value);
	expected = std::vector<double>({ 5, 1., 3., 1., 0.5, 1.0 });
	check(1, value, expected);
	s.update("x", std::vector<double>({ 0.5 }));
	s.update("y", std::vector<double>({ -1, 2, 4 }));
	s.run(value);
	expected = std::vector<double>({ 0.5, 2, 4, -1, 0.5, 0.5 });
	check(2, value, expected);
	s.update("x", std::vector<double>({ 0.5 }));
	s.update("y", std::vector<double>({ -0.5 }));
	s.run(value);
	expected = std::vector<double>({ 0.5, -0.5 });
	check(3, value, expected);
}

TEST_F(ScriptTest, TestTernary) {
	auto check = [](int testNo, const std::vector<double>& res, const std::vector<double>& saved) {
		EXPECT_EQ(res.size(), saved.size());
		for (size_t i = 0; i < res.size(); ++i) {
			EXPECT_NEAR(res[i], saved[i], eps);
		}
		};
	std::vector<std::string> in_name({ "x", "y" });
	std::vector<std::vector<double> > in_value({ {90, 100, 110}, {1, 2, 4} });
	std::vector<double> value;
	std::string script = "$xx = 85 & 95 & 105 & 115; interp(xx, x, y)";
	client::ScriptInterface s(script, in_name, in_value);
	s.run(value);
	std::vector<double> expected({ 0.5, 1.5, 3, 5 });
	check(0, value, expected);
	s.update("y", std::vector<double>({ 1.0 }));
	s.run(value);
	std::fill(expected.begin(), expected.end(), 1.0);
	check(1, value, expected);
	std::string script1 = "x[0&1&2] & y[0&2]";
	client::ScriptInterface s1(script1, in_name, in_value);
	s1.run(value);
	expected = std::vector<double>({ 90, 100, 110, 1, 4 });
	check(2, value, expected);
	std::string script2 = "$xx = x[0:2]; $yy = y[0:2:2]; xx & yy";
	client::ScriptInterface s2(script2, in_name, in_value);
	s2.run(value);
	expected = std::vector<double>({ 90, 100, 110, 1, 4 });
	check(2, value, expected);
	std::string script3 = "$x[0:2:2] = 2; $y[0:2] = 0.5; x & y[0:2:2]";
	client::ScriptInterface s3(script3, in_name, in_value);
	s3.run(value);
	expected = std::vector<double>({ 2, 100, 2, 0.5, 0.5 });
	check(3, value, expected);
	std::string script4 = "$z = x & y; z[0:1,2]";
	client::ScriptInterface s4(script4, in_name, in_value);
	s4.run(value);
	expected = std::vector<double>({ 110, 110 });
	check(4, value, expected);
	client::ScriptInterface s4_(script4, in_name, in_value, {"z"}, { 3 });
	s4_.run(value);
	expected = std::vector<double>({ 110, 4 });
	check(4, value, expected);
	std::string script5 = "$z = x & y; $z[0:1,2] = 0; z";
	client::ScriptInterface s5(script5, in_name, in_value);
	s5.run(value);
	expected = std::vector<double>({ 90, 100, 0, 1, 2, 4 });
	check(5, value, expected);
	client::ScriptInterface s5_(script5, in_name, in_value, { "z" }, { 3 });
	s5_.run(value);
	expected = std::vector<double>({ 90, 100, 0, 1, 2, 0 });
	check(5, value, expected);
}

TEST_F(ScriptTest, TestOthers) {
	auto callPayoff = [](const double& spot, const double strike) {
		return spot > strike ? spot - strike : 0.0;
		};
	std::vector<std::string> in_name({ "spot", "strike" });
	std::vector<std::vector<double> > in_value({ {50, 96, 101, 106, 150}, {100} });
	std::vector<double> value, expected;


	auto check = [&](int testNo) {
		EXPECT_EQ(value.size(), expected.size());
		for (size_t i = 0; i < value.size(); ++i) {
			EXPECT_NEAR(value[i], expected[i], eps);
		}
		};

	expected = { 0., 10., 0., 10. };
	// array assignment, power
	std::string script0 = "$test = zero(4); $test[1 & 3] = strike ^ 0.5; test";
	client::ScriptInterface s0(script0, in_name, in_value);
	value.clear();
	s0.run(value);
	check(0);

	expected.resize(in_value[0].size());
	for (size_t i = 0; i < expected.size(); ++i) {
		expected[i] = callPayoff(in_value[0].size() > 1 ? in_value[0][i] : in_value[0][0], in_value[1].size() > 1 ? in_value[1][i] : in_value[1][0]);
	}

	// simple assignment
	std::string script1 = "$payoff = plus(spot - strike); payoff";
	client::ScriptInterface s1(script1, in_name, in_value);
	value.clear();
	s1.run(value);
	check(1);

	// if, list, and compound expressions
	std::string script2 = "$payoff = list($i, 0:(#spot-1), $x = if(spot[i] > strike, $y = spot[i]-strike; y, $z = 0.0; z); x); payoff";
	client::ScriptInterface s2(script2, in_name, in_value);
	value.clear();
	s2.run(value);
	check(2);

	// array index
	std::string script3 = "$payoff = zero(#spot); $payoff[0:#spot-1] = plus(spot - strike); payoff";
	client::ScriptInterface s3(script3, in_name, in_value);
	value.clear();
	s3.run(value);
	check(3);

	expected = { 1., 1., 2., 1., 2., 3., 1., 2., 3., 4., };
	// non-homogeneous list
	std::string script4 = "list($i, 1:4, 1:i)";
	client::ScriptInterface s4(script4, {}, {});
	value.clear();
	s4.run(value);
	check(4);

	expected = { 1., 2., 4., 3., 6., 9., };
	// nested list
	std::string script5 = "list($i, 1:3, list($j, 1:i, i*j))";
	client::ScriptInterface s5(script5, {}, {});
	value.clear();
	s5.run(value);
	check(5);

	expected = { 0.96, 1.05, 101, 105, 97, 99};
	// array2D with functions
	std::string script6 = "min(S[2,0:2]/S[0,0:2]) & max(S[1,0:2]/S[0,0:2]) & S[max(0:2),min(0:2)] & S[(0:2)+3]";
	client::ScriptInterface s6(script6, { "S" }, { {100, 100, 100, 105, 97, 99, 101, 97, 96} }, { "S" }, { 3 });
	value.clear();
	s6.run(value);
	check(6);
}