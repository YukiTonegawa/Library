import os
import sys
import shutil
import subprocess

# argsの形式がおかしい場合
def invalid_args(MESSAGE):
    print('Error: invalid arguments')
    print("Please use like this")
    print(MESSAGE)
    exit(0)


# check c++ compiler
def check_cpp_compiler(__CPP_COMPILER):
    try:
        compiler_check = str(subprocess.check_output([__CPP_COMPILER, "--version"], stderr=subprocess.PIPE))
        is_gcc = ('GCC' in compiler_check) or ('gcc' in compiler_check)
        is_clang = ('clang' in compiler_check)

        if (not is_gcc) and (not is_clang):
            print('Error: invalid c++ compiler')
            exit(0)
    except:
        print('Error: invalid c++ compiler')
        exit(0)


# check python compiler
def check_python_interpreter(__PYTHON_INTERPRETER):
    try:
        compiler_check = str(subprocess.check_output([__PYTHON_INTERPRETER, "--version"], stderr=subprocess.PIPE))
        is_ok = ('Pypy' in compiler_check) or ('Python' in compiler_check)
        if not is_ok:
            print('Error: invalid python interpreter')
            exit(0)
    except:
        print('Error: invalid python interpreter')
        exit(0)

# code_pathが存在するか
def find_code(code_path):
    if not os.path.isfile(code_path):
        print('Error: there is no file named ' + r'"' + code_path +  r'"')
        exit(0)



