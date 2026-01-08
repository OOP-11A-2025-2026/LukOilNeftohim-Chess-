# Chess Engine

Високопроизводителен шахматен engine написан на C++ с битборд представяне и минималистичен дизайн.

## Архитектура

### Философия на дизайна

Този проект следва функционален подход към C++:
- **Никакви ненужни класове**: Използваме POD структури и свободни функции където е възможно
- **Максимална производителност**: Bitboard представяне за O(1) операции
- **Нулев overhead**: Constexpr функции, inline операции, минимално копиране
- **Чист интерфейс**: Ясно разделение между модули

### Файлова структура
```
chess-engine/
├── apps/               # Executable applications
│   └── main.cpp       # Main console interface
├── include/chess/     # Public headers
│   ├── core/          # Core chess logic
│   │   ├── board.hpp  # Bitboard representation & operations
│   │   ├── move.hpp   # Move encoding (16-bit)
│   │   ├── piece.hpp  # Piece types & utilities
│   │   └── rules.hpp  # Move generation & game rules
│   ├── engine/        # Search & evaluation
│   │   ├── eval.hpp   # Static position evaluation
│   │   └── search.hpp # Alpha-beta search with TT
│   ├── parser/        # Notation parsing
│   │   ├── fen.hpp    # FEN import/export
│   │   ├── san.hpp    # Standard Algebraic Notation
│   │   └── png.hpp    # PGN game format
│   ├── storage/       # File I/O
│   │   ├── file_reader.hpp
│   │   └── file_writer.hpp
│   └── ui/            # User interface
│       └── render.hpp # Console board rendering
├── src/               # Implementation files
│   ├── core/
│   ├── engine/
│   ├── parser/
│   ├── storage/
│   └── ui/
├── tests/             # Unit tests (future)
├── external/          # Third-party dependencies
├── CMakeLists.txt     # Build configuration
└── LICENSE
```

## Модули

### Core (`chess/core/`)

Основната логика на играта без зависимости.

**board.hpp/cpp**
- Bitboard представяне (64-bit integers)
- 12 bitboards: 6 типа фигури × 2 цвята
- Zobrist hashing за transposition tables
- Attack generation с magic bitboards (за sliding pieces)

**move.hpp/cpp**
- 16-bit move encoding (from/to/promotion/flags)
- Константно време за encode/decode
- Специални флагове за рокада, en passant

**piece.hpp/cpp**
- Enum типове за фигури и цветове
- Utility функции за конвертиране
- Никакви класове - само типове и функции

**rules.hpp/cpp**
- Legal move generation
- Pseudo-legal move generation (по-бързо)
- Game state detection (мат, пат, ремита)

### Engine (`chess/engine/`)

AI компонентът на шахматния engine.

**eval.hpp/cpp**
- Static evaluation функция
- Material, position, mobility, king safety
- Piece-square tables (middlegame/endgame)
- Tapered evaluation

**search.hpp/cpp**
- Alpha-beta pruning
- Quiescence search
- Transposition table
- Move ordering (MVV-LVA, history heuristic)
- Time management

### Parser (`chess/parser/`)

Парсване и генериране на различни шахматни формати.

**fen.hpp/cpp**
- Forsyth-Edwards Notation
- Импорт/експорт на позиции

**san.hpp/cpp**
- Standard Algebraic Notation
- Long Algebraic Notation (LAN)
- UCI notation

**png.hpp/cpp**
- Portable Game Notation
- Импорт/експорт на цели партии
- Metadata (играчи, дата, резултат)

### Storage (`chess/storage/`)

Файлови операции без external dependencies.

**file_reader.hpp/cpp**
- Text/binary четене
- Line-by-line streaming

**file_writer.hpp/cpp**
- Text/binary писане
- Buffered writing

### UI (`chess/ui/`)

Конзолен интерфейс.

**render.hpp/cpp**
- ASCII/Unicode rendering
- Координати и highlighting
- Move list formatting

## Технически детайли

### Bitboards

Използваме 64-bit integers за представяне на дъската:
```
  A  B  C  D  E  F  G  H
8 56 57 58 59 60 61 62 63
7 48 49 50 51 52 53 54 55
6 40 41 42 43 44 45 46 47
5 32 33 34 35 36 37 38 39
4 24 25 26 27 28 29 30 31
3 16 17 18 19 20 21 22 23
2  8  9 10 11 12 13 14 15
1  0  1  2  3  4  5  6  7
```

### Move Encoding (16 bits)
```
Bits  0-5:  From square (0-63)
Bits  6-11: To square (0-63)
Bits 12-13: Promotion type (0=N, 1=B, 2=R, 3=Q)
Bits 14-15: Special flags (castle, en passant, etc)
```

### Performance Features

- **Magic Bitboards**: Pre-computed sliding piece attacks
- **Zobrist Hashing**: O(1) position hashing
- **Transposition Table**: Avoid re-searching positions
- **Move Ordering**: Search best moves first
- **Quiescence Search**: Avoid horizon effect

## Build Instructions
```bash
mkdir build && cd build
cmake ..
cmake --build .
./apps/chess-console
```

## Usage Example
```cpp
#include "chess/core/board.hpp"
#include "chess/parser/fen.hpp"
#include "chess/engine/search.hpp"

using namespace chess;

int main() {
    auto board = parse_fen(STARTING_FEN).value();
    
    SearchLimits limits;
    limits.max_depth = 6;
    
    auto result = search(board, limits);
    
    std::cout << "Best move: " << move_to_uci(result.best_move) << "\n";
    std::cout << "Score: " << result.score << "\n";
    std::cout << "Nodes: " << result.nodes_searched << "\n";
    
    return 0;
}
```

## Dependencies

- **Стандартна библиотека**: C++17 or later
- **Build system**: CMake 3.15+
- **Compiler**: GCC 9+, Clang 10+, MSVC 2019+

Никакви external dependencies - всичко е self-contained.

## Roadmap

- [ ] Magic bitboard generation
- [ ] Opening book
- [ ] Endgame tablebases (Syzygy)
- [ ] UCI protocol support
- [ ] Multi-threaded search
- [ ] Neural network evaluation (NNUE)
- [ ] Tunable parameters


