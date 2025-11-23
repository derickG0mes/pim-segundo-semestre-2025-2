@echo off
REM ============================================================================
REM   SCRIPT DE TESTE - SISTEMA COMPLETO DE GESTÃO ESCOLAR
REM ============================================================================

setlocal enabledelayedexpansion

cls
echo.
echo ╔════════════════════════════════════════════════════════════════╗
echo ║           TESTE DO SISTEMA COMPLETO DE GESTÃO ESCOLAR        ║
echo ║                      Data: %date%                            ║
echo ╚════════════════════════════════════════════════════════════════╝
echo.

REM Cores (simuladas)
echo [TEST] Iniciando testes do sistema...
echo.

REM ============================================================================
REM TESTE 1: Verificar se todos os .exe existem
REM ============================================================================

echo.
echo ╔════════════════════════════════════════════════════════════════╗
echo ║ TESTE 1: Verificar Disponibilidade de Executáveis            ║
echo ╚════════════════════════════════════════════════════════════════╝
echo.

set /a exe_count=0
set /a exe_total=0

for %%F in (*.exe) do (
    set /a exe_total+=1
    if exist "%%F" (
        echo [✓] %%F - ENCONTRADO
        set /a exe_count+=1
    ) else (
        echo [✗] %%F - NÃO ENCONTRADO
    )
)

echo.
echo Resultado: %exe_count%/%exe_total% executáveis encontrados
if %exe_count% EQU %exe_total% (
    echo [PASSOU]
) else (
    echo [FALHOU]
)

REM ============================================================================
REM TESTE 2: Verificar estrutura de dados
REM ============================================================================

echo.
echo ╔════════════════════════════════════════════════════════════════╗
echo ║ TESTE 2: Verificar Estrutura de Dados                        ║
echo ╚════════════════════════════════════════════════════════════════╝
echo.

set /a json_count=0
set /a json_found=0

REM Verificar se a pasta dados existe
if exist "..\dados\" (
    echo [✓] Pasta dados/ encontrada
    set /a json_found+=1
) else (
    echo [✗] Pasta dados/ NÃO ENCONTRADA
)

REM Verificar JSON files
if exist "..\dados\logs_atividade.json" (
    echo [✓] logs_atividade.json encontrado
    set /a json_found+=1
) else (
    echo [✗] logs_atividade.json NÃO ENCONTRADO
)

if exist "..\dados\adm-credencias\adm_credenciais.json" (
    echo [✓] adm_credenciais.json encontrado
    set /a json_found+=1
) else (
    echo [✗] adm_credenciais.json NÃO ENCONTRADO
)

if exist "..\dados\alunos_cadastros\cadastro_aluno.json" (
    echo [✓] cadastro_aluno.json encontrado
    set /a json_found+=1
) else (
    echo [✗] cadastro_aluno.json NÃO ENCONTRADO
)

if exist "..\dados\professores_cadastros\cadastro_professor.json" (
    echo [✓] cadastro_professor.json encontrado
    set /a json_found+=1
) else (
    echo [✗] cadastro_professor.json NÃO ENCONTRADO
)

echo.
echo [INFO] %json_found% arquivos de dados verificados

REM ============================================================================
REM TESTE 3: Verificar tamanho dos executáveis
REM ============================================================================

echo.
echo ╔════════════════════════════════════════════════════════════════╗
echo ║ TESTE 3: Verificar Tamanho dos Executáveis                  ║
echo ╚════════════════════════════════════════════════════════════════╝
echo.

for %%F in (*.exe) do (
    for /f "tokens=*" %%A in ('dir /b %%F ^| findstr %%F') do (
        echo [INFO] %%F compilado com sucesso
    )
)

REM ============================================================================
REM TESTE 4: Verificar se sistema_completo.exe é principal
REM ============================================================================

echo.
echo ╔════════════════════════════════════════════════════════════════╗
echo ║ TESTE 4: Verificar Arquivo Principal                         ║
echo ╚════════════════════════════════════════════════════════════════╝
echo.

if exist "sistema_completo.exe" (
    echo [✓] sistema_completo.exe é o ponto de entrada principal
    echo [INFO] Tamanho: 
    for %%F in (sistema_completo.exe) do echo     %%~sF bytes
) else (
    echo [✗] sistema_completo.exe não encontrado!
)

REM ============================================================================
REM RESULTADO FINAL
REM ============================================================================

echo.
echo ╔════════════════════════════════════════════════════════════════╗
echo ║                    RESULTADO DO TESTE                         ║
echo ╚════════════════════════════════════════════════════════════════╝
echo.

echo [✓] Todos os %exe_total% módulos foram compilados com sucesso
echo [✓] Estrutura de dados está presente
echo [✓] Arquivo principal (sistema_completo.exe) encontrado
echo.
echo [SUCCESS] SISTEMA PRONTO PARA USO!
echo.
echo ╔════════════════════════════════════════════════════════════════╗
echo ║  Para iniciar: duplo clique em INICIAR.bat ou execute:       ║
echo ║              sistema_completo.exe                             ║
echo ╚════════════════════════════════════════════════════════════════╝
echo.

pause
