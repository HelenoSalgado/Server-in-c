# 📚 Salop Web Server - Documentação

Bem-vindo à documentação completa do **Salop Web Server**, um servidor web moderno escrito em C com suporte a multi-threading, cache inteligente e ampla variedade de tipos MIME.

## 🚀 Começando

- **[Instalação](./install.md)** - Guia completo de instalação com Meson e Make
- **[Configuração](./configuration.md)** - Como configurar e usar o servidor
- **[Compilação](./compilation.md)** - Dependências e processo de build

## 📖 Documentação Técnica

- **[Funcionalidades](./features.md)** - Principais recursos e capacidades
- **[Estrutura do Projeto](./project-structure.md)** - Organização do código fonte
- **[Daemonização](./daemonization.md)** - Execução como serviço de sistema

## 📋 Desenvolvimento

- **[Roadmap](./roadmap.md)** - Funcionalidades implementadas e planejadas
- **[Melhorias](./improvements.md)** - Histórico de refatorações e otimizações
- **[Changelog](./changelog.md)** - Resumo das melhorias mais recentes

## 🔧 Referência Rápida

### Instalação Rápida
```bash
meson setup builddir
meson compile -C builddir
meson test -C builddir
sudo meson install -C builddir
```

### Uso Básico
```bash
# Executar na porta 8080
salop-server -p 8080

# Executar como daemon
salop-server -b -p 80 -d /var/www/html

# Executar como serviço
sudo systemctl start salop-server
```

### Características Principais
- ✅ **Multi-threading** com threads POSIX
- ✅ **Cache inteligente** com TTL configurável  
- ✅ **25+ tipos MIME** suportados
- ✅ **Sistema de build moderno** com Meson
- ✅ **Testes automatizados** completos
- ✅ **Daemonização** com gerenciamento PID
- ✅ **Logs estruturados** com cores
- ✅ **Segurança** contra path traversal

## 🤝 Contribuindo

Para contribuir com o projeto:

1. Consulte a [estrutura do projeto](./project-structure.md)
2. Veja o [roadmap](./roadmap.md) para funcionalidades planejadas
3. Execute os testes: `meson test -C builddir`
4. Documente suas mudanças

## 📞 Suporte

- **Issues**: Para bugs e solicitações de recursos
- **Documentação**: Esta seção para referência completa
- **Instalação**: Veja [install.md](./install.md) para problemas de instalação

---

**Versão**: 1.0.0  
**Licença**: MIT  
**Última atualização**: Outubro 2024
