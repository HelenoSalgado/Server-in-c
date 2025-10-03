#ifndef PATH_H
#define PATH_H

#include <stdlib.h>

/**
 * @brief Valida e sanitiza um caminho de arquivo para previnir Path Traversal.
 *
 * Esta função constrói um caminho completo a partir de um diretório base e um
 * caminho de entrada, resolve para seu caminho absoluto e canônico, e verifica
 * se ele está contido dentro do diretório base.
 *
 * @param base_dir O diretório raiz absoluto a partir do qual os arquivos devem ser servidos.
 * @param requested_path O caminho do arquivo solicitado pelo cliente.
 * @param sanitized_path Um buffer para armazenar o caminho absoluto e seguro.
 * @param sanitized_path_size O tamanho do buffer `sanitized_path`.
 * @return 0 se o caminho for seguro e válido, -1 caso contrário.
 */
int sanitize_path(const char *base_dir, const char *requested_path, char *sanitized_path, size_t sanitized_path_size);

#endif
