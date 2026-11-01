#pragma once
#include <cstddef>
#include <string>
#include <vector>

namespace wb {
struct Rod { double length=1, area=1e-4, conductivity=10, generation=10000, left=100, right=20; };
struct Node { double x=0, temperature=0; };
struct SolveResult { std::vector<Node> nodes; double max_midpoint_error=0; };
struct Sample { double x=0,y=0,z=0,temperature=0; long long id=0; };
struct Gradient { double loss=0, dw=0, db=0, dv=0, dc=0, prediction=0, hidden=0; };
struct Device { std::string kind,name,voltage,connector; };
SolveResult solve_rod(const Rod&, int elements);
double exact_temperature(const Rod&, double x);
std::vector<Sample> read_temperature_csv(const std::string& path);
std::size_t count_ascii_stl_triangles(const std::string& path);
std::vector<double> run_batch(std::size_t cases, int elements, unsigned seed, unsigned workers);
Gradient nn_gradient(double w,double b,double v,double c,double x=1,double y=2);
std::vector<Device> mock_devices();
std::string mock_telemetry(const std::string& device, bool fail=false);
}
