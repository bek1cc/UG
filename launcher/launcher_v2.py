#!/usr/bin/env python3
# =============================================================================
#  UNICATE GAMING RPG - LAUNCHER V3
#  Ultra-modern CustomTkinter Launcher sa open.mp + CEF auto-instalacijom
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
import zipfile
import io
import shutil
import hashlib
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

LAUNCHER_VERSION = "3.0.0"
LAUNCHER_TITLE = "UNICATE GAMING"

# open.mp / CEF download linkovi
OMP_CEF_RELEASE_URL = "https://github.com/aurora-mp/omp-cef/releases/tag/v1.2.0"
OMP_CEF_ASI_URL = "https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/cef.asi"
OMP_CEF_CLIENT_URL = "https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/client-files-v1.2.0.zip"

# open.mp client download (sluzbeni open.mp client)
OMP_CLIENT_API_URL = "https://api.github.com/repos/openmultiplayer/open.mp/releases/latest"

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
COLOR_PROGRESS_BG = "#1e293b"
COLOR_PROGRESS_FILL = "#3b82f6"

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
#  DETEKCIJA GTA Sa + OPEN.MP + CEF
# =============================================================================
def find_gta_sa_path():
    """Pronadji GTA San Andreas instalaciju"""
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


def find_omp_client(gta_path):
    """Provjeri da li je open.mp client instaliran"""
    if not gta_path:
        return False
    # open.mp client se instalira u GTA SA folder
    omp_client = os.path.join(gta_path, "omp-client.exe")
    omp_dll = os.path.join(gta_path, "omp-client.dll")
    # Također provjeri sa-mp client kao fallback
    samp_exe = os.path.join(gta_path, "samp.exe")
    return os.path.exists(omp_client) or os.path.exists(omp_dll) or os.path.exists(samp_exe)


def find_samp_exe(gta_path):
    """Pronadji samp.exe (fallback)"""
    if not gta_path:
        return None
    samp_path = os.path.join(gta_path, "samp.exe")
    if os.path.exists(samp_path):
        return samp_path
    return None


def check_cef_plugin(gta_path):
    """Provjeri da li je CEF plugin instaliran (cef.asi + client files)"""
    if not gta_path:
        return False, "GTA SA nije pronadjen"
    cef_asi = os.path.join(gta_path, "cef.asi")
    cef_folder = os.path.join(gta_path, "cef")
    has_asi = os.path.exists(cef_asi)
    has_folder = os.path.exists(cef_folder)
    if has_asi and has_folder:
        return True, "CEF Plugin instaliran"
    elif has_asi:
        return False, "cef.asi pronadjen, ali fali cef/ folder sa Chromium runtime"
    elif has_folder:
        return False, "cef/ folder pronadjen, ali fali cef.asi"
    else:
        return False, "CEF Plugin NIJE instaliran"


def check_asi_loader(gta_path):
    """Provjeri da li je ASI loader instaliran"""
    if not gta_path:
        return False
    # Voronov ASI loader
    asi_loader = os.path.join(gta_path, "voronAntiaslr.asi")
    # ili dyom loader
    dyom = os.path.join(gta_path, "dyom.asi")
    # ili bilo koji .asi u root folderu (osim cef.asi)
    if os.path.exists(asi_loader) or os.path.exists(dyom):
        return True
    # Provjeri ima li bilo kojeg .asi loadera
    for f in os.listdir(gta_path):
        if f.lower().endswith('.asi') and f.lower() != 'cef.asi':
            return True
    # Provjeri dsound.dll (custom ASI loader)
    dsound = os.path.join(gta_path, "dsound.dll")
    if os.path.exists(dsound):
        return True
    return False


def get_install_status(gta_path):
    """Vrati kompletan status instalacije"""
    status = {
        'gta_found': gta_path is not None,
        'gta_path': gta_path,
        'has_samp': find_samp_exe(gta_path) is not None if gta_path else False,
        'has_omp': find_omp_client(gta_path),
        'cef_installed': False,
        'cef_status': "Nepoznato",
        'has_asi_loader': False,
        'ready_to_play': False,
        'missing': []
    }

    if gta_path:
        cef_ok, cef_msg = check_cef_plugin(gta_path)
        status['cef_installed'] = cef_ok
        status['cef_status'] = cef_msg
        status['has_asi_loader'] = check_asi_loader(gta_path)

        # Provjeri sta fali
        if not status['has_samp'] and not status['has_omp']:
            status['missing'].append('client')  # Treba SA-MP ili open.mp client
        if not status['cef_installed']:
            if not os.path.exists(os.path.join(gta_path, "cef.asi")):
                status['missing'].append('cef_asi')
            if not os.path.exists(os.path.join(gta_path, "cef")):
                status['missing'].append('cef_runtime')
        if not status['has_asi_loader']:
            status['missing'].append('asi_loader')

        # Spreman za igru ako ima client + CEF + ASI loader
        status['ready_to_play'] = (status['has_samp'] or status['has_omp']) and status['cef_installed'] and status['has_asi_loader']

    return status


def save_settings(gta_path, omp_installed=False, cef_installed=False):
    settings = {
        "gta_path": gta_path,
        "omp_installed": omp_installed,
        "cef_installed": cef_installed
    }
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
#  DOWNLOAD MANAGER
# =============================================================================
class DownloadManager:
    """Upravlja automatskim skidanjem i instalacijom komponenti"""

    # ASI Loader (mali, ~50KB)
    ASI_LOADER_URL = "https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/v4.76/dsound.zip"

    def __init__(self, on_progress=None, on_status=None, on_complete=None, on_error=None):
        self.on_progress = on_progress  # callback(progress_percent, downloaded_mb, total_mb, speed_kbps)
        self.on_status = on_status      # callback(status_text)
        self.on_complete = on_complete  # callback(component_name)
        self.on_error = on_error        # callback(error_message)
        self.cancelled = False

    def cancel(self):
        self.cancelled = True

    def _download_file(self, url, dest_path, component_name="file"):
        """Skini fajl sa progress callbackom"""
        try:
            self.on_status(f"Skidam {component_name}...")
            response = requests.get(url, stream=True, timeout=30)
            response.raise_for_status()

            total_size = int(response.headers.get('content-length', 0))
            downloaded = 0
            start_time = time.time()
            chunk_size = 8192

            with open(dest_path, 'wb') as f:
                for chunk in response.iter_content(chunk_size=chunk_size):
                    if self.cancelled:
                        self.on_status("Preuzimanje otkazano")
                        return False

                    f.write(chunk)
                    downloaded += len(chunk)

                    if total_size > 0:
                        progress = (downloaded / total_size) * 100
                        elapsed = time.time() - start_time
                        speed_kbps = (downloaded / 1024) / elapsed if elapsed > 0 else 0
                        downloaded_mb = downloaded / (1024 * 1024)
                        total_mb = total_size / (1024 * 1024)
                        self.on_progress(progress, downloaded_mb, total_mb, speed_kbps)

            self.on_status(f"{component_name} skinut!")
            return True

        except Exception as e:
            self.on_error(f"Greska pri skidanju {component_name}: {str(e)}")
            return False

    def install_asi_loader(self, gta_path):
        """Instaliraj ASI loader (dsound.dll metod)"""
        try:
            self.on_status("Skidam ASI Loader...")

            dest = os.path.join(LAUNCHER_DIR, "temp_asiloader.zip")
            if not self._download_file(self.ASI_LOADER_URL, dest, "ASI Loader"):
                return False

            self.on_status("Instaliram ASI Loader...")

            # Extract dsound.dll iz zip-a
            with zipfile.ZipFile(dest, 'r') as zf:
                for name in zf.namelist():
                    if name.lower().endswith('dsound.dll'):
                        with zf.open(name) as src, \
                             open(os.path.join(gta_path, "dsound.dll"), 'wb') as dst:
                            dst.write(src.read())
                        break

            # Cleanup
            try:
                os.remove(dest)
            except:
                pass

            self.on_status("ASI Loader instaliran!")
            self.on_complete("asi_loader")
            return True

        except Exception as e:
            self.on_error(f"Greska pri instalaciji ASI Loadera: {str(e)}")
            return False

    def install_cef_asi(self, gta_path):
        """Instaliraj cef.asi (~94KB)"""
        try:
            dest = os.path.join(gta_path, "cef.asi")
            if not self._download_file(OMP_CEF_ASI_URL, dest, "cef.asi"):
                return False

            self.on_complete("cef_asi")
            return True

        except Exception as e:
            self.on_error(f"Greska pri instalaciji cef.asi: {str(e)}")
            return False

    def install_cef_runtime(self, gta_path):
        """Instaliraj CEF Chromium runtime (~296MB)"""
        try:
            dest = os.path.join(LAUNCHER_DIR, "temp_cef_runtime.zip")
            if not self._download_file(OMP_CEF_CLIENT_URL, dest, "CEF Chromium Runtime (~296MB)"):
                return False

            self.on_status("Raspakujem CEF Runtime (ovo moze potrajati)...")

            # Extract u GTA SA folder
            with zipfile.ZipFile(dest, 'r') as zf:
                total_files = len(zf.namelist())
                for i, name in enumerate(zf.namelist()):
                    if self.cancelled:
                        break
                    # Preskoci foldere
                    if name.endswith('/'):
                        continue
                    # Izvadi relativnu putanju
                    # client-files ima cef/ unutar sebe
                    target = os.path.join(gta_path, name)
                    os.makedirs(os.path.dirname(target), exist_ok=True)
                    with zf.open(name) as src, open(target, 'wb') as dst:
                        dst.write(src.read())

                    if i % 20 == 0:
                        progress = (i / total_files) * 100
                        self.on_progress(progress, 0, 0, 0)
                        self.on_status(f"Raspakujem CEF Runtime... {i}/{total_files} fajlova")

            # Cleanup
            try:
                os.remove(dest)
            except:
                pass

            self.on_status("CEF Runtime instaliran!")
            self.on_complete("cef_runtime")
            return True

        except Exception as e:
            self.on_error(f"Greska pri instalaciji CEF Runtimea: {str(e)}")
            return False

    def install_all(self, gta_path, missing_components):
        """Instaliraj sve potrebne komponente"""
        try:
            total = len(missing_components)
            completed = 0

            for component in missing_components:
                if self.cancelled:
                    break

                success = False
                if component == 'asi_loader':
                    success = self.install_asi_loader(gta_path)
                elif component == 'cef_asi':
                    success = self.install_cef_asi(gta_path)
                elif component == 'cef_runtime':
                    success = self.install_cef_runtime(gta_path)
                elif component == 'client':
                    # SA-MP client se ne moze automatski skinuti vise
                    # ali open.mp server je kompatibilan sa SA-MP klijentom
                    self.on_status("SA-MP client nije pronadjen. Pokusavam pronaci na racunaru...")
                    # Ne mozemo automatski instalirati SA-MP client
                    success = True  # Nastavi dalje, launch ce prijaviti gresku

                if success:
                    completed += 1

            if not self.cancelled:
                self.on_status("Instalacija zavrsena!")
                self.on_complete("all")

        except Exception as e:
            self.on_error(f"Greska pri instalaciji: {str(e)}")


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
        self.create_oval(0, 0, self.size+4, self.size+4,
                        fill="", outline=self.color, width=1)
        self.create_oval(2, 2, self.size+2, self.size+2,
                        fill=self.color, outline="")

    def set_color(self, color):
        self.color = color
        self._draw()


class ProgressBar(ctk.CTkFrame):
    """Custom progress bar"""
    def __init__(self, master, height=8, **kwargs):
        super().__init__(master, height=height, fg_color=COLOR_PROGRESS_BG,
                         corner_radius=4, **kwargs)
        self.pack_propagate(False)
        self.progress = 0
        self.fill = ctk.CTkFrame(self, height=height-2, fg_color=COLOR_PROGRESS_FILL,
                                  corner_radius=3)
        self.fill.place(relx=0, rely=0.5, relwidth=0, relheight=1, anchor="w")

    def set_progress(self, percent):
        self.progress = max(0, min(100, percent))
        self.fill.place(relx=0, rely=0.5, relwidth=self.progress/100, relheight=1, anchor="w")

    def reset(self):
        self.set_progress(0)


# =============================================================================
#  GLAVNI LAUNCHER
# =============================================================================
class UnicateGamingLauncher:
    def __init__(self):
        self.root = ctk.CTk()
        self.root.title("Unicate Gaming Launcher")
        self.root.geometry("1050x720")
        self.root.resizable(False, False)
        self.root.configure(fg_color=COLOR_BG)

        # Centriraj prozor
        self.root.update_idletasks()
        w = 1050
        h = 720
        x = (self.root.winfo_screenwidth() // 2) - (w // 2)
        y = (self.root.winfo_screenheight() // 2) - (h // 2)
        self.root.geometry(f'{w}x{h}+{x}+{y}')

        # Varijable
        settings = load_settings()
        self.gta_path = settings.get("gta_path", find_gta_sa_path())
        self.server_info = None
        self.is_launching = False
        self.is_installing = False
        self.download_manager = None
        self.logo_image = None
        self.install_status = get_install_status(self.gta_path)

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
            logo_path = os.path.join(LAUNCHER_DIR, "logo.png")
            if not os.path.exists(logo_path):
                logo_path = os.path.join(LAUNCHER_DIR, "ug_logo.png")
            if os.path.exists(logo_path):
                self.logo_image = ctk.CTkImage(
                    light_image=Image.open(logo_path),
                    dark_image=Image.open(logo_path),
                    size=(180, 180)
                )
                self.logo_small = ctk.CTkImage(
                    light_image=Image.open(logo_path),
                    dark_image=Image.open(logo_path),
                    size=(40, 40)
                )
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

        ctk.CTkLabel(self.topbar, text=f"v{LAUNCHER_VERSION} | open.mp",
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
            ("open.mp Server", "Server je sad na open.mp platformi sa ugradjenim CEF-om! Tablet, inventar i laptop rade bez dodatnih pluginova.", COLOR_ACCENT),
            ("Auto-Instalacija", "Launcher automatski skida i instalira sve sto trebas - CEF, ASI loader, Chromium runtime. Samo klikni LAUNCH!", "#8b5cf6"),
            ("Bounty Sistem", "Postavljanje nagrada za igrace putem tableta ili laptopa.", COLOR_WARNING),
        ]

        for title, desc, color in news_items:
            card = ctk.CTkFrame(news_frame, fg_color=COLOR_CARD, corner_radius=10,
                               height=55)
            card.pack(fill="x", padx=15, pady=3)
            card.pack_propagate(False)

            inner = ctk.CTkFrame(card, fg_color="transparent")
            inner.pack(fill="both", expand=True, padx=12, pady=8)

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
            link_label = ctk.CTkLabel(links_frame, text=name,
                        font=ctk.CTkFont(size=10, underline=True),
                        text_color=COLOR_ACCENT, fg_color="transparent",
                        cursor="hand2")
            link_label.pack(side="left", padx=(0, 15))
            link_label.bind("<Button-1>", lambda e, u=url: webbrowser.open(u))

        # --- DESNI PANEL (Launch + Settings + Install) ---
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
        conn_card.pack(fill="x", pady=(0, 12))

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
                    anchor="w").pack(anchor="w", pady=(5, 5))

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

        ctk.CTkButton(right_content, text="Browse", width=80, height=28,
                      fg_color=COLOR_ACCENT, hover_color=COLOR_ACCENT_HOVER,
                      font=ctk.CTkFont(size=11, weight="bold"),
                      command=self.browse_gta_path).pack(anchor="w", pady=(0, 12))

        # === KOMPONENTE STATUS ===
        ctk.CTkLabel(right_content, text="KOMPONENTE",
                    font=ctk.CTkFont(size=12, weight="bold"),
                    text_color=COLOR_TEXT_DIM, fg_color="transparent",
                    anchor="w").pack(anchor="w", pady=(5, 5))

        components_card = ctk.CTkFrame(right_content, fg_color=COLOR_INPUT_BG,
                                       corner_radius=10)
        components_card.pack(fill="x", pady=(0, 5))

        comp_inner = ctk.CTkFrame(components_card, fg_color="transparent")
        comp_inner.pack(fill="both", expand=True, padx=12, pady=8)

        # SA-MP / open.mp client
        client_row = ctk.CTkFrame(comp_inner, fg_color="transparent")
        client_row.pack(fill="x", pady=2)

        self.client_dot = StatusDot(client_row, size=8)
        self.client_dot.pack(side="left", padx=(0, 6))

        client_label_text = "SA-MP Client OK" if self.install_status['has_samp'] else \
                           "open.mp Client OK" if self.install_status['has_omp'] else \
                           "Nije pronadjen"
        client_color = COLOR_SUCCESS if (self.install_status['has_samp'] or self.install_status['has_omp']) else COLOR_ERROR

        self.client_label = ctk.CTkLabel(client_row, text=client_label_text,
                    font=ctk.CTkFont(size=11),
                    text_color=client_color, fg_color="transparent")
        self.client_label.pack(side="left")

        if self.install_status['has_samp'] or self.install_status['has_omp']:
            self.client_dot.set_color(COLOR_SUCCESS)
        else:
            self.client_dot.set_color(COLOR_ERROR)

        # ASI Loader
        asi_row = ctk.CTkFrame(comp_inner, fg_color="transparent")
        asi_row.pack(fill="x", pady=2)

        self.asi_dot = StatusDot(asi_row, size=8)
        self.asi_dot.pack(side="left", padx=(0, 6))
        self.asi_dot.set_color(COLOR_SUCCESS if self.install_status['has_asi_loader'] else COLOR_WARNING)

        self.asi_label = ctk.CTkLabel(asi_row,
                    text="ASI Loader instaliran" if self.install_status['has_asi_loader'] else "ASI Loader fali",
                    font=ctk.CTkFont(size=11),
                    text_color=COLOR_SUCCESS if self.install_status['has_asi_loader'] else COLOR_WARNING,
                    fg_color="transparent")
        self.asi_label.pack(side="left")

        # CEF Plugin
        cef_row = ctk.CTkFrame(comp_inner, fg_color="transparent")
        cef_row.pack(fill="x", pady=2)

        self.cef_dot = StatusDot(cef_row, size=8)
        self.cef_dot.pack(side="left", padx=(0, 6))
        self.cef_dot.set_color(COLOR_SUCCESS if self.install_status['cef_installed'] else COLOR_WARNING)

        self.cef_label = ctk.CTkLabel(cef_row,
                    text=self.install_status['cef_status'],
                    font=ctk.CTkFont(size=11),
                    text_color=COLOR_SUCCESS if self.install_status['cef_installed'] else COLOR_WARNING,
                    fg_color="transparent")
        self.cef_label.pack(side="left")

        # CEF Runtime
        cef_runtime_installed = self.gta_path and os.path.exists(os.path.join(self.gta_path, "cef"))
        runtime_row = ctk.CTkFrame(comp_inner, fg_color="transparent")
        runtime_row.pack(fill="x", pady=2)

        self.runtime_dot = StatusDot(runtime_row, size=8)
        self.runtime_dot.pack(side="left", padx=(0, 6))
        self.runtime_dot.set_color(COLOR_SUCCESS if cef_runtime_installed else COLOR_WARNING)

        self.runtime_label = ctk.CTkLabel(runtime_row,
                    text="Chromium Runtime instaliran" if cef_runtime_installed else "Chromium Runtime fali (~296MB)",
                    font=ctk.CTkFont(size=11),
                    text_color=COLOR_SUCCESS if cef_runtime_installed else COLOR_WARNING,
                    fg_color="transparent")
        self.runtime_label.pack(side="left")

        # === PROGRESS BAR ===
        self.progress_frame = ctk.CTkFrame(right_content, fg_color="transparent")
        self.progress_frame.pack(fill="x", pady=(8, 0))

        self.progress_bar = ProgressBar(self.progress_frame, height=8)
        self.progress_bar.pack(fill="x", pady=(0, 4))

        self.progress_label = ctk.CTkLabel(self.progress_frame, text="",
                    font=ctk.CTkFont(size=10),
                    text_color=COLOR_TEXT_DIM, fg_color="transparent",
                    anchor="w")
        self.progress_label.pack(anchor="w")

        self.download_detail_label = ctk.CTkLabel(self.progress_frame, text="",
                    font=ctk.CTkFont(size=9),
                    text_color=COLOR_TEXT_DIM, fg_color="transparent",
                    anchor="w")
        self.download_detail_label.pack(anchor="w")

        # === AUTO-INSTALL DUGME ===
        self.install_btn = ctk.CTkButton(
            right_content, text="AUTO-INSTALACIJA", height=35,
            font=ctk.CTkFont(family="Segoe UI", size=13, weight="bold"),
            fg_color="#8b5cf6", hover_color="#7c3aed",
            corner_radius=10,
            command=self.auto_install
        )

        # Prikazi install dugme samo ako nesto fali
        if not self.install_status['ready_to_play']:
            self.install_btn.pack(fill="x", pady=(8, 5))
        else:
            self.install_btn.pack_forget()

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
            self.refresh_install_status()

    # =========================================================================
    #  REFRESH INSTALL STATUS
    # =========================================================================
    def refresh_install_status(self):
        """Osvjezi status svih komponenti"""
        self.install_status = get_install_status(self.gta_path)

        # Update UI
        has_client = self.install_status['has_samp'] or self.install_status['has_omp']
        self.client_dot.set_color(COLOR_SUCCESS if has_client else COLOR_ERROR)
        self.client_label.configure(
            text="SA-MP Client OK" if self.install_status['has_samp'] else \
                 "open.mp Client OK" if self.install_status['has_omp'] else "Nije pronadjen",
            text_color=COLOR_SUCCESS if has_client else COLOR_ERROR
        )

        self.asi_dot.set_color(COLOR_SUCCESS if self.install_status['has_asi_loader'] else COLOR_WARNING)
        self.asi_label.configure(
            text="ASI Loader instaliran" if self.install_status['has_asi_loader'] else "ASI Loader fali",
            text_color=COLOR_SUCCESS if self.install_status['has_asi_loader'] else COLOR_WARNING
        )

        self.cef_dot.set_color(COLOR_SUCCESS if self.install_status['cef_installed'] else COLOR_WARNING)
        self.cef_label.configure(
            text=self.install_status['cef_status'],
            text_color=COLOR_SUCCESS if self.install_status['cef_installed'] else COLOR_WARNING
        )

        cef_runtime_installed = self.gta_path and os.path.exists(os.path.join(self.gta_path, "cef"))
        self.runtime_dot.set_color(COLOR_SUCCESS if cef_runtime_installed else COLOR_WARNING)
        self.runtime_label.configure(
            text="Chromium Runtime instaliran" if cef_runtime_installed else "Chromium Runtime fali (~296MB)",
            text_color=COLOR_SUCCESS if cef_runtime_installed else COLOR_WARNING
        )

        # Show/hide install button
        if self.install_status['ready_to_play']:
            self.install_btn.pack_forget()
            self.progress_label.configure(text="Sve komponente spremne!", text_color=COLOR_SUCCESS)
            self.progress_bar.set_progress(100)
        else:
            self.install_btn.pack(fill="x", pady=(8, 5))

    # =========================================================================
    #  AUTO-INSTALL
    # =========================================================================
    def auto_install(self):
        """Automatski instaliraj sve potrebne komponente"""
        if self.is_installing:
            return

        if not self.gta_path:
            messagebox.showerror("Greska",
                "GTA San Andreas nije pronadjen!\n\n"
                "Klikni 'Browse' dugme i pronadji gta_sa.exe na svom racunaru.")
            return

        if not HAS_REQUESTS:
            messagebox.showerror("Greska",
                "Requests biblioteka nije instalirana!\n\n"
                "Pokreni: pip install requests")
            return

        self.is_installing = True
        self.install_btn.configure(text="INSTALIRAM...", state="disabled", fg_color="#6d28d9")
        self.launch_btn.configure(state="disabled")
        self.progress_bar.reset()
        self.progress_label.configure(text="Zapocinjem instalaciju...", text_color=COLOR_ACCENT)
        self.download_detail_label.configure(text="")

        self.download_manager = DownloadManager(
            on_progress=self._on_download_progress,
            on_status=self._on_download_status,
            on_complete=self._on_download_complete,
            on_error=self._on_download_error
        )

        thread = threading.Thread(
            target=self.download_manager.install_all,
            args=(self.gta_path, self.install_status['missing']),
            daemon=True
        )
        thread.start()

    def _on_download_progress(self, percent, downloaded_mb, total_mb, speed_kbps):
        """Progress callback - update UI iz glavnog threada"""
        self.root.after(0, lambda: self._update_progress_ui(percent, downloaded_mb, total_mb, speed_kbps))

    def _update_progress_ui(self, percent, downloaded_mb, total_mb, speed_kbps):
        self.progress_bar.set_progress(percent)
        if total_mb > 0:
            self.download_detail_label.configure(
                text=f"{downloaded_mb:.1f} MB / {total_mb:.1f} MB  |  {speed_kbps:.0f} KB/s"
            )
        else:
            if downloaded_mb > 0:
                self.download_detail_label.configure(text=f"{downloaded_mb:.1f} MB skinuto")

    def _on_download_status(self, status_text):
        """Status callback"""
        self.root.after(0, lambda: self.progress_label.configure(text=status_text, text_color=COLOR_ACCENT))

    def _on_download_complete(self, component_name):
        """Complete callback"""
        self.root.after(0, self._handle_install_complete, component_name)

    def _handle_install_complete(self, component_name):
        if component_name == "all":
            self.is_installing = False
            self.install_btn.configure(text="AUTO-INSTALACIJA", state="normal", fg_color="#8b5cf6")
            self.launch_btn.configure(state="normal")
            self.progress_label.configure(text="Sve komponente instalirane!", text_color=COLOR_SUCCESS)
            self.progress_bar.set_progress(100)
            self.refresh_install_status()
            save_settings(self.gta_path, True, True)

    def _on_download_error(self, error_message):
        """Error callback"""
        self.root.after(0, lambda: self._handle_install_error(error_message))

    def _handle_install_error(self, error_message):
        self.is_installing = False
        self.install_btn.configure(text="AUTO-INSTALACIJA", state="normal", fg_color="#8b5cf6")
        self.launch_btn.configure(state="normal")
        self.progress_label.configure(text="Greska pri instalaciji", text_color=COLOR_ERROR)
        messagebox.showerror("Instalacija Greska",
            f"Greska pri automatskoj instalaciji:\n\n{error_message}\n\n"
            "Pokusi ponovo ili instaliraj rucno sa:\n"
            f"{OMP_CEF_RELEASE_URL}")

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
        if self.is_launching or self.is_installing:
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

        # Provjeri client (SA-MP ili open.mp)
        samp_exe = find_samp_exe(self.gta_path)
        if not samp_exe:
            # Ako nema samp.exe, provjeri ima li open.mp clienta
            # open.mp koristi isti samp.exe princip
            messagebox.showerror("Greska",
                "SA-MP / open.mp client nije pronadjen!\n\n"
                "Morate instalirati SA-MP client (0.3.7-R4) u GTA SA folder.\n"
                "Preuzmite ga sa sa-mp.com ili koristite AUTO-INSTALACIJA dugme.")
            return

        # Provjeri CEF ako nije instaliran
        cef_ok, cef_msg = check_cef_plugin(self.gta_path)
        if not cef_ok:
            result = messagebox.askyesno("CEF Plugin",
                f"CEF Plugin nije instaliran!\n\n"
                f"{cef_msg}\n\n"
                "Bez CEF plugin-a necete moci koristiti Tablet, Inventar i Laptop.\n\n"
                "Zelite li pokrenuti AUTO-INSTALACIJU sada?")
            if result:
                self.auto_install()
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
                text=f"Pokrecem igru... {connect_str}", text_color=COLOR_ACCENT))

            # Pokreni SA-MP client sa server adresom
            # Ovo radi i sa open.mp serverom jer je kompatibilan
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
        if self.download_manager:
            self.download_manager.cancel()
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
