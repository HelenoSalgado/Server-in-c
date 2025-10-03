# Guia de Instalação - Salop Web Server

## 🚀 Instalação com Meson (Recomendado)

### Pré-requisitos

- **Meson** (>= 0.56.0)
- **Ninja** (build backend)
- **GCC** ou **Clang**
- **Threads** (pthread)
- **OpenSSL** (opcional, para HTTPS)
- **Python3** (opcional, para testes)

#### Instalação de dependências

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install meson ninja-build gcc libc6-dev python3 python3-requests
# Para HTTPS (opcional):
sudo apt install libssl-dev
```

**Arch Linux:**
```bash
sudo pacman -S meson ninja gcc python python-requests
# Para HTTPS (opcional):
sudo pacman -S openssl
```

**Fedora/RHEL:**
```bash
sudo dnf install meson ninja-build gcc glibc-devel python3 python3-requests
# Para HTTPS (opcional):
sudo dnf install openssl-devel
```

### Compilação e Instalação

1. **Configuração:**
```bash
# Configuração básica (HTTP apenas)
meson setup builddir

# Configuração com HTTPS/TLS
meson setup builddir -Denable_tls=true

# Configuração customizada
meson setup builddir \
    -Denable_tls=true \
    -Dcache_size=200 \
    -Dcache_ttl=600 \
    -Dweb_root=/var/www/salop
```

2. **Compilação:**
```bash
meson compile -C builddir
```

3. **Testes:**
```bash
meson test -C builddir
```

4. **Instalação:**
```bash
# Instalação no sistema (requer sudo)
sudo meson install -C builddir

# Instalação em diretório personalizado
meson setup builddir --prefix=/opt/salop
meson compile -C builddir
sudo meson install -C builddir
```

### Opções de Configuração

| Opção | Descrição | Padrão |
|-------|-----------|--------|
| `enable_tls` | Habilitar HTTPS/TLS | `false` |
| `enable_tests` | Compilar testes | `true` |
| `enable_docs` | Instalar documentação | `false` |
| `cache_size` | Máximo de entradas no cache | `100` |
| `cache_ttl` | TTL do cache (segundos) | `300` |
| `buffer_size` | Tamanho do buffer (bytes) | `131072` |
| `web_root` | Diretório web padrão | `/var/www/salop` |

## 🔧 Instalação com Makefile (Alternativa)

```bash
# Compilação
make

# Testes
make test

# Limpeza
make clean
```

## 🖥️ Pós-instalação

### 1. Configuração como Serviço Systemd

```bash
# Habilitar e iniciar o serviço
sudo systemctl enable salop-server
sudo systemctl start salop-server

# Verificar status
sudo systemctl status salop-server

# Ver logs
sudo journalctl -u salop-server -f
```

### 2. Uso Manual

```bash
# Execução básica
salop-server

# Execução customizada
salop-server -p 8080 -d /var/www/html

# Execução em background
salop-server -b -p 80 -d /var/www/salop
```

### 3. Configuração de Firewall

```bash
# UFW (Ubuntu)
sudo ufw allow 80/tcp
sudo ufw allow 443/tcp

# Firewalld (Fedora/RHEL)
sudo firewall-cmd --permanent --add-service=http
sudo firewall-cmd --permanent --add-service=https
sudo firewall-cmd --reload
```

## 📁 Estrutura Pós-instalação

```
/usr/local/bin/salop-server              # Executável
/usr/local/lib/systemd/system/           # Serviço systemd
    salop-server.service
/usr/local/share/salop-server/           # Scripts auxiliares
    scripts/post-install.sh
/var/www/salop/                          # Arquivos web (se instalado)
    index.html
    404.html
    mime-test.html
    ...
```

## 🐛 Solução de Problemas

### Servidor não inicia
```bash
# Verificar permissões
ls -la /usr/local/bin/salop-server

# Verificar diretório web
ls -la /var/www/salop/

# Verificar logs
sudo journalctl -u salop-server -n 20
```

### Porta já em uso
```bash
# Verificar processos usando a porta
sudo netstat -tlnp | grep :80
sudo lsof -i :80

# Parar processo conflitante
sudo systemctl stop apache2  # ou nginx
```

### Permissões insuficientes
```bash
# Criar usuário www-data se não existir
sudo useradd -r -s /bin/false -d /var/www -c "Web Server User" www-data

# Configurar permissões
sudo chown -R www-data:www-data /var/www/salop/
sudo chmod 755 /var/www/salop/
```

## 🚀 Próximos Passos

- **TLS/HTTPS**: Configure certificados SSL em `/etc/ssl/` 
- **Performance**: Ajuste `cache_size` e `buffer_size` conforme necessário
- **Monitoramento**: Configure logs e métricas conforme sua infraestrutura
- **Backup**: Configure backup automático do diretório web

## ℹ️ Mais Informações

- [README.md](README.md) - Visão geral do projeto
- [docs/](docs/) - Documentação detalhada
- [ROADMAP.md](ROADMAP.md) - Funcionalidades futuras
- [IMPROVEMENTS.md](IMPROVEMENTS.md) - Melhorias implementadas