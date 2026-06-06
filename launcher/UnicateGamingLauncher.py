#!/usr/bin/env python3
# =============================================================================
#  UNICATE GAMING LAUNCHER
#  Modern Dark Gaming Portal | Sidebar Layout | Neon Blue Theme
#
#  Instalacija:
#    pip install customtkinter pillow requests
#
#  Kompajliranje u .exe:
#    pyinstaller --onefile --windowed --icon=ug_icon.ico --name="UnicateGaming" UnicateGamingLauncher.py
# =============================================================================

import customtkinter as ctk
import os, sys, subprocess, threading, time, struct, socket, webbrowser
import json, zipfile, math, random
from tkinter import filedialog, messagebox

try:
    from PIL import Image, ImageTk, ImageDraw, ImageFilter, ImageEnhance
    HAS_PIL = True
except ImportError:
    HAS_PIL = False

try:
    import requests
    HAS_REQUESTS = True
except ImportError:
    HAS_REQUESTS = False

# =============================================================================
#  KONFIGURACIJA
# =============================================================================
SERVER_IP = "135.125.156.197"
SERVER_PORT = 7777
SERVER_NAME = "Unicate Gaming RPG"
WEBSITE_URL = "https://ug-ogc.com"
DISCORD_URL = "https://discord.gg/unicategaming"
LAUNCHER_VERSION = "2.0.0"

OMP_CEF_ASI_URL = "https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/cef.asi"
OMP_CEF_CLIENT_URL = "https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/client-files-v1.2.0.zip"
ASI_LOADER_URL = "https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/v4.76/dsound.zip"

if getattr(sys, 'frozen', False):
    LAUNCHER_DIR = os.path.dirname(sys.executable)
else:
    LAUNCHER_DIR = os.path.dirname(os.path.abspath(__file__))

# =============================================================================
#  COLOR PALETTE
# =============================================================================
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

# Primary
C_BG         = "#0b0f1a"
C_BG_SIDE    = "#0d1220"
C_BG_CARD    = "#111827"
C_BG_INPUT   = "#0e1528"
C_BG_HOVER   = "#162040"
C_BG_ACTIVE  = "#0c2d5e"

# Accent Blues
C_BLUE       = "#0099ff"
C_NEON       = "#00d4ff"
C_BLUE_DIM   = "#1e40af"
C_BLUE_DARK  = "#0a1e3d"
C_BLUE_700   = "#1d4ed8"

# Status
C_GREEN      = "#22c55e"
C_RED        = "#ef4444"
C_ORANGE     = "#f59e0b"
C_PURPLE     = "#a855f7"

# Text
C_WHITE      = "#f8fafc"
C_LIGHT      = "#cbd5e1"
C_DIM        = "#64748b"
C_DARKER     = "#475569"

# =============================================================================
#  UTILITY FUNCTIONS
# =============================================================================
def query_samp_server(ip, port, timeout=3):
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.settimeout(timeout)
        pkt = b'SAMP' + struct.pack('4B', *[int(x) for x in ip.split('.')]) + struct.pack('H', port) + b'i'
        sock.sendto(pkt, (ip, port))
        data, _ = sock.recvfrom(2048)
        sock.close()
        if len(data) < 11: return None
        off = 11
        pwlen = struct.unpack_from('H', data, off)[0]; off += 2 + pwlen
        players = struct.unpack_from('H', data, off)[0]; off += 2
        maxp = struct.unpack_from('H', data, off)[0]; off += 2
        nlen = struct.unpack_from('I', data, off)[0]; off += 4
        name = data[off:off+nlen].decode('latin-1', errors='replace'); off += nlen
        mlen = struct.unpack_from('I', data, off)[0]; off += 4
        mode = data[off:off+mlen].decode('latin-1', errors='replace')
        return {'players': players, 'max_players': maxp, 'name': name, 'gamemode': mode, 'online': True}
    except:
        return {'online': False}

def find_gta_sa_path():
    try:
        import winreg
        k = winreg.OpenKey(winreg.HKEY_CURRENT_USER, r"Software\SAMP")
        p, _ = winreg.QueryValueEx(k, "gta_sa_exe")
        winreg.CloseKey(k)
        if p and os.path.exists(p): return os.path.dirname(p)
    except: pass
    for p in [r"C:\Program Files (x86)\Rockstar Games\GTA San Andreas",
              r"D:\GTA San Andreas", r"D:\GTA SA", r"D:\Games\GTA SA",
              r"E:\GTA San Andreas", r"E:\GTA SA"]:
        if os.path.exists(os.path.join(p, "gta_sa.exe")): return p
    return None

def find_samp_exe(gta_path):
    if not gta_path: return None
    p = os.path.join(gta_path, "samp.exe")
    return p if os.path.exists(p) else None

def check_cef(gta_path):
    if not gta_path: return False, "Nije pronadjen"
    asi = os.path.exists(os.path.join(gta_path, "cef.asi"))
    fld = os.path.exists(os.path.join(gta_path, "cef"))
    if asi and fld: return True, "CEF OK"
    if asi: return False, "Fali cef/ folder"
    if fld: return False, "Fali cef.asi"
    return False, "Nije instaliran"

def check_asi_loader(gta_path):
    if not gta_path: return False
    return os.path.exists(os.path.join(gta_path, "dsound.dll"))

def get_status(gta_path):
    s = {'gta_path': gta_path, 'has_samp': bool(find_samp_exe(gta_path)),
         'cef_ok': False, 'cef_msg': '-', 'has_asi': False, 'ready': False, 'missing': []}
    if gta_path:
        ok, msg = check_cef(gta_path)
        s['cef_ok'], s['cef_msg'] = ok, msg
        s['has_asi'] = check_asi_loader(gta_path)
        if not s['has_samp']: s['missing'].append('client')
        if not s['cef_ok']:
            if not os.path.exists(os.path.join(gta_path, "cef.asi")): s['missing'].append('cef_asi')
            if not os.path.exists(os.path.join(gta_path, "cef")): s['missing'].append('cef_runtime')
        if not s['has_asi']: s['missing'].append('asi_loader')
        s['ready'] = s['has_samp'] and s['cef_ok'] and s['has_asi']
    return s

def save_settings(gta_path, **kw):
    d = {"gta_path": gta_path}; d.update(kw)
    try:
        with open(os.path.join(LAUNCHER_DIR, "settings.json"), 'w') as f: json.dump(d, f)
    except: pass

def load_settings():
    try:
        sf = os.path.join(LAUNCHER_DIR, "settings.json")
        if os.path.exists(sf):
            with open(sf, 'r') as f: return json.load(f)
    except: pass
    return {}

# =============================================================================
#  DOWNLOAD MANAGER
# =============================================================================
class DownloadManager:
    def __init__(self, on_progress=None, on_status=None, on_complete=None, on_error=None):
        self.on_progress = on_progress
        self.on_status = on_status
        self.on_complete = on_complete
        self.on_error = on_error
        self.cancelled = False

    def cancel(self): self.cancelled = True

    def _dl(self, url, dest, name="file"):
        try:
            self.on_status(f"Skidam {name}...")
            r = requests.get(url, stream=True, timeout=30); r.raise_for_status()
            total = int(r.headers.get('content-length', 0)); dl = 0; t0 = time.time()
            with open(dest, 'wb') as f:
                for chunk in r.iter_content(8192):
                    if self.cancelled: return False
                    f.write(chunk); dl += len(chunk)
                    if total > 0:
                        p = (dl/total)*100; sp = (dl/1024)/(time.time()-t0) if time.time()-t0 > 0 else 0
                        self.on_progress(p, dl/(1024*1024), total/(1024*1024), sp)
            return True
        except Exception as e:
            self.on_error(str(e)); return False

    def install_all(self, gta_path, missing):
        try:
            for c in missing:
                if self.cancelled: break
                if c == 'asi_loader':
                    dest = os.path.join(LAUNCHER_DIR, "tmp_asi.zip")
                    if not self._dl(ASI_LOADER_URL, dest, "ASI Loader"): continue
                    self.on_status("Instaliram ASI Loader...")
                    with zipfile.ZipFile(dest, 'r') as zf:
                        for n in zf.namelist():
                            if n.lower().endswith('dsound.dll'):
                                with zf.open(n) as s, open(os.path.join(gta_path, "dsound.dll"), 'wb') as d: d.write(s.read())
                                break
                    try: os.remove(dest)
                    except: pass
                elif c == 'cef_asi':
                    self._dl(OMP_CEF_ASI_URL, os.path.join(gta_path, "cef.asi"), "cef.asi")
                elif c == 'cef_runtime':
                    dest = os.path.join(LAUNCHER_DIR, "tmp_cef.zip")
                    if not self._dl(OMP_CEF_CLIENT_URL, dest, "CEF Runtime (~296MB)"): continue
                    self.on_status("Raspakujem CEF Runtime...")
                    with zipfile.ZipFile(dest, 'r') as zf:
                        tot = len(zf.namelist())
                        for i, n in enumerate(zf.namelist()):
                            if self.cancelled: break
                            if n.endswith('/'): continue
                            t = os.path.join(gta_path, n); os.makedirs(os.path.dirname(t), exist_ok=True)
                            with zf.open(n) as s, open(t, 'wb') as d: d.write(s.read())
                            if i % 20 == 0: self.on_progress((i/tot)*100, 0, 0, 0)
                    try: os.remove(dest)
                    except: pass
            if not self.cancelled:
                self.on_status("Gotovo!")
                self.on_complete("all")
        except Exception as e:
            self.on_error(str(e))

# =============================================================================
#  ANIMATED BACKGROUND
# =============================================================================
class AnimatedBg(ctk.CTkCanvas):
    def __init__(self, master, **kw):
        super().__init__(master, bg=C_BG, highlightthickness=0, **kw)
        self._run = False
        self._bg_photo = None
        self._particles = []
        self._phase = 0

    def set_bg(self, path):
        try:
            if os.path.exists(path) and HAS_PIL:
                img = Image.open(path)
                img = img.resize((self.winfo_screenwidth(), self.winfo_screenheight()), Image.LANCZOS)
                img = ImageEnhance.Brightness(img).enhance(0.2)
                img = img.filter(ImageFilter.GaussianBlur(4))
                overlay = Image.new('RGBA', img.size, (0, 30, 100, 50))
                img = img.convert('RGBA'); img = Image.alpha_composite(img, overlay)
                self._bg_photo = ImageTk.PhotoImage(img.convert('RGB'))
        except: pass

    def start(self):
        self.update_idletasks()
        self._run = True; self._anim()

    def stop(self): self._run = False

    def _anim(self):
        if not self._run: return
        self.delete("all")
        w = self.winfo_width(); h = self.winfo_height()
        if self._bg_photo:
            self.create_image(0, 0, image=self._bg_photo, anchor="nw")
        # Subtle grid
        for x in range(0, max(w, 1920), 80):
            self.create_line(x, 0, x, h, fill="#060d18", width=1)
        for y in range(0, max(h, 1080), 80):
            self.create_line(0, y, w, y, fill="#060d18", width=1)
        # Particles
        self._phase += 0.03
        while len(self._particles) < 30:
            self._particles.append({
                'x': random.randint(0, max(w, 1920)),
                'y': random.randint(0, max(h, 1080)),
                'vx': random.uniform(-0.15, 0.15),
                'vy': random.uniform(-0.3, -0.05),
                's': random.uniform(1, 3),
                'life': random.randint(200, 600)
            })
        alive = []
        for p in self._particles:
            p['x'] += p['vx']; p['y'] += p['vy']; p['life'] -= 1
            if p['life'] > 0 and 0 <= p['x'] <= w and 0 <= p['y'] <= h:
                a = min(1.0, p['life'] / 300)
                v = int(40 * a)
                c = f"#{0:02x}{min(255,80+v):02x}{min(255,160+v):02x}"
                s = p['s'] * a
                if s > 0.5:
                    self.create_oval(p['x']-s, p['y']-s, p['x']+s, p['y']+s, fill=c, outline="")
                alive.append(p)
        self._particles = alive
        self.after(40, self._anim)

# =============================================================================
#  CUSTOM WIDGETS
# =============================================================================
class SidebarButton(ctk.CTkFrame):
    """Stavka u sidebar navigaciji"""
    def __init__(self, master, text="", icon="", active=False, command=None, **kw):
        super().__init__(master, fg_color=C_BG_ACTIVE if active else "transparent",
                         corner_radius=8, height=38, cursor="hand2", **kw)
        self._cmd = command
        self._active = active
        self._text = text
        self.pack_propagate(False)

        inner = ctk.CTkFrame(self, fg_color="transparent")
        inner.pack(fill="both", expand=True, padx=10)

        color = C_NEON if active else C_DIM
        if icon:
            ctk.CTkLabel(inner, text=icon, font=ctk.CTkFont(size=14),
                        text_color=color, fg_color="transparent").pack(side="left", padx=(0, 10))
        ctk.CTkLabel(inner, text=text, font=ctk.CTkFont(size=12, weight="bold" if active else "normal"),
                    text_color=color, fg_color="transparent", anchor="w").pack(side="left")

        self.bind("<Button-1>", lambda e: self._click())
        for w in [self, inner] + list(inner.winfo_children()):
            try: w.bind("<Button-1>", lambda e: self._click())
            except: pass

        self.bind("<Enter>", lambda e: self._hover(True))
        self.bind("<Leave>", lambda e: self._hover(False))

    def _hover(self, enter):
        if not self._active:
            self.configure(fg_color=C_BG_HOVER if enter else "transparent")

    def _click(self):
        if self._cmd: self._cmd()

    def set_active(self, active):
        self._active = active
        self.configure(fg_color=C_BG_ACTIVE if active else "transparent")


class GlowButton(ctk.CTkButton):
    """Dugme sa neon glow efektom"""
    def __init__(self, master, glow_color=C_BLUE, **kw):
        self._glow = glow_color
        self._pulse = kw.pop('pulse', False)
        self._phase = 0; self._running = False
        super().__init__(master, **kw)
        self.bind("<Enter>", lambda e: self.configure(fg_color=self._glow, text_color=C_BG))
        self.bind("<Leave>", lambda e: self.configure(fg_color=C_BG_CARD, text_color=self._glow) if not self._running else None)

    def start_pulse(self):
        self._running = True; self._do_pulse()

    def stop_pulse(self): self._running = False

    def _do_pulse(self):
        if not self._running: return
        self._phase += 0.05
        i = (math.sin(self._phase) + 1) / 2
        r1, g1, b1 = 17, 24, 39
        r2 = int(self._glow[1:3], 16); g2 = int(self._glow[3:5], 16); b2 = int(self._glow[5:7], 16)
        r = int(r1 + (r2 - r1) * i * 0.5); g = int(g1 + (g2 - g1) * i * 0.5); b = int(b1 + (b2 - b1) * i * 0.5)
        self.configure(fg_color=f"#{r:02x}{g:02x}{b:02x}", text_color=C_WHITE)
        self.after(50, self._do_pulse)


class PulseDot(ctk.CTkCanvas):
    def __init__(self, master, sz=8, color=C_GREEN, **kw):
        super().__init__(master, width=sz+6, height=sz+6, bg=C_BG, highlightthickness=0, **kw)
        self.sz = sz; self.color = color; self._draw()

    def set_color(self, c): self.color = c; self._draw()

    def _draw(self):
        self.delete("all"); s = self.sz; cx = s//2+3; cy = s//2+3
        self.create_oval(cx-s//2-2, cy-s//2-2, cx+s//2+2, cy+s//2+2, fill="", outline=self.color, width=1)
        self.create_oval(cx-s//2, cy-s//2, cx+s//2, cy+s//2, fill=self.color, outline="")


class ProgressBar(ctk.CTkCanvas):
    def __init__(self, master, h=4, **kw):
        super().__init__(master, height=h, bg=C_BG, highlightthickness=0, **kw)
        self._pct = 0; self._anim = 0

    def set_progress(self, pct):
        self._pct = max(0, min(100, pct)); self._tick()

    def _tick(self):
        if abs(self._anim - self._pct) < 0.5: self._anim = self._pct; self._draw(); return
        self._anim += (self._pct - self._anim) * 0.12; self._draw(); self.after(16, self._tick)

    def _draw(self):
        self.delete("all"); w = self.winfo_width(); h = self.winfo_height()
        if w < 2: return
        self.create_rectangle(0, 0, w, h, fill="#0a1020", outline="")
        fw = (self._anim / 100) * w
        if fw > 0:
            self.create_rectangle(0, 0, fw, h, fill=C_BLUE, outline="")
            self.create_rectangle(max(0, fw - 8), 0, fw, h, fill=C_NEON, outline="")

    def reset(self): self._pct = 0; self._anim = 0; self._draw()


class NewsCard(ctk.CTkFrame):
    def __init__(self, master, title="", desc="", tag="", tag_color=C_BLUE, **kw):
        super().__init__(master, fg_color=C_BG_CARD, corner_radius=10,
                         border_width=1, border_color="#1a2540", **kw)
        inner = ctk.CTkFrame(self, fg_color="transparent")
        inner.pack(fill="both", expand=True, padx=12, pady=10)

        # Tag
        tf = ctk.CTkFrame(inner, fg_color=tag_color, corner_radius=4, height=20)
        tf.pack(anchor="w", pady=(0, 6)); tf.pack_propagate(False)
        ctk.CTkLabel(tf, text=tag, font=ctk.CTkFont(size=9, weight="bold"),
                    text_color=C_WHITE, fg_color="transparent").pack(padx=8)

        ctk.CTkLabel(inner, text=title, font=ctk.CTkFont(size=12, weight="bold"),
                    text_color=C_WHITE, fg_color="transparent", anchor="w", wraplength=280).pack(anchor="w")
        ctk.CTkLabel(inner, text=desc, font=ctk.CTkFont(size=10),
                    text_color=C_DIM, fg_color="transparent", anchor="w", wraplength=280).pack(anchor="w", pady=(2, 0))


# =============================================================================
#  MAIN LAUNCHER
# =============================================================================
class UnicateGamingLauncher:
    def __init__(self):
        self.root = ctk.CTk()
        self.root.title("Unicate Gaming Launcher")
        self.root.overrideredirect(True)

        # Ikona
        icon_ico = os.path.join(LAUNCHER_DIR, "ug_icon.ico")
        icon_png = os.path.join(LAUNCHER_DIR, "ug_logo.png")
        if os.path.exists(icon_ico):
            try: self.root.iconbitmap(icon_ico)
            except: pass
        elif os.path.exists(icon_png) and HAS_PIL:
            try: self.root.iconphoto(True, ImageTk.PhotoImage(Image.open(icon_png)))
            except: pass

        sw = self.root.winfo_screenwidth(); sh = self.root.winfo_screenheight()
        self.root.geometry(f"{sw}x{sh}+0+0")
        self.root.configure(fg_color=C_BG)

        settings = load_settings()
        self.gta_path = settings.get("gta_path", find_gta_sa_path())
        self.nickname = settings.get("nickname", "Unicate_Player")
        self.server_info = None
        self.is_launching = False; self.is_installing = False
        self.dl_mgr = None; self.status = get_status(self.gta_path)

        self.imgs = {}
        self._load_images()
        self.build_ui()
        self.bg.start()
        self.check_server()
        self._sched_server()

        self._drag = None
        self.root.bind("<ButtonPress-1>", self._drag_start)
        self.root.bind("<B1-Motion>", self._drag_move)
        self.root.bind("<Escape>", lambda e: self.on_close())

    def _load_images(self):
        try:
            logo = os.path.join(LAUNCHER_DIR, "ug_logo.png")
            if os.path.exists(logo) and HAS_PIL:
                img = Image.open(logo)
                self.imgs['logo_big'] = ctk.CTkImage(img, img, size=(120, 120))
                self.imgs['logo_med'] = ctk.CTkImage(img, img, size=(48, 48))
                self.imgs['logo_sm'] = ctk.CTkImage(img, img, size=(32, 32))
                # Glow
                glow = img.resize((140, 140), Image.LANCZOS)
                glow = glow.filter(ImageFilter.GaussianBlur(15))
                glow = ImageEnhance.Brightness(glow).enhance(2.5)
                self.imgs['logo_glow'] = ctk.CTkImage(glow, glow, size=(160, 160))

            bg = os.path.join(LAUNCHER_DIR, "bg_gta.png")
            self.imgs['bg_path'] = bg if os.path.exists(bg) else None
        except Exception as e:
            print(f"Img err: {e}")

    def _drag_start(self, e): self._drag = (e.x, e.y)
    def _drag_move(self, e):
        if self._drag and self._drag[1] < 50:
            self.root.geometry(f"+{self.root.winfo_x()+e.x-self._drag[0]}+{self.root.winfo_y()+e.y-self._drag[1]}")

    # ================================================================
    #  BUILD UI
    # ================================================================
    def build_ui(self):
        # Background
        self.bg = AnimatedBg(self.root)
        self.bg.place(x=0, y=0, relwidth=1, relheight=1)
        if self.imgs.get('bg_path'):
            self.bg.set_bg(self.imgs['bg_path'])

        # Main container
        main = ctk.CTkFrame(self.root, fg_color="transparent")
        main.place(relx=0.5, rely=0.5, anchor="center", relwidth=0.96, relheight=0.96)

        # Top bar
        self._build_topbar(main)

        # Body: sidebar + content + right panel
        body = ctk.CTkFrame(main, fg_color="transparent")
        body.pack(fill="both", expand=True, pady=(8, 0))

        self._build_sidebar(body)
        self._build_center(body)
        self._build_right(body)

        # Bottom bar
        self._build_bottom(main)

    # ----- TOP BAR -----
    def _build_topbar(self, parent):
        top = ctk.CTkFrame(parent, height=48, fg_color="#080c18", corner_radius=10,
                           border_width=1, border_color="#0f1a30")
        top.pack(fill="x"); top.pack_propagate(False)
        inner = ctk.CTkFrame(top, fg_color="transparent")
        inner.pack(fill="both", expand=True, padx=16, pady=6)

        # Logo + Name
        left = ctk.CTkFrame(inner, fg_color="transparent")
        left.pack(side="left", fill="y")

        if 'logo_sm' in self.imgs:
            ctk.CTkLabel(left, image=self.imgs['logo_sm'], text="", fg_color="transparent").pack(side="left", padx=(0, 10))

        ctk.CTkLabel(left, text="UNICATE", font=ctk.CTkFont(size=15, weight="bold"),
                    text_color=C_NEON, fg_color="transparent").pack(side="left")
        ctk.CTkLabel(left, text=" GAMING", font=ctk.CTkFont(size=15, weight="bold"),
                    text_color=C_WHITE, fg_color="transparent").pack(side="left")

        # Nav menu
        nav = ctk.CTkFrame(inner, fg_color="transparent")
        nav.pack(side="left", padx=30)
        for nm in ["POCETNA", "SERVER", "NOVOSTI", "SHOP"]:
            ctk.CTkLabel(nav, text=nm, font=ctk.CTkFont(size=11, weight="bold"),
                        text_color=C_NEON if nm == "POCETNA" else C_DIM,
                        fg_color="transparent", cursor="hand2").pack(side="left", padx=14)

        # Right side
        right = ctk.CTkFrame(inner, fg_color="transparent")
        right.pack(side="right", fill="y")

        # Server status badge
        badge = ctk.CTkFrame(right, fg_color="#0a0f1e", corner_radius=6, height=26)
        badge.pack(side="left", padx=(0, 10)); badge.pack_propagate(False)
        self.dot_top = PulseDot(badge, 6, C_ORANGE); self.dot_top.pack(side="left", padx=(6, 4))
        self.lbl_top_status = ctk.CTkLabel(badge, text="Provjera...", font=ctk.CTkFont(size=9, weight="bold"),
                                           text_color=C_DIM, fg_color="transparent")
        self.lbl_top_status.pack(side="left", padx=(0, 8))

        # Window controls
        ctk.CTkButton(right, text="-", width=28, height=24, fg_color="transparent", hover_color=C_BG_HOVER,
                      text_color=C_DIM, font=ctk.CTkFont(size=12), corner_radius=6,
                      command=self.root.iconify).pack(side="left", padx=2)
        ctk.CTkButton(right, text="X", width=28, height=24, fg_color="transparent", hover_color="#2a0a0a",
                      text_color=C_RED, font=ctk.CTkFont(size=11, weight="bold"), corner_radius=6,
                      command=self.on_close).pack(side="left", padx=2)

    # ----- LEFT SIDEBAR -----
    def _build_sidebar(self, parent):
        side = ctk.CTkFrame(parent, width=180, fg_color=C_BG_SIDE, corner_radius=10,
                            border_width=1, border_color="#0f1a30")
        side.pack(side="left", fill="y", padx=(0, 8)); side.pack_propagate(False)

        # Nav items
        nav_frame = ctk.CTkFrame(side, fg_color="transparent")
        nav_frame.pack(fill="both", expand=True, padx=8, pady=12)

        nav_items = [
            ("Pocetna", "H", True),
            ("Server Info", "S", False),
            ("Player Panel", "P", False),
            ("Novosti", "N", False),
            ("Podesavanja", "G", False),
            ("Addons", "A", False),
            ("Statistika", "T", False),
        ]
        for text, icon, active in nav_items:
            SidebarButton(nav_frame, text=text, icon=icon, active=active).pack(fill="x", pady=1)

        # Separator
        ctk.CTkFrame(nav_frame, height=1, fg_color="#1a2540").pack(fill="x", pady=10)

        # Social links
        ctk.CTkLabel(nav_frame, text="DRUSTVENE MREZE", font=ctk.CTkFont(size=9, weight="bold"),
                    text_color=C_DARKER, fg_color="transparent", anchor="w").pack(anchor="w", pady=(0, 6))

        social_frame = ctk.CTkFrame(nav_frame, fg_color="transparent")
        social_frame.pack(fill="x")
        socials = [
            ("Discord", C_BLUE, DISCORD_URL),
            ("Website", C_NEON, WEBSITE_URL),
        ]
        for name, color, url in socials:
            btn = ctk.CTkButton(social_frame, text=name, height=28, fg_color=C_BG_CARD,
                               hover_color=C_BG_HOVER, text_color=color,
                               font=ctk.CTkFont(size=10, weight="bold"), corner_radius=6,
                               border_width=1, border_color="#1a2540",
                               command=lambda u=url: webbrowser.open(u))
            btn.pack(fill="x", pady=2)

    # ----- CENTER CONTENT -----
    def _build_center(self, parent):
        center = ctk.CTkFrame(parent, fg_color="transparent")
        center.pack(side="left", fill="both", expand=True, padx=(0, 8))

        self._build_server_panel(center)
        self._build_news_panel(center)

    def _build_server_panel(self, parent):
        panel = ctk.CTkFrame(parent, fg_color=C_BG_CARD, corner_radius=12,
                             border_width=1, border_color="#1a2540")
        panel.pack(fill="x", pady=(0, 8))
        inner = ctk.CTkFrame(panel, fg_color="transparent")
        inner.pack(fill="both", expand=True, padx=20, pady=16)

        # Left: Logo + info
        left = ctk.CTkFrame(inner, fg_color="transparent")
        left.pack(side="left", padx=(0, 20))

        # Logo with glow
        if 'logo_big' in self.imgs:
            logo_frame = ctk.CTkFrame(left, fg_color="transparent", width=130, height=130)
            logo_frame.pack(side="left", padx=(0, 20)); logo_frame.pack_propagate(False)
            if 'logo_glow' in self.imgs:
                ctk.CTkLabel(logo_frame, image=self.imgs['logo_glow'], text="",
                            fg_color="transparent").place(relx=0.5, rely=0.5, anchor="center")
            ctk.CTkLabel(logo_frame, image=self.imgs['logo_big'], text="",
                        fg_color="transparent").place(relx=0.5, rely=0.5, anchor="center")

        info = ctk.CTkFrame(left, fg_color="transparent")
        info.pack(side="left", fill="both", expand=True)

        ctk.CTkLabel(info, text="UNICATE GAMING", font=ctk.CTkFont(size=26, weight="bold"),
                    text_color=C_WHITE, fg_color="transparent", anchor="w").pack(anchor="w")
        ctk.CTkLabel(info, text="SAMP SERVER", font=ctk.CTkFont(size=11),
                    text_color=C_BLUE, fg_color="transparent", anchor="w").pack(anchor="w", pady=(0, 8))

        # Server status row
        sr = ctk.CTkFrame(info, fg_color="transparent"); sr.pack(anchor="w", pady=2)
        self.dot_status = PulseDot(sr, 8, C_ORANGE); self.dot_status.pack(side="left", padx=(0, 8))
        self.lbl_status = ctk.CTkLabel(sr, text="Provjeravam...", font=ctk.CTkFont(size=11),
                                       text_color=C_DIM, fg_color="transparent")
        self.lbl_status.pack(side="left")

        # Stats grid
        stats = ctk.CTkFrame(info, fg_color="transparent"); stats.pack(anchor="w", pady=(8, 0))
        for i, (val, label) in enumerate([("0/1000", "Igraca Online"), ("0", "Rekord"),
                                           ("Los Santos", "Lokacija"), ("v1.0", "Mod Verzija")]):
            card = ctk.CTkFrame(stats, fg_color="#0a0f1e", corner_radius=8, width=100, height=50)
            card.pack(side="left", padx=(0 if i == 0 else 6, 0)); card.pack_propagate(False)
            ci = ctk.CTkFrame(card, fg_color="transparent"); ci.pack(expand=True)
            lbl_val = ctk.CTkLabel(ci, text=val, font=ctk.CTkFont(size=13, weight="bold"),
                                  text_color=C_NEON, fg_color="transparent")
            lbl_val.pack()
            ctk.CTkLabel(ci, text=label, font=ctk.CTkFont(size=8),
                        text_color=C_DIM, fg_color="transparent").pack()

        # Store references for updating
        self.stat_labels = stats.winfo_children()

        # Right: Connect button area
        right = ctk.CTkFrame(inner, fg_color="transparent")
        right.pack(side="right", fill="y")

        self.connect_btn = GlowButton(right, text="PRIKLJUCI SE", width=180, height=52,
                                      font=ctk.CTkFont(size=16, weight="bold"),
                                      fg_color=C_BG_CARD, hover_color=C_BLUE,
                                      text_color=C_NEON, corner_radius=12,
                                      border_width=2, border_color=C_BLUE,
                                      glow_color=C_BLUE, pulse=True,
                                      command=self.launch_game)
        self.connect_btn.pack(pady=(10, 4))

        ctk.CTkLabel(right, text="LAUNCH SAMP", font=ctk.CTkFont(size=9),
                    text_color=C_DIM, fg_color="transparent").pack()

    def _build_news_panel(self, parent):
        panel = ctk.CTkFrame(parent, fg_color=C_BG_CARD, corner_radius=12,
                             border_width=1, border_color="#1a2540")
        panel.pack(fill="both", expand=True)

        inner = ctk.CTkFrame(panel, fg_color="transparent")
        inner.pack(fill="both", expand=True, padx=16, pady=12)

        ctk.CTkLabel(inner, text="NOVOSTI", font=ctk.CTkFont(size=12, weight="bold"),
                    text_color=C_NEON, fg_color="transparent", anchor="w").pack(anchor="w", pady=(0, 8))

        # News cards
        cards_frame = ctk.CTkFrame(inner, fg_color="transparent")
        cards_frame.pack(fill="both", expand=True)

        news_data = [
            ("Dobrodosli na Unicate Gaming!", "Server je otvoren! Pridruzi se nasoj zajednici i uzivaj u RP iskustvu.", "OBAVIJEST", C_BLUE),
            ("Update v1.0 - Novi sistemi", "CEF tablet, inventar i laptop su sada dostupni. open.mp server sa ugradjenim CEF-om.", "UPDATE", C_PURPLE),
            ("Happy Hours - Dupli Respekti", "Svaki dan od 18-22h dobijas duplo respekata! Iskoristi priliku.", "EVENT", C_GREEN),
        ]
        for title, desc, tag, color in news_data:
            NewsCard(cards_frame, title=title, desc=desc, tag=tag, tag_color=color).pack(fill="x", pady=2)

    # ----- RIGHT PANEL -----
    def _build_right(self, parent):
        right = ctk.CTkFrame(parent, width=240, fg_color=C_BG_CARD, corner_radius=12,
                             border_width=1, border_color="#1a2540")
        right.pack(side="right", fill="y"); right.pack_propagate(False)

        inner = ctk.CTkFrame(right, fg_color="transparent")
        inner.pack(fill="both", expand=True, padx=14, pady=14)

        # KONEKCIJA
        ctk.CTkLabel(inner, text="KONEKCIJA", font=ctk.CTkFont(size=10, weight="bold"),
                    text_color=C_BLUE, fg_color="transparent", anchor="w").pack(anchor="w", pady=(0, 6))

        # IP
        ip_frame = ctk.CTkFrame(inner, fg_color="#0a0f1e", corner_radius=8)
        ip_frame.pack(fill="x", pady=(0, 4))
        ctk.CTkLabel(ip_frame, text="IP Adresa", font=ctk.CTkFont(size=8),
                    text_color=C_DIM, fg_color="transparent", anchor="w").pack(anchor="w", padx=8, pady=(6, 0))
        ip_row = ctk.CTkFrame(ip_frame, fg_color="transparent"); ip_row.pack(fill="x", padx=8, pady=(0, 6))
        self.ip_lbl = ctk.CTkLabel(ip_row, text=f"{SERVER_IP}:{SERVER_PORT}", font=ctk.CTkFont("Consolas", size=10, weight="bold"),
                                   text_color=C_NEON, fg_color="transparent", anchor="w")
        self.ip_lbl.pack(side="left")
        ctk.CTkLabel(ip_row, text="Kopiraj", font=ctk.CTkFont(size=8),
                    text_color=C_DARKER, fg_color="transparent", cursor="hand2").pack(side="right")
        # Bind copy
        self.ip_lbl.bind("<Button-1>", lambda e: self._copy_ip())

        # GTA Path
        path_frame = ctk.CTkFrame(inner, fg_color="#0a0f1e", corner_radius=8)
        path_frame.pack(fill="x", pady=(0, 4))
        ctk.CTkLabel(path_frame, text="GTA:SA Putanja", font=ctk.CTkFont(size=8),
                    text_color=C_DIM, fg_color="transparent", anchor="w").pack(anchor="w", padx=8, pady=(6, 0))
        path_row = ctk.CTkFrame(path_frame, fg_color="transparent"); path_row.pack(fill="x", padx=8, pady=(0, 6))
        pt = self.gta_path or "Nije pronadjen!"
        ptc = C_GREEN if self.gta_path else C_RED
        self.path_lbl = ctk.CTkLabel(path_row, text=pt, font=ctk.CTkFont("Consolas", size=8),
                                    text_color=ptc, fg_color="transparent", anchor="w", wraplength=160)
        self.path_lbl.pack(side="left", fill="x", expand=True)
        ctk.CTkButton(path_row, text="BROWSE", width=50, height=22, fg_color=C_BLUE_DARK, hover_color=C_BLUE,
                      text_color=C_NEON, font=ctk.CTkFont(size=8, weight="bold"), corner_radius=6,
                      command=self.browse).pack(side="right", padx=(4, 0))

        # Nickname
        nick_frame = ctk.CTkFrame(inner, fg_color="#0a0f1e", corner_radius=8)
        nick_frame.pack(fill="x", pady=(0, 8))
        ctk.CTkLabel(nick_frame, text="Nickname", font=ctk.CTkFont(size=8),
                    text_color=C_DIM, fg_color="transparent", anchor="w").pack(anchor="w", padx=8, pady=(6, 0))
        nick_row = ctk.CTkFrame(nick_frame, fg_color="transparent"); nick_row.pack(fill="x", padx=8, pady=(0, 6))
        self.nick_lbl = ctk.CTkLabel(nick_row, text=self.nickname, font=ctk.CTkFont(size=10, weight="bold"),
                                    text_color=C_WHITE, fg_color="transparent", anchor="w")
        self.nick_lbl.pack(side="left")

        # Separator
        ctk.CTkFrame(inner, height=1, fg_color="#1a2540").pack(fill="x", pady=6)

        # KOMPONENTE
        ctk.CTkLabel(inner, text="KOMPONENTE", font=ctk.CTkFont(size=10, weight="bold"),
                    text_color=C_BLUE, fg_color="transparent", anchor="w").pack(anchor="w", pady=(0, 6))

        comps = [
            ("SA-MP Client", self.status['has_samp']),
            ("ASI Loader", self.status['has_asi']),
            ("CEF Plugin", self.status['cef_ok']),
            ("Chromium RT", self.gta_path and os.path.exists(os.path.join(self.gta_path, "cef"))),
        ]
        self.comp_widgets = []
        for name, ok in comps:
            r = ctk.CTkFrame(inner, fg_color="transparent"); r.pack(fill="x", pady=1)
            d = PulseDot(r, 6, C_GREEN if ok else C_ORANGE); d.pack(side="left", padx=(0, 6))
            l = ctk.CTkLabel(r, text=name + (" OK" if ok else " X"), font=ctk.CTkFont(size=10),
                            text_color=C_GREEN if ok else C_ORANGE, fg_color="transparent")
            l.pack(side="left")
            self.comp_widgets.append((d, l, name))

        # Progress bar
        self.pbar = ProgressBar(inner, h=4); self.pbar.pack(fill="x", pady=(8, 2))
        self.prog_lbl = ctk.CTkLabel(inner, text="", font=ctk.CTkFont(size=9),
                                    text_color=C_NEON, fg_color="transparent", anchor="w")
        self.prog_lbl.pack(anchor="w")
        self.prog_det = ctk.CTkLabel(inner, text="", font=ctk.CTkFont(size=8),
                                    text_color=C_DIM, fg_color="transparent", anchor="w")
        self.prog_det.pack(anchor="w")

        # Install button
        self.install_btn = GlowButton(inner, text="AUTO-INSTALACIJA", height=32,
                                      font=ctk.CTkFont(size=10, weight="bold"),
                                      fg_color=C_BG_CARD, hover_color=C_BLUE,
                                      text_color=C_BLUE, corner_radius=8,
                                      border_width=1, border_color=C_BLUE_700,
                                      glow_color=C_BLUE,
                                      command=self.auto_install)
        if not self.status['ready']:
            self.install_btn.pack(fill="x", pady=(8, 0))

        # Separator
        ctk.CTkFrame(inner, height=1, fg_color="#1a2540").pack(fill="x", pady=8)

        # BRZE AKCIJE
        ctk.CTkLabel(inner, text="BRZE AKCIJE", font=ctk.CTkFont(size=10, weight="bold"),
                    text_color=C_BLUE, fg_color="transparent", anchor="w").pack(anchor="w", pady=(0, 6))

        actions = [
            ("Ocisti Chat", "F9"),
            ("Reconnect", "F5"),
            ("Screenshot", "F8"),
        ]
        for name, key in actions:
            r = ctk.CTkFrame(inner, fg_color="transparent"); r.pack(fill="x", pady=1)
            ctk.CTkLabel(r, text=name, font=ctk.CTkFont(size=10),
                        text_color=C_LIGHT, fg_color="transparent").pack(side="left")
            ctk.CTkLabel(r, text=key, font=ctk.CTkFont(size=9),
                        text_color=C_DIM, fg_color="transparent").pack(side="right")

    # ----- BOTTOM BAR -----
    def _build_bottom(self, parent):
        bottom = ctk.CTkFrame(parent, height=44, fg_color="#080c18", corner_radius=10,
                              border_width=1, border_color="#0f1a30")
        bottom.pack(fill="x", pady=(8, 0)); bottom.pack_propagate(False)
        inner = ctk.CTkFrame(bottom, fg_color="transparent")
        inner.pack(fill="both", expand=True, padx=16, pady=6)

        # Radio player
        radio = ctk.CTkFrame(inner, fg_color="transparent")
        radio.pack(side="left")
        ctk.CTkLabel(radio, text="Unicate Radio", font=ctk.CTkFont(size=10, weight="bold"),
                    text_color=C_WHITE, fg_color="transparent").pack(side="left", padx=(0, 4))
        ctk.CTkLabel(radio, text="Uzivo 24/7", font=ctk.CTkFont(size=9),
                    text_color=C_DIM, fg_color="transparent").pack(side="left")

        # Fake progress bar
        bar = ctk.CTkFrame(radio, fg_color="#0a1020", corner_radius=3, width=100, height=4)
        bar.pack(side="left", padx=10)

        # Play controls
        for icon in ["<<", "||", ">>"]:
            ctk.CTkLabel(radio, text=icon, font=ctk.CTkFont(size=10),
                        text_color=C_DIM, fg_color="transparent", cursor="hand2").pack(side="left", padx=4)

        # Right side info
        info = ctk.CTkFrame(inner, fg_color="transparent")
        info.pack(side="right")

        self.lbl_players_bottom = ctk.CTkLabel(info, text="0/1000 igraca", font=ctk.CTkFont(size=9),
                                               text_color=C_DIM, fg_color="transparent")
        self.lbl_players_bottom.pack(side="left", padx=(0, 12))

        ctk.CTkLabel(info, text=f"v{LAUNCHER_VERSION}", font=ctk.CTkFont(size=9),
                    text_color=C_DARKER, fg_color="transparent").pack(side="left", padx=(0, 6))
        ctk.CTkLabel(info, text="SAMP 0.3.7-R4", font=ctk.CTkFont(size=9),
                    text_color=C_DARKER, fg_color="transparent").pack(side="left")

    # ================================================================
    #  ACTIONS
    # ================================================================
    def _copy_ip(self):
        try:
            self.root.clipboard_clear()
            self.root.clipboard_append(f"{SERVER_IP}:{SERVER_PORT}")
        except: pass

    def browse(self):
        e = filedialog.askopenfilename(title="Pronadji gta_sa.exe",
            filetypes=[("GTA SA", "gta_sa.exe"), ("All", "*.*")])
        if e:
            self.gta_path = os.path.dirname(e)
            self.path_lbl.configure(text=self.gta_path, text_color=C_GREEN)
            save_settings(self.gta_path, nickname=self.nickname); self._refresh_status()

    def _refresh_status(self):
        self.status = get_status(self.gta_path)
        comps = [self.status['has_samp'], self.status['has_asi'], self.status['cef_ok'],
                 self.gta_path and os.path.exists(os.path.join(self.gta_path, "cef"))]
        for (d, l, n), ok in zip(self.comp_widgets, comps):
            d.set_color(C_GREEN if ok else C_ORANGE)
            l.configure(text=n + (" OK" if ok else " X"), text_color=C_GREEN if ok else C_ORANGE)
        if self.status['ready']:
            self.install_btn.pack_forget()
            self.pbar.set_progress(100)
            self.prog_lbl.configure(text="Sve spremno!", text_color=C_GREEN)

    def auto_install(self):
        if self.is_installing: return
        if not self.gta_path:
            messagebox.showerror("Greska", "GTA SA nije pronadjen! Klikni Browse.")
            return
        self.is_installing = True
        self.install_btn.configure(text="INSTALIRAM...", state="disabled")
        self.connect_btn.stop_pulse()
        self.connect_btn.configure(state="disabled")
        self.pbar.reset()
        self.prog_lbl.configure(text="Zapocinjem...", text_color=C_NEON)
        self.dl_mgr = DownloadManager(
            on_progress=lambda p, d, t, s: self.root.after(0, lambda: self._prog(p, d, t, s)),
            on_status=lambda t: self.root.after(0, lambda: self.prog_lbl.configure(text=t, text_color=C_NEON)),
            on_complete=lambda c: self.root.after(0, lambda: self._done()),
            on_error=lambda e: self.root.after(0, lambda: self._err(e)))
        threading.Thread(target=self.dl_mgr.install_all, args=(self.gta_path, self.status['missing']), daemon=True).start()

    def _prog(self, pct, dl, total, spd):
        self.pbar.set_progress(pct)
        if total > 0: self.prog_det.configure(text=f"{dl:.1f}/{total:.1f} MB | {spd:.0f} KB/s")
        elif dl > 0: self.prog_det.configure(text=f"{dl:.1f} MB")

    def _done(self):
        self.is_installing = False
        self.install_btn.configure(text="AUTO-INSTALACIJA", state="normal")
        self.connect_btn.configure(state="normal"); self.connect_btn.start_pulse()
        self.prog_lbl.configure(text="Sve instalirano!", text_color=C_GREEN)
        self.pbar.set_progress(100); self._refresh_status()
        save_settings(self.gta_path, nickname=self.nickname, cef_installed=True)

    def _err(self, msg):
        self.is_installing = False
        self.install_btn.configure(text="AUTO-INSTALACIJA", state="normal")
        self.connect_btn.configure(state="normal"); self.connect_btn.start_pulse()
        self.prog_lbl.configure(text="Greska!", text_color=C_RED)
        messagebox.showerror("Greska", f"Instalacija neuspjesna:\n\n{msg}")

    # ================================================================
    #  SERVER CHECK
    # ================================================================
    def check_server(self):
        threading.Thread(target=self._do_check, daemon=True).start()

    def _do_check(self):
        info = query_samp_server(SERVER_IP, SERVER_PORT)
        self.server_info = info; self.root.after(0, self._update_status, info)

    def _update_status(self, info):
        if info and info.get('online'):
            self.dot_status.set_color(C_GREEN); self.dot_top.set_color(C_GREEN)
            self.lbl_status.configure(text=f"Online | {info.get('name', SERVER_NAME)}", text_color=C_GREEN)
            self.lbl_top_status.configure(text=f"{info['players']}/{info['max_players']}", text_color=C_GREEN)
            self.lbl_players_bottom.configure(text=f"{info['players']}/{info['max_players']} igraca")
            # Update stat cards
            try:
                for i, child in enumerate(self.stat_labels):
                    for w in child.winfo_children():
                        if i == 0:
                            try: w.configure(text=f"{info['players']}/{info['max_players']}")
                            except: pass
                        elif i == 1:
                            try: w.configure(text=str(info.get('players', 0)))
                            except: pass
            except: pass
        else:
            self.dot_status.set_color(C_RED); self.dot_top.set_color(C_RED)
            self.lbl_status.configure(text="Server Offline", text_color=C_RED)
            self.lbl_top_status.configure(text="Offline", text_color=C_RED)
            self.lbl_players_bottom.configure(text="0/1000 igraca")

    def _sched_server(self):
        self.check_server(); self.root.after(30000, self._sched_server)

    # ================================================================
    #  LAUNCH
    # ================================================================
    def launch_game(self):
        if self.is_launching or self.is_installing: return
        if not self.gta_path:
            messagebox.showerror("Greska", "GTA SA nije pronadjen! Klikni Browse."); return
        if not os.path.exists(os.path.join(self.gta_path, "gta_sa.exe")):
            messagebox.showerror("Greska", f"gta_sa.exe nije pronadjen u:\n{self.gta_path}"); return
        samp = find_samp_exe(self.gta_path)
        if not samp:
            messagebox.showerror("Greska", "SA-MP client nije pronadjen! Instaliraj SA-MP 0.3.7-R4."); return
        ok, msg = check_cef(self.gta_path)
        if not ok:
            if messagebox.askyesno("CEF", f"CEF nije instaliran!\n\n{msg}\n\nBez CEF-a necete moci koristiti Tablet/Inventar/Laptop.\n\nPokrenuti AUTO-INSTALACIJU?"):
                self.auto_install(); return
        self.is_launching = True
        self.connect_btn.stop_pulse()
        self.connect_btn.configure(text="POKRECEM...", fg_color=C_NEON, text_color=C_BG, state="disabled")
        threading.Thread(target=self._do_launch, args=(samp,), daemon=True).start()

    def _do_launch(self, exe):
        try:
            self.root.after(0, lambda: self.connect_btn.configure(text="KONEKTUJEM..."))
            subprocess.Popen([exe, f"{SERVER_IP}:{SERVER_PORT}"], cwd=self.gta_path)
            time.sleep(2)
            self.root.after(0, lambda: self.connect_btn.configure(text="IGRA POKRENUTA!", fg_color=C_GREEN, text_color=C_BG))
            time.sleep(5); self.root.after(0, self.root.destroy)
        except Exception as e:
            self.root.after(0, lambda: self._launch_err(str(e)))

    def _launch_err(self, msg):
        self.is_launching = False
        self.connect_btn.configure(text="PRIKLJUCI SE", fg_color=C_BG_CARD, text_color=C_NEON, state="normal")
        self.connect_btn.start_pulse()
        messagebox.showerror("Greska", f"Launch neuspjesan:\n\n{msg}")

    # ================================================================
    #  CLOSE
    # ================================================================
    def on_close(self):
        if self.dl_mgr: self.dl_mgr.cancel()
        self.bg.stop()
        if self.gta_path: save_settings(self.gta_path, nickname=self.nickname)
        self.root.destroy(); sys.exit(0)


if __name__ == "__main__":
    UnicateGamingLauncher().root.mainloop()
