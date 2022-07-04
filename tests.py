from subprocess import getoutput
from os import listdir, path
from binascii import hexlify
import sys

tests_folder = "tests"
passed = []

def run_test(test_name: str):
    awaited = ""

    awaited_path = path.join(tests_folder, test_name + '.txt')
    test_path = path.join(tests_folder, test_name + '.corth')
    with open(awaited_path, 'r') as file:
        awaited = file.read()
    file.close()
    sim_test_result = getoutput("./corth sim %s" % (test_path))
    compilation_command = "./corth com {0}/{1}.corth && nasm -felf64 -o {0}/{1}.o output.asm && ld {0}/{1}.o -o {0}/{1} && ./{0}/{1}".format(tests_folder, test_name)
    print(compilation_command)
    com_test_result = getoutput(compilation_command)
    if awaited != sim_test_result:
        print("The test %s didn't pass in simulation mode." % (test_name), file=sys.stderr)
    if awaited != com_test_result:
        print("The test %s didn't pass in compilation mode." % (test_name), file=sys.stderr)

for test in listdir(tests_folder):
    test_name = test.split('.')[0]
    if not test.endswith(".corth"):
        continue
    if test in passed:
        continue
    run_test(test_name)
    passed.append(test_name)
