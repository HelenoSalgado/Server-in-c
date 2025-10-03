#!/bin/bash

echo "🚀 Testando servidor com cache habilitado..."

# Inicia o servidor em background
echo "Iniciando servidor na porta 3000..."
./bin/server -p 3000 &
SERVER_PID=$!

# Aguarda um pouco para o servidor inicializar
sleep 2

echo -e "\n📋 Fazendo primeira requisição (cache MISS esperado)..."
curl -s -I http://localhost:3000/data.json | grep -E "(X-Cache|Content-Type|Content-Length)"

echo -e "\n📋 Fazendo segunda requisição (cache HIT esperado)..."
curl -s -I http://localhost:3000/data.json | grep -E "(X-Cache|Content-Type|Content-Length)"

echo -e "\n📋 Testando página de teste de MIME types..."
curl -s -I http://localhost:3000/mime-test.html | grep -E "(X-Cache|Content-Type|Content-Length)"

echo -e "\n📋 Testando arquivo XML..."
curl -s -I http://localhost:3000/config.xml | grep -E "(X-Cache|Content-Type|Content-Length)"

echo -e "\n📋 Segunda requisição do XML (cache HIT esperado)..."
curl -s -I http://localhost:3000/config.xml | grep -E "(X-Cache|Content-Type|Content-Length)"

echo -e "\n🛑 Parando servidor..."
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

echo "✅ Teste concluído!"