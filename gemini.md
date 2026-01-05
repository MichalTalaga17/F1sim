# Brief Technologiczny: Agentowy Symulator Strategii Wyścigów

**Projekt:** "Agentowy Symulator Strategii i Fizyki Wyścigu"
**Cel:** Stworzenie wysoce modularnego, konsolowego symulatora wyścigów w C++, z naciskiem na realistyczny model fizyki (opon, paliwa, pogody) oraz podejmowanie decyzji strategicznych przez agentów AI (teoria gier, optymalizacja).

---

## 1. Język i Standard

* **Język:** **C++**
* **Standard:** **C++17** (lub C++20)
    * **Uzasadnienie:** Dostęp do `std::variant`, `std::optional`, ulepszonej biblioteki `<random>` oraz `std::filesystem` do wczytywania konfiguracji.

## 2. Środowisko i Budowanie

* **System Budowania:** **CMake**
    * **Uzasadnienie:** Standard branżowy, umożliwia łatwe zarządzanie złożonym projektem i kompilację na różnych platformach.
* **Kompilator:** GCC (g++), Clang lub MSVC.
* **Kontrola Wersji:** **Git**

## 3. Biblioteki Podstawowe

Projekt będzie opierał się **wyłącznie na Bibliotece Standardowej C++ (STL)**.

* **Matematyka i Fizyka (`<cmath>`, `<random>`):**
    * `<cmath>`: Do obliczeń fizycznych (wzory na przyczepność, prędkość).
    * `<random>`: Do modelowania zdarzeń losowych i rozkładów (np. `std::normal_distribution` dla "konsystencji" kierowcy).
* **Struktury Danych (`<vector>`, `<map>`, `<memory>`):**
    * `std::vector`: Do przechowywania segmentów toru, listy kierowców.
    * `std::map`: Do przechowywania konfiguracji (np. parametry bolidu).
    * `std::unique_ptr` / `std::shared_ptr`: Do zarządzania czasem życia obiektów (np. strategiami AI).
* **Logika Czasu (`<chrono>`):**
    * Do precyzyjnego mierzenia i sumowania czasu symulacji (kroki czasowe $\Delta t$) oraz czasów okrążeń.
* **Wejście/Wyjście (`<iostream>`, `<fstream>`, `<sstream>`):**
    * Wczytywanie konfiguracji torów i bolidów z plików `.txt` lub `.csv`.
    * Logowanie przebiegu wyścigu do konsoli i plików `.log`.

## 4. Architektura i Wzorce Projektowe

Paradygmatem wiodącym będzie **Programowanie Obiektowe (OOP)**.

* **Kluczowe Klasy:**
    * `Vector2D`: Podstawa matematyczna.
    * `TrackSegment` (Baza) $\rightarrow$ `Straight`, `Corner` (Pochodne): Polimorficzna reprezentacja toru.
    * `Tire`: Przechowuje stan (mieszanka, zużycie, temperatura) i oblicza `μ` (przyczepność).
    * `Car`: Model fizyczny i komponenty.
    * `AgentAI`: Agent podejmujący decyzje.
    * `RaceControl`: Singleton zarządzający stanem wyścigu (pogoda, Safety Car).
* **Wzorce Projektowe:**
    * **Wzorzec Strategii (Strategy Pattern):** Kluczowy dla AI. `AgentAI` będzie posiadał `std::unique_ptr<IStrategy>`. Implementacje (np. `AggressivePush`, `ConserveTires`) będą dynamicznie podmieniane.
    * **Wzorzec Obserwatora (Observer Pattern):** `RaceControl` jako Podmiot, `AgentAI` jako Obserwatorzy. Agenci reagują na zdarzenia (np. `notify("SAFETY_CAR")`).
    * **Wzorzec Stanu (State Pattern):** Do zarządzania stanem bolidu (np. `OnTrack`, `InPit`) lub opon (`Cold`, `Optimal`, `Overheated`). 

## 5. Model AI

* **Typ:** Agentowy system decyzyjny (nie-ML).
* **Logika:** **Algorytm optymalizacyjny "What-If"**.
* **Implementacja:** Agent AI uruchamia wewnętrzne, uproszczone symulacje wyścigu do mety dla każdej z możliwych opcji strategicznych (np. "zjedź teraz", "zjedź za 5 okrążeń"). Wybiera strategię, która daje najlepszy (najkrótszy) przewidywany czas ukończenia wyścigu.
* **Rozszerzenie (Opcjonalne):** **Metoda Monte Carlo**, gdzie każda strategia jest symulowana wielokrotnie (np. 1000 razy) z uwzględnieniem prawdopodobieństwa zdarzeń losowych (awarie, SC), aby wybrać strategię o najlepszej *wartości oczekiwanej*.

## 6. Wejście / Wyjście

* **Wejście:** Proste pliki konfiguracyjne `.csv` lub `.txt` definiujące tory i parametry bolidów/kierowców.
* **Wyjście:** Logowanie przebiegu wyścigu na konsolę oraz zapis wyników i czasów okrążeń do pliku `results.csv` w celu analizy.

# O Projekcie: Agentowy Symulator Strategii Wyścigów

## Cel Projektu

Celem projektu jest stworzenie konsolowego symulatora wyścigów w C++. Nacisk położony jest nie na grafikę, lecz na głębię matematyczną i strategiczną, która napędza współczesne wyścigi.

Projekt symuluje dynamiczne środowisko toru wyścigowego, w którym wielu agentów AI (kierowców) rywalizuje ze sobą, podejmując decyzje w czasie rzeczywistym w oparciu o fizykę i teorię gier.

## Kluczowe Cechy

### 1. Model Fizyczny
Rdzeniem symulatora jest model fizyczny oparty na dyskretnych krokach czasowych. Oblicza on prędkość, przyspieszenie i czas przejazdu, bazując na:
* **Modelu Opon:** Dynamicznie obliczany współczynnik przyczepności ($\mu$) zależny od mieszanki, temperatury i poziomu zużycia.
* **Modelu Toru:** Tor jest sekwencją prostych i zakrętów, z których każdy ma własne parametry (długość, promień) wpływające na fizykę.
* **Modelu Bolidu:** Uwzględnienie mocy silnika, oporów powietrza i masy (zmieniającej się wraz ze zużyciem paliwa).

### 2. Dynamiczne Środowisko
Stan wyścigu nie jest statyczny. System (klasa `RaceControl`) zarządza globalnymi zdarzeniami, które wpływają na wszystkich agentów:
* **Pogoda:** Dynamiczne zmiany przyczepności toru (mokro/sucho), wymuszające zmianę opon.
* **Zdarzenia Losowe:** Symulacja Samochodów Bezpieczeństwa (Safety Car), wirtualnych samochodów bezpieczeństwa (VSC) oraz losowych awarii.

### 3. Inteligentni Agenci (AI)
"AI" w tym projekcie to nie model uczenia maszynowego, lecz zaawansowany **system decyzyjny** oparty na algorytmach optymalizacyjnych.
* **Świadomość Sytuacyjna:** Każdy agent zna stan swoich opon, paliwa, pozycję na torze oraz pogodę.
* **Strategia "What-If":** Głównym zadaniem AI jest optymalizacja strategii pit-stopów. Agent potrafi uruchomić tysiące wewnętrznych, uproszczonych symulacji "co-gdyby" (np. "co jeśli zjadę teraz?", "co jeśli poczekam 3 okrążenia?"), aby obliczyć, która strategia przyniesie najlepszy całkowity czas wyścigu.
* **Reaktywność:** Agenci natychmiast przeliczają swoje strategie w odpowiedzi na zdarzenia (np. wyjazd Safety Car drastycznie obniża "koszt" pit-stopu, co powinno wywołać falę zjazdów do alei).


KOMENTARZE ROB W JEZYKU POLSKIM