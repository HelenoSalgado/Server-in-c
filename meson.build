project('salop-web-server', 'c',
    version : '1.0.0',
    default_options : [
        'warning_level=2',
        'werror=false',
        'c_std=c99',
        'optimization=2',
        'strip=true'
    ],
    license : 'MIT',
    meson_version : '>=0.56.0'
)

# Informações do projeto
project_description = 'Servidor web multi-threaded em C com cache e tipos MIME expandidos'

# Configuração
conf_data = configuration_data()
conf_data.set('version', meson.project_version())
conf_data.set('project_name', meson.project_name())

# Dependências obrigatórias
cc = meson.get_compiler('c')
threads_dep = dependency('threads')

# Dependências opcionais
openssl_dep = dependency('openssl', required : get_option('enable_tls'))

# Verificar se TLS está realmente habilitado
tls_enabled = openssl_dep.found() and get_option('enable_tls')

# Headers do sistema
if host_machine.system() != 'windows'
    add_project_arguments('-D_GNU_SOURCE', language : 'c')
endif

# Verificar disponibilidade de regex.h
cc = meson.get_compiler('c')
if cc.has_header('regex.h')
    add_project_arguments('-DHAVE_REGEX_H=1', language : 'c')
    message('POSIX regex support: Available')
else
    add_project_arguments('-DHAVE_REGEX_H=0', language : 'c')
    message('POSIX regex support: Not available - using fallback implementation')
endif

# Incluir subdiretórios
subdir('src')

# Disponibilizar executável para testes
salop_server = get_variable('salop_server')

# Testes (opcional)
if get_option('enable_tests')
    subdir('tests')
endif

# Documentação
if get_option('enable_docs')
    subdir('docs')
endif

# Resumo da configuração
summary({
    'Versão' : meson.project_version(),
    'TLS/HTTPS' : tls_enabled,
    'Testes' : get_option('enable_tests'),
    'Documentação' : get_option('enable_docs'),
    'Otimização' : get_option('optimization'),
}, section : 'Configuração')

# Instalar arquivos de contribuição
install_data(
    files('contrib/salop-server.service'),
    install_dir : get_option('libdir') / 'systemd' / 'system',
    install_mode : 'rw-r--r--'
)

install_data(
    files('contrib/post-install.sh'),
    install_dir : get_option('datadir') / 'salop-server' / 'scripts',
    install_mode : 'rwxr-xr-x'
)