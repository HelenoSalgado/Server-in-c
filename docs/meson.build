# Instalar documentação
if get_option('enable_docs')
    install_data(
        files(
            '../README.md'
        ),
        install_dir : get_option('datadir') / 'doc' / 'salop-server'
    )
    
    install_data(
        files(
            '../docs/index.md',
            '../docs/compilation.md',
            '../docs/configuration.md',
            '../docs/features.md',
            '../docs/daemonization.md',
            '../docs/project-structure.md',
            '../docs/install.md',
            '../docs/improvements.md',
            '../docs/roadmap.md',
            '../docs/changelog.md'
        ),
        install_dir : get_option('datadir') / 'doc' / 'salop-server' / 'docs'
    )
endif

# Exemplo de arquivos web (opcional)
install_data(
    files(
        '../web/index.html',
        '../web/404.html',
        '../web/mime-test.html',
        '../web/data.json',
        '../web/config.xml',
        '../web/documentation.md',
        '../web/readme.txt'
    ),
    install_dir : get_option('web_root')
)