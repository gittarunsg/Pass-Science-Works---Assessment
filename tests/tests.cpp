#include "engine.hpp"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
int main(){wb::Rod r;double previous=1e9;for(int n:{10,20,40,80}){auto s=wb::solve_rod(r,n);assert(s.nodes.size()==static_cast<std::size_t>(n+1));assert(std::abs(s.nodes.front().temperature-100)<1e-10);assert(std::abs(s.nodes.back().temperature-20)<1e-10);assert(s.max_midpoint_error<previous);previous=s.max_midpoint_error;assert(std::abs(previous-125.0/(n*n))<1e-8);}auto g=wb::nn_gradient(.5,0,1,0);const double e=1e-6;for(int j=0;j<4;++j){double p[4]={.5,0,1,0};p[j]+=e;double plus=wb::nn_gradient(p[0],p[1],p[2],p[3]).loss;p[j]-=2*e;double minus=wb::nn_gradient(p[0],p[1],p[2],p[3]).loss;double analytic[]={g.dw,g.db,g.dv,g.dc};assert(std::abs((plus-minus)/(2*e)-analytic[j])<1e-5);}auto a=wb::run_batch(20,10,7,1),b=wb::run_batch(20,10,7,4);for(std::size_t i=0;i<a.size();++i)assert(std::abs(a[i]-b[i])<=1e-8);assert(wb::mock_devices().size()==3);bool recovered=false;try{(void)wb::mock_telemetry("mock",true);}catch(const std::exception&){recovered=true;}assert(recovered);std::cout<<"all checks passed\n";}
