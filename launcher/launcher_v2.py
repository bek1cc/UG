#!/usr/bin/env python3
# =============================================================================
#  UNICATE GAMING RPG - LAUNCHER V2
#  Ultra-modern CustomTkinter Launcher
#
#  Instalacija:
#    pip install customtkinter pillow requests
#
#  Kompajliranje u .exe:
#    pip install pyinstaller
#    pyinstaller --onefile --windowed --icon=ug_icon.ico --name="UnicateGaming" launcher_v2.py
# =============================================================================

import customtkinter as ctk
import os
import sys
import subprocess
import threading
import time
import struct
import socket
import webbrowser
import json
from tkinter import filedialog, messagebox

try:
    from PIL import Image, ImageTk, ImageDraw, ImageFont, ImageFilter
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
#  KONFIGURACIJA
# =============================================================================
SERVER_IP = "135.125.156.197"
SERVER_PORT = 7777
SERVER_NAME = "Unicate Gaming RPG"
WEBSITE_URL = "https://ug-ogc.com"
DISCORD_URL = "https://discord.gg/unicategaming"

LAUNCHER_VERSION = "2.0.0"
LAUNCHER_TITLE = "UNICATE GAMING"

# Folderi
if getattr(sys, 'frozen', False):
    LAUNCHER_DIR = os.path.dirname(sys.executable)
else:
    LAUNCHER_DIR = os.path.dirname(os.path.abspath(__file__))

# =============================================================================
#  CustomTkinter TEMA
# =============================================================================
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

# Boje
COLOR_BG = "#0a0e17"
COLOR_PANEL = "#111827"
COLOR_CARD = "#1a2332"
COLOR_ACCENT = "#3b82f6"
COLOR_ACCENT_HOVER = "#2563eb"
COLOR_ACCENT_GLOW = "#1d4ed8"
COLOR_TEXT = "#e2e8f0"
COLOR_TEXT_DIM = "#94a3b8"
COLOR_TEXT_BRIGHT = "#ffffff"
COLOR_SUCCESS = "#22c55e"
COLOR_ERROR = "#ef4444"
COLOR_WARNING = "#f59e0b"
COLOR_BORDER = "#1e3a5f"
COLOR_INPUT_BG = "#0f172a"

# =============================================================================
#  SA-MP QUERY
# =============================================================================
def query_samp_server(ip, port, timeout=3):
    """Salje SA-MP query paket i vraca info o serveru"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.settimeout(timeout)
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
        password_len = struct.unpack_from('H', data, offset)[0]
        offset += 2
        password = data[offset:offset+password_len].decode('latin-1', errors='replace')
        offset += password_len
        players = struct.unpack_from('H', data, offset)[0]
        offset += 2
        max_players = struct.unpack_from('H', data, offset)[0]
        offset += 2
        name_len = struct.unpack_from('I', data, offset)[0]
        offset += 4
        server_name = data[offset:offset+name_len].decode('latin-1', errors='replace')
        offset += name_len
        mode_len = struct.unpack_from('I', data, offset)[0]
        offset += 4
        gamemode = data[offset:offset+mode_len].decode('latin-1', errors='replace')
        offset += mode_len
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
#  DETEKCIJA GTA SA
# =============================================================================
def find_gta_sa_path():
    try:
        import winreg
        key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, r"Software\SAMP")
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

    common_paths = [
        r"C:\Program Files (x86)\Rockstar Games\GTA San Andreas",
        r"C:\Program Files\Rockstar Games\GTA San Andreas",
        r"C:\Games\GTA San Andreas",
        r"C:\Games\GTA SA",
        r"D:\Games\GTA San Andreas",
        r"D:\GTA San Andreas",
        r"D:\GTA SA",
        r"D:\Games\GTA SA",
        r"E:\GTA San Andreas",
        r"E:\GTA SA",
        os.path.expanduser(r"~\Desktop\GTA San Andreas"),
        os.path.expanduser(r"~\Desktop\GTA SA"),
    ]

    for path in common_paths:
        gta_exe = os.path.join(path, "gta_sa.exe")
        if os.path.exists(gta_exe):
            return path
    return None


def find_samp_exe(gta_path):
    if not gta_path:
        return None
    samp_path = os.path.join(gta_path, "samp.exe")
    if os.path.exists(samp_path):
        return samp_path
    return None


def check_cef_plugin(gta_path):
    if not gta_path:
        return False
    cef_asi = os.path.join(gta_path, "cef.asi")
    cef_folder = os.path.join(gta_path, "CEF")
    return os.path.exists(cef_asi) and os.path.exists(cef_folder)


def save_settings(gta_path):
    settings = {"gta_path": gta_path}
    settings_file = os.path.join(LAUNCHER_DIR, "settings.json")
    try:
        with open(settings_file, 'w') as f:
            json.dump(settings, f)
    except:
        pass


def load_settings():
    settings_file = os.path.join(LAUNCHER_DIR, "settings.json")
    try:
        if os.path.exists(settings_file):
            with open(settings_file, 'r') as f:
                return json.load(f)
    except:
        pass
    return {}


# =============================================================================
#  CUSTOM WIDGETS
# =============================================================================
class GlowButton(ctk.CTkButton):
    """Dugme sa glow efektom"""
    def __init__(self, master, **kwargs):
        super().__init__(master, **kwargs)
        self._glow_enabled = True

    def _hover_enter(self, event):
        if self._glow_enabled:
            self.configure(fg_color=COLOR_ACCENT_HOVER)

    def _hover_leave(self, event):
        if self._glow_enabled:
            self.configure(fg_color=COLOR_ACCENT)


class StatusDot(ctk.CTkCanvas):
    """Animirani status indikator"""
    def __init__(self, master, size=12, **kwargs):
        super().__init__(master, width=size+4, height=size+4,
                         bg=COLOR_BG, highlightthickness=0, **kwargs)
        self.size = size
        self.color = COLOR_WARNING
        self._draw()

    def _draw(self):
        self.delete("all")
        # Glow
        self.create_oval(0, 0, self.size+4, self.size+4,
                        fill="", outline=self.color, width=1)
        # Dot
        self.create_oval(2, 2, self.size+2, self.size+2,
                        fill=self.color, outline="")

    def set_color(self, color):
        self.color = color
        self._draw()


# =============================================================================
#  GLAVNI LAUNCHER
# =============================================================================
class UnicateGamingLauncher:
    def __init__(self):
        self.root = ctk.CTk()
        self.root.title("Unicate Gaming Launcher")
        self.root.geometry("1050x680")
        self.root.resizable(False, False)
        self.root.configure(fg_color=COLOR_BG)

        # Centriraj prozor
        self.root.update_idletasks()
        w = 1050
        h = 680
        x = (self.root.winfo_screenwidth() // 2) - (w // 2)
        y = (self.root.winfo_screenheight() // 2) - (h // 2)
        self.root.geometry(f'{w}x{h}+{x}+{y}')

        # Varijable
        settings = load_settings()
        self.gta_path = settings.get("gta_path", find_gta_sa_path())
        self.server_info = None
        self.is_launching = False
        self.logo_image = None

        # Load logo
        self._load_logo()

        # Build UI
        self.build_ui()

        # Pokreni server check
        self.check_server_status()

        # Periodicni refresh
        self.schedule_status_refresh()

    def _load_logo(self):
        """Ucitaj logo sliku"""
        try:
            # Pokusaj iz istog foldera
            logo_path = os.path.join(LAUNCHER_DIR, "logo.png")
            if not os.path.exists(logo_path):
                logo_path = os.path.join(LAUNCHER_DIR, "ug_logo.png")
            if os.path.exists(logo_path):
                self.logo_image = ctk.CTkImage(
                    light_image=Image.open(logo_path),
                    dark_image=Image.open(logo_path),
                    size=(180, 180)
                )
                # Manja verzija za sidebar
                self.logo_small = ctk.CTkImage(
                    light_image=Image.open(logo_path),
                    dark_image=Image.open(logo_path),
                    size=(40, 40)
                )
                # Manja za topbar
                self.logo_tiny = ctk.CTkImage(
                    light_image=Image.open(logo_path),
                    dark_image=Image.open(logo_path),
                    size=(28, 28)
                )
            else:
                self.logo_image = None
                self.logo_small = None
                self.logo_tiny = None
        except Exception as e:
            print(f"[UG Launcher] Logo greska: {e}")
            self.logo_image = None
            self.logo_small = None
            self.logo_tiny = None

    # =========================================================================
    #  UI BUILDING
    # =========================================================================
    def build_ui(self):
        # ===== TOP BAR =====
        self.topbar = ctk.CTkFrame(self.root, height=50, fg_color=COLOR_PANEL,
                                    corner_radius=0)
        self.topbar.pack(fill="x", side="top")
        self.topbar.pack_propagate(False)

        # Logo u topbaru
        top_left = ctk.CTkFrame(self.topbar, fg_color="transparent")
        top_left.pack(side="left", padx=15, pady=8)

        if self.logo_tiny:
            ctk.CTkLabel(top_left, image=self.logo_tiny, text="",
                        fg_color="transparent").pack(side="left", padx=(0, 8))

        ctk.CTkLabel(top_left, text="UNICATE", font=ctk.CTkFont(family="Segoe UI",
                    size=16, weight="bold"), text_color=COLOR_ACCENT,
                    fg_color="transparent").pack(side="left")
        ctk.CTkLabel(top_left, text=" GAMING", font=ctk.CTkFont(family="Segoe UI",
                    size=16, weight="bold"), text_color=COLOR_TEXT_BRIGHT,
                    fg_color="transparent").pack(side="left")

        # Verzija
        ctk.CTkLabel(self.topbar, text=f"v{LAUNCHER_VERSION}",
                    font=ctk.CTkFont(size=10), text_color=COLOR_TEXT_DIM,
                    fg_color="transparent").pack(side="left", padx=10)

        # Online badge
        self.online_badge = ctk.CTkFrame(self.topbar, fg_color="transparent")
        self.online_badge.pack(side="right", padx=15)

        self.online_count_label = ctk.CTkLabel(
            self.online_badge, text="Provjera...",
            font=ctk.CTkFont(size=11, weight="bold"),
            text_color=COLOR_TEXT_DIM, fg_color="transparent"
        )
        self.online_count_label.pack(side="right")

        self.status_dot_top = StatusDot(self.online_badge, size=8)
        self.status_dot_top.pack(side="right", padx=(0, 6))

        # Close/Min dugmad
        btn_frame = ctk.CTkFrame(self.topbar, fg_color="transparent")
        btn_frame.pack(side="right", padx=5)

        ctk.CTkButton(btn_frame, text="—", width=35, height=30,
                      fg_color="transparent", hover_color=COLOR_CARD,
                      text_color=COLOR_TEXT_DIM, font=ctk.CTkFont(size=13),
                      command=self.root.iconify).pack(side="left", padx=2)

        ctk.CTkButton(btn_frame, text="✕", width=35, height=30,
                      fg_color="transparent", hover_color="#7f1d1d",
                      text_color=COLOR_TEXT_DIM, font=ctk.CTkFont(size=13, weight="bold"),
                      command=self.on_close).pack(side="left", padx=2)

        # ===== GLAVNI SADRZAJ =====
        main = ctk.CTkFrame(self.root, fg_color=COLOR_BG, corner_radius=0)
        main.pack(fill="both", expand=True)

        # --- LIJEVI PANEL (Hero + News) ---
        left_panel = ctk.CTkFrame(main, fg_color=COLOR_BG, corner_radius=0, width=650)
        left_panel.pack(side="left", fill="both", expand=True, padx=(20, 5), pady=15)
        left_panel.pack_propagate(False)

        # Hero sekcija
        hero = ctk.CTkFrame(left_panel, fg_color=COLOR_PANEL, corner_radius=15,
                            height=240)
        hero.pack(fill="x", pady=(0, 10))
        hero.pack_propagate(False)

        hero_inner = ctk.CTkFrame(hero, fg_color="transparent")
        hero_inner.pack(fill="both", expand=True, padx=25, pady=20)

        # Logo + tekst
        hero_content = ctk.CTkFrame(hero_inner, fg_color="transparent")
        hero_content.pack(fill="both", expand=True)

        if self.logo_image:
            logo_label = ctk.CTkLabel(hero_content, image=self.logo_image, text="",
                                     fg_color="transparent")
            logo_label.pack(side="left", padx=(0, 25))

        hero_text = ctk.CTkFrame(hero_content, fg_color="transparent")
        hero_text.pack(side="left", fill="both", expand=True)

        ctk.CTkLabel(hero_text, text="UNICATE GAMING",
                    font=ctk.CTkFont(family="Segoe UI", size=32, weight="bold"),
                    text_color=COLOR_TEXT_BRIGHT, fg_color="transparent",
                    anchor="w").pack(anchor="w")

        ctk.CTkLabel(hero_text, text="R  P  G     S E R V E R",
                    font=ctk.CTkFont(family="Segoe UI", size=14, weight="bold"),
                    text_color=COLOR_ACCENT, fg_color="transparent",
                    anchor="w").pack(anchor="w", pady=(2, 12))

        # Status red
        status_row = ctk.CTkFrame(hero_text, fg_color="transparent")
        status_row.pack(anchor="w", pady=4)

        self.status_dot = StatusDot(status_row, size=10)
        self.status_dot.pack(side="left", padx=(0, 8))

        self.status_label = ctk.CTkLabel(status_row, text="Provjeravam status...",
                                         font=ctk.CTkFont(size=12),
                                         text_color=COLOR_TEXT_DIM,
                                         fg_color="transparent")
        self.status_label.pack(side="left")

        self.player_label = ctk.CTkLabel(hero_text, text="",
                                         font=ctk.CTkFont(size=12, weight="bold"),
                                         text_color=COLOR_TEXT,
                                         fg_color="transparent", anchor="w")
        self.player_label.pack(anchor="w", pady=2)

        self.gamemode_label = ctk.CTkLabel(hero_text, text="",
                                           font=ctk.CTkFont(size=11),
                                           text_color=COLOR_TEXT_DIM,
                                           fg_color="transparent", anchor="w")
        self.gamemode_label.pack(anchor="w")

        # --- NOVOSTI ---
        news_frame = ctk.CTkFrame(left_panel, fg_color=COLOR_PANEL, corner_radius=15)
        news_frame.pack(fill="both", expand=True)

        ctk.CTkLabel(news_frame, text="NOVOSTI",
                    font=ctk.CTkFont(size=13, weight="bold"),
                    text_color=COLOR_ACCENT, fg_color="transparent",
                    anchor="w").pack(anchor="w", padx=20, pady=(15, 8))

        news_items = [
            ("CEF Update", "Novi CEF tablet, inventar i laptop UI! /tablet, /inventory, /laptop", COLOR_ACCENT),
            ("Bounty Sistem", "Postavljanje nagrada za igrace putem tableta ili laptopa.", "#8b5cf6"),
            ("Launcher Obavezan", "Svi igraci moraju koristiti Unicate Gaming Launcher za konekciju.", COLOR_WARNING),
        ]

        for title, desc, color in news_items:
            card = ctk.CTkFrame(news_frame, fg_color=COLOR_CARD, corner_radius=10,
                               height=55)
            card.pack(fill="x", padx=15, pady=3)
            card.pack_propagate(False)

            inner = ctk.CTkFrame(card, fg_color="transparent")
            inner.pack(fill="both", expand=True, padx=12, pady=8)

            # Color bar
            bar = ctk.CTkFrame(inner, fg_color=color, width=3, corner_radius=2)
            bar.pack(side="left", fill="y", padx=(0, 10), pady=2)

            text_col = ctk.CTkFrame(inner, fg_color="transparent")
            text_col.pack(side="left", fill="both", expand=True)

            ctk.CTkLabel(text_col, text=title,
                        font=ctk.CTkFont(size=12, weight="bold"),
                        text_color=COLOR_TEXT_BRIGHT, fg_color="transparent",
                        anchor="w").pack(anchor="w")
            ctk.CTkLabel(text_col, text=desc,
                        font=ctk.CTkFont(size=10),
                        text_color=COLOR_TEXT_DIM, fg_color="transparent",
                        anchor="w", wraplength=450).pack(anchor="w")

        # Linkovi na dnu
        links_frame = ctk.CTkFrame(left_panel, fg_color="transparent")
        links_frame.pack(fill="x", padx=15, pady=(8, 5))

        for name, url in [("Website", WEBSITE_URL), ("Discord", DISCORD_URL)]:
            ctk.CTkLabel(links_frame, text=name,
                        font=ctk.CTkFont(size=10, underline=True),
                        text_color=COLOR_ACCENT, fg_color="transparent",
                        cursor="hand2").pack(side="left", padx=(0, 15))

        # --- DESNI PANEL (Launch + Settings) ---
        right_panel = ctk.CTkFrame(main, fg_color=COLOR_PANEL, corner_radius=15,
                                   width=340)
        right_panel.pack(side="right", fill="y", padx=(5, 20), pady=15)
        right_panel.pack_propagate(False)

        right_content = ctk.CTkFrame(right_panel, fg_color="transparent")
        right_content.pack(fill="both", expand=True, padx=20, pady=20)

        # Konekcija info
        ctk.CTkLabel(right_content, text="KONEKCIJA",
                    font=ctk.CTkFont(size=12, weight="bold"),
                    text_color=COLOR_TEXT_DIM, fg_color="transparent",
                    anchor="w").pack(anchor="w", pady=(0, 5))

        conn_card = ctk.CTkFrame(right_content, fg_color=COLOR_INPUT_BG,
                                 corner_radius=10)
        conn_card.pack(fill="x", pady=(0, 15))

        ctk.CTkLabel(conn_card, text=f"IP: {SERVER_IP}:{SERVER_PORT}",
                    font=ctk.CTkFont(family="Consolas", size=11),
                    text_color=COLOR_TEXT, fg_color="transparent",
                    anchor="w").pack(anchor="w", padx=12, pady=(10, 2))
        ctk.CTkLabel(conn_card, text=f"Server: {SERVER_NAME}",
                    font=ctk.CTkFont(family="Consolas", size=11),
                    text_color=COLOR_TEXT, fg_color="transparent",
                    anchor="w").pack(anchor="w", padx=12, pady=(0, 10))

        # GTA SA Path
        ctk.CTkLabel(right_content, text="GTA SAN ANDREAS",
                    font=ctk.CTkFont(size=12, weight="bold"),
                    text_color=COLOR_TEXT_DIM, fg_color="transparent",
                    anchor="w").pack(anchor="w", pady=(8, 5))

        path_card = ctk.CTkFrame(right_content, fg_color=COLOR_INPUT_BG,
                                 corner_radius=10)
        path_card.pack(fill="x", pady=(0, 5))

        self.path_label = ctk.CTkLabel(path_card,
                                       text=self.gta_path if self.gta_path else "GTA SA nije pronadjen!",
                                       font=ctk.CTkFont(family="Consolas", size=9),
                                       text_color=COLOR_SUCCESS if self.gta_path else COLOR_ERROR,
                                       fg_color="transparent", anchor="w",
                                       wraplength=270)
        self.path_label.pack(anchor="w", padx=12, pady=10)

        # Browse dugme
        ctk.CTkButton(right_content, text="Browse", width=80, height=28,
                      fg_color=COLOR_ACCENT, hover_color=COLOR_ACCENT_HOVER,
                      font=ctk.CTkFont(size=11, weight="bold"),
                      command=self.browse_gta_path).pack(anchor="w", pady=(0, 15))

        # CEF Plugin status
        ctk.CTkLabel(right_content, text="CEF PLUGIN",
                    font=ctk.CTkFont(size=12, weight="bold"),
                    text_color=COLOR_TEXT_DIM, fg_color="transparent",
                    anchor="w").pack(anchor="w", pady=(5, 5))

        cef_card = ctk.CTkFrame(right_content, fg_color=COLOR_INPUT_BG,
                                corner_radius=10)
        cef_card.pack(fill="x", pady=(0, 15))

        cef_status = check_cef_plugin(self.gta_path) if self.gta_path else False
        self.cef_label = ctk.CTkLabel(
            cef_card,
            text="CEF Plugin instaliran" if cef_status else "CEF Plugin NIJE pronadjen",
            font=ctk.CTkFont(size=11),
            text_color=COLOR_SUCCESS if cef_status else COLOR_WARNING,
            fg_color="transparent", anchor="w"
        )
        self.cef_label.pack(anchor="w", padx=12, pady=10)

        if not cef_status:
            ctk.CTkLabel(cef_card,
                        text="Potreban za Tablet, Inventar i Laptop UI",
                        font=ctk.CTkFont(size=9),
                        text_color=COLOR_TEXT_DIM, fg_color="transparent",
                        anchor="w").pack(anchor="w", padx=12, pady=(0, 8))

        # Spacer
        ctk.CTkFrame(right_content, fg_color="transparent").pack(fill="both", expand=True)

        # ===== LAUNCH DUGME =====
        self.launch_btn = ctk.CTkButton(
            right_content, text="LAUNCH", height=55,
            font=ctk.CTkFont(family="Segoe UI", size=20, weight="bold"),
            fg_color=COLOR_ACCENT, hover_color=COLOR_ACCENT_HOVER,
            corner_radius=12,
            command=self.launch_game
        )
        self.launch_btn.pack(fill="x", pady=(10, 5), side="bottom")

        # Status text
        self.launch_status = ctk.CTkLabel(right_content, text="",
                                          font=ctk.CTkFont(size=10),
                                          text_color=COLOR_TEXT_DIM,
                                          fg_color="transparent")
        self.launch_status.pack(pady=(0, 5), side="bottom")

    # =========================================================================
    #  BROWSE GTA PATH
    # =========================================================================
    def browse_gta_path(self):
        gta_exe = filedialog.askopenfilename(
            title="Pronadji gta_sa.exe",
            filetypes=[("GTA SA Executable", "gta_sa.exe"), ("All files", "*.*")]
        )
        if gta_exe:
            self.gta_path = os.path.dirname(gta_exe)
            self.path_label.configure(text=self.gta_path, text_color=COLOR_SUCCESS)
            save_settings(self.gta_path)

            cef_status = check_cef_plugin(self.gta_path)
            self.cef_label.configure(
                text="CEF Plugin instaliran" if cef_status else "CEF Plugin NIJE pronadjen",
                text_color=COLOR_SUCCESS if cef_status else COLOR_WARNING
            )

    # =========================================================================
    #  SERVER STATUS
    # =========================================================================
    def check_server_status(self):
        thread = threading.Thread(target=self._do_status_check, daemon=True)
        thread.start()

    def _do_status_check(self):
        info = query_samp_server(SERVER_IP, SERVER_PORT)
        self.server_info = info
        self.root.after(0, self._update_status_ui, info)

    def _update_status_ui(self, info):
        if info and info.get('online'):
            self.status_dot.set_color(COLOR_SUCCESS)
            self.status_dot_top.set_color(COLOR_SUCCESS)
            self.status_label.configure(
                text=f"Server Online  |  {info.get('name', SERVER_NAME)}",
                text_color=COLOR_SUCCESS
            )
            self.player_label.configure(
                text=f"Igraci: {info.get('players', 0)} / {info.get('max_players', 0)}"
            )
            self.gamemode_label.configure(
                text=f"Gamemode: {info.get('gamemode', 'N/A')}"
            )
            self.online_count_label.configure(
                text=f"{info.get('players', 0)}/{info.get('max_players', 0)} Online",
                text_color=COLOR_SUCCESS
            )
        else:
            self.status_dot.set_color(COLOR_ERROR)
            self.status_dot_top.set_color(COLOR_ERROR)
            self.status_label.configure(text="Server Offline", text_color=COLOR_ERROR)
            self.player_label.configure(text="Server trenutno nije dostupan")
            self.gamemode_label.configure(text="")
            self.online_count_label.configure(text="Offline", text_color=COLOR_ERROR)

    def schedule_status_refresh(self):
        self.check_server_status()
        self.root.after(30000, self.schedule_status_refresh)

    # =========================================================================
    #  LAUNCH GAME
    # =========================================================================
    def launch_game(self):
        if self.is_launching:
            return

        if not self.gta_path:
            messagebox.showerror("Greska",
                "GTA San Andreas nije pronadjen!\n\n"
                "Klikni 'Browse' dugme i pronadji gta_sa.exe na svom racunaru.")
            return

        gta_exe = os.path.join(self.gta_path, "gta_sa.exe")
        if not os.path.exists(gta_exe):
            messagebox.showerror("Greska",
                f"gta_sa.exe nije pronadjen u:\n{self.gta_path}\n\n"
                "Izaberi ispravni folder.")
            return

        samp_exe = find_samp_exe(self.gta_path)
        if not samp_exe:
            messagebox.showerror("Greska",
                "samp.exe nije pronadjen!\n\n"
                "Morate instalirati SA-MP client prvo.\n"
                "Preuzmite ga sa sa-mp.com")
            return

        self.is_launching = True
        self.launch_btn.configure(text="POKRECEM...", fg_color=COLOR_ACCENT_GLOW,
                                  state="disabled")
        self.launch_status.configure(text="Konektujem se na server...",
                                     text_color=COLOR_ACCENT)

        thread = threading.Thread(target=self._do_launch, args=(samp_exe,), daemon=True)
        thread.start()

    def _do_launch(self, samp_exe):
        try:
            connect_str = f"{SERVER_IP}:{SERVER_PORT}"

            self.root.after(0, lambda: self.launch_status.configure(
                text=f"Pokrecem SA-MP... {connect_str}", text_color=COLOR_ACCENT))

            subprocess.Popen([samp_exe, connect_str], cwd=self.gta_path)

            time.sleep(2)

            self.root.after(0, lambda: self.launch_status.configure(
                text="Igra pokrenuta! Uzivaj na Unicate Gaming!", text_color=COLOR_SUCCESS))

            time.sleep(5)
            self.root.after(0, self.root.destroy)

        except Exception as e:
            self.root.after(0, lambda: self._launch_error(str(e)))

    def _launch_error(self, error_msg):
        self.is_launching = False
        self.launch_btn.configure(text="LAUNCH", fg_color=COLOR_ACCENT, state="normal")
        self.launch_status.configure(text="", text_color=COLOR_TEXT_DIM)
        messagebox.showerror("Greska",
            f"Nije moguce pokrenuti igru!\n\n{error_msg}")

    # =========================================================================
    #  CLOSE
    # =========================================================================
    def on_close(self):
        if self.gta_path:
            save_settings(self.gta_path)
        self.root.destroy()
        sys.exit(0)


# =============================================================================
#  MAIN
# =============================================================================
def main():
    app = UnicateGamingLauncher()
    app.root.mainloop()


if __name__ == "__main__":
    main()
