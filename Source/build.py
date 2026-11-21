"""Build the Essential V40 Windows x64 DLL with LLVM (no Windows SDK or C++ runtime needed)."""
import argparse
import os
from pathlib import Path
import re
import shutil
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('--toolchain', type=Path, help='LLVM bin directory')
parser.add_argument('--out', type=Path, default=Path('../build'))
args = parser.parse_args()
source = Path(__file__).resolve().parent
out = args.out.resolve()
out.mkdir(parents=True, exist_ok=True)

def tool(name):
    if args.toolchain:
        p = args.toolchain / (name + ('.exe' if os.name == 'nt' else ''))
        if p.exists():
            return str(p)
    p = shutil.which(name)
    if not p:
        raise SystemExit(f'Missing LLVM tool: {name}')
    return p

def run(*cmd):
    subprocess.run(list(map(str, cmd)), check=True)

text = '\n'.join(p.read_text() for p in [source / 'essential_plus_plus.cpp', *source.glob('*.inc')])
core_symbols = sorted(set(re.findall(r'CORE_ALIAS\("([^"]+)"\)', text)))
definitions = {
    'sdrpp_core.dll': core_symbols,
    'USER32.dll': ['GetAsyncKeyState', 'GetForegroundWindow', 'GetWindowThreadProcessId', 'MessageBoxA'],
    'KERNEL32.dll': ['GetCurrentProcessId', 'GetModuleHandleA', 'GetCurrentProcess', 'K32EnumProcessModules',
        'GetProcAddress', 'GetModuleHandleExA', 'GetSystemTime', 'VirtualProtect', 'VirtualAlloc', 'VirtualFree', 'FlushInstructionCache', 'MultiByteToWideChar',
        'CreateFileW', 'GetFileSize', 'ReadFile', 'WriteFile', 'FlushFileBuffers',
        'CloseHandle', 'MoveFileExW', 'DeleteFileW', 'GetLastError', 'FindFirstFileW', 'FindNextFileW', 'FindClose'],
    'api-ms-win-crt-heap-l1-1-0.dll': ['malloc', 'free'],
    'api-ms-win-crt-string-l1-1-0.dll': ['memcpy', 'memset'],
}
libs = []
for dll, symbols in definitions.items():
    stem = dll[:-4]
    def_file = out / (stem + '.def')
    lib = out / (stem + '.lib')
    def_file.write_text(f'LIBRARY "{dll}"\nEXPORTS\n' + ''.join(
        f'  "{s}"' + (' DATA' if s in ('waterfall', 'mainWindow', 'modComManager', 'menu', 'configManager', 'sourceManager', 'vfoManager', 'sinkManager', 'uiScale', 'bigFont') or s.startswith('?selectedSource@sourcemenu@@') or s.startswith('?fullWaterfallUpdate@displaymenu@@') else '') + '\n'
        for s in symbols
    ))
    if shutil.which('llvm-dlltool'):
        run(tool('llvm-dlltool'), '-m', 'i386:x86-64', '-d', def_file, '-l', lib)
    else:
        # lld-link can emit the same MSVC import library from a .def file.
        run(tool('lld-link'), '/dll', '/noentry', '/machine:x64',
            f'/def:{def_file}', f'/out:{out / (stem + ".dll")}',
            f'/implib:{lib}', '/nodefaultlib')
    libs.append(lib)

obj = out / 'essential_plus_plus.obj'
run(tool('clang'), '--target=x86_64-pc-windows-msvc', '-std=c++17', '-O2',
    '-fno-exceptions', '-fno-rtti', '-fno-stack-protector', '-ffreestanding',
    '-Wno-return-type-c-linkage', '-c', source / 'essential_plus_plus.cpp', '-o', obj)
run(tool('lld-link'), '/dll', '/noentry', '/machine:x64', '/subsystem:windows',
    '/nodefaultlib', '/opt:ref', '/opt:icf', '/timestamp:0',
    f'/out:{out / "essential_plus_plus.dll"}', obj, *libs)
print(out / 'essential_plus_plus.dll')
