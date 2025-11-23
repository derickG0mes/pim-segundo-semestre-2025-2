@echo off
REM ============================================================================
REM   Script de Compilação - Sistema Completo de Gestão Escolar
REM   Compila todos os módulos C/C++ para gerar os .exe
REM ============================================================================

setlocal enabledelayedexpansion

REM Cores para saída (usando códigos ANSI)
for /F %%A in ('copy /Z "%~f0" nul') do set "BS=%%A"

echo.
echo ╔════════════════════════════════════════════════════════════════╗
echo ║          COMPILANDO SISTEMA COMPLETO DE GESTÃO ESCOLAR        ║
echo ╚════════════════════════════════════════════════════════════════╝
echo.

REM Verificar se GCC está instalado
gcc --version >nul 2>&1
if errorlevel 1 (
    echo ❌ ERRO: GCC não está instalado ou não está no PATH!
    echo.
    echo Para instalar GCC, você pode:
    echo   1. Instalar MinGW (https://www.mingw-w64.org/)
    echo   2. Instalar MSYS2 (https://www.msys2.org/)
    echo.
    pause
    exit /b 1
)

echo ✅ GCC encontrado! Iniciando compilação...
echo.

REM Diretório de compilação
set OUTPUT_DIR=.\build
if not exist %OUTPUT_DIR% mkdir %OUTPUT_DIR%

REM ============================================================================
REM COMPILAR MÓDULOS DE ALUNOS
REM ============================================================================

echo [1/11] Compilando: Login do Aluno...
gcc -fdiagnostics-color=always -o %OUTPUT_DIR%\area_login_aluno.exe alunos\area_login_aluno.c -lm
if errorlevel 1 (
    echo ❌ Erro ao compilar area_login_aluno.c
    goto erro
)
echo ✅ area_login_aluno.exe compilado com sucesso!
echo.

echo [2/11] Compilando: Cadastro do Aluno...
gcc -fdiagnostics-color=always -o %OUTPUT_DIR%\area_cadastro_aluno.exe alunos\area_cadastro_aluno.c -lm
if errorlevel 1 (
    echo ❌ Erro ao compilar area_cadastro_aluno.c
    goto erro
)
echo ✅ area_cadastro_aluno.exe compilado com sucesso!
echo.

echo [3/11] Compilando: Painel do Aluno...
gcc -fdiagnostics-color=always -o %OUTPUT_DIR%\area_aluno.exe alunos\area_aluno.c -lm
if errorlevel 1 (
    echo ❌ Erro ao compilar area_aluno.c
    goto erro
)
echo ✅ area_aluno.exe compilado com sucesso!
echo.

REM ============================================================================
REM COMPILAR MÓDULOS DE PROFESSORES
REM ============================================================================

echo [4/11] Compilando: Login do Professor...
gcc -fdiagnostics-color=always -o %OUTPUT_DIR%\area_login_professor.exe professores\area_login_professor.c -lm
if errorlevel 1 (
    echo ❌ Erro ao compilar area_login_professor.c
    goto erro
)
echo ✅ area_login_professor.exe compilado com sucesso!
echo.

echo [5/11] Compilando: Cadastro do Professor...
gcc -fdiagnostics-color=always -o %OUTPUT_DIR%\area_cadastro_professor.exe professores\area_cadastro_professor.c -lm
if errorlevel 1 (
    echo ❌ Erro ao compilar area_cadastro_professor.c
    goto erro
)
echo ✅ area_cadastro_professor.exe compilado com sucesso!
echo.

echo [6/11] Compilando: Painel de Atividades...
gcc -fdiagnostics-color=always -o %OUTPUT_DIR%\painel_atividade_avalicoes.exe professores\painel_atividade_avalicoes.c -lm
if errorlevel 1 (
    echo ❌ Erro ao compilar painel_atividade_avalicoes.c
    goto erro
)
echo ✅ painel_atividade_avalicoes.exe compilado com sucesso!
echo.

echo [7/11] Compilando: Histórico de Atividades...
gcc -fdiagnostics-color=always -o %OUTPUT_DIR%\historico_atividades_avaliacoes.exe professores\historico_atividades_avaliacoes.c -lm
if errorlevel 1 (
    echo ❌ Erro ao compilar historico_atividades_avaliacoes.c
    goto erro
)
echo ✅ historico_atividades_avaliacoes.exe compilado com sucesso!
echo.

REM ============================================================================
REM COMPILAR MÓDULO DE TURMAS
REM ============================================================================

echo [8/11] Compilando: Cadastro de Turmas...
gcc -fdiagnostics-color=always -o %OUTPUT_DIR%\area_cadastro_turma.exe turmas\area_cadastro_turma.c -lm
if errorlevel 1 (
    echo ❌ Erro ao compilar area_cadastro_turma.c
    goto erro
)
echo ✅ area_cadastro_turma.exe compilado com sucesso!
echo.

REM ============================================================================
REM COMPILAR MÓDULOS DE ADMINISTRADOR
REM ============================================================================

echo [9/11] Compilando: Login Administrador...
gcc -fdiagnostics-color=always -o %OUTPUT_DIR%\login_adm.exe administrador\login_adm.c -lm
if errorlevel 1 (
    echo ❌ Erro ao compilar login_adm.c
    goto erro
)
echo ✅ login_adm.exe compilado com sucesso!
echo.

echo [10/11] Compilando: Painel Administrativo...
gcc -fdiagnostics-color=always -o %OUTPUT_DIR%\painel_adm.exe administrador\painel_adm.c -lm
if errorlevel 1 (
    echo ❌ Erro ao compilar painel_adm.c
    goto erro
)
echo ✅ painel_adm.exe compilado com sucesso!
echo.

REM ============================================================================
REM COMPILAR SISTEMA COMPLETO (PRINCIPAL)
REM ============================================================================

echo [11/11] Compilando: Sistema Completo (Principal)...
gcc -fdiagnostics-color=always -o %OUTPUT_DIR%\sistema_completo.exe sistema_completo.c -lm
if errorlevel 1 (
    echo ❌ Erro ao compilar sistema_completo.c
    goto erro
)
echo ✅ sistema_completo.exe compilado com sucesso!
echo.

REM ============================================================================
REM FINALIZAR COMPILAÇÃO
REM ============================================================================

echo ╔════════════════════════════════════════════════════════════════╗
echo ║          ✅ COMPILAÇÃO CONCLUÍDA COM SUCESSO!                ║
echo ╚════════════════════════════════════════════════════════════════╝
echo.
echo 📁 Localização dos arquivos .exe: %cd%\%OUTPUT_DIR%
echo.
echo 🚀 Para iniciar o sistema, execute:
echo    .\build\sistema_completo.exe
echo.
echo 📝 NOTAS:
echo   1. Todos os arquivos .exe estão na pasta 'build'
echo   2. A pasta 'dados' deve estar no mesmo nível da pasta 'codigos'
echo   3. Python é necessário para o Painel de IA (painel_ia.py)
echo.

pause
exit /b 0

:erro
echo.
echo ╔════════════════════════════════════════════════════════════════╗
echo ║          ❌ ERRO DURANTE A COMPILAÇÃO!                        ║
echo ╚════════════════════════════════════════════════════════════════╝
echo.
echo Verifique o erro acima e tente novamente.
echo.
pause
exit /b 1
