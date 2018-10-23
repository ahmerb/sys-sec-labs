import subprocess
from subprocess import run, Popen, PIPE, STDOUT
from sys import exit

# session = subprocess.Popen(['access.sh'], stdout=PIPE, stderr=PIPE)
# stdout, stderr = session.communicate()

for i in range(10):
    for j in range(10):
        for k in range(10):
            p = Popen(['./access'], stdout=PIPE, stdin=PIPE, stderr=STDOUT)
            guess = i*1000 + j*100 + k*10 + 8
            str_input = f'david\n{guess}\n'
            stdout = p.communicate(input=str.encode(str_input))[0]
            str_output = stdout.decode()
            print(str_output)
            if "S" in str_output: # S is in 'Success.' but not 'Incorrect.' # substring search unnecessarily slow but only 1000 combinations to check anyway
                print(f'Found david\'s PIN: {guess}')
                exit(0)

# if didn't find pin, we failed
exit(1)
