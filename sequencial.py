def merge_sort(numeros):
    """Ordena uma lista de números usando merge sort e retorna uma nova lista."""
    if len(numeros) <= 1:
        return list(numeros)

    meio = len(numeros) // 2
    esquerda = merge_sort(numeros[:meio])
    direita = merge_sort(numeros[meio:])

    return merge(esquerda, direita)


def merge(esquerda, direita):
    """Intercala duas listas já ordenadas em uma única lista ordenada."""
    resultado = []
    i = j = 0

    while i < len(esquerda) and j < len(direita):
        if esquerda[i] <= direita[j]:
            resultado.append(esquerda[i])
            i += 1
        else:
            resultado.append(direita[j])
            j += 1

    resultado.extend(esquerda[i:])
    resultado.extend(direita[j:])
    return resultado
