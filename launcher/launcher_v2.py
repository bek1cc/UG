#!/usr/bin/env python3
# =============================================================================
#  UNICATE GAMING RPG - LAUNCHER V4
#  Full-Screen Gaming Portal sa animacijama, efektima, GTA temom
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
import math
import random
from tkinter import filedialog, messagebox

try:
    from PIL import Image, ImageTk, ImageDraw, ImageFont, ImageFilter, ImageEnhance
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

LAUNCHER_VERSION = "4.0.0"
LAUNCHER_TITLE = "UNICATE GAMING"

# open.mp / CEF download linkovi
OMP_CEF_RELEASE_URL = "https://github.com/aurora-mp/omp-cef/releases/tag/v1.2.0"
OMP_CEF_ASI_URL = "https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/cef.asi"
OMP_CEF_CLIENT_URL = "https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/client-files-v1.2.0.zip"

# Folderi
if getattr(sys, 'frozen', False):
    LAUNCHER_DIR = os.path.dirname(sys.executable)
else:
    LAUNCHER_DIR = os.path.dirname(os.path.abspath(__file__))

# =============================================================================
#  BOJE I TEMA
# =============================================================================
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

# Neon boje
NEON_BLUE = "#00d4ff"
NEON_PURPLE = "#8b5cf6"
NEON_PINK = "#ec4899"
NEON_GREEN = "#22c55e"
NEON_RED = "#ef4444"
NEON_ORANGE = "#f59e0b"
NEON_CYAN = "#06b6d4"

# Pozadine
BG_DARK = "#050810"
BG_PANEL = "transparent"
BG_CARD = "#0d1117"
BG_CARD_HOVER = "#161b22"
BG_INPUT = "#0a0f1a"

# Text
TEXT_BRIGHT = "#ffffff"
TEXT_NORMAL = "#c9d1d9"
TEXT_DIM = "#6e7681"
TEXT_ACCENT = NEON_BLUE

# Border
BORDER_GLOW = "#1a3a5f"
BORDER_SUBTLE = "#21262d"

# =============================================================================
#  PARTICLE SYSTEM (Animirane čestice u pozadini)
# =============================================================================
class Particle:
    def __init__(self, x, y, vx, vy, size, color, life):
        self.x = x
        self.y = y
        self.vx = vx
        self.vy = vy
        self.size = size
        self.color = color
        self.life = life
        self.max_life = life
        self.alpha = 1.0


class ParticleSystem:
    """Sistem animiranih čestica za pozadinu"""
    def __init__(self, width, height, max_particles=60):
        self.width = width
        self.height = height
        self.max_particles = max_particles
        self.particles = []
        self.colors = [NEON_BLUE, NEON_PURPLE, NEON_PINK, NEON_CYAN, "#ffffff"]

    def update(self):
        # Dodaj nove čestice
        while len(self.particles) < self.max_particles:
            self._spawn_particle()

        # Update postojeće
        alive = []
        for p in self.particles:
            p.x += p.vx
            p.y += p.vy
            p.life -= 1
            p.alpha = max(0, p.life / p.max_life)
            if p.life > 0 and 0 <= p.x <= self.width and 0 <= p.y <= self.height:
                alive.append(p)
        self.particles = alive

    def _spawn_particle(self):
        x = random.randint(0, self.width)
        y = random.randint(0, self.height)
        vx = random.uniform(-0.3, 0.3)
        vy = random.uniform(-0.8, -0.1)
        size = random.uniform(1, 3)
        color = random.choice(self.colors)
        life = random.randint(100, 400)
        self.particles.append(Particle(x, y, vx, vy, size, color, life))


# =============================================================================
#  SA-MP QUERY
# =============================================================================
def query_samp_server(ip, port, timeout=3):
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
            'password': password, 'players': players, 'max_players': max_players,
            'name': server_name, 'gamemode': gamemode, 'map': map_name, 'online': True
        }
    except:
        return {'online': False}


# =============================================================================
#  DETEKCIJA GTA SA + OPEN.MP + CEF
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
        r"C:\Games\GTA San Andreas", r"C:\Games\GTA SA",
        r"D:\Games\GTA San Andreas", r"D:\GTA San Andreas",
        r"D:\GTA SA", r"D:\Games\GTA SA",
        r"E:\GTA San Andreas", r"E:\GTA SA",
        os.path.expanduser(r"~\Desktop\GTA San Andreas"),
        os.path.expanduser(r"~\Desktop\GTA SA"),
    ]
    for path in common_paths:
        if os.path.exists(os.path.join(path, "gta_sa.exe")):
            return path
    return None


def find_samp_exe(gta_path):
    if not gta_path:
        return None
    samp_path = os.path.join(gta_path, "samp.exe")
    return samp_path if os.path.exists(samp_path) else None


def check_cef_plugin(gta_path):
    if not gta_path:
        return False, "GTA SA nije pronadjen"
    cef_asi = os.path.exists(os.path.join(gta_path, "cef.asi"))
    cef_folder = os.path.exists(os.path.join(gta_path, "cef"))
    if cef_asi and cef_folder:
        return True, "CEF instaliran"
    elif cef_asi:
        return False, "cef.asi OK, fali cef/ folder"
    elif cef_folder:
        return False, "cef/ OK, fali cef.asi"
    return False, "CEF NIJE instaliran"


def check_asi_loader(gta_path):
    if not gta_path:
        return False
    if os.path.exists(os.path.join(gta_path, "dsound.dll")):
        return True
    for f in os.listdir(gta_path):
        if f.lower().endswith('.asi') and f.lower() != 'cef.asi':
            return True
    return False


def get_install_status(gta_path):
    status = {
        'gta_found': gta_path is not None, 'gta_path': gta_path,
        'has_samp': find_samp_exe(gta_path) is not None if gta_path else False,
        'cef_installed': False, 'cef_status': "Nepoznato",
        'has_asi_loader': False, 'ready_to_play': False, 'missing': []
    }
    if gta_path:
        cef_ok, cef_msg = check_cef_plugin(gta_path)
        status['cef_installed'] = cef_ok
        status['cef_status'] = cef_msg
        status['has_asi_loader'] = check_asi_loader(gta_path)
        if not status['has_samp']:
            status['missing'].append('client')
        if not status['cef_installed']:
            if not os.path.exists(os.path.join(gta_path, "cef.asi")):
                status['missing'].append('cef_asi')
            if not os.path.exists(os.path.join(gta_path, "cef")):
                status['missing'].append('cef_runtime')
        if not status['has_asi_loader']:
            status['missing'].append('asi_loader')
        status['ready_to_play'] = status['has_samp'] and status['cef_installed'] and status['has_asi_loader']
    return status


def save_settings(gta_path, **kwargs):
    settings = {"gta_path": gta_path}
    settings.update(kwargs)
    try:
        with open(os.path.join(LAUNCHER_DIR, "settings.json"), 'w') as f:
            json.dump(settings, f)
    except:
        pass


def load_settings():
    try:
        sf = os.path.join(LAUNCHER_DIR, "settings.json")
        if os.path.exists(sf):
            with open(sf, 'r') as f:
                return json.load(f)
    except:
        pass
    return {}


# =============================================================================
#  DOWNLOAD MANAGER
# =============================================================================
class DownloadManager:
    ASI_LOADER_URL = "https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/v4.76/dsound.zip"

    def __init__(self, on_progress=None, on_status=None, on_complete=None, on_error=None):
        self.on_progress = on_progress
        self.on_status = on_status
        self.on_complete = on_complete
        self.on_error = on_error
        self.cancelled = False

    def cancel(self):
        self.cancelled = True

    def _download_file(self, url, dest_path, component_name="file"):
        try:
            self.on_status(f"Skidam {component_name}...")
            response = requests.get(url, stream=True, timeout=30)
            response.raise_for_status()
            total_size = int(response.headers.get('content-length', 0))
            downloaded = 0
            start_time = time.time()
            with open(dest_path, 'wb') as f:
                for chunk in response.iter_content(chunk_size=8192):
                    if self.cancelled:
                        return False
                    f.write(chunk)
                    downloaded += len(chunk)
                    if total_size > 0:
                        progress = (downloaded / total_size) * 100
                        elapsed = time.time() - start_time
                        speed = (downloaded / 1024) / elapsed if elapsed > 0 else 0
                        self.on_progress(progress, downloaded / (1024*1024), total_size / (1024*1024), speed)
            return True
        except Exception as e:
            self.on_error(f"Greska: {str(e)}")
            return False

    def install_asi_loader(self, gta_path):
        dest = os.path.join(LAUNCHER_DIR, "temp_asiloader.zip")
        if not self._download_file(self.ASI_LOADER_URL, dest, "ASI Loader"):
            return False
        self.on_status("Instaliram ASI Loader...")
        with zipfile.ZipFile(dest, 'r') as zf:
            for name in zf.namelist():
                if name.lower().endswith('dsound.dll'):
                    with zf.open(name) as src, open(os.path.join(gta_path, "dsound.dll"), 'wb') as dst:
                        dst.write(src.read())
                    break
        try: os.remove(dest)
        except: pass
        self.on_complete("asi_loader")
        return True

    def install_cef_asi(self, gta_path):
        dest = os.path.join(gta_path, "cef.asi")
        if not self._download_file(OMP_CEF_ASI_URL, dest, "cef.asi"):
            return False
        self.on_complete("cef_asi")
        return True

    def install_cef_runtime(self, gta_path):
        dest = os.path.join(LAUNCHER_DIR, "temp_cef_runtime.zip")
        if not self._download_file(OMP_CEF_CLIENT_URL, dest, "CEF Chromium Runtime (~296MB)"):
            return False
        self.on_status("Raspakujem CEF Runtime...")
        with zipfile.ZipFile(dest, 'r') as zf:
            total = len(zf.namelist())
            for i, name in enumerate(zf.namelist()):
                if self.cancelled: break
                if name.endswith('/'): continue
                target = os.path.join(gta_path, name)
                os.makedirs(os.path.dirname(target), exist_ok=True)
                with zf.open(name) as src, open(target, 'wb') as dst:
                    dst.write(src.read())
                if i % 20 == 0:
                    self.on_progress((i/total)*100, 0, 0, 0)
                    self.on_status(f"Raspakujem... {i}/{total}")
        try: os.remove(dest)
        except: pass
        self.on_complete("cef_runtime")
        return True

    def install_all(self, gta_path, missing):
        try:
            for component in missing:
                if self.cancelled: break
                if component == 'asi_loader':
                    self.install_asi_loader(gta_path)
                elif component == 'cef_asi':
                    self.install_cef_asi(gta_path)
                elif component == 'cef_runtime':
                    self.install_cef_runtime(gta_path)
            if not self.cancelled:
                self.on_status("Instalacija zavrsena!")
                self.on_complete("all")
        except Exception as e:
            self.on_error(str(e))


# =============================================================================
#  ANIMATED WIDGETS
# =============================================================================
class NeonButton(ctk.CTkButton):
    """Neon glow dugme sa pulse animacijom"""
    def __init__(self, master, glow_color=NEON_BLUE, pulse=False, **kwargs):
        self.glow_color = glow_color
        self.pulse = pulse
        self._pulse_phase = 0
        self._pulse_running = False
        super().__init__(master, **kwargs)
        self.bind("<Enter>", self._on_enter)
        self.bind("<Leave>", self._on_leave)

    def _on_enter(self, event):
        self.configure(fg_color=self.glow_color)

    def _on_leave(self, event):
        if not self.pulse:
            self.configure(fg_color="#1a1a2e")

    def start_pulse(self):
        self._pulse_running = True
        self._do_pulse()

    def stop_pulse(self):
        self._pulse_running = False

    def _do_pulse(self):
        if not self._pulse_running:
            return
        self._pulse_phase += 0.05
        intensity = (math.sin(self._pulse_phase) + 1) / 2
        # Interpolate between dark and glow color
        r1, g1, b1 = 26, 26, 46  # #1a1a2e
        r2 = int(self.glow_color[1:3], 16)
        g2 = int(self.glow_color[3:5], 16)
        b2 = int(self.glow_color[5:7], 16)
        r = int(r1 + (r2 - r1) * intensity * 0.5)
        g = int(g1 + (g2 - g1) * intensity * 0.5)
        b = int(b1 + (b2 - b1) * intensity * 0.5)
        color = f"#{r:02x}{g:02x}{b:02x}"
        self.configure(fg_color=color)
        self.after(50, self._do_pulse)


class AnimatedProgressBar(ctk.CTkCanvas):
    """Animirani neon progress bar"""
    def __init__(self, master, height=6, color=NEON_BLUE, **kwargs):
        super().__init__(master, height=height, bg=BG_DARK, highlightthickness=0, **kwargs)
        self.bar_color = color
        self.progress = 0
        self._anim_progress = 0
        self.height = height

    def set_progress(self, percent):
        self.progress = max(0, min(100, percent))
        self._animate_to()

    def _animate_to(self):
        if abs(self._anim_progress - self.progress) < 0.5:
            self._anim_progress = self.progress
            self._draw()
            return
        self._anim_progress += (self.progress - self._anim_progress) * 0.15
        self._draw()
        self.after(16, self._animate_to)

    def _draw(self):
        self.delete("all")
        w = self.winfo_width()
        h = self.winfo_height()
        if w < 2:
            return
        # Background track
        self.create_rectangle(0, 0, w, h, fill="#0a0f1a", outline="")
        # Progress fill
        fill_w = (self._anim_progress / 100) * w
        if fill_w > 0:
            # Glow effect - wider semi-transparent bar behind
            self.create_rectangle(0, 0, fill_w + 4, h, fill="#0a2a4a", outline="")
            # Main bar
            self.create_rectangle(0, 0, fill_w, h, fill=self.bar_color, outline="")
            # Bright tip
            tip_x = max(0, fill_w - 8)
            self.create_rectangle(tip_x, 0, fill_w, h, fill="#4dd9ff", outline="")

    def reset(self):
        self.progress = 0
        self._anim_progress = 0
        self._draw()


class PulseDot(ctk.CTkCanvas):
    """Pulsirajući status indikator"""
    def __init__(self, master, size=10, color=NEON_GREEN, **kwargs):
        super().__init__(master, width=size+8, height=size+8,
                         bg=BG_DARK, highlightthickness=0, **kwargs)
        self.size = size
        self.color = color
        self._phase = 0
        self._draw()

    def set_color(self, color):
        self.color = color
        self._draw()

    def _draw(self):
        self.delete("all")
        s = self.size
        cx, cy = s//2 + 4, s//2 + 4
        # Outer glow
        self.create_oval(cx-s//2-2, cy-s//2-2, cx+s//2+2, cy+s//2+2,
                        fill="", outline=self.color, width=1)
        # Inner dot
        self.create_oval(cx-s//2, cy-s//2, cx+s//2, cy+s//2,
                        fill=self.color, outline="")


class ParticleCanvas(ctk.CTkCanvas):
    """Canvas za animirane čestice u pozadini"""
    def __init__(self, master, **kwargs):
        super().__init__(master, bg=BG_DARK, highlightthickness=0, **kwargs)
        self.particle_system = None
        self._running = False
        self._bg_image = None
        self._bg_photo = None

    def set_background(self, image_path):
        """Postavi pozadinsku sliku"""
        try:
            if os.path.exists(image_path):
                img = Image.open(image_path)
                # Resize to fit screen
                screen_w = self.winfo_screenwidth()
                screen_h = self.winfo_screenheight()
                img = img.resize((screen_w, screen_h), Image.LANCZOS)
                # Tamni overlay
                enhancer = ImageEnhance.Brightness(img)
                img = enhancer.enhance(0.35)
                # Blur za dubinu
                img = img.filter(ImageFilter.GaussianBlur(radius=2))
                self._bg_photo = ImageTk.PhotoImage(img)
                self._bg_image = img
        except Exception as e:
            print(f"[UG Launcher] BG greska: {e}")

    def start_particles(self):
        self.update_idletasks()
        w = max(self.winfo_width(), 1920)
        h = max(self.winfo_height(), 1080)
        self.particle_system = ParticleSystem(w, h, max_particles=50)
        self._running = True
        self._animate()

    def stop_particles(self):
        self._running = False

    def _animate(self):
        if not self._running:
            return
        self.delete("all")

        # Pozadinska slika
        if self._bg_photo:
            self.create_image(0, 0, image=self._bg_photo, anchor="nw")

        # Čestice
        if self.particle_system:
            self.particle_system.update()
            for p in self.particle_system.particles:
                alpha_hex = max(0, min(255, int(p.alpha * 120)))
                # Koristi boju sa smanjenom jačinom
                try:
                    r = int(p.color[1:3], 16)
                    g = int(p.color[3:5], 16)
                    b = int(p.color[5:7], 16)
                    r = int(r * p.alpha * 0.5)
                    g = int(g * p.alpha * 0.5)
                    b = int(b * p.alpha * 0.5)
                    color = f"#{r:02x}{g:02x}{b:02x}"
                except:
                    color = p.color
                size = p.size * p.alpha
                if size > 0.5:
                    self.create_oval(
                        p.x - size, p.y - size,
                        p.x + size, p.y + size,
                        fill=color, outline=""
                    )

        # Neon grid linije (subtilne)
        w = self.winfo_width()
        h = self.winfo_height()
        if w > 100 and h > 100:
            grid_color = "#0a1628"
            for x in range(0, w, 80):
                self.create_line(x, 0, x, h, fill=grid_color, width=1)
            for y in range(0, h, 80):
                self.create_line(0, y, w, y, fill=grid_color, width=1)

        self.after(33, self._animate)  # ~30 FPS


# =============================================================================
#  GLAVNI LAUNCHER - FULL SCREEN PORTAL
# =============================================================================
class UnicateGamingLauncher:
    def __init__(self):
        self.root = ctk.CTk()
        self.root.title("Unicate Gaming")
        self.root.overrideredirect(True)  # Full screen bez bordera

        # Full screen dimenzije
        self.screen_w = self.root.winfo_screenwidth()
        self.screen_h = self.root.winfo_screenheight()
        self.root.geometry(f"{self.screen_w}x{self.screen_h}+0+0")
        self.root.configure(fg_color=BG_DARK)

        # Varijable
        settings = load_settings()
        self.gta_path = settings.get("gta_path", find_gta_sa_path())
        self.server_info = None
        self.is_launching = False
        self.is_installing = False
        self.download_manager = None
        self.install_status = get_install_status(self.gta_path)

        # Slike
        self.images = {}
        self._load_images()

        # Build UI
        self.build_ui()

        # Pokreni animacije
        self.particle_canvas.start_particles()

        # Pokreni pulse animaciju na launch dugme
        self.launch_neon_btn.start_pulse()

        # Server check
        self.check_server_status()
        self.schedule_status_refresh()

        # Drag za prozor
        self._drag_start = None
        self.root.bind("<ButtonPress-1>", self._start_drag)
        self.root.bind("<B1-Motion>", self._do_drag)
        self.root.bind("<Escape>", lambda e: self.on_close())

    def _load_images(self):
        """Učitaj sve slike"""
        try:
            # Logo
            logo_path = os.path.join(LAUNCHER_DIR, "ug_logo_pro.png")
            if not os.path.exists(logo_path):
                logo_path = os.path.join(LAUNCHER_DIR, "ug_logo.png")
            if not os.path.exists(logo_path):
                logo_path = os.path.join(LAUNCHER_DIR, "logo.png")
            if os.path.exists(logo_path):
                img = Image.open(logo_path)
                self.images['logo_large'] = ctk.CTkImage(light_image=img, dark_image=img, size=(200, 200))
                self.images['logo_medium'] = ctk.CTkImage(light_image=img, dark_image=img, size=(120, 120))
                self.images['logo_small'] = ctk.CTkImage(light_image=img, dark_image=img, size=(50, 50))
                self.images['logo_nav'] = ctk.CTkImage(light_image=img, dark_image=img, size=(36, 36))

            # Auto slika
            car_path = os.path.join(LAUNCHER_DIR, "car_lowrider.png")
            if os.path.exists(car_path):
                img = Image.open(car_path)
                self.images['car'] = ctk.CTkImage(light_image=img, dark_image=img, size=(400, 400))

            # Tablet slika
            tablet_path = os.path.join(LAUNCHER_DIR, "holo_tablet.png")
            if os.path.exists(tablet_path):
                img = Image.open(tablet_path)
                self.images['tablet'] = ctk.CTkImage(light_image=img, dark_image=img, size=(180, 180))

            # BG slika - koristimo canvas background
            bg_path = os.path.join(LAUNCHER_DIR, "bg_gta.png")
            if not os.path.exists(bg_path):
                # Fallback na drugu lokaciju
                bg_path = os.path.join(LAUNCHER_DIR, "bg_gta.png")
            self.images['bg_path'] = bg_path

        except Exception as e:
            print(f"[UG Launcher] Image load greska: {e}")

    def _start_drag(self, event):
        self._drag_start = (event.x, event.y)

    def _do_drag(self, event):
        if self._drag_start:
            dx = event.x - self._drag_start[0]
            dy = event.y - self._drag_start[1]
            # Samo drag sa top bar područja
            if self._drag_start[1] < 60:
                x = self.root.winfo_x() + dx
                y = self.root.winfo_y() + dy
                self.root.geometry(f"+{x}+{y}")

    # =========================================================================
    #  UI BUILDING
    # =========================================================================
    def build_ui(self):
        # ===== PARTICLE CANVAS (pozadina) =====
        self.particle_canvas = ParticleCanvas(self.root)
        self.particle_canvas.place(x=0, y=0, relwidth=1, relheight=1)

        # Postavi pozadinsku sliku
        if 'bg_path' in self.images and os.path.exists(self.images['bg_path']):
            self.particle_canvas.set_background(self.images['bg_path'])

        # ===== GLAVNI CONTAINER (iznad pozadine) =====
        self.main_container = ctk.CTkFrame(self.root, fg_color="transparent", corner_radius=0)
        self.main_container.place(relx=0.5, rely=0.5, anchor="center",
                                   relwidth=0.92, relheight=0.92)

        # ===== TOP NAVIGATION BAR =====
        self._build_navbar()

        # ===== CONTENT AREA =====
        self._build_content()

    def _build_navbar(self):
        """Napravi top navigation bar"""
        navbar = ctk.CTkFrame(self.main_container, height=60, fg_color="#0a0f1a",
                              corner_radius=12, border_width=1, border_color="#1a2a4a")
        navbar.pack(fill="x", pady=(0, 15))
        navbar.pack_propagate(False)

        nav_inner = ctk.CTkFrame(navbar, fg_color="transparent")
        nav_inner.pack(fill="both", expand=True, padx=20, pady=8)

        # Lijevo - Logo + Ime
        left_nav = ctk.CTkFrame(nav_inner, fg_color="transparent")
        left_nav.pack(side="left", fill="y")

        if 'logo_nav' in self.images:
            ctk.CTkLabel(left_nav, image=self.images['logo_nav'], text="",
                        fg_color="transparent").pack(side="left", padx=(0, 10))

        ctk.CTkLabel(left_nav, text="UNICATE",
                    font=ctk.CTkFont(family="Segoe UI", size=18, weight="bold"),
                    text_color=NEON_BLUE, fg_color="transparent").pack(side="left")
        ctk.CTkLabel(left_nav, text=" GAMING",
                    font=ctk.CTkFont(family="Segoe UI", size=18, weight="bold"),
                    text_color=TEXT_BRIGHT, fg_color="transparent").pack(side="left")

        # Verzija badge
        ver_frame = ctk.CTkFrame(left_nav, fg_color="#1a1a2e", corner_radius=8,
                                  height=22)
        ver_frame.pack(side="left", padx=(15, 0))
        ver_frame.pack_propagate(False)
        ctk.CTkLabel(ver_frame, text=f"v{LAUNCHER_VERSION}",
                    font=ctk.CTkFont(size=9, weight="bold"),
                    text_color=NEON_PURPLE, fg_color="transparent").pack(padx=8)

        # Center - Nav links
        center_nav = ctk.CTkFrame(nav_inner, fg_color="transparent")
        center_nav.pack(side="left", expand=True)

        nav_items = [("SERVER", NEON_BLUE), ("CEF UI", NEON_PURPLE),
                     ("NOVOSTI", NEON_CYAN), ("POSTAVKE", TEXT_DIM)]
        self.nav_labels = {}
        for name, color in nav_items:
            lbl = ctk.CTkLabel(center_nav, text=name,
                        font=ctk.CTkFont(size=12, weight="bold"),
                        text_color=color, fg_color="transparent",
                        cursor="hand2")
            lbl.pack(side="left", padx=18)
            self.nav_labels[name] = lbl

        # Desno - Online + Controls
        right_nav = ctk.CTkFrame(nav_inner, fg_color="transparent")
        right_nav.pack(side="right", fill="y")

        # Online badge
        self.online_badge = ctk.CTkFrame(right_nav, fg_color="#0a1628", corner_radius=8,
                                          height=28)
        self.online_badge.pack(side="left", padx=(0, 15))
        self.online_badge.pack_propagate(False)

        self.status_dot_nav = PulseDot(self.online_badge, size=6, color=NEON_ORANGE)
        self.status_dot_nav.pack(side="left", padx=(8, 4))

        self.online_count_nav = ctk.CTkLabel(
            self.online_badge, text="Provjera...",
            font=ctk.CTkFont(size=10, weight="bold"),
            text_color=TEXT_DIM, fg_color="transparent"
        )
        self.online_count_nav.pack(side="left", padx=(0, 8))

        # Minimize
        ctk.CTkButton(right_nav, text="—", width=35, height=30,
                      fg_color="transparent", hover_color="#1a1a2e",
                      text_color=TEXT_DIM, font=ctk.CTkFont(size=12),
                      corner_radius=8,
                      command=self.root.iconify).pack(side="left", padx=2)

        # Close
        ctk.CTkButton(right_nav, text="✕", width=35, height=30,
                      fg_color="transparent", hover_color="#4a1010",
                      text_color=NEON_RED, font=ctk.CTkFont(size=12, weight="bold"),
                      corner_radius=8,
                      command=self.on_close).pack(side="left", padx=2)

    def _build_content(self):
        """Glavni sadržaj - 3 kolone portal layout"""
        content = ctk.CTkFrame(self.main_container, fg_color="transparent")
        content.pack(fill="both", expand=True)

        # Koristimo grid layout za precise control
        content.grid_columnconfigure(0, weight=3)   # Lijeva kolona
        content.grid_columnconfigure(1, weight=2)   # Desna kolona
        content.grid_rowconfigure(0, weight=3)      # Top red
        content.grid_rowconfigure(1, weight=2)      # Bottom red

        # ===== LIJEVO - HERO SECTION =====
        self._build_hero(content)

        # ===== DESNO GORE - LAUNCH PANEL =====
        self._build_launch_panel(content)

        # ===== LIJEVO DOLJE - NEWS =====
        self._build_news_panel(content)

        # ===== DESNO DOLJE - STATUS =====
        self._build_status_panel(content)

    def _build_hero(self, parent):
        """Hero sekcija - logo, auto, info"""
        hero = ctk.CTkFrame(parent, fg_color="#0a0f1a", corner_radius=16,
                            border_width=1, border_color="#1a2a4a")
        hero.grid(row=0, column=0, sticky="nsew", padx=(0, 8), pady=(0, 8))

        hero_inner = ctk.CTkFrame(hero, fg_color="transparent")
        hero_inner.pack(fill="both", expand=True, padx=30, pady=25)

        # Top red - Logo + Info
        top_section = ctk.CTkFrame(hero_inner, fg_color="transparent")
        top_section.pack(fill="x")

        # Logo
        if 'logo_medium' in self.images:
            logo_frame = ctk.CTkFrame(top_section, fg_color="transparent")
            logo_frame.pack(side="left", padx=(0, 25))

            # Glow efekt oko loga
            glow_frame = ctk.CTkFrame(logo_frame, fg_color="#0d1a2e", corner_radius=20,
                                       border_width=2, border_color=NEON_BLUE)
            glow_frame.pack(padx=5, pady=5)

            ctk.CTkLabel(glow_frame, image=self.images['logo_medium'], text="",
                        fg_color="transparent").pack(padx=8, pady=8)

        # Info
        info_frame = ctk.CTkFrame(top_section, fg_color="transparent")
        info_frame.pack(side="left", fill="both", expand=True)

        ctk.CTkLabel(info_frame, text="UNICATE GAMING",
                    font=ctk.CTkFont(family="Segoe UI", size=36, weight="bold"),
                    text_color=TEXT_BRIGHT, fg_color="transparent",
                    anchor="w").pack(anchor="w")

        ctk.CTkLabel(info_frame, text="R  P  G     S E R V E R",
                    font=ctk.CTkFont(family="Segoe UI", size=14),
                    text_color=NEON_BLUE, fg_color="transparent",
                    anchor="w").pack(anchor="w", pady=(0, 10))

        # Status red
        status_row = ctk.CTkFrame(info_frame, fg_color="transparent")
        status_row.pack(anchor="w", pady=5)

        self.status_dot = PulseDot(status_row, size=10, color=NEON_ORANGE)
        self.status_dot.pack(side="left", padx=(0, 8))

        self.status_label = ctk.CTkLabel(status_row, text="Provjeravam status...",
                                         font=ctk.CTkFont(size=12),
                                         text_color=TEXT_DIM,
                                         fg_color="transparent")
        self.status_label.pack(side="left")

        self.player_label = ctk.CTkLabel(info_frame, text="",
                                         font=ctk.CTkFont(size=14, weight="bold"),
                                         text_color=TEXT_BRIGHT, fg_color="transparent",
                                         anchor="w")
        self.player_label.pack(anchor="w", pady=3)

        self.gamemode_label = ctk.CTkLabel(info_frame, text="",
                                           font=ctk.CTkFont(size=11),
                                           text_color=TEXT_DIM, fg_color="transparent",
                                           anchor="w")
        self.gamemode_label.pack(anchor="w")

        # Separator
        ctk.CTkFrame(hero_inner, height=1, fg_color="#1a2a4a").pack(fill="x", pady=15)

        # Feature cards
        features_frame = ctk.CTkFrame(hero_inner, fg_color="transparent")
        features_frame.pack(fill="both", expand=True)

        # 3 feature carda
        features = [
            ("TABLET", "Moderni CEF tablet UI sa\nGPS, porukama i kontaktima", NEON_BLUE, "tablet" in self.images),
            ("INVENTAR", "Drag & drop inventar\nsa kategorijama i detaljima", NEON_PURPLE, False),
            ("LAPTOP", "Dark Web, Banka, Email\ni Terminal komande", NEON_PINK, False),
        ]

        cards_row = ctk.CTkFrame(features_frame, fg_color="transparent")
        cards_row.pack(fill="both", expand=True)

        for i, (title, desc, color, has_img) in enumerate(features):
            card = ctk.CTkFrame(cards_row, fg_color="#0d1520", corner_radius=12,
                               border_width=1, border_color=color)
            card.pack(side="left", fill="both", expand=True, padx=(0 if i == 0 else 8, 0))

            card_inner = ctk.CTkFrame(card, fg_color="transparent")
            card_inner.pack(fill="both", expand=True, padx=15, pady=12)

            # Title sa neon accent
            title_frame = ctk.CTkFrame(card_inner, fg_color="transparent")
            title_frame.pack(anchor="w", pady=(0, 5))

            # Color bar
            ctk.CTkFrame(title_frame, fg_color=color, width=20, height=3,
                        corner_radius=2).pack(side="left", padx=(0, 8))

            ctk.CTkLabel(title_frame, text=title,
                        font=ctk.CTkFont(size=11, weight="bold"),
                        text_color=color, fg_color="transparent").pack(side="left")

            ctk.CTkLabel(card_inner, text=desc,
                        font=ctk.CTkFont(size=10),
                        text_color=TEXT_DIM, fg_color="transparent",
                        anchor="w", justify="left").pack(anchor="w")

    def _build_launch_panel(self, parent):
        """Launch panel - glavni panel sa LAUNCH dugmetom"""
        panel = ctk.CTkFrame(parent, fg_color="#0a0f1a", corner_radius=16,
                             border_width=1, border_color=NEON_BLUE)
        panel.grid(row=0, column=1, sticky="nsew", padx=(8, 0), pady=(0, 8))

        panel_inner = ctk.CTkFrame(panel, fg_color="transparent")
        panel_inner.pack(fill="both", expand=True, padx=25, pady=25)

        # Server info
        ctk.CTkLabel(panel_inner, text="KONEKCIJA",
                    font=ctk.CTkFont(size=11, weight="bold"),
                    text_color=NEON_BLUE, fg_color="transparent",
                    anchor="w").pack(anchor="w", pady=(0, 8))

        conn_card = ctk.CTkFrame(panel_inner, fg_color=BG_INPUT, corner_radius=10,
                                  border_width=1, border_color="#1a2a4a")
        conn_card.pack(fill="x", pady=(0, 15))

        ctk.CTkLabel(conn_card, text=f"  {SERVER_IP}:{SERVER_PORT}",
                    font=ctk.CTkFont(family="Consolas", size=11),
                    text_color=NEON_BLUE, fg_color="transparent",
                    anchor="w").pack(anchor="w", padx=8, pady=(8, 2))
        ctk.CTkLabel(conn_card, text=f"  {SERVER_NAME}",
                    font=ctk.CTkFont(family="Consolas", size=10),
                    text_color=TEXT_DIM, fg_color="transparent",
                    anchor="w").pack(anchor="w", padx=8, pady=(0, 8))

        # GTA Path
        ctk.CTkLabel(panel_inner, text="GTA SAN ANDREAS",
                    font=ctk.CTkFont(size=11, weight="bold"),
                    text_color=NEON_PURPLE, fg_color="transparent",
                    anchor="w").pack(anchor="w", pady=(5, 8))

        path_card = ctk.CTkFrame(panel_inner, fg_color=BG_INPUT, corner_radius=10,
                                  border_width=1, border_color="#1a2a4a")
        path_card.pack(fill="x", pady=(0, 5))

        path_text = self.gta_path if self.gta_path else "Nije pronadjen!"
        path_color = NEON_GREEN if self.gta_path else NEON_RED

        self.path_label = ctk.CTkLabel(path_card, text=path_text,
                    font=ctk.CTkFont(family="Consolas", size=9),
                    text_color=path_color, fg_color="transparent",
                    anchor="w", wraplength=250)
        self.path_label.pack(anchor="w", padx=10, pady=8)

        ctk.CTkButton(panel_inner, text="BROWSE", width=80, height=28,
                      fg_color="#1a1a2e", hover_color="#2a2a3e",
                      text_color=NEON_BLUE, font=ctk.CTkFont(size=10, weight="bold"),
                      corner_radius=8, border_width=1, border_color=NEON_BLUE,
                      command=self.browse_gta_path).pack(anchor="w", pady=(0, 15))

        # Progress bar
        self.progress_bar = AnimatedProgressBar(panel_inner, height=6, color=NEON_BLUE)
        self.progress_bar.pack(fill="x", pady=(0, 5))

        self.progress_label = ctk.CTkLabel(panel_inner, text="",
                    font=ctk.CTkFont(size=10),
                    text_color=NEON_BLUE, fg_color="transparent", anchor="w")
        self.progress_label.pack(anchor="w")

        self.download_detail = ctk.CTkLabel(panel_inner, text="",
                    font=ctk.CTkFont(size=9),
                    text_color=TEXT_DIM, fg_color="transparent", anchor="w")
        self.download_detail.pack(anchor="w")

        # Spacer
        ctk.CTkFrame(panel_inner, fg_color="transparent").pack(fill="both", expand=True)

        # Auto-install dugme
        self.install_btn = NeonButton(
            panel_inner, text="AUTO-INSTALACIJA", height=42,
            font=ctk.CTkFont(family="Segoe UI", size=13, weight="bold"),
            fg_color="#1a1a2e", hover_color=NEON_PURPLE,
            text_color=NEON_PURPLE, corner_radius=10,
            border_width=1, border_color=NEON_PURPLE,
            glow_color=NEON_PURPLE,
            command=self.auto_install
        )
        if not self.install_status['ready_to_play']:
            self.install_btn.pack(fill="x", pady=(0, 8), side="bottom")

        # LAUNCH DUGME - veliko, neon, pulse
        self.launch_neon_btn = NeonButton(
            panel_inner, text="▶  LAUNCH  ▶", height=65,
            font=ctk.CTkFont(family="Segoe UI", size=24, weight="bold"),
            fg_color="#1a1a2e", hover_color=NEON_BLUE,
            text_color=NEON_BLUE, corner_radius=14,
            border_width=2, border_color=NEON_BLUE,
            glow_color=NEON_BLUE, pulse=True,
            command=self.launch_game
        )
        self.launch_neon_btn.pack(fill="x", pady=(0, 5), side="bottom")

        self.launch_status = ctk.CTkLabel(panel_inner, text="",
                                          font=ctk.CTkFont(size=10),
                                          text_color=TEXT_DIM,
                                          fg_color="transparent")
        self.launch_status.pack(pady=(0, 5), side="bottom")

    def _build_news_panel(self, parent):
        """Novosti panel"""
        panel = ctk.CTkFrame(parent, fg_color="#0a0f1a", corner_radius=16,
                             border_width=1, border_color="#1a2a4a")
        panel.grid(row=1, column=0, sticky="nsew", padx=(0, 8), pady=(8, 0))

        panel_inner = ctk.CTkFrame(panel, fg_color="transparent")
        panel_inner.pack(fill="both", expand=True, padx=20, pady=15)

        ctk.CTkLabel(panel_inner, text="NOVOSTI",
                    font=ctk.CTkFont(size=12, weight="bold"),
                    text_color=NEON_CYAN, fg_color="transparent",
                    anchor="w").pack(anchor="w", pady=(0, 10))

        news_items = [
            ("open.mp Server Aktivan", "Server je sad na open.mp platformi sa ugradjenim CEF-om! Tablet, inventar i laptop rade bez dodatnih pluginova.", NEON_BLUE),
            ("Auto-Instalacija", "Launcher automatski skida i instalira sve sto trebas - CEF, ASI loader, Chromium runtime. Samo klikni LAUNCH!", NEON_PURPLE),
            ("Bounty Sistem", "Postavljanje nagrada za igrace putem tableta ili laptopa. Koristi /bounty komandu.", NEON_PINK),
        ]

        for title, desc, color in news_items:
            card = ctk.CTkFrame(panel_inner, fg_color="#0d1520", corner_radius=10,
                               border_width=1, border_color="#1a2a4a")
            card.pack(fill="x", pady=3)

            inner = ctk.CTkFrame(card, fg_color="transparent")
            inner.pack(fill="both", expand=True, padx=12, pady=8)

            # Color bar
            ctk.CTkFrame(inner, fg_color=color, width=3, corner_radius=2).pack(
                side="left", fill="y", padx=(0, 10), pady=2)

            text_col = ctk.CTkFrame(inner, fg_color="transparent")
            text_col.pack(side="left", fill="both", expand=True)

            ctk.CTkLabel(text_col, text=title,
                        font=ctk.CTkFont(size=11, weight="bold"),
                        text_color=TEXT_BRIGHT, fg_color="transparent",
                        anchor="w").pack(anchor="w")
            ctk.CTkLabel(text_col, text=desc,
                        font=ctk.CTkFont(size=9),
                        text_color=TEXT_DIM, fg_color="transparent",
                        anchor="w", wraplength=500).pack(anchor="w")

    def _build_status_panel(self, parent):
        """Status panel - komponente i linkovi"""
        panel = ctk.CTkFrame(parent, fg_color="#0a0f1a", corner_radius=16,
                             border_width=1, border_color="#1a2a4a")
        panel.grid(row=1, column=1, sticky="nsew", padx=(8, 0), pady=(8, 0))

        panel_inner = ctk.CTkFrame(panel, fg_color="transparent")
        panel_inner.pack(fill="both", expand=True, padx=20, pady=15)

        ctk.CTkLabel(panel_inner, text="KOMPONENTE",
                    font=ctk.CTkFont(size=12, weight="bold"),
                    text_color=NEON_GREEN, fg_color="transparent",
                    anchor="w").pack(anchor="w", pady=(0, 10))

        # Status komponenti
        components = [
            ("SA-MP Client", self.install_status['has_samp']),
            ("ASI Loader", self.install_status['has_asi_loader']),
            ("CEF Plugin", self.install_status['cef_installed']),
            ("Chromium RT", self.gta_path and os.path.exists(os.path.join(self.gta_path, "cef"))),
        ]

        self.comp_dots = []
        self.comp_labels = []
        for name, installed in components:
            row = ctk.CTkFrame(panel_inner, fg_color="transparent")
            row.pack(fill="x", pady=2)

            dot = PulseDot(row, size=8, color=NEON_GREEN if installed else NEON_ORANGE)
            dot.pack(side="left", padx=(0, 8))
            self.comp_dots.append((dot, installed))

            lbl = ctk.CTkLabel(row, text=name + (" ✓" if installed else " ✗"),
                        font=ctk.CTkFont(size=11),
                        text_color=NEON_GREEN if installed else NEON_ORANGE,
                        fg_color="transparent")
            lbl.pack(side="left")
            self.comp_labels.append((lbl, name))

        # Separator
        ctk.CTkFrame(panel_inner, height=1, fg_color="#1a2a4a").pack(fill="x", pady=12)

        # Quick links
        ctk.CTkLabel(panel_inner, text="BRZE VEZE",
                    font=ctk.CTkFont(size=11, weight="bold"),
                    text_color=TEXT_DIM, fg_color="transparent",
                    anchor="w").pack(anchor="w", pady=(0, 8))

        links_frame = ctk.CTkFrame(panel_inner, fg_color="transparent")
        links_frame.pack(fill="x")

        for name, url, color in [("Website", WEBSITE_URL, NEON_BLUE),
                                  ("Discord", DISCORD_URL, NEON_PURPLE)]:
            link_btn = NeonButton(
                links_frame, text=name, height=30, width=110,
                font=ctk.CTkFont(size=10, weight="bold"),
                fg_color="#0d1520", hover_color=color,
                text_color=color, corner_radius=8,
                border_width=1, border_color=color,
                glow_color=color,
                command=lambda u=url: webbrowser.open(u)
            )
            link_btn.pack(side="left", padx=(0, 8))

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
            self.path_label.configure(text=self.gta_path, text_color=NEON_GREEN)
            save_settings(self.gta_path)
            self.refresh_install_status()

    def refresh_install_status(self):
        self.install_status = get_install_status(self.gta_path)
        components = [
            self.install_status['has_samp'],
            self.install_status['has_asi_loader'],
            self.install_status['cef_installed'],
            self.gta_path and os.path.exists(os.path.join(self.gta_path, "cef")),
        ]
        for (dot, _), installed in zip(self.comp_dots, components):
            dot.set_color(NEON_GREEN if installed else NEON_ORANGE)
        for (lbl, name), installed in zip(self.comp_labels, components):
            lbl.configure(text=name + (" ✓" if installed else " ✗"),
                         text_color=NEON_GREEN if installed else NEON_ORANGE)

        if self.install_status['ready_to_play']:
            self.install_btn.pack_forget()
            self.progress_bar.set_progress(100)
            self.progress_label.configure(text="Sve spremno!", text_color=NEON_GREEN)
        else:
            self.install_btn.pack(fill="x", pady=(0, 8), side="bottom")

    # =========================================================================
    #  AUTO-INSTALL
    # =========================================================================
    def auto_install(self):
        if self.is_installing or not self.gta_path:
            if not self.gta_path:
                messagebox.showerror("Greska",
                    "GTA San Andreas nije pronadjen!\nKlikni Browse dugme.")
            return

        if not HAS_REQUESTS:
            messagebox.showerror("Greska", "Pokreni: pip install requests")
            return

        self.is_installing = True
        self.install_btn.configure(text="INSTALIRAM...", state="disabled")
        self.launch_neon_btn.stop_pulse()
        self.launch_neon_btn.configure(state="disabled")
        self.progress_bar.reset()
        self.progress_label.configure(text="Zapocinjem instalaciju...", text_color=NEON_BLUE)

        self.download_manager = DownloadManager(
            on_progress=self._on_progress,
            on_status=self._on_status,
            on_complete=self._on_complete,
            on_error=self._on_error
        )

        threading.Thread(
            target=self.download_manager.install_all,
            args=(self.gta_path, self.install_status['missing']),
            daemon=True
        ).start()

    def _on_progress(self, pct, dl_mb, total_mb, speed):
        self.root.after(0, lambda: self.progress_bar.set_progress(pct))
        if total_mb > 0:
            self.root.after(0, lambda: self.download_detail.configure(
                text=f"{dl_mb:.1f}/{total_mb:.1f} MB | {speed:.0f} KB/s"))
        elif dl_mb > 0:
            self.root.after(0, lambda: self.download_detail.configure(
                text=f"{dl_mb:.1f} MB skinuto"))

    def _on_status(self, text):
        self.root.after(0, lambda: self.progress_label.configure(
            text=text, text_color=NEON_BLUE))

    def _on_complete(self, component):
        self.root.after(0, self._handle_install_done, component)

    def _handle_install_done(self, component):
        if component == "all":
            self.is_installing = False
            self.install_btn.configure(text="AUTO-INSTALACIJA", state="normal")
            self.launch_neon_btn.configure(state="normal")
            self.launch_neon_btn.start_pulse()
            self.progress_label.configure(text="Sve instalirano!", text_color=NEON_GREEN)
            self.progress_bar.set_progress(100)
            self.refresh_install_status()
            save_settings(self.gta_path, omp_installed=True, cef_installed=True)

    def _on_error(self, msg):
        self.root.after(0, lambda: self._handle_install_err(msg))

    def _handle_install_err(self, msg):
        self.is_installing = False
        self.install_btn.configure(text="AUTO-INSTALACIJA", state="normal")
        self.launch_neon_btn.configure(state="normal")
        self.launch_neon_btn.start_pulse()
        self.progress_label.configure(text="Greska!", text_color=NEON_RED)
        messagebox.showerror("Greska", f"Instalacija neuspjesna:\n\n{msg}")

    # =========================================================================
    #  SERVER STATUS
    # =========================================================================
    def check_server_status(self):
        threading.Thread(target=self._do_check, daemon=True).start()

    def _do_check(self):
        info = query_samp_server(SERVER_IP, SERVER_PORT)
        self.server_info = info
        self.root.after(0, self._update_status, info)

    def _update_status(self, info):
        if info and info.get('online'):
            self.status_dot.set_color(NEON_GREEN)
            self.status_dot_nav.set_color(NEON_GREEN)
            self.status_label.configure(
                text=f"Server Online  |  {info.get('name', SERVER_NAME)}",
                text_color=NEON_GREEN)
            self.player_label.configure(
                text=f"Igraci: {info.get('players', 0)} / {info.get('max_players', 0)}")
            self.gamemode_label.configure(
                text=f"Gamemode: {info.get('gamemode', 'N/A')}")
            self.online_count_nav.configure(
                text=f"{info.get('players', 0)}/{info.get('max_players', 0)} Online",
                text_color=NEON_GREEN)
        else:
            self.status_dot.set_color(NEON_RED)
            self.status_dot_nav.set_color(NEON_RED)
            self.status_label.configure(text="Server Offline", text_color=NEON_RED)
            self.player_label.configure(text="Server nije dostupan")
            self.gamemode_label.configure(text="")
            self.online_count_nav.configure(text="Offline", text_color=NEON_RED)

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
                "GTA San Andreas nije pronadjen!\nKlikni Browse dugme.")
            return

        gta_exe = os.path.join(self.gta_path, "gta_sa.exe")
        if not os.path.exists(gta_exe):
            messagebox.showerror("Greska", f"gta_sa.exe nije pronadjen u:\n{self.gta_path}")
            return

        samp_exe = find_samp_exe(self.gta_path)
        if not samp_exe:
            messagebox.showerror("Greska",
                "SA-MP client nije pronadjen!\nInstaliraj SA-MP client (0.3.7-R4).")
            return

        # CEF warning
        cef_ok, cef_msg = check_cef_plugin(self.gta_path)
        if not cef_ok:
            result = messagebox.askyesno("CEF",
                f"CEF Plugin nije instaliran!\n\n{cef_msg}\n\n"
                "Bez CEF-a necete moci koristiti Tablet/Inventar/Laptop.\n\n"
                "Pokrenuti AUTO-INSTALACIJU?")
            if result:
                self.auto_install()
                return

        self.is_launching = True
        self.launch_neon_btn.stop_pulse()
        self.launch_neon_btn.configure(text="POKRECEM...", fg_color=NEON_BLUE,
                                       text_color=BG_DARK, state="disabled")
        self.launch_status.configure(text="Konektujem se...", text_color=NEON_BLUE)

        threading.Thread(target=self._do_launch, args=(samp_exe,), daemon=True).start()

    def _do_launch(self, samp_exe):
        try:
            connect_str = f"{SERVER_IP}:{SERVER_PORT}"
            self.root.after(0, lambda: self.launch_status.configure(
                text=f"Pokrecem {connect_str}...", text_color=NEON_BLUE))
            subprocess.Popen([samp_exe, connect_str], cwd=self.gta_path)
            time.sleep(2)
            self.root.after(0, lambda: self.launch_status.configure(
                text="Igra pokrenuta! Uzivaj!", text_color=NEON_GREEN))
            time.sleep(5)
            self.root.after(0, self.root.destroy)
        except Exception as e:
            self.root.after(0, lambda: self._launch_err(str(e)))

    def _launch_err(self, msg):
        self.is_launching = False
        self.launch_neon_btn.configure(text="▶  LAUNCH  ▶", fg_color="#1a1a2e",
                                       text_color=NEON_BLUE, state="normal")
        self.launch_neon_btn.start_pulse()
        self.launch_status.configure(text="", text_color=TEXT_DIM)
        messagebox.showerror("Greska", f"Nije moguce pokrenuti igru!\n\n{msg}")

    # =========================================================================
    #  CLOSE
    # =========================================================================
    def on_close(self):
        if self.download_manager:
            self.download_manager.cancel()
        self.particle_canvas.stop_particles()
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
