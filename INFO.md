# INFO: Podsumowanie Projektu Symulatora Wyścigów

Ten plik służy jako szybkie podsumowanie projektu, jego architektury i historii rozwoju, aby umożliwić łatwe wznowienie pracy w przyszłości.

## 1. Opis Projektu

**Nazwa:** Agentowy Symulator Strategii i Fizyki Wyścigu
**Cel:** Stworzenie konsolowego symulatora wyścigów w C++, który skupia się na realistycznym modelu fizyki i danych, a nie na grafice.
**Obecny Stan:** Aplikacja symuluje pojedyncze okrążenie wyścigu dla pełnej stawki kierowców F1. Wydajność każdego bolidu jest wypadkową parametrów jego zespołu oraz indywidualnych umiejętności kierowcy, opartych na oficjalnych ratingach z gry EA F1. Symulacja uwzględnia dynamiczną prędkość, ograniczenia w zakrętach, wpływ pogody na przyczepność oraz zużycie paliwa.

## 2. Kluczowe Elementy Architektury

Projekt opiera się na podejściu obiektowym i data-driven design.

*   **`Track`**: Reprezentuje tor jako sekwencję segmentów (`Straight`, `Corner`). Obecnie jest to model 1D (dystans).
*   **`Weather`**: Symuluje warunki pogodowe (Słonecznie, Deszczowo), generując `gripModifier` (modyfikator przyczepności).
*   **`Team`**: Struktura danych przechowująca bazowe parametry techniczne bolidu danego zespołu (np. `topSpeed`, `baseTireGrip`).
*   **`Driver`**: Struktura danych przechowująca statystyki kierowcy (`Pace`, `Experience`, itd.), pobrane z oficjalnych ratingów.
*   **`Car`**: Główna klasa symulacji. Jest tworzona z konkretnego `Driver` i `Team`. W konstruktorze oblicza *efektywne* parametry wydajności, które są kombinacją statystyk zespołu i kierowcy. Metoda `update()` zawiera logikę fizyki (model "prędkości docelowej").
*   **`Race`**: Klasa orkiestrująca. Inicjalizuje wyścig, zarządza pętlą symulacji (krok po kroku) i na końcu wyświetla posortowane wyniki.
*   **`main.cpp`**: Pełni rolę "bazy danych". Definiuje wszystkie obiekty `Team` i `Driver`, mapuje je do siebie i inicjalizuje symulację.

## 3. Historia Rozwoju (Streszczenie Konwersacji)

1.  **MVP**: Stworzenie szkieletu aplikacji z jednym bolidem jadącym po prostym torze ze stałą prędkością.
2.  **Wiele Bolidów**: Rozbudowa logiki `Race` do obsługi pełnej stawki kierowców.
3.  **Dynamiczna Fizyka (Iteracja 1)**: Implementacja modelu "prędkości docelowej". Bolidy zaczęły zwalniać w zakrętach. Wprowadzono pogodę i zużycie paliwa.
4.  **Refaktoryzacja (Data-Driven Design)**:
    *   **Krok 1 (`Team`)**: Wydzielono parametry bolidu do struktury `Team`, aby rozróżnić osiągi między zespołami.
    *   **Krok 2 (`Driver`)**: Wydzielono umiejętności kierowcy do struktury `Driver`.
5.  **Integracja Danych**: Zaktualizowano dane w `main.cpp`, aby odzwierciedlały oficjalne ratingi EA F1. Statystyki `Pace` i `Experience` zostały włączone do modelu fizyki, wpływając odpowiednio na prędkość/przyczepność i zużycie paliwa.
6.  **Zarządzanie Projektem**: Stworzono plik `ROADMAP.md` do śledzenia postępów i planowania przyszłych prac.

## 4. Aktualny Status i Dalsze Kroki

Projekt jest w stabilnym stanie z zaimplementowanym rozbudowanym modelem danych i fizyki. Wszystkie dotychczasowe zmiany zostały zapisane w repozytorium Git.

Pełna lista zrealizowanych i planowanych zadań znajduje się w pliku **`ROADMAP.md`**.

Następne logiczne kroki to implementacja **modelu opon** oraz **wyścigów na wiele okrążeń z pit stopami**.
