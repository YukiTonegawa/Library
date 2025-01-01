import os
import sys
import shutil
import subprocess
from tqdm import tqdm

sys.path.append("../")
from lib import core

"""
使い方
$python3 interactive_tester.py solution.cpp judge.cpp

judge, solutionは .cpp ファイル

動作
"""

# パラメータ
#-------------------------------------------------------------------------------

# 自分のc++コンパイラとオプション
CPP_COMPILER = "g++-13"
CPP_OPTION = ["-std=gnu++17", "-O2"]

#-------------------------------------------------------------------------------

def main():
    if len(sys.argv) != 3:
        invalid_args("python3 interactive_tester.py solution.cpp judge.cpp")
    
    sol = sys.argv[1]
    s_sol = os.path.splitext(sol)
    assert len(s_sol) == 2
    is_cpp_sol = (s_sol[1] == '.cpp')
    judge = sys.argv[2]
    s_judge = os.path.splitext(judge)
    assert len(s_judge) == 2
    is_cpp_judge = (s_judge[1] == '.cpp')

    assert is_cpp_sol and is_cpp_judge


    core.check_cpp_compiler(CPP_COMPILER)

    try:
        subprocess.check_output([CPP_COMPILER, *CPP_OPTION, sol, '-o' '__tmp_file_sol'], stderr=subprocess.PIPE)
    except:
        assert False, 'compile ' + sol + ' failed'
        exit(0)
    
    try:
        subprocess.check_output([CPP_COMPILER, *CPP_OPTION, judge, '-o' '__tmp_file_judge'], stderr=subprocess.PIPE)
    except:
        if os.path.isfile('__tmp_file_sol'):
            os.remove('__tmp_file_sol')
        assert False, 'compile ' + judge + ' failed'
        exit(0)
    
    
    try:
        cmd = "rm fifo && mkfifo fifo && (./__tmp_file_sol < fifo) 1>&2  | ./__tmp_file_judge > fifo 1>&2"
        result = subprocess.run(cmd, shell=True)
        #print(result.stderr)
    except:
        pass

    if os.path.isfile('fifo'):
        os.remove('fifo')
    if os.path.isfile('__tmp_file_sol'):
        os.remove('__tmp_file_sol')
    if os.path.isfile('__tmp_file_judge'):
        os.remove('__tmp_file_judge')



    
main()
    


