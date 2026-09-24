# Programacao Paralela (CCO085) — IESB 2026/2
# Uso: make                -> compila tudo
#      make perfilamento   -> compila e executa o perfilamento do marco 1
#      make clean          -> remove os binarios

CXX      = g++
CXXFLAGS = -O2 -Wall

BUILD  = build
MARCO1 = marco1

BIN = $(BUILD)/07_mergesort $(BUILD)/perfilamento

all: $(BIN)

perfilamento: $(BUILD)/perfilamento
	./$(BUILD)/perfilamento

$(BUILD)/%: $(MARCO1)/%.cpp
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf $(BUILD)

.PHONY: all perfilamento clean
