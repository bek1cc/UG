#!/usr/bin/env python3
# =============================================================================
#  UNICATE GAMING LAUNCHER
#  Profesionalni launcher za SA-MP server
#  Svi igraci MORAJU instalirati launcher da bi igrali
#
#  Kompajliranje u .exe:
#    pip install pyinstaller pillow requests
#    pyinstaller --onefile --windowed --icon=ug_icon.ico --name="Unicate Gaming" launcher.py
# =============================================================================

import tkinter as tk
from tkinter import messagebox
import os
import sys
import subprocess
import threading
import time
import json
import struct
import socket
import webbrowser

try:
    from PIL import Image, ImageTk, ImageDraw, ImageFont
    HAS_PIL = True
except ImportError:
    HAS_PIL = False
    print("[UG Launcher] Pillow nije instaliran. Pokreni: pip install Pillow")

try:
    import requests
    HAS_REQUESTS = True
except ImportError:
    HAS_REQUESTS = False
    print("[UG Launcher] Requests nije instaliran. Pokreni: pip install requests")

# =============================================================================
#  KONFIGURACIJA SERVERA
# =============================================================================
SERVER_IP = "135.125.156.197"   # <-- PRAVI SERVER IP
SERVER_PORT = 7777              # <-- PROMIJENI NA PRAVI PORT
SERVER_NAME = "Unicate Gaming"
WEBSITE_URL = "https://ug-ogc.com"
DISCORD_URL = "https://discord.gg/unicategaming"

# Verzija launchera
LAUNCHER_VERSION = "1.0.0"

# Folderi
if getattr(sys, 'frozen', False):
    # Pokrenuto iz .exe
    LAUNCHER_DIR = os.path.dirname(sys.executable)
else:
    LAUNCHER_DIR = os.path.dirname(os.path.abspath(__file__))

# =============================================================================
#  BOJE (Unicate Gaming tema - plava/tamna)
# =============================================================================
COLOR_BG_DARK    = "#0a0e17"
COLOR_BG_PANEL   = "#111827"
COLOR_BG_CARD    = "#1a2332"
COLOR_ACCENT     = "#3b82f6"
COLOR_ACCENT_HOVER = "#2563eb"
COLOR_ACCENT_GLOW = "#1d4ed8"
COLOR_TEXT        = "#e2e8f0"
COLOR_TEXT_DIM    = "#94a3b8"
COLOR_TEXT_BRIGHT = "#ffffff"
COLOR_SUCCESS     = "#22c55e"
COLOR_ERROR       = "#ef4444"
COLOR_WARNING     = "#f59e0b"
COLOR_BORDER      = "#1e3a5f"
COLOR_INPUT_BG    = "#0f172a"

# =============================================================================
#  SA-MP QUERY - provjera statusa servera
# =============================================================================
def query_samp_server(ip, port, timeout=3):
    """Salje SA-MP query paket i vraca info o serveru"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.settimeout(timeout)

        # SA-MP 'i' info query paket
        packet = b'SAMP'
        ip_parts = ip.split('.')
        packet += struct.pack('4B', int(ip_parts[0]), int(ip_parts[1]),
                              int(ip_parts[2]), int(ip_parts[3]))
        packet += struct.pack('H', port)
        packet += b'i'

        sock.sendto(packet, (ip, port))
        data, _ = sock.recvfrom(2048)
        sock.close()

        if len(data) < 11:
            return None

        offset = 11
        # Password
        password_len = struct.unpack_from('H', data, offset)[0]
        offset += 2
        password = data[offset:offset+password_len].decode('latin-1', errors='replace')
        offset += password_len

        # Players
        players = struct.unpack_from('H', data, offset)[0]
        offset += 2

        # Max players
        max_players = struct.unpack_from('H', data, offset)[0]
        offset += 2

        # Server name
        name_len = struct.unpack_from('I', data, offset)[0]
        offset += 4
        server_name = data[offset:offset+name_len].decode('latin-1', errors='replace')
        offset += name_len

        # Gamemode
        mode_len = struct.unpack_from('I', data, offset)[0]
        offset += 4
        gamemode = data[offset:offset+mode_len].decode('latin-1', errors='replace')
        offset += mode_len

        # Map
        map_len = struct.unpack_from('I', data, offset)[0]
        offset += 4
        map_name = data[offset:offset+map_len].decode('latin-1', errors='replace')

        return {
            'password': password,
            'players': players,
            'max_players': max_players,
            'name': server_name,
            'gamemode': gamemode,
            'map': map_name,
            'online': True
        }
    except socket.timeout:
        return {'online': False}
    except Exception as e:
        return {'online': False, 'error': str(e)}


# =============================================================================
#  DETEKCIJA GTA SA INSTALACIJE
# =============================================================================
def find_gta_sa_path():
    """Traziti GTA San Andreas instalaciju na racunaru"""
    # 1. Provjeri registry
    try:
        import winreg
        key = winreg.OpenKey(winreg.HKEY_CURRENT_USER,
                             r"Software\SAMP")
        path, _ = winreg.QueryValueEx(key, "gta_sa_exe")
        winreg.CloseKey(key)
        if path and os.path.exists(path):
            return os.path.dirname(path)
    except:
        pass

    try:
        import winreg
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,
                             r"SOFTWARE\WOW6432Node\Rockstar Games\GTA San Andreas")
        path, _ = winreg.QueryValueEx(key, "InstallFolder")
        winreg.CloseKey(key)
        if path and os.path.exists(path):
            return path
    except:
        pass

    # 2. Provjeri uobicajene lokacije
    common_paths = [
        r"C:\Program Files (x86)\Rockstar Games\GTA San Andreas",
        r"C:\Program Files\Rockstar Games\GTA San Andreas",
        r"C:\Games\GTA San Andreas",
        r"C:\Games\GTA SA",
        r"D:\Games\GTA San Andreas",
        r"D:\GTA San Andreas",
        r"D:\Games\GTA SA",
        r"D:\GTA SA",
        r"E:\GTA San Andreas",
        r"E:\GTA SA",
        os.path.expanduser(r"~\Desktop\GTA San Andreas"),
        os.path.expanduser(r"~\Desktop\GTA SA"),
    ]

    for path in common_paths:
        gta_exe = os.path.join(path, "gta_sa.exe")
        if os.path.exists(gta_exe):
            return path

    # 3. Provjeri sa samp.exe (ako je SA-MP vec instaliran)
    try:
        import winreg
        key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, r"Software\SAMP")
        path, _ = winreg.QueryValueEx(key, "gta_sa_exe")
        winreg.CloseKey(key)
        if path and os.path.exists(path):
            return os.path.dirname(path)
    except:
        pass

    return None


def find_samp_exe(gta_path):
    """Pronadji samp.exe u GTA folderu"""
    if not gta_path:
        return None
    samp_path = os.path.join(gta_path, "samp.exe")
    if os.path.exists(samp_path):
        return samp_path
    return None


# =============================================================================
#  CEF PLUGIN INSTALACIJA
# =============================================================================
def check_cef_plugin(gta_path):
    """Provjeri da li je CEF plugin instaliran"""
    if not gta_path:
        return False
    cef_asi = os.path.join(gta_path, "cef.asi")
    cef_folder = os.path.join(gta_path, "CEF")
    return os.path.exists(cef_asi) and os.path.exists(cef_folder)


# =============================================================================
#  MAIN LAUNCHER CLASS
# =============================================================================
class UnicateGamingLauncher:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("Unicate Gaming Launcher")
        self.root.geometry("900x650")
        self.root.resizable(False, False)
        self.root.configure(bg=COLOR_BG_DARK)

        # Ukloni window decorations za moderniji izgled
        self.root.overrideredirect(False)

        # Centriraj prozor
        self.center_window()

        # Varijable
        self.gta_path = find_gta_sa_path()
        self.server_info = None
        self.is_launching = False
        self.drag_start_x = 0
        self.drag_start_y = 0

        # Build UI
        self.build_ui()

        # Pokreni server status check u background threadu
        self.check_server_status()

        # Periodicni refresh (svakih 30 sekundi)
        self.schedule_status_refresh()

    def center_window(self):
        self.root.update_idletasks()
        w = 900
        h = 650
        x = (self.root.winfo_screenwidth() // 2) - (w // 2)
        y = (self.root.winfo_screenheight() // 2) - (h // 2)
        self.root.geometry(f'{w}x{h}+{x}+{y}')

    # =========================================================================
    #  UI BUILDING
    # =========================================================================
    def build_ui(self):
        # ===== TOP BAR (naslov + close/minimize) =====
        self.top_bar = tk.Frame(self.root, bg=COLOR_BG_PANEL, height=45)
        self.top_bar.pack(fill=tk.X, side=tk.TOP)
        self.top_bar.pack_propagate(False)

        # Drag za pomicanje prozora
        self.top_bar.bind('<Button-1>', self.start_drag)
        self.top_bar.bind('<B1-Motion>', self.do_drag)

        # Logo text
        title_frame = tk.Frame(self.top_bar, bg=COLOR_BG_PANEL)
        title_frame.pack(side=tk.LEFT, padx=15, pady=8)

        tk.Label(title_frame, text="UNICATE", font=("Segoe UI", 16, "bold"),
                fg=COLOR_ACCENT, bg=COLOR_BG_PANEL).pack(side=tk.LEFT)
        tk.Label(title_frame, text=" GAMING", font=("Segoe UI", 16, "bold"),
                fg=COLOR_TEXT_BRIGHT, bg=COLOR_BG_PANEL).pack(side=tk.LEFT)

        # Verzija
        tk.Label(self.top_bar, text=f"v{LAUNCHER_VERSION}",
                font=("Segoe UI", 9), fg=COLOR_TEXT_DIM, bg=COLOR_BG_PANEL).pack(side=tk.LEFT, padx=10)

        # Close / Minimize dugmad
        btn_frame = tk.Frame(self.top_bar, bg=COLOR_BG_PANEL)
        btn_frame.pack(side=tk.RIGHT, padx=5)

        self.min_btn = tk.Label(btn_frame, text="  —  ", font=("Segoe UI", 11),
                               fg=COLOR_TEXT_DIM, bg=COLOR_BG_PANEL, cursor="hand2")
        self.min_btn.pack(side=tk.LEFT, pady=5)
        self.min_btn.bind('<Enter>', lambda e: self.min_btn.configure(bg=COLOR_BG_CARD))
        self.min_btn.bind('<Leave>', lambda e: self.min_btn.configure(bg=COLOR_BG_PANEL))
        self.min_btn.bind('<Button-1>', lambda e: self.root.iconify())

        self.close_btn = tk.Label(btn_frame, text="  ✕  ", font=("Segoe UI", 11, "bold"),
                                  fg=COLOR_TEXT_DIM, bg=COLOR_BG_PANEL, cursor="hand2")
        self.close_btn.pack(side=tk.LEFT, pady=5)
        self.close_btn.bind('<Enter>', lambda e: self.close_btn.configure(bg=COLOR_ERROR))
        self.close_btn.bind('<Leave>', lambda e: self.close_btn.configure(bg=COLOR_BG_PANEL))
        self.close_btn.bind('<Button-1>', lambda e: self.on_close())

        # ===== MAIN CONTENT =====
        main_frame = tk.Frame(self.root, bg=COLOR_BG_DARK)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=0, pady=0)

        # --- LEFT PANEL (Server Info + News) ---
        left_panel = tk.Frame(main_frame, bg=COLOR_BG_PANEL, width=550)
        left_panel.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=0, pady=0)
        left_panel.pack_propagate(False)

        # Hero sekcija
        hero_frame = tk.Frame(left_panel, bg=COLOR_BG_DARK, height=200)
        hero_frame.pack(fill=tk.X, padx=0, pady=0)
        hero_frame.pack_propagate(False)

        # Hero content
        hero_inner = tk.Frame(hero_frame, bg=COLOR_BG_DARK)
        hero_inner.pack(fill=tk.BOTH, expand=True, padx=25, pady=20)

        tk.Label(hero_inner, text="Unicate Gaming", font=("Segoe UI", 28, "bold"),
                fg=COLOR_TEXT_BRIGHT, bg=COLOR_BG_DARK).pack(anchor=tk.W)
        tk.Label(hero_inner, text="Online Gaming Community",
                font=("Segoe UI", 13), fg=COLOR_ACCENT, bg=COLOR_BG_DARK).pack(anchor=tk.W, pady=(0,10))

        # Status indikator
        self.status_frame = tk.Frame(hero_inner, bg=COLOR_BG_DARK)
        self.status_frame.pack(anchor=tk.W, pady=5)

        self.status_dot = tk.Canvas(self.status_frame, width=12, height=12,
                                    bg=COLOR_BG_DARK, highlightthickness=0)
        self.status_dot.pack(side=tk.LEFT, padx=(0,8))
        self.status_dot.create_oval(2, 2, 10, 10, fill=COLOR_WARNING, outline="")

        self.status_label = tk.Label(self.status_frame, text="Provjeravam status servera...",
                                     font=("Segoe UI", 10), fg=COLOR_TEXT_DIM, bg=COLOR_BG_DARK)
        self.status_label.pack(side=tk.LEFT)

        # Player count
        self.player_label = tk.Label(hero_inner, text="",
                                     font=("Segoe UI", 11), fg=COLOR_TEXT, bg=COLOR_BG_DARK)
        self.player_label.pack(anchor=tk.W, pady=2)

        # Gamemode
        self.gamemode_label = tk.Label(hero_inner, text="",
                                       font=("Segoe UI", 10), fg=COLOR_TEXT_DIM, bg=COLOR_BG_DARK)
        self.gamemode_label.pack(anchor=tk.W)

        # --- NEWS SEKCIJA ---
        news_section = tk.Frame(left_panel, bg=COLOR_BG_PANEL)
        news_section.pack(fill=tk.BOTH, expand=True, padx=20, pady=(10,15))

        tk.Label(news_section, text="NOVOSTI", font=("Segoe UI", 11, "bold"),
                fg=COLOR_ACCENT, bg=COLOR_BG_PANEL).pack(anchor=tk.W, pady=(0,8))

        # News kartice
        news_items = [
            ("CEF Update", "Novi CEF tablet, inventar i laptop UI sada dostupni! Koristite /tablet, /inventory, /laptop.", COLOR_ACCENT),
            ("Bounty Sistem", "Postavljanje nagrada za igrace putem tableta ili laptopa. PIN registracija obavezna!", "#8b5cf6"),
            ("Launcher Obavezan", "Svi igraci moraju koristiti Unicate Gaming Launcher za konekciju na server.", COLOR_WARNING),
        ]

        for title, desc, color in news_items:
            card = tk.Frame(news_section, bg=COLOR_BG_CARD, padx=12, pady=8)
            card.pack(fill=tk.X, pady=3)

            # Color bar
            bar = tk.Frame(card, bg=color, width=3)
            bar.pack(side=tk.LEFT, fill=tk.Y, padx=(0,10), pady=2)

            tk.Label(card, text=title, font=("Segoe UI", 10, "bold"),
                    fg=COLOR_TEXT_BRIGHT, bg=COLOR_BG_CARD, anchor=tk.W).pack(anchor=tk.W)
            tk.Label(card, text=desc, font=("Segoe UI", 9),
                    fg=COLOR_TEXT_DIM, bg=COLOR_BG_CARD, anchor=tk.W,
                    wraplength=420, justify=tk.LEFT).pack(anchor=tk.W, pady=(2,0))

        # --- BOTTOM LINKOVI ---
        bottom_links = tk.Frame(left_panel, bg=COLOR_BG_PANEL)
        bottom_links.pack(fill=tk.X, padx=20, pady=(0,10))

        links = [
            ("Website", WEBSITE_URL),
            ("Discord", DISCORD_URL),
            ("Forum", f"{WEBSITE_URL}/forum"),
        ]

        for name, url in links:
            link_label = tk.Label(bottom_links, text=name, font=("Segoe UI", 9, "underline"),
                                 fg=COLOR_ACCENT, bg=COLOR_BG_PANEL, cursor="hand2")
            link_label.pack(side=tk.LEFT, padx=(0,15))
            link_label.bind('<Button-1>', lambda e, u=url: webbrowser.open(u))

        # --- RIGHT PANEL (Launch + Settings) ---
        right_panel = tk.Frame(main_frame, bg=COLOR_BG_CARD, width=350)
        right_panel.pack(side=tk.RIGHT, fill=tk.BOTH, padx=0, pady=0)
        right_panel.pack_propagate(False)

        # Right content
        right_content = tk.Frame(right_panel, bg=COLOR_BG_CARD)
        right_content.pack(fill=tk.BOTH, expand=True, padx=25, pady=25)

        # Server connect info
        tk.Label(right_content, text="KONEKCIJA", font=("Segoe UI", 11, "bold"),
                fg=COLOR_TEXT_DIM, bg=COLOR_BG_CARD).pack(anchor=tk.W, pady=(0,5))

        info_frame = tk.Frame(right_content, bg=COLOR_INPUT_BG, padx=12, pady=10)
        info_frame.pack(fill=tk.X, pady=(0,15))

        tk.Label(info_frame, text=f"IP: {SERVER_IP}:{SERVER_PORT}",
                font=("Consolas", 10), fg=COLOR_TEXT, bg=COLOR_INPUT_BG).pack(anchor=tk.W)
        tk.Label(info_frame, text=f"Server: {SERVER_NAME}",
                font=("Consolas", 10), fg=COLOR_TEXT, bg=COLOR_INPUT_BG).pack(anchor=tk.W)

        # GTA SA Path
        tk.Label(right_content, text="GTA SAN ANDREAS", font=("Segoe UI", 11, "bold"),
                fg=COLOR_TEXT_DIM, bg=COLOR_BG_CARD).pack(anchor=tk.W, pady=(10,5))

        path_frame = tk.Frame(right_content, bg=COLOR_INPUT_BG, padx=12, pady=10)
        path_frame.pack(fill=tk.X, pady=(0,5))

        self.path_label = tk.Label(path_frame, text="Pretrazujem...",
                                   font=("Consolas", 9), fg=COLOR_TEXT, bg=COLOR_INPUT_BG,
                                   wraplength=270, justify=tk.LEFT)
        self.path_label.pack(anchor=tk.W)

        if self.gta_path:
            self.path_label.configure(text=self.gta_path, fg=COLOR_SUCCESS)
        else:
            self.path_label.configure(text="GTA San Andreas nije pronadjen! Klikni Browse.",
                                      fg=COLOR_ERROR)

        # Browse dugme
        browse_btn = tk.Frame(right_content, bg=COLOR_ACCENT, padx=15, pady=6, cursor="hand2")
        browse_btn.pack(anchor=tk.W, pady=(0,15))

        browse_inner = tk.Label(browse_btn, text="Browse", font=("Segoe UI", 10, "bold"),
                               fg=COLOR_TEXT_BRIGHT, bg=COLOR_ACCENT, cursor="hand2")
        browse_inner.pack()
        browse_btn.bind('<Enter>', lambda e: browse_btn.configure(bg=COLOR_ACCENT_HOVER))
        browse_btn.bind('<Leave>', lambda e: browse_btn.configure(bg=COLOR_ACCENT))
        browse_btn.bind('<Button-1>', lambda e: self.browse_gta_path())
        browse_inner.bind('<Button-1>', lambda e: self.browse_gta_path())

        # CEF Plugin status
        tk.Label(right_content, text="CEF PLUGIN", font=("Segoe UI", 11, "bold"),
                fg=COLOR_TEXT_DIM, bg=COLOR_BG_CARD).pack(anchor=tk.W, pady=(10,5))

        cef_frame = tk.Frame(right_content, bg=COLOR_INPUT_BG, padx=12, pady=10)
        cef_frame.pack(fill=tk.X, pady=(0,15))

        cef_status = check_cef_plugin(self.gta_path) if self.gta_path else False
        self.cef_label = tk.Label(cef_frame,
                                  text="CEF Plugin instaliran" if cef_status else "CEF Plugin NIJE pronadjen",
                                  font=("Segoe UI", 9),
                                  fg=COLOR_SUCCESS if cef_status else COLOR_WARNING,
                                  bg=COLOR_INPUT_BG)
        self.cef_label.pack(anchor=tk.W)

        if not cef_status:
            cef_info = tk.Label(cef_frame,
                               text="Potreban za Tablet, Inventar i Laptop UI",
                               font=("Segoe UI", 8),
                               fg=COLOR_TEXT_DIM, bg=COLOR_INPUT_BG)
            cef_info.pack(anchor=tk.W, pady=(2,0))

        # Spacer
        tk.Frame(right_content, bg=COLOR_BG_CARD).pack(fill=tk.BOTH, expand=True)

        # ===== LAUNCH DUGME =====
        self.launch_btn = tk.Frame(right_content, bg=COLOR_ACCENT, padx=0, pady=0,
                                   cursor="hand2")
        self.launch_btn.pack(fill=tk.X, pady=(10,0), ipady=12)

        self.launch_text = tk.Label(self.launch_btn, text="LAUNCH",
                                    font=("Segoe UI", 18, "bold"),
                                    fg=COLOR_TEXT_BRIGHT, bg=COLOR_ACCENT,
                                    cursor="hand2")
        self.launch_text.pack()

        # Bind hover efekte
        self.launch_btn.bind('<Enter>', self.on_launch_hover)
        self.launch_btn.bind('<Leave>', self.on_launch_leave)
        self.launch_btn.bind('<Button-1>', lambda e: self.launch_game())
        self.launch_text.bind('<Enter>', self.on_launch_hover)
        self.launch_text.bind('<Leave>', self.on_launch_leave)
        self.launch_text.bind('<Button-1>', lambda e: self.launch_game())

        # Status text ispod launch
        self.launch_status = tk.Label(right_content, text="",
                                      font=("Segoe UI", 9),
                                      fg=COLOR_TEXT_DIM, bg=COLOR_BG_CARD)
        self.launch_status.pack(pady=(8,0))

    # =========================================================================
    #  EVENT HANDLERS
    # =========================================================================
    def start_drag(self, event):
        self.drag_start_x = event.x
        self.drag_start_y = event.y

    def do_drag(self, event):
        x = self.root.winfo_x() + (event.x - self.drag_start_x)
        y = self.root.winfo_y() + (event.y - self.drag_start_y)
        self.root.geometry(f'+{x}+{y}')

    def on_launch_hover(self, event):
        if not self.is_launching:
            self.launch_btn.configure(bg=COLOR_ACCENT_HOVER)
            self.launch_text.configure(bg=COLOR_ACCENT_HOVER)

    def on_launch_leave(self, event):
        if not self.is_launching:
            self.launch_btn.configure(bg=COLOR_ACCENT)
            self.launch_text.configure(bg=COLOR_ACCENT)

    def on_close(self):
        self.root.destroy()
        sys.exit(0)

    # =========================================================================
    #  BROWSE GTA PATH
    # =========================================================================
    def browse_gta_path(self):
        from tkinter import filedialog
        gta_exe = filedialog.askopenfilename(
            title="Pronadji gta_sa.exe",
            filetypes=[("GTA SA Executable", "gta_sa.exe"), ("All files", "*.*")]
        )
        if gta_exe:
            self.gta_path = os.path.dirname(gta_exe)
            self.path_label.configure(text=self.gta_path, fg=COLOR_SUCCESS)

            # Azuriraj CEF status
            cef_status = check_cef_plugin(self.gta_path)
            self.cef_label.configure(
                text="CEF Plugin instaliran" if cef_status else "CEF Plugin NIJE pronadjen",
                fg=COLOR_SUCCESS if cef_status else COLOR_WARNING
            )

    # =========================================================================
    #  SERVER STATUS CHECK
    # =========================================================================
    def check_server_status(self):
        """Pokreni provjeru statusa u background threadu"""
        thread = threading.Thread(target=self._do_status_check, daemon=True)
        thread.start()

    def _do_status_check(self):
        info = query_samp_server(SERVER_IP, SERVER_PORT)
        self.server_info = info
        self.root.after(0, self._update_status_ui, info)

    def _update_status_ui(self, info):
        if info and info.get('online'):
            # Server je online
            self.status_dot.delete("all")
            self.status_dot.create_oval(2, 2, 10, 10, fill=COLOR_SUCCESS, outline="")

            self.status_label.configure(
                text=f"Server Online  |  {info.get('name', SERVER_NAME)}",
                fg=COLOR_SUCCESS
            )
            self.player_label.configure(
                text=f"Igraci: {info.get('players', 0)} / {info.get('max_players', 0)}"
            )
            self.gamemode_label.configure(
                text=f"Gamemode: {info.get('gamemode', 'N/A')}  |  Map: {info.get('map', 'N/A')}"
            )
        else:
            # Server je offline
            self.status_dot.delete("all")
            self.status_dot.create_oval(2, 2, 10, 10, fill=COLOR_ERROR, outline="")

            self.status_label.configure(text="Server Offline", fg=COLOR_ERROR)
            self.player_label.configure(text="Server trenutno nije dostupan")
            self.gamemode_label.configure(text="")

    def schedule_status_refresh(self):
        """Osvjezavaj status svakih 30 sekundi"""
        self.check_server_status()
        self.root.after(30000, self.schedule_status_refresh)

    # =========================================================================
    #  LAUNCH GAME
    # =========================================================================
    def launch_game(self):
        if self.is_launching:
            return

        # Provjeri da li je GTA SA pronadjen
        if not self.gta_path:
            messagebox.showerror("Greska",
                "GTA San Andreas nije pronadjen!\n\n"
                "Klikni 'Browse' dugme i pronadji gta_sa.exe na svom racunaru.",
                parent=self.root)
            return

        gta_exe = os.path.join(self.gta_path, "gta_sa.exe")
        if not os.path.exists(gta_exe):
            messagebox.showerror("Greska",
                f"gta_sa.exe nije pronadjen u:\n{self.gta_path}\n\n"
                "Izaberi ispravni folder.",
                parent=self.root)
            return

        # Provjeri da li je samp.exe dostupan
        samp_exe = find_samp_exe(self.gta_path)
        if not samp_exe:
            messagebox.showerror("Greska",
                "samp.exe nije pronadjen!\n\n"
                "Morate instalirati SA-MP client prvo.\n"
                "Preuzmite ga sa sa-mp.com",
                parent=self.root)
            return

        # Pokreni launcher animaciju
        self.is_launching = True
        self.launch_btn.configure(bg=COLOR_ACCENT_GLOW, cursor="watch")
        self.launch_text.configure(text="POKRECEM...", bg=COLOR_ACCENT_GLOW, cursor="watch")
        self.launch_status.configure(text="Konektujem se na server...", fg=COLOR_ACCENT)

        # Pokreni u threadu da ne freezea UI
        thread = threading.Thread(target=self._do_launch, args=(samp_exe,), daemon=True)
        thread.start()

    def _do_launch(self, samp_exe):
        try:
            # Pokreni SA-MP sa server IP-om
            # samp.exe prima IP:PORT kao argument
            connect_str = f"{SERVER_IP}:{SERVER_PORT}"

            self.root.after(0, lambda: self.launch_status.configure(
                text=f"Pokrecem SA-MP... {connect_str}", fg=COLOR_ACCENT))

            # Pokreni proces
            subprocess.Popen([samp_exe, connect_str], cwd=self.gta_path)

            # Pricekaj malo i azuriraj UI
            time.sleep(2)

            self.root.after(0, lambda: self.launch_status.configure(
                text="Igra pokrenuta! Uzivaj na Unicate Gaming!", fg=COLOR_SUCCESS))

            # Nakon 5 sekundi zatvori launcher
            time.sleep(5)
            self.root.after(0, self.root.destroy)

        except Exception as e:
            self.root.after(0, lambda: self._launch_error(str(e)))

    def _launch_error(self, error_msg):
        self.is_launching = False
        self.launch_btn.configure(bg=COLOR_ACCENT, cursor="hand2")
        self.launch_text.configure(text="LAUNCH", bg=COLOR_ACCENT, cursor="hand2")
        self.launch_status.configure(text="", fg=COLOR_TEXT_DIM)

        messagebox.showerror("Greska",
            f"Nije moguce pokrenuti igru!\n\n{error_msg}",
            parent=self.root)


# =============================================================================
#  MAIN
# =============================================================================
def main():
    app = UnicateGamingLauncher()
    app.root.mainloop()


if __name__ == "__main__":
    main()
