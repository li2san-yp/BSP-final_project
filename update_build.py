#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
自动更新 build_hex.bat 编译脚本
扫描 source 文件夹中的所有 .c 文件并生成相应的编译命令
"""

import os
import glob
import shutil
from datetime import datetime

def main():
    print("=" * 50)
    print("    Auto Build Script Updater")
    print("=" * 50)
    
    # 检查当前目录
    if not os.path.exists("source"):
        print("错误：找不到 source 文件夹！")
        return
    
    # 备份原始文件
    if os.path.exists("build_hex.bat"):
        backup_name = f"build_hex.bat.backup_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
        shutil.copy2("build_hex.bat", backup_name)
        print(f"✓ 已备份原文件为: {backup_name}")
    
    # 扫描C文件
    c_files = glob.glob("source/*.c")
    if not c_files:
        print("错误：source 文件夹中没有找到 .c 文件！")
        return
    
    # 提取文件名（不含扩展名）
    file_names = [os.path.splitext(os.path.basename(f))[0] for f in c_files]
    print(f"\n发现 {len(file_names)} 个 C 文件:")
    for name in file_names:
        print(f"  • {name}.c")
    
    # 生成编译脚本
    script_content = generate_build_script(file_names)
    
    # 写入文件
    with open("build_hex.bat", "w", encoding="utf-8") as f:
        f.write(script_content)
    
    print(f"\n✓ 编译脚本已更新！")
    print("\n使用方法:")
    print("  • 命令行运行: build_hex.bat")
    print("  • VS Code: 按 Ctrl+Shift+B")
    print("\n" + "=" * 50)

def generate_build_script(file_names):
    """生成编译脚本内容"""
    
    # 构建目标文件列表
    obj_files = [f"output\\{name}.obj" for name in file_names]
    obj_list = ",".join(obj_files)
    
    script = """@echo off
cd /d "%~dp0"

echo ========================================
echo      STC MCU Quick Build
echo ========================================

REM Create output directory
if not exist "output" mkdir output

echo [1/3] Compiling C files...

"""
    
    # 为每个C文件添加编译命令
    for name in file_names:
        script += f"""D:\\Keil\\C51\\BIN\\C51.exe source\\{name}.c INCDIR(inc) OBJECT(output\\{name}.obj) CODE OPTIMIZE(8,SPEED) DEFINE(FOSC_11059200)

if not exist "output\\{name}.obj" (
    echo [ERROR] {name} compilation failed!
    exit /b 1
)
"""
    
    # 添加链接部分
    script += f"""echo [OK] Compilation successful

echo.
echo [2/3] Linking...
D:\\Keil\\C51\\BIN\\BL51.exe {obj_list},source\\STCBSP_V3.6.LIB TO output\\BSP_Demo

if not exist "output\\BSP_Demo" (
    echo [ERROR] Linking failed!
    exit /b 1
)
echo [OK] Linking successful

echo.
echo [3/3] Generating HEX...
D:\\Keil\\C51\\BIN\\OH51.exe output\\BSP_Demo HEXFILE(output\\BSP_Demo.hex)

if not exist "output\\BSP_Demo.hex" (
    echo [ERROR] HEX generation failed!
    exit /b 1
)

echo.
echo ========================================
echo BUILD SUCCESSFUL!
echo ========================================
echo HEX file: output\\BSP_Demo.hex
for %%A in ("output\\BSP_Demo.hex") do echo Size: %%~zA bytes
echo ========================================
"""
    
    return script

if __name__ == "__main__":
    main()
    input("\n按任意键继续...")