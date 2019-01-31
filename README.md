## Řešení soustav lineárních rovnic

Semestrální práce na předmět PJC.

### Popis

Tento program slouží k výpočtu soustav lineárních
rovnic o n neznámých. Jako vstup slouží textový soubor
obsahující matici ve tvaru: **číslo[mezera]číslo[mezera]číslo**, a každý řádek matice je na novém řádku souboru.

### Implementace

Pro implementaci je použit algoritmus Gaussova eliminace se
zpětným dosazením. Vícevláknová varianta používá takový počet vláken, jako má počítač jader: Každému vláknu je přidělena
část matice, nad kterou provede Gaussovu eliminaci. 

### Použití

Program může najednou přijmout 1-2 argumenty, a to v tomto pořadí:
* [path to file] - zde je použit jednovláknový přístup
* [algorithm type] [path to file]

Přepínače:
* --help
* --sequence
* --parallel

### Měření

Měření proběhlo na počítači se 4 jádry a operačním systémem macOS Mojave. Měření proběhlo nad náhodně vygenerovanou maticí
velikosti 1000x1001. Vyšlo mi, že jednovláknová varianta potřebuje k dokončení výpočtu 965 ms, zatímco ta vícevláknová
pouze 553 ms.
