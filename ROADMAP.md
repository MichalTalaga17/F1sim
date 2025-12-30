# Roadmap Symulatora Wyścigów F1

Stan na: 30 Grudnia 2025

## Zrealizowane (Done)

### 1. Podstawy i Architektura
- [x] Inicjalizacja projektu CMake i struktura katalogów (src/include).
- [x] System wczytywania konfiguracji z plików `.txt` (Tory, Kierowcy, Zespoły).
- [x] Klasy bazowe: `Car`, `Track` (Segmenty), `Race`, `ConfigParser`.
- [x] Ograniczenie komentarzy w kodzie do niezbędnego minimum (PL).

### 2. Fizyka i Model Jazdy
- [x] Model ruchu oparty na segmentach (Proste/Zakręty).
- [x] Fizyka przyczepności (wpływ opon, pogody, umiejętności kierowcy).
- [x] Zużycie opon (Tire Wear) wpływające na osiągi.
- [x] Symulacja Kwalifikacji (generowanie Gridu na podstawie osiągów).
- [x] Podstawowe wyprzedzanie (Traffic Logic) i błędy kierowców.

### 3. Interfejs i Logika Wyścigu
- [x] Pętla wyścigu z wizualizacją postępu w konsoli (ASCII bars).
- [x] Tabela wyników na żywo (Live Leaderboard).
- [x] Obsługa Pit Stopów (prosta logika czasowa).
- [x] Wybór pogody przed wyścigiem.

## Do Zrobienia (To-Do)

### 1. Inteligencja (AI) i Strategia – PRIORYTET
- [ ] **Strategia "What-If"**: Zastąpienie prostego `if (tire < 40%)` algorytmem symulującym przyszłość. Agent powinien przeliczyć, czy opłaca się zjechać teraz, czy później.
- [ ] **Wybór Mieszanki Opon**: Dodanie typów opon (Soft/Medium/Hard) i decyzji AI, którą mieszankę założyć.

### 2. Zdarzenia Losowe i Środowisko
- [ ] **Safety Car (SC) / VSC**: Implementacja globalnego spowolnienia stawki w przypadku "wypadku".
- [ ] **Dynamiczna Pogoda**: Zmiana pogody *w trakcie* wyścigu (np. zaczyna padać na 15. okrążeniu), wymuszająca zmianę strategii.
- [ ] **Awarie mechaniczne**: Losowe wycofania z wyścigu (DNF).

### 3. Fizyka - Rozszerzenia
- [ ] **Model Paliwa**: Wpływ malejącej masy paliwa na czasy okrążeń (wypalanie paliwa = szybszy bolid).
- [ ] **System DRS**: Bardziej formalne strefy DRS na prostych.

### 4. Dane i Wyniki
- [ ] **Eksport Wyników**: Zapis końcowej klasyfikacji i czasów wszystkich okrążeń do pliku `.csv`.
