//===- saber.cpp -- Source-sink bug checker------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013-2017>  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // Saber: Software Bug Check.
 //
 // Author: Yulei Sui,
 */

#include "SVF-FE/LLVMUtil.h"
#include "SABER/LeakChecker.h"
#include "SABER/FileChecker.h"
#include "SABER/DpdChecker.h"
#include "SABER/DoubleFreeChecker.h"
#include "Util/Options.h"
#include "Util/Z3Expr.h"


using namespace llvm;
using namespace SVF;

static llvm::cl::opt<std::string> InputFilename(cl::Positional,
        llvm::cl::desc("<input bitcode>"), llvm::cl::init("-"));

static llvm::cl::opt<bool> LEAKCHECKER("leak", llvm::cl::init(false),
                                       llvm::cl::desc("Memory Leak Detection"));

static llvm::cl::opt<bool> FILECHECKER("fileck", llvm::cl::init(false),
                                       llvm::cl::desc("File Open/Close Detection"));

static llvm::cl::opt<bool> DFREECHECKER("dfree", llvm::cl::init(false),
                                        llvm::cl::desc("Double Free Detection"));

static llvm::cl::opt<bool> DPDCHECKER("dangling", llvm::cl::init(false),
                                        llvm::cl::desc("Dangling pointer detecter"));

int main(int argc, char ** argv)
{

    int arg_num = 0;
    char **arg_value = new char*[argc];
    std::vector<std::string> moduleNameVec;
    LLVMUtil::processArguments(argc, argv, arg_num, arg_value, moduleNameVec);
    cl::ParseCommandLineOptions(arg_num, arg_value,
                                "Source-Sink Bug Detector\n");

    if (Options::WriteAnder == "ir_annotator")
    {
        LLVMModuleSet::getLLVMModuleSet()->preProcessBCs(moduleNameVec);
    }

    SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
    svfModule->buildSymbolTableInfo();

    if(LEAKCHECKER){
        LeakChecker *saber = new LeakChecker();
        saber->runOnModule(svfModule);
      }
    else if(FILECHECKER){
        LeakChecker *saber = new FileChecker();
        saber->runOnModule(svfModule);
      }
    else if(DFREECHECKER){
        LeakChecker *saber = new DoubleFreeChecker();
        saber->runOnModule(svfModule);
      }
    else if(DPDCHECKER) {
        DpdChecker *saber = new DpdChecker();
        saber->runOnModule(svfModule);
      }
    else{
        LeakChecker *saber = new LeakChecker();  // if no checker is specified, we use leak checker as the default one.
        saber->runOnModule(svfModule);
      }

    return 0;

}
