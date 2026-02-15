#!/bin/bash
CERT_DIR="build/server"

mkdir -p $CERT_DIR

echo "Generating .pem certificates for Terminal Chat Server..."
openssl req -x509 -newkey rsa:4096 -keyout "$CERT_DIR/key.pem" -out "$CERT_DIR/server.pem" -days 365 -nodes -subj "/CN=localhost"

if [ $? -eq 0 ]; then
    echo "Success: Certificates generated in $CERT_DIR"
else
    echo "Error: Certificate generation failed."
    exit 1
fi
