# Meson build system configuration for Salop Web Server

# Testes
if get_option('enable_tests')
    # Teste de cache
    test_cache = executable('test-cache',
        files('../tests/simple_cache_test.c', '../src/utils/cache.c'),
        include_directories : inc,
        c_args : compile_args,
        dependencies : threads_dep
    )
    
    # Teste de utilitários
    test_utils = executable('test-utils',
        files('../tests/simple_utils_test.c', '../src/utils/regex.c', '../src/utils/verify.c'),
        include_directories : inc,
        c_args : compile_args
    )
    
    # Teste de path
    test_path = executable('test-path',
        files('../tests/simple_path_test.c', '../src/utils/path.c'),
        include_directories : inc,
        c_args : compile_args
    )
    
    # Registrar testes
    test('Cache Test', test_cache)
    test('Utils Test', test_utils)
    test('Path Test', test_path)
    
    # Teste de integração (se Python disponível)
    python3 = find_program('python3', required : false)
    if python3.found()
        test('Integration Test',
            python3,
            args : [files('../tests/integration_test.py')],
            depends : [salop_server],
            workdir : meson.project_source_root(),
            env : ['SALOP_SERVER_PATH=' + salop_server.full_path()],
            timeout : 60
        )
    endif
endif