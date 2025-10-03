#!/bin/bash
# Post-installation script for Salop Web Server

# Criar usuário www-data se não existir
if ! id -u www-data > /dev/null 2>&1; then
    useradd -r -s /bin/false -d /var/www -c "Web Server User" www-data
fi

# Criar diretórios necessários
mkdir -p /var/www/salop
mkdir -p /var/log/salop-server
mkdir -p /var/run

# Definir permissões
chown -R www-data:www-data /var/www/salop
chown -R www-data:www-data /var/log/salop-server
chmod 755 /var/www/salop

# Instalar e habilitar serviço systemd (se disponível)
if command -v systemctl > /dev/null 2>&1; then
    systemctl daemon-reload
    systemctl enable salop-server.service
    echo "Serviço systemd instalado. Use 'systemctl start salop-server' para iniciar."
fi

echo "Salop Web Server instalado com sucesso!"
echo "Diretório web: /var/www/salop"
echo "Logs: /var/log/salop-server/"
echo ""
echo "Para iniciar manualmente:"
echo "  salop-server -p 8080 -d /var/www/salop"
echo ""
echo "Para iniciar como serviço:"
echo "  systemctl start salop-server"