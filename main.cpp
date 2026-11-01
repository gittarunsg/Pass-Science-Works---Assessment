#include "engine.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <thread>

int main(int argc,char** argv){try{
 if(argc<2){std::cout<<"RA2311003020323 Engineering Workbench\nCommands: fea [csv], inspect <csv|stl>, batch [cases] [elements], nn, devices\n";return 0;}
 std::string cmd=argv[1];
 if(cmd=="fea"){std::string file=argc>2?argv[2]:"results/fea_N40.csv";wb::Rod r;for(int n:{10,20,40,80}){auto s=wb::solve_rod(r,n);std::cout<<"N="<<n<<" max_midpoint_error_C="<<s.max_midpoint_error<<"\n";if(n==40){std::ofstream f(file);f<<"x_m,temperature_C\n";for(auto p:s.nodes)f<<p.x<<','<<p.temperature<<'\n';}}return 0;}
 if(cmd=="inspect"){if(argc<3)throw std::invalid_argument("inspect requires a file path");std::string p=argv[2];if(p.size()>=4&&p.substr(p.size()-4)==".stl")std::cout<<"ASCII STL triangles: "<<wb::count_ascii_stl_triangles(p)<<"\n";else std::cout<<"Temperature samples: "<<wb::read_temperature_csv(p).size()<<"\n";return 0;}
 if(cmd=="batch"){std::size_t count=argc>2?std::stoul(argv[2]):1000;int n=argc>3?std::stoi(argv[3]):80;auto base=wb::run_batch(count,n,2311003,1);unsigned logical=std::max(1u,std::thread::hardware_concurrency());for(unsigned w:{1u,2u,4u}){w=std::min(w,logical);auto begin=std::chrono::steady_clock::now();auto result=wb::run_batch(count,n,2311003,w);double sec=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count();double delta=0;for(std::size_t i=0;i<base.size();++i)delta=std::max(delta,std::abs(base[i]-result[i]));std::cout<<"workers="<<w<<" seconds="<<sec<<" max_delta_C="<<delta<<"\n";}return 0;}
 if(cmd=="nn"){double w=.5,b=0,v=1,c=0;auto g=wb::nn_gradient(w,b,v,c);std::cout<<"loss="<<g.loss<<" prediction="<<g.prediction<<" dw="<<g.dw<<" db="<<g.db<<" dv="<<g.dv<<" dc="<<g.dc<<"\n";std::ofstream f("results/nn_loss.csv");f<<"step,loss\n";f<<"0,"<<g.loss<<'\n';for(int i=1;i<=100;++i){g=wb::nn_gradient(w,b,v,c);w-=.1*g.dw;b-=.1*g.db;v-=.1*g.dv;c-=.1*g.dc;f<<i<<','<<wb::nn_gradient(w,b,v,c).loss<<'\n';}std::cout<<"wrote results/nn_loss.csv\n";return 0;}
 if(cmd=="devices"){for(auto d:wb::mock_devices()){std::cout<<d.kind<<" | "<<d.name<<" | "<<d.voltage<<" | "<<d.connector<<"\n";try{std::cout<<wb::mock_telemetry(d.name,d.kind=="FPGA")<<"\n";}catch(const std::exception& e){std::cout<<e.what()<<"\n";}}return 0;}
 throw std::invalid_argument("unknown command: "+cmd);
 }catch(const std::exception& e){std::cerr<<"error: "<<e.what()<<"\n";return 2;}}
