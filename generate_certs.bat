@echo off
set CERT_DIR=build\server

if not exist %CERT_DIR% mkdir %CERT_DIR%

echo Generating .pem certificates for Terminal Chat Server...

openssl req -x509 -newkey rsa:4096 -keyout "%CERT_DIR%\key.pem" -out "%CERT_DIR%\server.pem" -days 365 -nodes -subj "/CN=localhost"

if %ERRORLEVEL% EQU 0 (
    echo Success: Certificates generated in %CERT_DIR%
) else (
    echo Error: Certificate generation failed.
    pause
)
