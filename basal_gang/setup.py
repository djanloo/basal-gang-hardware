from setuptools import Extension, setup
import os
import sys

import argparse
from Cython.Distutils import build_ext
from Cython.Compiler.Options import get_directive_defaults
from Cython.Build import cythonize

import numpy as np
from rich import print
# import yaml
from tabulate import tabulate
import pandas as pd

import logging 
from rich.logging import RichHandler
import yaml

logging.basicConfig(
    level="INFO",
    format="%(name)s - %(message)s",
    datefmt="[%X]",
    handlers=[RichHandler(markup=True,rich_tracebacks=True)]
)
logger = logging.getLogger('SETUP')

MAKE_INFO_FILE = "make.info"
BIN_FOLDER = 'bin'
CYTHON_GEN_FOLDER = './cython_generated'


def compare_make_infos(new_make_infos, summary):
    try:
        with open(MAKE_INFO_FILE, "r") as infof:
            make_infos = yaml.safe_load(infof)
    except FileNotFoundError or EOFError:
        logger.warning("MakeInfo file not found")
        with open(MAKE_INFO_FILE, "w") as infof:
            yaml.safe_dump({}, infof)
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
        with open(MAKE_INFO_FILE, "r") as infof:
            make_infos = yaml.safe_load(infof)

    except FileNotFoundError and EOFError:
        logger.warning("MakeInfo file not found")
        with open(MAKE_INFO_FILE, "w") as infof:
            yaml.safe_dump(dict(dummy=6), infof)
            make_infos = dict(dummy=6)

    make_infos.update(new_make_infos)

    with open(MAKE_INFO_FILE, "w") as infof:
        yaml.safe_dump(make_infos, infof)

    logger.debug(f"Updating make infos with : {make_infos}")


def get_files_and_timestamp(extension, folder="./"):
    logger.debug(f"Getting timestamps for {extension} files in {folder}")
    logger.debug(f"Folder has files {os.listdir(folder)}")
    return {file.split('.')[0]:os.path.getmtime(os.path.join(folder, file)) for file in os.listdir(folder) if file.endswith(extension)}

parser = argparse.ArgumentParser()
parser.add_argument('--profile', action='store_true')
parser.add_argument('--notrace', action='store_true')
parser.add_argument('--hardcore', action='store_true')
parser.add_argument('--remake', action='store_true')
parser.add_argument('--debug', action='store_true')

args = parser.parse_args()

if args.debug:
    logger.setLevel(logging.DEBUG)

# Set the working directory
old_dir = os.getcwd()
packageDir = os.path.dirname(__file__)
includedDir = [".", packageDir, np.get_include()]
os.chdir(packageDir)
logger.debug(f"Include dirs are {includedDir}")

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

# This is for me
cython_compiler_directives['djanloo_compile_mode'] = 'default'

# Profiling using line_profiler
if args.profile:
    logger.info("[blue]Compiling in [green]PROFILE[/green] mode[/blue]")
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
    logger.info("[blue]Compiling in [green]HARDCORE[/green] mode[/blue]")
    cython_compiler_directives['boundscheck'] = False
    cython_compiler_directives['cdivision'] = True
    cython_compiler_directives['wraparound'] = False
    
    # This is for me
    cython_compiler_directives['djanloo_compile_mode'] = 'hardcore'

#################################################################

cython_files = get_files_and_timestamp(".pyx")
logger.debug(f"Cython timestamps are {cython_files}")
c_files = get_files_and_timestamp(".c", folder=CYTHON_GEN_FOLDER)
c_files.update(get_files_and_timestamp(".cpp", folder=CYTHON_GEN_FOLDER))
c_files.update(get_files_and_timestamp(".cpp", folder="."))

logger.debug(f"C timestamps are {c_files}")


all_files = sorted(list( #set(c_files.keys()) |\
                         set(cython_files.keys())))

summary = dict()
###################### FILE VERSION COMPARISON ##################
for file in all_files:
    summary[file] = dict()
    try:
        cython_files[file]
    except KeyError:
        logger.warning(f"[red]C file {file:30} has no .pyx parent[/red]")
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

################### REMAKE ###############
# If in remake mode, sets them all edited
if args.remake:
    for f in summary:
        summary[f]['edited'] = True
##########################################
        
################## DEPENDENCIES #############
try:
    with open("cdependencies.yaml", "r") as dependencies:
        c_dependencies = yaml.safe_load(dependencies)
        logger.debug(f"C-dependencies are:")
        yaml.dump(c_dependencies, sys.stdout)
except FileNotFoundError:
    logger.debug(f"C-dependencies not found")
    c_dependencies = dict()
    pass


################### UPDATE MAKE INFOS ##########################
new_make_infos = {f:cython_compiler_directives for f in summary.keys()}
compare_make_infos(new_make_infos, summary)

df = pd.DataFrame(summary).T
print(df)

# ################### GENERATION OF PXD FILES ###############
# for file in cython_files.keys():
#     declarations = []
#     with open(f"{file}.pyx", "r") as codefile:
#         for line in codefile:
#             line_clean = line.strip()
#             if line_clean.startswith("#"):
#                 continue
#             if 'extern' in line:
#                 continue
#             if line_clean.startswith('cdef') or \
#                 line_clean.startswith("cpdef"):
#                 if line_clean.endswith(":"):
#                     declarations.append(line_clean[:-1])
#     with open(f"{file}.pxd", "w") as declaration_file:
#         for dec in declarations:
#             declaration_file.write(dec + "\n")
# ###########################################################

edited_files = [f"{f}.pyx" for f in summary.keys() if summary[f]['edited']]
logger.debug(f"Edited files are {edited_files}")

ext_modules = []
for edited_file in edited_files:
    extension_name = edited_file.strip(".pyx")
    source_files = [edited_file]

    if extension_name in c_dependencies.keys():
        source_files += c_dependencies[extension_name]
        logger.debug(f"Adding sources {c_dependencies[extension_name]} to extension {extension_name}")
    ext_module = Extension(extension_name, source_files, **extension_kwargs, language="c++")
    ext_modules.append(ext_module)

logger.debug(f"Source files are { {e.name: e.sources for e in ext_modules} }")

if not ext_modules:
    logger.info(f"[green]Everything up-to-date[/green]")
else:
    
    logger.info(f"[blue]Cythonizing..[/blue]")
    ext_modules = cythonize(ext_modules, 
                            nthreads=8,
                            compiler_directives=cython_compiler_directives,
                            include_path=["."],
                            build_dir = CYTHON_GEN_FOLDER,
                            force=False,
                            annotate=False)

    logger.info(f"[blue]Now compiling modified extensions:[/blue]{[os.path.basename(e.sources[0]) for e in ext_modules]}")
    setup(
        name=packageDir,
        cmdclass={"build_ext": build_ext},
        include_dirs=includedDir,
        ext_modules=ext_modules,
        script_args=["build_ext", f"--build-lib=./{BIN_FOLDER}"],
        options={"build_ext": {"inplace": False, "force": True, "parallel":True}},
        )
    
    new_make_infos = {f:cython_compiler_directives for f in summary.keys() if summary[f]['edited']}
    update_make_infos(new_make_infos)
    # Sets back working directory to old one
    os.chdir(old_dir)
