#include "calc.hpp"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>


namespace {
bool test_rad_on = false;
}
inline double process_line(double current, const std::string &line) {
    return process_line(current, test_rad_on, line);
}

namespace {

int failures = 0;

void expect_close(const std::string &name, const double actual,
                  const double expected) {
  constexpr double epsilon = 1e-9;
  if (std::fabs(actual - expected) > epsilon) {
    std::cerr << name << " failed: expected " << expected << ", got " << actual
              << std::endl;
    ++failures;
  }
}

void expect_error(const std::string &name, double current,
                  const std::string &line, double expected,
                  const std::string &expected_err) {
  std::stringstream buffer;
  std::streambuf *old_cerr = std::cerr.rdbuf(buffer.rdbuf());

  double actual = process_line(current, line);

  std::cerr.rdbuf(old_cerr);

  expect_close(name, actual, expected);

  std::string err_msg = buffer.str();
  if (!err_msg.empty() && err_msg.back() == '\n')
    err_msg.pop_back();
  if (!err_msg.empty() && err_msg.back() == '\r')
    err_msg.pop_back();

  if (err_msg != expected_err) {
    std::cerr << name << " failed error text: expected \"" << expected_err
              << "\", got \"" << err_msg << "\"" << std::endl;
    ++failures;
  }
}

void test_binary_operations() {
  expect_close("addition with padding", process_line(10, "+          5"), 15);
  expect_close("addition with spaces", process_line(10, "+   5"), 15);
  expect_close("subtraction", process_line(10, "- 3"), 7);
  expect_close("multiplication", process_line(4, "* 2.5"), 10);
  expect_close("division", process_line(9, "/ 4.5"), 2);
  expect_close("remainder", process_line(7.5, "% 2"), 1.5);
  expect_close("set from number", process_line(123, "42.75"), 42.75);
}

void test_unary_operations() {
  expect_close("negation", process_line(8, "_"), -8);
  expect_close("square root", process_line(49, "SQRT"), 7);
  expect_close("power", process_line(2, "^ 3"), 8);
}

void test_trig_operations() {
  test_rad_on = false; // ensure degrees
  expect_close("sin 30 deg", process_line(30, "SIN"), 0.5);
  expect_close("cos 60 deg", process_line(60, "COS"), 0.5);
  expect_close("tan 45 deg", process_line(45, "TAN"), 1.0);
  expect_close("ctn 45 deg", process_line(45, "CTN"), 1.0);

  expect_close("asin 0.5 deg", process_line(0.5, "ASIN"), 30.0);
  expect_close("acos 0.5 deg", process_line(0.5, "ACOS"), 60.0);
  expect_close("atan 1.0 deg", process_line(1.0, "ATAN"), 45.0);
  expect_close("actn 1.0 deg", process_line(1.0, "ACTN"), 45.0);

  process_line(0, "RAD");
  if (!test_rad_on) {
    std::cerr << "RAD failed to switch state\n";
    ++failures;
  }

  expect_close("sin pi/6 rad", process_line(M_PI / 6.0, "SIN"), 0.5);
  expect_close("cos pi/3 rad", process_line(M_PI / 3.0, "COS"), 0.5);
  expect_close("tan pi/4 rad", process_line(M_PI / 4.0, "TAN"), 1.0);
  expect_close("ctn pi/4 rad", process_line(M_PI / 4.0, "CTN"), 1.0);

  expect_close("asin 0.5 rad", process_line(0.5, "ASIN"), M_PI / 6.0);
  expect_close("acos 0.5 rad", process_line(0.5, "ACOS"), M_PI / 3.0);
  expect_close("atan 1.0 rad", process_line(1.0, "ATAN"), M_PI / 4.0);
  expect_close("actn 1.0 rad", process_line(1.0, "ACTN"), M_PI / 4.0);

  process_line(0, "DEG");
  if (test_rad_on) {
    std::cerr << "DEG failed to switch state\n";
    ++failures;
  }
}

void test_invalid_inputs() {
  expect_error("unknown operation", 5, "@ 10", 5, "Unknown operation @ 10");
  expect_error("missing argument", 5, "+", 5,
               "No argument for a binary operation");
  expect_error("bad unary suffix", 5, "_ extra", 5,
               "Unexpected suffix for a unary operation: ' extra'");
  expect_error("bad sqrt argument", -4, "SQRT", -4,
               "Bad argument for SQRT: -4");
  expect_error("division by zero", 8, "/ 0", 8,
               "Bad right argument for division: 0");
  expect_error("remainder by zero", 8, "% 0", 8,
               "Bad right argument for remainder: 0");
  expect_error("asin limits", 2.0, "ASIN", 2.0, "Bad argument for ASIN: 2");
  expect_error("acos limits", 1.5, "ACOS", 1.5, "Bad argument for ACOS: 1.5");
  expect_error("ctn limits", 0.0, "CTN", 0.0, "Bad argument for CTN: 0");
}

} // namespace

int main() {
  test_binary_operations();
  test_unary_operations();
  test_trig_operations();
  test_invalid_inputs();

  if (failures != 0) {
    std::cerr << failures << " test(s) failed" << std::endl;
    return 1;
  }

  std::cout << "All calculator tests passed" << std::endl;
  return 0;
}