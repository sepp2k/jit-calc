#!/bin/sh

# Compile
g++ -O2 -Wall -pedantic -DNDEBUG -Wno-return-type eval_parse.cc -o eval_parse
g++ -O2 -Wall -pedantic -DNDEBUG -Wno-return-type eval_ast.cc -o eval_ast
g++ -O2 -Wall -pedantic -DNDEBUG -Wno-return-type stack_vm.cc -o stack_vm
g++ -O2 -Wall -pedantic -DNDEBUG -Wno-return-type jit.cc -o jit -llightning

# Helper definitions
N=$1
input() {
    echo $1
    seq 0 $N
}

run() {
    echo $2
    input $1 | time -p ./$2
    echo
}

run_bench() {
    echo "==== $1 ===="
    run $1 eval_parse
    run $1 eval_ast
    run $1 stack_vm
    run $1 jit
    echo
}

run_bench 'x+1'
run_bench 'x+(x+1)*x*2-23+x*x*x*x*(x+2*x-x/3)+x*(x+5)*(x+10)*x'
run_bench 'x+(x+1)*x*2-23+x*x*x*x*(x+2*x-x/3)+x*(x+5)*(x+10)*x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x'