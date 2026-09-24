import random
import time

import sequencial

TAMANHOS = [50_000, 100_000]
SEMENTE = 42


def merge_cronometrado(esquerda, direita, tempos_merge):
    """Executa sequencial.merge e guarda quanto tempo ele levou."""
    inicio = time.perf_counter()
    resultado = sequencial.merge(esquerda, direita)
    tempos_merge.append(time.perf_counter() - inicio)
    return resultado


def merge_sort_instrumentado(numeros, tempos_merge):
    """Cópia de sequencial.merge_sort que cronometra cada merge."""
    if len(numeros) <= 1:
        return list(numeros)

    meio = len(numeros) // 2
    esquerda = merge_sort_instrumentado(numeros[:meio], tempos_merge)
    direita = merge_sort_instrumentado(numeros[meio:], tempos_merge)

    return merge_cronometrado(esquerda, direita, tempos_merge)


def perfilar(numeros):
    """Roda o merge sort instrumentado e retorna os tempos de cada trecho."""
    tempos_merge = []

    # Primeira divisão feita aqui para marcar os trechos do nível mais alto
    inicio = time.perf_counter()
    meio = len(numeros) // 2
    esquerda = merge_sort_instrumentado(numeros[:meio], tempos_merge)
    # Trecho 2: da primeira divisão até a lista_2 começar a ser dividida
    tempo_ate_lista_2 = time.perf_counter() - inicio
    direita = merge_sort_instrumentado(numeros[meio:], tempos_merge)
    resultado = merge_cronometrado(esquerda, direita, tempos_merge)
    # Trecho 1: tempo total, já incluindo o custo dos timers
    tempo_total = time.perf_counter() - inicio

    assert resultado == sorted(numeros)

    return {
        "Tempo total do merge sort": tempo_total,
        "Da 1ª divisão até dividir a lista_2": tempo_ate_lista_2,
        "Pior tempo de um único merge": max(tempos_merge),
        "Soma de todos os merges": sum(tempos_merge),
        "Quantidade de merges": len(tempos_merge),
    }


def formatar(valor):
    if isinstance(valor, int):
        return f"{valor:>12}"
    return f"{valor * 1000:>9.3f} ms"


if __name__ == "__main__":
    random.seed(SEMENTE)
    resultados = {}
    for tamanho in TAMANHOS:
        numeros = [random.randint(0, 1_000_000) for _ in range(tamanho)]
        resultados[tamanho] = perfilar(numeros)

    largura = max(len(trecho) for trecho in resultados[TAMANHOS[0]])
    cabecalho = f"{'Trecho':<{largura}}" + "".join(f"  {f'n = {t}':>12}" for t in TAMANHOS)
    print(cabecalho)
    print("-" * len(cabecalho))
    for trecho in resultados[TAMANHOS[0]]:
        linha = f"{trecho:<{largura}}"
        linha += "".join(f"  {formatar(resultados[t][trecho])}" for t in TAMANHOS)
        print(linha)
