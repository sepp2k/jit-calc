# Calculator Comparison

## Explanation

This is a comparison between the following four approaches to evaluate simple arithmetic expressions:

1. Evaluate while parsing (eval_parse.cc)
2. Create an AST while parsing and then evaluate by walking the AST (eval_ast.cc)
3. Create stack-based bytecode while parsing and then evaluate by interpreting the bytecode (stack_vm.cc)
4. Create native machine code while parsing and then evaluate by running the machine code (jit.cc)

The scenario under consideration is that the user enters an expression once and it is then evaluated multiple times using different input values. If an expression is only evaluated once, then of course no approach other than 1 makes any sense.

The expressions can consist of positive integer numbers, the four basic infix operators (+, -, \+, /) and the variable `x`. No other variable names are supported. Unary operators or negative integer constants are also not supported (to get negative 42, you have to write `0 - 42`).

All implementations use the same parsing algorithm (shunting yard) implemented in `Parser.hh`. All implementations expect the input to contain an expression on the first line. The following lines of input should then contain values for `x` that can be separated by spaces or newlines. The expression will be evaluated for each of the input values, the results of the evaluation will be summed and the sum will be printed after all inputs have been processed.

The implementation for approach 4 uses GNU lightning to JIT compile the expressions to native code. The others have no dependency beyond standard C++.

Usage of the benchmark script: `./benchmark.sh 10000000` where the argument is the number of input values that are fed to each expression.

None of the implementations perform except 4 perform any types of optimizations on the expression (such as rewriting `x+x+x+x+x` to `x*5`). The JIT compiler does perform a limited amount of constant folding to make the implementation easier (because there are no arithmetic instructions that take two constants as arguments), but this does not come into play for the expressions that are being tested.

## Results

Comparing the approaches using a simple expression (`x+1`), a complicated expression (`x+(x+1)*x*2-23+x*x*x*x*(x+2*x-x/3)+x*(x+5)*(x+10)*x`) and an even more complicated expression (`x+(x+1)*x*2-23+x*x*x*x*(x+2*x-x/3)+x*(x+5)*(x+10)*x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x`), I got the following results using 10000000 input values for each run:

For the simple expression, approach one took 2.84s, two took 2.25s, three 2.39s and four 2.21s. So evaluating while parsing is noticeably slower than the other three, but not by an extreme amount. And the other three are mostly the same with the bytecode solution being the slowest of the three.

For the more complicated expression approach one took 5.54s, two took 2.75s, three 2.68s (now being a bit faster than two) and four 2.23s. So the first approach significantly slowed down as the expression got more complex, but the other three were less affected, with solution four being the least affected (if at all). So the other three significantly increased their lead over solution one, now being at least twice as fast as solution one. And solution four also increased its lead over all other solutions, now being 17% faster than the 2nd fasted solution and 60% faster than solution one.

For the most complicated expression approach one took 6.54s, two took 3.21s, three 2.90s (now being a bit faster than two) and four 2.22s. So again solution one slowed down significantly (one second), two and three less significantly and solution four not at all. Solution 3 also increased its lead of solution 2 a bit. Solution 4 is now 25% faster than solution 3 and 76% faster than solution 1, making it the clear winner for complicated expressions.
