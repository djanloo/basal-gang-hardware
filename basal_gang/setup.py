from setuptools import Extension, setup
import os
import argparse
from Cython.Distutils import build_ext
from Cython.Compiler.Options import get_directive_defaults
from Cython.Build import cythonize

import numpy as np
from rich import print
import pickle
from tabulate import tabulate
import pandas as pd

MAKE_INFO_FILE = "make.info"


def compare_make_infos(new_make_infos, summary):
    try:
        with open(MAKE_INFO_FILE, 'rb') as infof:
            make_infos = pickle.load(infof)
    except FileNotFoundError or EOFError:
        print("MakeInfo file not found")
        with open(MAKE_INFO_FILE, 'wb') as infof:
            pickle.dump({},infof)
        for file in new_make_infos.keys():
            summary[file]['compilation diff'] = True
        return
    else:
        for file in new_make_infos.keys():
            try:
                summary[file]['mode'] = make_infos[file]['djanloo_compile_mode']
            except KeyError:
                summary[file]['mode'] = "?"
            try:
                if make_infos[file] != new_make_infos[file]:
                    summary[file]['compilation diff'] = True 
                else:
                    summary[file]['compilation diff'] = False 
            except KeyError:
                summary[file]['compilation diff'] = True 
                pass


def update_make_infos(new_make_infos):
    try:
        with open(MAKE_INFO_FILE, 'rb') as infof:
            make_infos = pickle.load(infof)

    except FileNotFoundError and EOFError:
        print("MakeInfo file not found")
        with open(MAKE_INFO_FILE, 'wb') as infof:
            pickle.dump(dict(), infof)
            make_infos = dict()

    make_infos.update(new_make_infos)

    with open(MAKE_INFO_FILE, 'wb') as infof:
        pickle.dump(make_infos, infof)


def get_files_and_timestamp( extension):
    return {file.split('.')[0]:os.path.getmtime(file) for file in os.listdir(".") if file.endswith(extension)}

parser = argparse.ArgumentParser()
parser.add_argument('--profile', action='store_true')
parser.add_argument('--notrace', action='store_true')
parser.add_argument('--hardcore', action='store_true')

args = parser.parse_args()

# Set the working directory
old_dir = os.getcwd()
packageDir = os.path.dirname(__file__)
includedDir = [".", packageDir, np.get_include()]
os.chdir(packageDir)
print(f"Include dirs are {includedDir}")

extension_kwargs = dict( 
        include_dirs=includedDir,
        libraries=["m"],                       # Unix-like specific link to C math libraries
        extra_compile_args=["-fopenmp", "-O3"],# Links OpenMP for parallel computing
        extra_link_args=["-fopenmp"],
        define_macros= [('NPY_NO_DEPRECATED_API','NPY_1_7_API_VERSION')] #Silences npy deprecated warn
        )

cython_compiler_directives = get_directive_defaults()
cython_compiler_directives['language_level'] = "3"
cython_compiler_directives['warn'] = True
# cython_compiler_directives['warn.undeclared'] = True 
# cython_compiler_directives['warn.maybe_uninitialized'] = True
# cython_compiler_directives['warn.unused'] = True

# This is for me
cython_compiler_directives['djanloo_compile_mode'] = 'default'

# Profiling using line_profiler
if args.profile:
    print("[blue]Compiling in [green]PROFILE[/green] mode[/blue]")
    cython_compiler_directives['profile'] = True
    cython_compiler_directives['linetrace'] = True
    cython_compiler_directives['binding'] = True

    # Activates profiling
    extension_kwargs["define_macros"].append(('CYTHON_TRACE', '1'))
    extension_kwargs['define_macros'].append(('CYTHON_TRACE_NOGIL', '1'))

    # This is for me
    cython_compiler_directives['djanloo_compile_mode'] = 'profile'

# Globally boost speed by disabling checks
# see https://cython.readthedocs.io/en/latest/src/userguide/source_files_and_compilation.html#compiler-directives
if args.hardcore:
    print("[blue]Compiling in [green]HARDCORE[/green] mode[/blue]")
    cython_compiler_directives['boundscheck'] = False
    cython_compiler_directives['cdivision'] = True
    cython_compiler_directives['wraparound'] = False
    
    # This is for me
    cython_compiler_directives['djanloo_compile_mode'] = 'hardcore'

#################################################################

cython_files = get_files_and_timestamp(".pyx")
c_files = get_files_and_timestamp(".c")
c_files.update(get_files_and_timestamp(".cpp"))

all_files = sorted(list(set(c_files.keys()) | set(cython_files.keys())))

summary = dict()
###################### FILE VERSION COMPARISON ##################
for file in all_files:
    summary[file] = dict()
    try:
        cython_files[file]
    except KeyError:
        print(f"[red]C file {file:30} has no .pyx parent[/red]")
        continue
    try:
        c_files[file]
    except KeyError:
        summary[file]['edited'] = True
    else:
        if cython_files[file] >= c_files[file]:
            summary[file]['edited'] = True
        else:
            summary[file]['edited'] = False

################### UPDATE MAKE INFOS ##########################

new_make_infos = {f:cython_compiler_directives for f in summary.keys()}
compare_make_infos(new_make_infos, summary)

df = pd.DataFrame(summary).T
print(df)
################### GENERATION OF PXD FILES ###############
for file in cython_files.keys():
    declarations = []
    with open(f"{file}.pyx", "r") as codefile:
        for line in codefile:
            line_clean = line.strip()
            if line_clean.startswith("#"):
                continue
            if 'extern' in line:
                continue
            if line_clean.startswith('cdef') or \
                line_clean.startswith("cpdef"):
                if line_clean.endswith(":"):
                    declarations.append(line_clean[:-1])
    with open(f"{file}.pxd", "w") as declaration_file:
        for dec in declarations:
            declaration_file.write(dec + "\n")
###########################################################
edited_files = [f"{f}.pyx" for f in summary.keys() if summary[f]['edited']]

ext_modules = [
    Extension(
        cfile.strip(".pyx"),
        [cfile],
        **extension_kwargs
    )
    for cfile in edited_files
]

if not ext_modules:
    print(f"[green]Everything up-to-date[/green]")
else:
    
    print(f"[blue]Cythonizing..[/blue]")
    ext_modules = cythonize(ext_modules, 
                            nthreads=8,
                            compiler_directives=cython_compiler_directives,
                            include_path=["."],
                            force=False,
                            annotate=False)

    print(f"[blue]Now compiling modified extensions:[/blue]{[e.sources[0].split('.')[0] for e in ext_modules]}")
    setup(
        name=packageDir,
        cmdclass={"build_ext": build_ext},
        include_dirs=includedDir,
        ext_modules=ext_modules,
        script_args=["build_ext"],
        options={"build_ext": {"inplace": True, "force": True, "parallel":True}},
        )
    
    new_make_infos = {f:cython_compiler_directives for f in summary.keys() if summary[f]['edited']}
    update_make_infos(new_make_infos)
    # Sets back working directory to old one
    os.chdir(old_dir)
