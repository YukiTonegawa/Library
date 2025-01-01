import os
import sys
import shutil
import subprocess
from tqdm import tqdm

sys.path.append("../")
from lib import core


"""
使い方
$python3 output_generator.py aaa.cpp
$python3 output_generator.py aaa.py

.pyファイル, .cppファイルでない場合エラー

動作
in/xxx.in, in/xxx2.in...を入力としてaaaを実行する
inフォルダ内のin拡張子ファイル全てに対して行われる
out/xxx.outを生成する

"""

# パラメータ
#-------------------------------------------------------------------------------

# 自分のc++コンパイラとオプション
CPP_COMPILER = "g++-13"
CPP_OPTION = ["-std=gnu++17", "-O2"]

# pythonコマンド
PYTHON_INTERPRETER = "python3"

#-------------------------------------------------------------------------------
# 基本的に変えない
INPUT_DIR = '../in'
OUTPUT_DIR = '../out'

# outフォルダ内の過去に作られたファイルを全て消す
def clear_output_folder():
    shutil.rmtree(OUTPUT_DIR)
    os.mkdir(OUTPUT_DIR)

def main():
    if len(sys.argv) != 2:
        invalid_args("python3 output_generator.py aaa.cpp")

    code_path = sys.argv[1]

    s = os.path.splitext(code_path)
    assert len(s) == 2
    is_cpp = (s[1] == '.cpp')
    is_py = (s[1] == '.py')

    if (not is_cpp) and (not is_py):
        core.invalid_args("python3 output_generator.py (aaa.cpp / aaa.py)")
    
    core.find_code(code_path)
    clear_output_folder()

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

    for input_file_name in tqdm(os.listdir(INPUT_DIR)):
        s = os.path.splitext(input_file_name)
        # s = ['xxx', '.in']
        if len(s) != 2 or s[1] != '.in':
            continue

        # input_path: INPUT_DIR/xxx.in
        input_path = INPUT_DIR + '/' + input_file_name

        # output_path: OUTPUT_DIR/xxx.out
        output_path = OUTPUT_DIR + '/' + s[0] + '.out'

        try:
            with open(input_path) as f:
                input_s = f.read()
        except:
            print(input_file_name, 'passed: failed to open the input file')
            continue

        try:
            if is_cpp:
                output_s = subprocess.check_output(['./__tmp_file'], input = input_s, text = True)
            else:
                output_s = subprocess.check_output([PYTHON_INTERPRETER, code_path], input = input_s, text = True)

            with open(output_path, mode = 'w') as f:
                f.write(output_s)
        except:
            print(input_file_name, 'passed: runtime error')

    # 実行可能ファイルを削除
    if os.path.isfile('__tmp_file'):
        os.remove('__tmp_file')

main()
