import os
import sys
import shutil
import subprocess
from tqdm import tqdm

sys.path.append("../")
from lib import core

"""
使い方
$python3 input_generator.py

入力(標準入力)
xxx(ケース名) yyy(実行する回数)
xxx2...     yyy2
...

動作
gen/xxx, gen/xxx2...を実行してin/xxx.in, in/xxx2.in...を生成する

xxxは.cppまたは.pyで終わる
"""

# パラメータ
#-------------------------------------------------------------------------------

# 自分のc++コンパイラとオプション
CPP_COMPILER = "g++-13"
CPP_OPTION = ["-std=gnu++17", "-O2"]

# pythonコマンド
PYTHON_INTERPRETER = "python3"
#-------------------------------------------------------------------------------
# inフォルダ, 基本的に変えない
INPUT_DIR = '../in'

# gen/case_name.cppを探す. ある場合は 文字列 "gen/case_name.cpp", ない場合は"not found"を返す
def find_gen_file(test_case_name):
    gen_folder_name = '../gen/'
    file_path = gen_folder_name + test_case_name
    if os.path.isfile(file_path):
        return file_path
    else:
        return 'not found'

def format_error(test_case_name, reason):
    print('Test case named ' + r'"' + test_case_name +  r'"' + ' was passed:', end = ' ')
    print(reason)

# inフォルダ内の過去に作られたファイルを全て消す
def clear_input_folder():
    shutil.rmtree(INPUT_DIR)
    os.mkdir(INPUT_DIR)

# (テストケース名前, 回数)のフォーマットに従っているか
def check_format(S):
    if len(S) != 2:
        format_error(S, 'invalid test case name')
        return False
    
    test_case_name, execute_num = S

    # .cpp or .py
    s = os.path.splitext(test_case_name)
    valid_code_name = True
    is_cpp, is_py = False, False
    if len(s) != 2:
        valid_code_name = False
    else:
        is_cpp = (s[1] == '.cpp')
        is_py = (s[1] == '.py')
        if (not is_cpp) and (not is_py):
            valid_code_name = False
    if not valid_code_name:
        format_error(test_case_name, 'invalid test case name')
        return False
    
    # check exe_num
    if (not execute_num.isdecimal()) or int(execute_num) < 0:
        format_error(test_case_name, 'invalid test case number')
        return False

    # check test_case_name
    path_test_case = find_gen_file(test_case_name)
    if path_test_case == 'not found':
        format_error(test_case_name, 'generator was not found')
        return False
    
    return True


def main():
    clear_input_folder()
    is_ok_cpp = False
    is_ok_py = False
    while True:
        try:
            tmp_str = input().split()
            print(tmp_str)
            if len(tmp_str) == 0 or not check_format(tmp_str):
                continue
            test_case_name, execute_num = tmp_str
            execute_num = int(execute_num)

            s = os.path.splitext(test_case_name)
            is_cpp = (s[1] == '.cpp')
            is_py = (s[1] == '.py')

            # check_formatで確認済
            assert is_cpp or is_py
            
            path_code = find_gen_file(test_case_name)
            if is_cpp:
                if not is_ok_cpp:
                    core.check_cpp_compiler(CPP_COMPILER)
                    is_ok_cpp = True
                # 一時的なファイル__tmp_fileを生成
                try:
                    subprocess.check_output([CPP_COMPILER, *CPP_OPTION, path_code, '-o' '__tmp_file'], stderr=subprocess.PIPE)
                except:
                    format_error(test_case_name, 'compile failed')
                    continue

            elif is_py:
                if not is_ok_py:
                    core.check_python_interpreter(PYTHON_INTERPRETER)
                    is_ok_py = True
            
            test_case_name = s[0] + '_' + ('py' if is_py else 'cpp')
            try:
                keta = len(str(execute_num - 1))
                for i in tqdm(range(execute_num)):
                    case_path = INPUT_DIR + '/' + test_case_name + '_' + str(i).zfill(keta) + '.in'
                    
                    if is_cpp:
                        output_s = subprocess.check_output(['./__tmp_file'], text = True)
                    else:
                        output_s = subprocess.check_output([PYTHON_INTERPRETER, path_code], text = True)

                    with open(case_path, mode = 'w') as f:
                        f.write(output_s)
            except:
                format_error(test_case_name, 'runtime error')
            
        except:
            # EOF
            # 実行可能ファイルを削除
            if os.path.isfile('__tmp_file'):
                os.remove('__tmp_file')
            exit(0)

main()
