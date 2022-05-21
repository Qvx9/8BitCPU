#include <iostream>
#include <vector>
#include <memory>
//8Bit Register Based MVC CPU. | By: Qvx9 
typedef uint8_t Byte;
static const Byte MAX = 0xFF;

using std::clog;
using std::cerr;
using std::endl;
using std::vector;

template <class T>
class CPU{
  Byte 
  register0, 
  register1, 
  instructionRegister, 
  programCounter;
  
  T temporaryRegister;
  bool status, overflow, underflow;
  
  CPU(const CPU&){}
  
  void reset(){
    register0 = register1 = instructionRegister = programCounter = 0;
    temporaryRegister = 0;
    status = true, overflow = false, underflow = false;
  }
  
  void fault(){
    status=false;
    cerr<<"FAULT"<<endl
    <<"           Register0: "<<register0<<endl
    <<"           Register1: "<<register1<<endl
    <<"Instruction Register: "<<instructionRegister<<endl
    <<"     Program Counter: "<<programCounter<<endl
    <<"  Temporary Register: "<<temporaryRegister<<endl
    <<"              Status: "<<status<<endl
    <<"            Overflow: "<<overflow<<endl
    <<"           Underflow: "<<underflow<<endl;
  }
  
  void load0(const vector<Byte>& program){
    register0 = program[programCounter];
    programCounter++;
  }
  void load1(const vector<Byte>& program){
    register1 = program[programCounter];
    programCounter++;
  }
  void store0(vector<Byte>& program){
    program[programCounter] = register0;
    programCounter++;
  }
  void store1(vector<Byte>& program){
    program[programCounter] = register1;
    programCounter++;
  }
  
  void multiply(){
    temporaryRegister = register0 * register1;
    if(temporaryRegister > MAX){
      overflow = true;
      temporaryRegister = MAX;
    }
    register0 = temporaryRegister;
  }
  
  void add(){
    temporaryRegister = register0 + register1;
    if(temporaryRegister > MAX){
      overflow = true;
      temporaryRegister = MAX;
    }
    register0 = temporaryRegister;
  }
  
  void subtract(){
    temporaryRegister = register0 - register1;
    if(temporaryRegister < 0){
      underflow = true;
      temporaryRegister = 0;
    }
    register0 = temporaryRegister;
  }
  
public:
  CPU(){reset();}
  enum InstructionSet{
    LOAD0=0,LOAD1,
    STORE0,STORE1,
    MULTIPLY,ADD,SUBTRACT
  };
  void execute(vector<Byte>& program, const bool overflow=true, const bool underflow=true){
    reset();
    if(program.size() > MAX){
      cerr<<"Program cannot exceed "<<MAX<<endl;
      return;
    }else{
      while(programCounter < program.size()){
        instructionRegister = program[programCounter];
        programCounter++;
        switch(instructionRegister){
          case LOAD0:
            load0(program);
            break;
          case LOAD1:
            load1(program);
            break;
          case STORE0:
            store0(program);
            break;
          case STORE1:
            store1(program);
            break;
          case ADD:
            add();
            break;
          case SUBTRACT:
            subtract();
            break;
          case MULTIPLY:
            multiply();
            break;
          default:
            fault();
            return;
        }
      }
    }
  }
};

class View{
  std::unique_ptr<CPU<int>> cpu {std::make_unique<CPU<int>>()};
  vector<Byte> x;
public:
  void load0(const char& arg){
    x.push_back(cpu->LOAD0);
    x.push_back(arg);
  }
  
  void load1(const char& arg){
    x.push_back(cpu->LOAD1);
    x.push_back(arg);
  }

  void multiply(){
    x.push_back(cpu->MULTIPLY);
    x.push_back(cpu->STORE0);
    x.push_back(MAX);
  }
  
  void add(){
    x.push_back(cpu->ADD);
    x.push_back(cpu->STORE0);
    x.push_back(MAX);
  }
  
  void subtract(){
    x.push_back(cpu->SUBTRACT);
    x.push_back(cpu->STORE0);
    x.push_back(MAX);
  }
  
  void print(){
    cpu->execute(x);
    vector<Byte>::const_iterator c = x.begin();
    for(;c!=x.end(); c++) 
      clog<<'['<<static_cast<int>(*c)<<']'<<endl;
  }
};

class Controller{
  View view;
public:
  Controller(){}
  void load0(const char& arg){ view.load0(arg); }
  void load1(const char& arg){ view.load1(arg); }
  void multiply(){ view.multiply(); }
  void add(){ view.add(); }
  void subtract(){ view.subtract(); }
  void result(){ view.print(); }
};

int main(int argc, char** argv){
  Controller controller;
  controller.load0(3);
  controller.load1(3);
  controller.multiply();
  controller.result();
  return 0;
}
