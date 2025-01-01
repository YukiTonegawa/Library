import os
import sys
import shutil
import subprocess
import time
from tqdm import tqdm
sys.path.append("../")
from lib import core

"""
使い方
$python3 checker.py aaa.cpp
$python3 checker.py aaa.py

.pyファイル, .cppファイルでない場合エラー

動作
in/xxx.in, in/xxx2.in...を入力としてaaaを実行する
inフォルダ内のin拡張子ファイル全てに対してコードを実行, out/xxx.outが存在するなら正答かどうかの判定を返す

"""

#判定
#-------------------------------------------------------------------------------

# 2つのテキストが完全一致する時にAC
def judge_perfect_match(a, b):
    return a == b

# パラメータ
#-------------------------------------------------------------------------------

# 自分のc++コンパイラとオプション
CPP_COMPILER = "g++-13"
CPP_OPTION = ["-std=gnu++17", "-O2"]

# pythonコマンド
PYTHON_INTERPRETER = "python3"

# 判定関数を指定
JUDGE_FUNC = judge_perfect_match

#-------------------------------------------------------------------------------
# 基本的に変えない
INPUT_DIR = '../in'
OUTPUT_DIR = '../out'

def main():
    if len(sys.argv) != 2:
        invalid_args("python3 checker.py aaa.cpp")

    code_path = sys.argv[1]

    s = os.path.splitext(code_path)
    assert len(s) == 2
    is_cpp = (s[1] == '.cpp')
    is_py = (s[1] == '.py')

    if (not is_cpp) and (not is_py):
        invalid_args("python3 checker.py (aaa.cpp / aaa.py)")

    core.find_code(code_path)

    if is_cpp:
        core.check_cpp_compiler(CPP_COMPILER)

        # compile
        # 一時的なファイル__tmp_fileを生成
        try:
            subprocess.check_output([CPP_COMPILER, *CPP_OPTION, code_path, '-o' '__tmp_file'], stderr=subprocess.PIPE)
        except:
            assert False, 'compile failed'
            exit(0)
    elif is_py:
        core.check_python_interpreter(PYTHON_INTERPRETER)

    is_first_execute = True
    
    for input_file_name in os.listdir(INPUT_DIR):
        s = os.path.splitext(input_file_name)
        # s = ['xxx', '.in']
        if len(s) != 2 or s[1] != '.in':
            continue

        # input_path: INPUT_DIR/xxx.in
        input_path = INPUT_DIR + '/' + input_file_name

        # output_path: OUTPUT_DIR/xxx.out
        output_path = OUTPUT_DIR + '/' + s[0] + '.out'

        print('case: ' + input_file_name)

        try:
            with open(input_path) as f:
                input_s = f.read()
        except:
            print('verdict: failed to open the input file')
            print()
            continue

        try:
            # 初回実行時に遅くなる場合があるため
            if is_first_execute:
                if is_cpp:
                    answer_s = subprocess.check_output(['./__tmp_file'], input = input_s, text = True)
                else:
                    answer_s = subprocess.check_output([PYTHON_INTERPRETER, code_path], input = input_s, text = True)
                is_first_execute = False

            __time1 = time.time()

            if is_cpp:
                answer_s = subprocess.check_output(['./__tmp_file'], input = input_s, text = True)
            else:
                answer_s = subprocess.check_output([PYTHON_INTERPRETER, code_path], input = input_s, text = True)

            __time_elapsed = (time.time() - __time1) * 1000

            with open(output_path) as f:
                correct_s = f.read()
                if JUDGE_FUNC(answer_s, correct_s):
                    print('verdict: AC  time: ' + format(__time_elapsed, '.2f') + 'ms')
                    print()
                else:
                    print('verdict: WA  time: ' + format(__time_elapsed, '.2f') + 'ms')
                    print()
        except:
            print('verdict: RE')
            print()

    # 実行可能ファイルを削除
    if os.path.isfile('__tmp_file'):
        os.remove('__tmp_file')

main()
