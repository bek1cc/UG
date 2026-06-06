# UG Gamemode - Changelog

## [2026-06-06] - CEF Plugin Integration + Phone Auto-Grant

### Popravljeno
- **/phone komanda** - svi igraci UVIJEK imaju telefon, nema vise "Nemate telefon"
  - `iPhone_SyncPlayerData` sada automatski dodjeljuje broj ako je 0
  - `iPhone_HasPhone = true` i `iPhone_Online = true` uvijek
  - Ako igrac nema sacuvan broj, automatski se generise (10000 + playerid)
- **CEF plugin** - sada je DIO standardne konfiguracije
  - `server.cfg` - CEF.so dodat u plugins liniju
  - `fg-ogc.pwn` - `#define USE_CEF` dodato PRIJE `#include <cef_tablet>`
  - `cef_tablet.inc` - koristi pravi samp-cef API (cef_create_browser, cef_emit_event, cef_subscribe)
  - `app.js` - koristi pravi samp-cef JS API (window.cef.emit / window.cef.subscribe)
  - Fix: `Bounty_CheckPIN` → `Bounty_LoadPIN`, `Bounty_SetPIN` → `Bounty_SavePIN`

### Novo
- **CEF Tablet UI** kompletno redizajniran
  - Lock screen sa "UNICATE GAMING" brandingom i plavom gradient pozadinom
  - Login screen sa PIN kodom (prijava + registracija)
  - Home screen sa app ikonama (Telefon, Poruke, Bounty, Banka, Kontakt, GPS, Dark Web, Hitna, Podesi)
  - Bounty Board app sa formom za postavljanje nagrade i listom
  - Telefon app sa keypad-om
  - SMS app sa formom za slanje
  - Toast notifikacije
  - Modern dark theme sa plavim akcentima

### Fajlovi
- `CEF/tablet/index.html` - Kompletni tablet UI sa svim screenovima
- `CEF/tablet/style.css` - Moderan CSS sa dark theme, animacijama, gradient bojama
- `CEF/tablet/app.js` - JS sa pravim samp-cef API (window.cef.emit / window.cef.subscribe)
- `pawno/include/cef_tablet.inc` - PAWN integracija sa samp-cef nativnim funkcijama
- `pawno/include/iphone.inc` - Fix: SyncPlayerData uvijek daje telefon
- `server.cfg` - CEF.so dodat u plugins liniju
- `gamemodes/fg-ogc.pwn` - #define USE_CEF dodato

### Kako pokrenuti CEF
1. Preuzmi CEF plugin sa https://github.com/samp-cef/CEF/releases
2. Stavi CEF.so (Linux) ili CEF.dll (Windows) u /plugins/ folder
3. Pokreni server - CEF.so je vec u plugins liniji
4. Ako CEF plugin nije instaliran, /tablet koristi textdraw fallback

---

## [2026-06-06] - CEF Tablet Portal UI v4.0

### Novo
- **CEF (Chromium Embedded Framework) Tablet UI** - moderni HTML/CSS/JS interfejs za tablet
  - Kompletna zamjena textdraw sistema sa web baziranim UI-jem
  - **Portal/Lock Screen** sa "UNICATE GAMING" brandingom, particle efekti, glow animacije
  - **Login sistem** sa PIN kodom (registracija + prijava)
  - **Home Screen** sa 6 app ikona: Bounty Board, Profil, Podešavanja, Trgovina, Mapa, Oglasi
  - **Bounty Board App** sa listom nagrada, paginacijom, modal za naručivanje ubojstva
  - **Status Bar** sa satom, baterijom (postotak!), signalom UG 4G
  - **Toast notifikacije** za feedback korisniku
  - **Glassmorphism** i **3D efekti** - sjajne ivice, pozadinski glow, blur efekti

### Fajlovi
- `CEF/tablet/index.html` - Glavni HTML sa svim screenovima
- `CEF/tablet/style.css` - Moderan CSS sa plavom paletom, animacijama, glassmorphism
- `CEF/tablet/app.js` - JavaScript kontroler za interakciju i PAWN komunikaciju
- `pawno/include/cef_tablet.inc` - PAWN integracija (Pawn.CEF plugin potreban)

### Zavisnosti
- **Pawn.CEF plugin** - mora biti instaliran na serveru (`plugins/cef` u server.cfg)
- Font Awesome 6.5 - za ikone (loaduje se sa CDN-a)
- Google Fonts Inter + JetBrains Mono

### Komande
- `/tablet` - otvori/zatvori CEF tablet
- `/bounties` - otvori tablet na bounty stranici

### Web Preview
- Live preview dostupan u browseru za testiranje UI-ja bez pokretanja servera

### Napomene
- Ovo je PARALELNO sa postojećim textdraw sistemom (`bounty.inc`)
- Textdraw verzija i dalje radi ako CEF plugin nije instaliran
- Za potpuni prelaz na CEF, potrebno instalirati Pawn.CEF plugin

---

## [2026-06-06] - iPhone.inc Kompilacija Fix

### Popravljeno
- **2D enum nizovi unutar enuma** - Pawn ne podržava `array[SIZE][EnumType]` unutar enuma
  - `iPhone_Contacts`, `iPhone_SMS`, `iPhone_Recent` premješteni u zasebne globalne nizove
  - `g_iPhoneContacts`, `g_iPhoneSMS`, `g_iPhoneRecent` - odvojene varijable
- **DWLocations Float tag sa stringovima** - zamijenjeno sa enum-baziranim pristupom `DWLocData`
  - `iPhone_DWLocations[locIdx][dwX/dwY/dwZ/dwName]` umjesto indeksa `[0]/[1]/[2]/[3]`
- **Konflikt imena** - `iPhone_IsOpen` bio i enum konstanta i funkcija
  - Enum polja preimenovana u `iPhone_Opened`, funkcija zadržala ime `iPhone_IsOpen()`

---

## [2026-06-06] - Bounty Board Tablet Portal UI v3.0

### Novo
- **Kompletni redizajn Bounty Board-a** sa modernim TABLET PORTAL UI-jem
  - **Login Portal** sa "UNICATE GAMING" brandingom, plave nijanse, 3D detalji
  - **Baterija** - prikazuje postotak baterije tableta (vizualni efekat)
  - **Registracija** - igrači postavljaju 4-cifreni PIN kod za pristup tabletu
  - **Prijava** - unos PIN koda za ulazak, 3 pogrešna pokušaja = zaključavanje
  - **PIN se čuva** u fajlu `Tablet/%s.ini` (perzistentno između sesija)
  - **Main ekran** - BOUNTY BOARD, PROFIL, PODEŠAVANJA ikone
  - **Bounty Board ekran** - lista svih aktivnih nagrada sa "NARUČI UBOJSTVO" dugmetom
  - **ESC zatvara** tablet (OnPlayerClickTextDraw callback dodan)
  - **Signal/Vrijeme** indikatori u status baru
  - Plave nijanse pozadine sa sjajnim ivicama (3D efekt)

### Komande
- `/bounty [ID] [Iznos]` - naruči ubojstvo (izvan tableta)
- `/bounties` - otvori tablet portal
- `/bountyme` - provjeri da li imaš nagradu na sebi

### Tok rada
1. Igrač kuca `/bounties` → Otvara se portal sa "UNICATE GAMING" logom
2. Ako nije registrovan → Klikne "REGISTRUJ SE" → Postavi 4-cifreni PIN
3. Ako je registrovan → Klikne "PRIJAVI SE" ili lozinku polje → Unese PIN
4. Uspješna prijava → Main ekran sa ikonama
5. Klikne BOUNTY BOARD → Lista svih nagrada + "NARUČI UBOJSTVO" dugme
6. ESC ili "ODJAVI SE" → Zatvara tablet

---

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
