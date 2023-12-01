"""Profiling examle using line_profiler.
"""
from os import chdir
from os.path import dirname, join
from line_profiler import LineProfiler
from dummy_pkg import dummy_module

# Sets the working directory as the one with the code inside
# Without this line line_profiler won't find anything
chdir(join(dirname(__file__), "dummy_pkg"))

lp = LineProfiler()

lp.add_function(dummy_module.dummy_func)
wrap = lp(dummy_module.dummy_func)
wrap()

lp.print_stats()