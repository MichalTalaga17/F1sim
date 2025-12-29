# Roadmap Symulatora Wyścigów F1

Ten plik śledzi postęp prac nad projektem, zrealizowane funkcjonalności oraz plany na przyszłość.

## Zrealizowane Funkcjonalności (MVP i Rozszerzenia)

- [] **Podstawy Projektu**
  - [] Inicjalizacja projektu w C++ z użyciem CMake.
  - [] Stworzenie podstawowej pętli symulacji.
- [] **Struktura Danych**
  - [] Wprowadzenie podstawowych klas (`Car`, `Track`, `Race`).
  - [] Refaktoryzacja struktury plików (`src`, `include`).
  - [] Wprowadzenie polimorficznych segmentów toru (`Straight`, `Corner`).
  - [] Obsługa lewych/prawych zakrętów (ujemny/dodatni kąt).
- [] **Model Symulacji (1D)**
  - [] Symulacja wyścigu dla jednego bolidu.
  - [] Rozszerzenie symulacji na wiele bolidów (pełna stawka).
- [] **Fizyka i Parametry**
  - [] Wprowadzenie dynamicznego modelu fizyki (przyspieszenie, hamowanie, prędkość w zakrętach).
  - [] Wprowadzenie modelu pogody (`Weather`) i jej wpływu na przyczepność.
- [] **Model Danych (Data-Driven Design)**
  - [] Stworzenie struktury `Team` do przechowywania parametrów bolidu.
  - [] Stworzenie struktury `Driver` do przechowywania umiejętności kierowcy.
  - [] Pełna integracja parametrów Zespołu i Kierowcy z modelem fizyki.
  - [] Zasilenie symulacji danymi z oficjalnych ratingów EA F1.

## Planowane Funkcjonalności (Do Wdrożenia)

- [] **Zaawansowany Model Opon**
  - [] Różne mieszanki opon (Soft, Medium, Hard) z unikalnymi parametrami przyczepności i zużycia.
  - [] Model zużycia opon w trakcie jazdy, wpływający na przyczepność.
  - [ ] Model temperatury opon i jej wpływ na przyczepność.

- [] **Strategia Wyścigowa**
  - [ ] Rozszerzenie symulacji na wiele okrążeń. (Częściowo zrobione, ale nie w pełni)
  - [] Implementacja Pit Stopów (zmiana opon).
  - [] Podstawy AI strategicznego (decyzje o zjeździe do boksu).

- [ ] **Zaawansowana Fizyka i Model Toru**
  - [ ] Wprowadzenie modelu toru w 2D/3D (pozycja X, Y dla każdego bolidu).
  - [ ] Implementacja logiki wyprzedzania i obrony pozycji.
  - [ ] Wykorzystanie statystyk `Racecraft (RAC)` i `Awareness (AWA)`.
  - [ ] Modelowanie zdarzeń losowych (wypadki, samochód bezpieczeństwa).

- [] **Ulepszenia i Refaktoryzacja**
  - [ ] Dostrajanie parametrów fizyki dla większego realizmu. (Częściowo zrobione)
  - [ ] Wczytywanie konfiguracji zespołów/kierowców z zewnętrznych plików (np. CSV, JSON).
  - [] Poprawa wizualizacji wyników i przebiegu wyścigu w konsoli.