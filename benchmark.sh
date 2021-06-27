#!/bin/zsh

# Compile
g++ -O2 -Wall -pedantic eval_parse.cc -o eval_parse
g++ -O2 -Wall -pedantic eval_ast_virtual.cc -o eval_ast
g++ -O2 -Wall -pedantic jit.cc -o jit -llightning

# Helper definitions
N=$1
input() {
    echo $1
    seq 0 $N
}

run_bench() {
    echo "==== $1 ===="
    echo "eval_parse"
    input $1 | time ./eval_parse
    echo "eval_ast"
    input $1 | time ./eval_ast
    echo "jit"
    input $1 | time ./jit
    echo
}

run_bench 'x+1'
run_bench 'x+(x+1)*x*2-23+x*x*x*x*(x+2*x-x/3)+x*(x+5)*(x+10)*x'
run_bench 'x+(x+1)*x*2-23+x*x*x*x*(x+2*x-x/3)+x*(x+5)*(x+10)*x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x'