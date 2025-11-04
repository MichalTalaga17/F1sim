# Roadmap Symulatora Wyścigów F1

Ten plik śledzi postęp prac nad projektem, zrealizowane funkcjonalności oraz plany na przyszłość.

## Zrealizowane Funkcjonalności (MVP i Rozszerzenia)

- [x] **Podstawy Projektu**
  - [x] Inicjalizacja projektu w C++ z użyciem CMake.
  - [x] Stworzenie podstawowej pętli symulacji.
- [x] **Struktura Danych**
  - [x] Wprowadzenie podstawowych klas (`Car`, `Track`, `Race`).
  - [x] Refaktoryzacja struktury plików (`src`, `include`).
  - [x] Wprowadzenie polimorficznych segmentów toru (`Straight`, `Corner`).
  - [x] Obsługa lewych/prawych zakrętów (ujemny/dodatni kąt).
- [x] **Model Symulacji (1D)**
  - [x] Symulacja wyścigu dla jednego bolidu.
  - [x] Rozszerzenie symulacji na wiele bolidów (pełna stawka).
- [x] **Fizyka i Parametry**
  - [x] Wprowadzenie dynamicznego modelu fizyki (przyspieszenie, hamowanie, prędkość w zakrętach).
  - [x] Wprowadzenie modelu pogody (`Weather`) i jej wpływu na przyczepność.
- [x] **Model Danych (Data-Driven Design)**
  - [x] Stworzenie struktury `Team` do przechowywania parametrów bolidu.
  - [x] Stworzenie struktury `Driver` do przechowywania umiejętności kierowcy.
  - [x] Pełna integracja parametrów Zespołu i Kierowcy z modelem fizyki.
  - [x] Zasilenie symulacji danymi z oficjalnych ratingów EA F1.

## Planowane Funkcjonalności (Do Wdrożenia)

- [x] **Zaawansowany Model Opon**
  - [x] Różne mieszanki opon (Soft, Medium, Hard) z unikalnymi parametrami przyczepności i zużycia.
  - [x] Model zużycia opon w trakcie jazdy, wpływający na przyczepność.
  - [ ] Model temperatury opon i jej wpływ na przyczepność.

- [x] **Strategia Wyścigowa**
  - [ ] Rozszerzenie symulacji na wiele okrążeń. (Częściowo zrobione, ale nie w pełni)
  - [x] Implementacja Pit Stopów (zmiana opon).
  - [x] Podstawy AI strategicznego (decyzje o zjeździe do boksu).

- [ ] **Zaawansowana Fizyka i Model Toru**
  - [ ] Wprowadzenie modelu toru w 2D/3D (pozycja X, Y dla każdego bolidu).
  - [ ] Implementacja logiki wyprzedzania i obrony pozycji.
  - [ ] Wykorzystanie statystyk `Racecraft (RAC)` i `Awareness (AWA)`.
  - [ ] Modelowanie zdarzeń losowych (wypadki, samochód bezpieczeństwa).

- [x] **Ulepszenia i Refaktoryzacja**
  - [ ] Dostrajanie parametrów fizyki dla większego realizmu. (Częściowo zrobione)
  - [ ] Wczytywanie konfiguracji zespołów/kierowców z zewnętrznych plików (np. CSV, JSON).
  - [x] Poprawa wizualizacji wyników i przebiegu wyścigu w konsoli.