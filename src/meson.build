# Arquivos fonte principais - Nova estrutura organizada
sources = files(
    '../src/core/server.c',
    '../src/core/config.c',
    '../src/http/http_handler.c',
    '../src/socket/create.c',
    '../src/utils/logger.c',
    '../src/utils/regex.c',
    '../src/utils/verify.c',
    '../src/utils/path.c',
    '../src/utils/cache.c'
)

# Diretórios de include organizados
inc = include_directories('../src/include')

# Headers
headers = files(
    '../src/include/config.h',
    '../src/include/context.h',
    '../src/include/http_handler.h',
    '../src/include/create.h',
    '../src/include/logger.h',
    '../src/include/regex.h',
    '../src/include/verify.h',
    '../src/include/path.h',
    '../src/include/cache.h'
)

if tls_enabled
    sources += files('../src/utils/tls.c')
    headers += files('../src/include/tls.h')
    message('TLS/HTTPS: Habilitado')
else
    message('TLS/HTTPS: Desabilitado (use -Denable_tls=true para habilitar)')
endif

# Configurar defines baseado nas opções
compile_args = [
    '-DCACHE_MAX_ENTRIES=' + get_option('cache_size').to_string(),
    '-DCACHE_TTL_SECONDS=' + get_option('cache_ttl').to_string(),
    '-DBUFFER_SIZE=' + get_option('buffer_size').to_string(),
    '-DWEB_ROOT_DEFAULT="' + get_option('web_root') + '"'
]

if tls_enabled
    compile_args += ['-DTLS_ENABLED=1']
endif

# Executável principal
salop_server = executable('salop-server',
    sources,
    include_directories : inc,
    dependencies : tls_enabled ? [threads_dep, openssl_dep] : [threads_dep],
    c_args : compile_args,
    install : true,
    install_dir : get_option('bindir')
)

# Instalar headers (opcional para desenvolvimento)
if get_option('enable_docs')
    install_headers(headers, subdir : 'salop-server')
endif