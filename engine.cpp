#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <random>
#include <sstream>
#include <stdexcept>
#include <thread>

namespace wb {
double exact_temperature(const Rod& r,double x){return r.left+(r.right-r.left)*x/r.length+(r.generation/(2*r.conductivity))*x*(r.length-x);}
SolveResult solve_rod(const Rod& r,int n){
 if(n<1||r.length<=0||r.area<=0||r.conductivity<=0) throw std::invalid_argument("positive length, area, conductivity and element count are required");
 const double h=r.length/n, diag=r.conductivity*r.area/h, off=-diag, load=r.generation*r.area*h/2;
 std::vector<double> d(n+1,0), rhs(n+1,0);
 for(int e=0;e<n;++e){ d[e]+=diag; d[e+1]+=diag; rhs[e]+=load; rhs[e+1]+=load; }
 // Eliminate prescribed endpoint temperatures from the interior system.
 std::vector<double> lower(n+1,off), upper(n+1,off);
 if(n>1){rhs[1]-=off*r.left;lower[1]=0;rhs[n-1]-=off*r.right;upper[n-1]=0;}
 d[0]=1; rhs[0]=r.left; upper[0]=0; d[n]=1; rhs[n]=r.right; lower[n]=0;
 // Thomas solve for the tridiagonal assembled system.
 for(int i=1;i<=n;++i){const double m=lower[i]/d[i-1]; d[i]-=m*upper[i-1]; rhs[i]-=m*rhs[i-1];}
 std::vector<double> t(n+1); t[n]=rhs[n]/d[n]; for(int i=n-1;i>=0;--i)t[i]=(rhs[i]-upper[i]*t[i+1])/d[i];
 SolveResult out; out.nodes.reserve(n+1); for(int i=0;i<=n;++i)out.nodes.push_back({i*h,t[i]});
 for(int e=0;e<n;++e){double x=(e+.5)*h, interp=(t[e]+t[e+1])/2; out.max_midpoint_error=std::max(out.max_midpoint_error,std::abs(interp-exact_temperature(r,x)));}
 return out;
}
static std::vector<std::string> split(const std::string& s){std::vector<std::string> a;std::stringstream ss(s);std::string x;while(std::getline(ss,x,','))a.push_back(x);return a;}
std::vector<Sample> read_temperature_csv(const std::string& path){
 std::ifstream f(path); if(!f)throw std::runtime_error("cannot open CSV: "+path); std::string line; if(!std::getline(f,line))throw std::runtime_error("empty CSV");
 auto head=split(line); const std::vector<std::string> expected={"node_id","x_m","y_m","z_m","temperature_C"};if(head!=expected)throw std::runtime_error("expected header node_id,x_m,y_m,z_m,temperature_C");
 std::vector<Sample> out; std::size_t row=1; while(std::getline(f,line)){++row;if(line.empty())continue;auto c=split(line);if(c.size()!=5)throw std::runtime_error("row "+std::to_string(row)+": expected 5 columns");
  try{Sample s;s.id=std::stoll(c[0]);s.x=std::stod(c[1]);s.y=std::stod(c[2]);s.z=std::stod(c[3]);s.temperature=std::stod(c[4]);if(!std::isfinite(s.x)||!std::isfinite(s.y)||!std::isfinite(s.z)||!std::isfinite(s.temperature))throw std::runtime_error("non-finite value");out.push_back(s);}catch(const std::exception&){throw std::runtime_error("row "+std::to_string(row)+": invalid or non-finite value");}}
 if(out.empty())throw std::runtime_error("CSV contains no data rows");return out;
}
std::size_t count_ascii_stl_triangles(const std::string& path){std::ifstream f(path);if(!f)throw std::runtime_error("cannot open STL: "+path);std::string line;std::size_t n=0;bool ascii=false;while(std::getline(f,line)){if(line.find("solid ")==0)ascii=true;if(line.find("facet normal ")==0)++n;}if(!ascii||n==0)throw std::runtime_error("expected non-empty ASCII STL");return n;}
std::vector<double> run_batch(std::size_t count,int n,unsigned seed,unsigned workers){if(count==0||workers==0)throw std::invalid_argument("case count and worker count must be positive");std::mt19937 gen(seed);std::uniform_real_distribution<double> kd(1,100),qd(100,20000);std::vector<Rod> rods(count);for(auto& r:rods){r.conductivity=kd(gen);r.generation=qd(gen);}std::vector<double> out(count);std::size_t next=0;std::mutex m;auto work=[&]{for(;;){std::size_t i;{std::lock_guard<std::mutex> l(m);if(next==count)return;i=next++;}out[i]=solve_rod(rods[i],n).nodes[n/2].temperature;}};std::vector<std::thread> pool;for(unsigned i=0;i<std::min<unsigned>(workers,static_cast<unsigned>(count));++i)pool.emplace_back(work);for(auto& t:pool)t.join();return out;}
Gradient nn_gradient(double w,double b,double v,double c,double x,double y){Gradient g;g.hidden=std::tanh(w*x+b);g.prediction=v*g.hidden+c;double e=g.prediction-y;g.loss=.5*e*e;g.dv=e*g.hidden;g.dc=e;double da=e*v*(1-g.hidden*g.hidden);g.dw=da*x;g.db=da;return g;}
std::vector<Device> mock_devices(){return {{"Microcontroller","MCU mock","3.3 V I/O","USB-C / UART"},{"FPGA","FPGA mock","1.8 V bank (assumed)","USB-JTAG"},{"JTAG adapter","Debug adapter mock","target-referenced","10-pin ARM-style (assumed)"}};}
std::string mock_telemetry(const std::string& name,bool fail){if(fail)throw std::runtime_error("simulated timeout; reconnect and retry");return name+": {\"temperature_C\": 23.5, \"status\": \"simulated\"}";}
}
