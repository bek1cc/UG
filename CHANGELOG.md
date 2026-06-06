# UG Gamemode - Changelog

## [2026-06-06] - Bounty Board Sistem v1.0

### Novo
- **Bounty Board sistem** (`pawno/include/bounty.inc`) - javna tablica nagrada
  - Igrači stavljaju novac na glavu drugih igrača
  - Ko ubije metu, dobija nagradu
  - Više bountyja se zbrajaju na istu metu
  - Tab lista sa statusom (online/offline), iznosom, ko je postavio
  - Potvrda prije postavljanja (dialog)
  - Cooldown 5 min između postavljanja
  - Meta dobija obavijest kad dobije bounty
  - Globalna obavijest za sve igrače
  - GameText + zvuk kad ubica pokupi nagradu
  - Bounty ostaje i kad meta disconnecta (match po imenu)

### Komande
- `/bounty [ID] [Iznos]` - postavi nagradu na igrača
- `/bounty` (bez parametara) - pogledaj listu svih nagrada
- `/bounties` - pregled svih aktivnih bountyja
- `/bountyme` - provjeri da li imaš nagradu na sebi

### Pravila
- Minimum: $1,000 | Maksimum: $500,000
- Ne možeš staviti na sebe
- Cooldown: 5 minuta

---

## [2026-06-06] - iPhone Telefon Sistem v2.0

### Novo
- **iPhone UI sistem** (`pawno/include/iphone.inc`) - moderan telefon sa textdraw UI
  - Aplikacije: Telefon, SMS, Dark Web, Kontakti, Banka, GPS, Oglasi, Kiosk, Settings
  - Hitne sluzbe: 911, 555 (Mehanicar), 777 (Taxi)
  - Dynamic Island notch, status bar, app grid (3x4), dock (4 ikone)
  - Kontakti save/load po broju telefona
  - Dark Web marketplace sa dostavom na random lokaciju
  - GPS tracker za pracenje igraca
  - Kiosk za kupovinu bona ($20/$50/$100/$500)
  - Banka: stanje, prenos novca
  - SMS notifikacija sa zvukom

### Komande
- `/phone` - otvara/zatvara iPhone UI
- **Y tipka** - brzo otvaranje telefona
- **ESC tipka** - zatvaranje telefona

### Integracija
- SMS kredit sync u realnom vremenu
- iPhone notifikacija kad stigne SMS
- `/togphone` sync sa iPhone online statusom

---

## [2026-06-06] - Plastika Baza CP Fix

### Popravljeno
- Uklonjen permanentni `CreateDynamicCP` u bazi Plastike koji je bio stalno vidljiv
- Uklonjen permanentni 3D label "[BAZA - PLATA]" koji je bio stalno vidljiv
- Per-player `SetPlayerCheckpoint` vec radio ispravno (pali samo u PLAS_TRUCK_RETURNING)
- Dodan `RemovePlayerMapIcon` u `Plastika_OnVehicleDeath()` cleanup

---

## [2026-06-05] - Plastika Fabrika Popravke

### Popravljeno
- ALT key za uzimanje opreme - fixan early return koji je blokirao ALT
- Gajba (crate) freezing i invisible - promijenjen model, animacija, velicina
- Minigame brzina i zelena zona podeseni
- Crveni CP na mapi za dostavu cijevi - dodani map icons
- Nova mapa fabrike plastike (`plastika_mapa.inc`) - 349 objekata + materijali
- Kliziste mapa - objekti se ucitavaju samo kad je aktivan event
- `UnistiObjektePlastike()` funkcija dodana
- `Kliziste_DestroyMapObjects()` funkcija dodana
