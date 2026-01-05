# Agentowy Symulator Strategii i Fizyki Wyścigu

## O Projekcie
Agentowy Symulator Strategii i Fizyki Wyścigu to zaawansowany, konsolowy symulator wyścigów napisany w języku C++. W odróżnieniu od typowych gier wyścigowych, ten projekt kładzie nacisk na głębię matematyczną, modelowanie fizyki opon i paliwa, oraz strategiczne podejmowanie decyzji przez agentów AI.

System symuluje dynamiczne środowisko wyścigowe, gdzie kierowcy (agenci) rywalizują ze sobą, adaptując swoje strategie w czasie rzeczywistym w zależności od zużycia opon, zmieniającej się pogody i zdarzeń na torze.

## Kluczowe Funkcjonalności

### 1. Zaawansowany Model Fizyczny
Silnik fizyczny oblicza zachowanie bolidu w dyskretnych krokach czasowych, uwzględniając:
* **Model Opon:** Dynamiczny współczynnik przyczepności ($\mu$) zależny od mieszanki, temperatury, zużycia oraz warunków pogodowych.
* **Charakterystykę Toru:** Tor składa się z segmentów (proste, zakręty) o różnej długości i promieniu, co wpływa na prędkość maksymalną i przeciążenia.
* **Osiągi Bolidu:** Moc silnika, aerodynamika, masa (zmienna wraz ze zużyciem paliwa).

### 2. Inteligentni Agenci (AI)
Każdy kierowca jest autonomicznym agentem podejmującym decyzje strategiczne:
* **Analiza "What-If":** Agenci symulują przyszłe scenariusze (np. "zjechać teraz czy za 3 okrążenia?"), aby zminimalizować całkowity czas wyścigu.
* **Adaptacja:** Reakcja na zmieniające się warunki (np. deszcz) i stan bolidu.

### 3. Konfigurowalność
Symulacja jest w pełni konfigurowalna za pomocą plików tekstowych:
* **Tory:** Definicje segmentów toru (proste, zakręty).
* **Kierowcy i Zespoły:** Parametry umiejętności kierowców i osiągów bolidów.
* **Konfiguracja Globalna:** Liczba okrążeń, parametry symulacji.

## Wymagania Techniczne
* **Język:** C++20 (wykorzystanie `std::filesystem`, nowoczesnych elementów biblioteki standardowej).
* **System Budowania:** CMake (wersja 3.10 lub nowsza).
* **Kompilator:** GCC, Clang lub MSVC wspierający C++20.
* **Zależności:** Wyłącznie Biblioteka Standardowa C++ (STL).


## Struktura Projektu

```
.
├── CMakeLists.txt      # Konfiguracja CMake
├── src/                # Pliki źródłowe (.cpp)
│   ├── main.cpp        # Punkt wejścia, pętla główna
│   ├── Car.cpp         # Logika bolidu i fizyki
│   ├── Race.cpp        # Logika wyścigu
│   └── ConfigParser.cpp # Obsługa plików konfiguracyjnych
├── include/            # Pliki nagłówkowe (.h)
│   ├── Car.h
│   ├── Race.h
│   ├── ConfigParser.h
│   └── DataStructures.h
└── config/             # Pliki konfiguracyjne
    ├── config.txt      # Główna konfiguracja
    ├── *_track.txt     # Definicje torów
    └── ...
```

## Jak korzystać?
1. Uruchom aplikację.
2. Wybierz tor z listy dostępnych (wczytanych z katalogu `config/`).
3. Wybierz warunki pogodowe.
4. Obserwuj przebieg kwalifikacji i wyścigu w konsoli.
5. Wyniki i logi są wyświetlane na bieżąco.

## Autor
Projekt stworzony w ramach zaliczenia przedmiotu studenckiego.
