#!/usr/bin/env python3
# =============================================================================
#  UNICATE GAMING - LAUNCHER V5
#  Full-Screen Gaming Portal | Blue Neon Theme | Custom Logo
#
#  Instalacija:
#    pip install customtkinter pillow requests
#
#  Kompajliranje u .exe:
#    pyinstaller --onefile --windowed --icon=ug_icon.ico --name="UnicateGaming" launcher_v2.py
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
LAUNCHER_VERSION = "5.0.0"

OMP_CEF_ASI_URL = "https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/cef.asi"
OMP_CEF_CLIENT_URL = "https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/client-files-v1.2.0.zip"
ASI_LOADER_URL = "https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/v4.76/dsound.zip"

if getattr(sys, 'frozen', False):
    LAUNCHER_DIR = os.path.dirname(sys.executable)
else:
    LAUNCHER_DIR = os.path.dirname(os.path.abspath(__file__))

# =============================================================================
#  BLUE NEON PALETTE
# =============================================================================
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

# Blue nijanse - glavna paleta
BLUE_50 = "#eff6ff"
BLUE_100 = "#dbeafe"
BLUE_200 = "#bfdbfe"
BLUE_300 = "#93c5fd"
BLUE_400 = "#60a5fa"
BLUE_500 = "#3b82f6"      # Primary blue
BLUE_600 = "#2563eb"
BLUE_700 = "#1d4ed8"
BLUE_800 = "#1e40af"
BLUE_900 = "#1e3a8a"
BLUE_950 = "#172554"

# Neon blues
NEON_SKY = "#00d4ff"       # Svijetli neon blue
NEON_ELECTRIC = "#0088ff"  # Elektricni blue
NEON_ICE = "#7dd3fc"       # Ice blue
NEON_DEEP = "#1e40af"      # Deep neon

# Pozadine
BG_VOID = "#030712"        # Najtamniji
BG_SPACE = "#0a0f1e"       # Space tamna
BG_PANEL = "#0c1222"       # Panel pozadina
BG_CARD = "#0f172a"        # Kartica pozadina
BG_INPUT = "#0c1528"       # Input pozadina
BG_HOVER = "#162040"       # Hover stanje

# Status boje
GREEN_OK = "#22c55e"
RED_ERR = "#ef4444"
ORANGE_WARN = "#f59e0b"

# Text
T_WHITE = "#f8fafc"
T_LIGHT = "#cbd5e1"
T_DIM = "#475569"
T_BLUE = NEON_SKY

# =============================================================================
#  UTILITY - DETEKCIJA / QUERY / SETTINGS
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
        pwlen = struct.unpack_from('H', data, off)[0]; off += 2
        off += pwlen
        players = struct.unpack_from('H', data, off)[0]; off += 2
        maxp = struct.unpack_from('H', data, off)[0]; off += 2
        nlen = struct.unpack_from('I', data, off)[0]; off += 4
        name = data[off:off+nlen].decode('latin-1', errors='replace'); off += nlen
        mlen = struct.unpack_from('I', data, off)[0]; off += 4
        mode = data[off:off+mlen].decode('latin-1', errors='replace'); off += mlen
        maplen = struct.unpack_from('I', data, off)[0]; off += 4
        mapn = data[off:off+maplen].decode('latin-1', errors='replace')
        return {'players': players, 'max_players': maxp, 'name': name, 'gamemode': mode, 'map': mapn, 'online': True}
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
    if os.path.exists(os.path.join(gta_path, "dsound.dll")): return True
    for f in os.listdir(gta_path):
        if f.lower().endswith('.asi') and f.lower() != 'cef.asi': return True
    return False

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
#  PARTICLE SYSTEM
# =============================================================================
class Particle:
    __slots__ = ['x','y','vx','vy','size','color','life','max_life','alpha']
    def __init__(self, x, y, vx, vy, size, color, life):
        self.x=x; self.y=y; self.vx=vx; self.vy=vy; self.size=size
        self.color=color; self.life=life; self.max_life=life; self.alpha=1.0

class ParticleSystem:
    def __init__(self, w, h, n=50):
        self.w=w; self.h=h; self.n=n; self.particles=[]
        self.colors=[NEON_SKY, NEON_ELECTRIC, NEON_ICE, BLUE_400, BLUE_300, "#ffffff"]

    def update(self):
        while len(self.particles) < self.n:
            self.particles.append(Particle(
                random.randint(0,self.w), random.randint(0,self.h),
                random.uniform(-0.2,0.2), random.uniform(-0.6,-0.05),
                random.uniform(0.5,2.5), random.choice(self.colors),
                random.randint(120,500)))
        alive=[]
        for p in self.particles:
            p.x+=p.vx; p.y+=p.vy; p.life-=1; p.alpha=max(0,p.life/p.max_life)
            if p.life>0 and 0<=p.x<=self.w and 0<=p.y<=self.h: alive.append(p)
        self.particles=alive

# =============================================================================
#  CUSTOM WIDGETS
# =============================================================================
class NeonButton(ctk.CTkButton):
    def __init__(self, master, glow=NEON_SKY, pulse=False, **kw):
        self.glow=glow; self._pulse=pulse; self._phase=0; self._running=False
        super().__init__(master, **kw)
        self.bind("<Enter>", lambda e: self.configure(fg_color=self.glow, text_color=BG_VOID))
        self.bind("<Leave>", lambda e: self.configure(fg_color="#0f172a", text_color=self.glow) if not self._running else None)

    def start_pulse(self):
        self._running=True; self._do_pulse()

    def stop_pulse(self):
        self._running=False

    def _do_pulse(self):
        if not self._running: return
        self._phase+=0.06
        i=(math.sin(self._phase)+1)/2
        r1,g1,b1=15,23,42
        r2=int(self.glow[1:3],16); g2=int(self.glow[3:5],16); b2=int(self.glow[5:7],16)
        r=int(r1+(r2-r1)*i*0.6); g=int(g1+(g2-g1)*i*0.6); b=int(b1+(b2-b1)*i*0.6)
        self.configure(fg_color=f"#{r:02x}{g:02x}{b:02x}", text_color=T_WHITE)
        self.after(50, self._do_pulse)


class GlowBar(ctk.CTkCanvas):
    def __init__(self, master, h=6, **kw):
        super().__init__(master, height=h, bg=BG_VOID, highlightthickness=0, **kw)
        self.progress=0; self._anim=0

    def set_progress(self, pct):
        self.progress=max(0,min(100,pct)); self._tick()

    def _tick(self):
        if abs(self._anim-self.progress)<0.5: self._anim=self.progress; self._draw(); return
        self._anim+=(self.progress-self._anim)*0.12; self._draw(); self.after(16,self._tick)

    def _draw(self):
        self.delete("all"); w=self.winfo_width(); h=self.winfo_height()
        if w<2: return
        self.create_rectangle(0,0,w,h,fill="#0a1020",outline="")
        fw=(self._anim/100)*w
        if fw>0:
            self.create_rectangle(0,0,fw+6,h,fill="#0a2a5a",outline="")
            self.create_rectangle(0,0,fw,h,fill=NEON_ELECTRIC,outline="")
            self.create_rectangle(max(0,fw-10),0,fw,h,fill=NEON_SKY,outline="")

    def reset(self): self.progress=0; self._anim=0; self._draw()


class PulseDot(ctk.CTkCanvas):
    def __init__(self, master, sz=10, color=GREEN_OK, **kw):
        super().__init__(master, width=sz+8, height=sz+8, bg=BG_VOID, highlightthickness=0, **kw)
        self.sz=sz; self.color=color; self._draw()

    def set_color(self, c): self.color=c; self._draw()

    def _draw(self):
        self.delete("all"); s=self.sz; cx=s//2+4; cy=s//2+4
        self.create_oval(cx-s//2-3,cy-s//2-3,cx+s//2+3,cy+s//2+3,fill="",outline=self.color,width=1)
        self.create_oval(cx-s//2,cy-s//2,cx+s//2,cy+s//2,fill=self.color,outline="")


class BgCanvas(ctk.CTkCanvas):
    """Animirana pozadina sa česticama i grid-om"""
    def __init__(self, master, **kw):
        super().__init__(master, bg=BG_VOID, highlightthickness=0, **kw)
        self.ps=None; self._run=False; self._bg_photo=None
        self._logo_photo=None; self._logo_alpha_phase=0

    def set_bg(self, path):
        try:
            if os.path.exists(path):
                img=Image.open(path)
                img=img.resize((self.winfo_screenwidth(),self.winfo_screenheight()),Image.LANCZOS)
                img=ImageEnhance.Brightness(img).enhance(0.3)
                img=img.filter(ImageFilter.GaussianBlur(3))
                # Plavi tint overlay
                overlay=Image.new('RGBA',img.size,(0,40,120,40))
                img=img.convert('RGBA'); img=Image.alpha_composite(img,overlay)
                self._bg_photo=ImageTk.PhotoImage(img.convert('RGB'))
        except: pass

    def set_watermark(self, path):
        """Postavi logo kao watermark u pozadinu"""
        try:
            if os.path.exists(path):
                img=Image.open(path).convert('RGBA')
                img=img.resize((300,300),Image.LANCZOS)
                # Smanji opacity
                alpha=img.getchannel('A')
                alpha=alpha.point(lambda a: int(a*0.08))
                img.putalpha(alpha)
                self._logo_watermark=img
        except: pass

    def start(self):
        self.update_idletasks()
        w=max(self.winfo_width(),1920); h=max(self.winfo_height(),1080)
        self.ps=ParticleSystem(w,h,45); self._run=True; self._anim()

    def stop(self): self._run=False

    def _anim(self):
        if not self._run: return
        self.delete("all")
        if self._bg_photo: self.create_image(0,0,image=self._bg_photo,anchor="nw")
        w=self.winfo_width(); h=self.winfo_height()
        # Grid
        if w>100 and h>100:
            for x in range(0,w,100): self.create_line(x,0,x,h,fill="#060e1e",width=1)
            for y in range(0,h,100): self.create_line(0,y,w,y,fill="#060e1e",width=1)
        # Čestice
        if self.ps:
            self.ps.update()
            for p in self.ps.particles:
                try:
                    r=int(int(p.color[1:3],16)*p.alpha*0.45)
                    g=int(int(p.color[3:5],16)*p.alpha*0.45)
                    b=int(int(p.color[5:7],16)*p.alpha*0.45)
                    c=f"#{max(0,min(255,r)):02x}{max(0,min(255,g)):02x}{max(0,min(255,b)):02x}"
                except: c=p.color
                s=p.size*p.alpha
                if s>0.3: self.create_oval(p.x-s,p.y-s,p.x+s,p.y+s,fill=c,outline="")
        # Vodeni žig logo - pulsirajući
        if hasattr(self,'_logo_watermark'):
            self._logo_alpha_phase+=0.02
            a=(math.sin(self._logo_alpha_phase)+1)/2
            # Samo prikaži ako je dovoljno veliki canvas
            if w>500 and h>500:
                cx=w-180; cy=h-180
                try:
                    wm=self._logo_watermark.copy()
                    alpha=wm.getchannel('A')
                    base_a=int(a*15)+5
                    alpha=alpha.point(lambda x: min(base_a, x))
                    wm.putalpha(alpha)
                    self._wm_tk=ImageTk.PhotoImage(wm)
                    self.create_image(cx,cy,image=self._wm_tk,anchor="center")
                except: pass
        self.after(33,self._anim)

# =============================================================================
#  MAIN LAUNCHER
# =============================================================================
class UnicateGamingLauncher:
    def __init__(self):
        self.root=ctk.CTk()
        self.root.title("UNICATE GAMING - LAUNCHER")
        self.root.overrideredirect(True)

        # Postavi ikonicu
        icon_path = os.path.join(LAUNCHER_DIR, "ug_icon.ico")
        if os.path.exists(icon_path):
            try:
                self.root.iconbitmap(icon_path)
            except: pass

        sw=self.root.winfo_screenwidth(); sh=self.root.winfo_screenheight()
        self.root.geometry(f"{sw}x{sh}+0+0")
        self.root.configure(fg_color=BG_VOID)

        settings=load_settings()
        self.gta_path=settings.get("gta_path",find_gta_sa_path())
        self.server_info=None; self.is_launching=False; self.is_installing=False
        self.dl_mgr=None; self.status=get_status(self.gta_path)

        self.imgs={}
        self._load_images()
        self.build_ui()
        self.bg.start()
        self.launch_btn.start_pulse()
        self.check_server()
        self._sched_server()

        self._drag=None
        self.root.bind("<ButtonPress-1>",self._drag_start)
        self.root.bind("<B1-Motion>",self._drag_move)
        self.root.bind("<Escape>",lambda e:self.on_close())

    def _load_images(self):
        try:
            # Glavni logo - prvo traži ug_logo.png (korisnikov upload), pa ostale
            logo=os.path.join(LAUNCHER_DIR,"ug_logo.png")
            if not os.path.exists(logo):
                logo=os.path.join(LAUNCHER_DIR,"ug_logo_pro.png")
            if not os.path.exists(logo):
                logo=os.path.join(LAUNCHER_DIR,"logo.png")
            if os.path.exists(logo):
                img=Image.open(logo)
                self.imgs['logo_huge']=ctk.CTkImage(img,img,size=(220,220))
                self.imgs['logo_big']=ctk.CTkImage(img,img,size=(150,150))
                self.imgs['logo_med']=ctk.CTkImage(img,img,size=(80,80))
                self.imgs['logo_sm']=ctk.CTkImage(img,img,size=(44,44))
                self.imgs['logo_nav']=ctk.CTkImage(img,img,size=(32,32))
                # Splash sa glow efektom
                glow=img.resize((160,160),Image.LANCZOS)
                glow=glow.filter(ImageFilter.GaussianBlur(12))
                glow=ImageEnhance.Brightness(glow).enhance(2.0)
                glow=ImageEnhance.Color(glow).enhance(2.0)
                self.imgs['logo_glow']=ctk.CTkImage(glow,glow,size=(200,200))

            bg=os.path.join(LAUNCHER_DIR,"bg_gta.png")
            self.imgs['bg_path']=bg if os.path.exists(bg) else None
        except Exception as e:
            print(f"Img err: {e}")

    def _drag_start(self,e): self._drag=(e.x,e.y)
    def _drag_move(self,e):
        if self._drag and self._drag[1]<70:
            self.root.geometry(f"+{self.root.winfo_x()+e.x-self._drag[0]}+{self.root.winfo_y()+e.y-self._drag[1]}")

    # ===== BUILD UI =====
    def build_ui(self):
        # Pozadina
        self.bg=BgCanvas(self.root)
        self.bg.place(x=0,y=0,relwidth=1,relheight=1)
        if self.imgs.get('bg_path'):
            self.bg.set_bg(self.imgs['bg_path'])
        # Logo watermark
        logo_wm=os.path.join(LAUNCHER_DIR,"ug_logo.png")
        if os.path.exists(logo_wm):
            self.bg.set_watermark(logo_wm)

        # Main container
        self.main=ctk.CTkFrame(self.root,fg_color="transparent")
        self.main.place(relx=0.5,rely=0.5,anchor="center",relwidth=0.94,relheight=0.94)

        self._build_nav()
        self._build_body()

    def _build_nav(self):
        nav=ctk.CTkFrame(self.main,height=56,fg_color="#070d1a",corner_radius=14,
                         border_width=1,border_color=BLUE_950)
        nav.pack(fill="x",pady=(0,12)); nav.pack_propagate(False)
        inner=ctk.CTkFrame(nav,fg_color="transparent")
        inner.pack(fill="both",expand=True,padx=20,pady=6)

        # Logo + Title
        left=ctk.CTkFrame(inner,fg_color="transparent")
        left.pack(side="left",fill="y")

        if 'logo_nav' in self.imgs:
            ctk.CTkLabel(left,image=self.imgs['logo_nav'],text="",fg_color="transparent").pack(side="left",padx=(0,8))

        ctk.CTkLabel(left,text="UNICATE",font=ctk.CTkFont("Segoe UI",17,"bold"),
                    text_color=NEON_SKY,fg_color="transparent").pack(side="left")
        ctk.CTkLabel(left,text=" GAMING",font=ctk.CTkFont("Segoe UI",17,"bold"),
                    text_color=T_WHITE,fg_color="transparent").pack(side="left")

        # Verzija
        vf=ctk.CTkFrame(left,fg_color=BLUE_950,corner_radius=6,height=20)
        vf.pack(side="left",padx=(12,0)); vf.pack_propagate(False)
        ctk.CTkLabel(vf,text=f"v{LAUNCHER_VERSION}",font=ctk.CTkFont(size=8, weight="bold"),
                    text_color=BLUE_300,fg_color="transparent").pack(padx=7)

        # Center nav
        center=ctk.CTkFrame(inner,fg_color="transparent")
        center.pack(side="left",expand=True)
        for nm,c in [("SERVER",NEON_SKY),("CEF UI",BLUE_400),("NOVOSTI",NEON_ICE),("DISCORD",BLUE_300)]:
            lbl=ctk.CTkLabel(center,text=nm,font=ctk.CTkFont(size=11, weight="bold"),text_color=c,fg_color="transparent",cursor="hand2")
            lbl.pack(side="left",padx=16)
            if nm=="DISCORD": lbl.bind("<Button-1>",lambda e:webbrowser.open(DISCORD_URL))

        # Right
        right=ctk.CTkFrame(inner,fg_color="transparent")
        right.pack(side="right",fill="y")

        badge=ctk.CTkFrame(right,fg_color="#060e1e",corner_radius=8,height=26)
        badge.pack(side="left",padx=(0,12)); badge.pack_propagate(False)
        self.dot_nav=PulseDot(badge,5,ORANGE_WARN); self.dot_nav.pack(side="left",padx=(6,3))
        self.online_nav=ctk.CTkLabel(badge,text="...",font=ctk.CTkFont(size=9, weight="bold"),
                                     text_color=T_DIM,fg_color="transparent")
        self.online_nav.pack(side="left",padx=(0,6))

        ctk.CTkButton(right,text="—",width=32,height=26,fg_color="transparent",hover_color="#0f172a",
                      text_color=T_DIM,font=ctk.CTkFont(11),corner_radius=6,
                      command=self.root.iconify).pack(side="left",padx=2)
        ctk.CTkButton(right,text="✕",width=32,height=26,fg_color="transparent",hover_color="#3b0a0a",
                      text_color=RED_ERR,font=ctk.CTkFont(size=11, weight="bold"),corner_radius=6,
                      command=self.on_close).pack(side="left",padx=2)

    def _build_body(self):
        body=ctk.CTkFrame(self.main,fg_color="transparent")
        body.pack(fill="both",expand=True)
        body.grid_columnconfigure(0,weight=5)
        body.grid_columnconfigure(1,weight=3)
        body.grid_rowconfigure(0,weight=5)
        body.grid_rowconfigure(1,weight=3)

        self._build_hero(body)
        self._build_launch(body)
        self._build_news(body)
        self._build_status(body)

    def _build_hero(self,parent):
        hero=ctk.CTkFrame(parent,fg_color=BG_PANEL,corner_radius=16,
                          border_width=1,border_color=BLUE_950)
        hero.grid(row=0,column=0,sticky="nsew",padx=(0,6),pady=(0,6))
        hi=ctk.CTkFrame(hero,fg_color="transparent")
        hi.pack(fill="both",expand=True,padx=28,pady=22)

        # Logo sekcija
        top=ctk.CTkFrame(hi,fg_color="transparent")
        top.pack(fill="x")

        if 'logo_big' in self.imgs:
            # Glow iza loga
            glow_f=ctk.CTkFrame(top,fg_color="transparent")
            glow_f.pack(side="left",padx=(0,22))
            if 'logo_glow' in self.imgs:
                ctk.CTkLabel(glow_f,image=self.imgs['logo_glow'],text="",
                            fg_color="transparent").place(relx=0.5,rely=0.5,anchor="center")
            ctk.CTkLabel(glow_f,image=self.imgs['logo_big'],text="",
                        fg_color="transparent").place(relx=0.5,rely=0.5,anchor="center")
            glow_f.configure(width=170,height=170)

        info=ctk.CTkFrame(top,fg_color="transparent")
        info.pack(side="left",fill="both",expand=True)

        ctk.CTkLabel(info,text="UNICATE GAMING",font=ctk.CTkFont("Segoe UI",38,"bold"),
                    text_color=T_WHITE,fg_color="transparent",anchor="w").pack(anchor="w")
        ctk.CTkLabel(info,text="R  P  G     S E R V E R",font=ctk.CTkFont("Segoe UI",13),
                    text_color=NEON_SKY,fg_color="transparent",anchor="w").pack(anchor="w",pady=(0,12))

        sr=ctk.CTkFrame(info,fg_color="transparent"); sr.pack(anchor="w",pady=4)
        self.dot_status=PulseDot(sr,10,ORANGE_WARN); self.dot_status.pack(side="left",padx=(0,8))
        self.lbl_status=ctk.CTkLabel(sr,text="Provjeravam...",font=ctk.CTkFont(12),
                                     text_color=T_DIM,fg_color="transparent")
        self.lbl_status.pack(side="left")
        self.lbl_players=ctk.CTkLabel(info,text="",font=ctk.CTkFont(size=13, weight="bold"),
                                      text_color=T_WHITE,fg_color="transparent",anchor="w")
        self.lbl_players.pack(anchor="w",pady=2)
        self.lbl_mode=ctk.CTkLabel(info,text="",font=ctk.CTkFont(10),
                                   text_color=T_DIM,fg_color="transparent",anchor="w")
        self.lbl_mode.pack(anchor="w")

        # Separator
        ctk.CTkFrame(hi,height=1,fg_color=BLUE_950).pack(fill="x",pady=14)

        # CEF Feature kartice
        cards=ctk.CTkFrame(hi,fg_color="transparent"); cards.pack(fill="both",expand=True)
        for i,(t,d,c) in enumerate([
            ("TABLET","Moderni CEF tablet UI\nGPS, poruke, kontakti",NEON_SKY),
            ("INVENTAR","Drag & drop inventar\nkategorije, detalji",BLUE_400),
            ("LAPTOP","Dark Web, Banka\nEmail, Terminal",NEON_ELECTRIC)]):
            card=ctk.CTkFrame(cards,fg_color="#080e1e",corner_radius=12,border_width=1,border_color=c)
            card.pack(side="left",fill="both",expand=True,padx=(0 if i==0 else 6,0))
            ci=ctk.CTkFrame(card,fg_color="transparent"); ci.pack(fill="both",expand=True,padx=14,pady=10)
            tf=ctk.CTkFrame(ci,fg_color="transparent"); tf.pack(anchor="w",pady=(0,4))
            ctk.CTkFrame(tf,fg_color=c,width=24,height=3,corner_radius=2).pack(side="left",padx=(0,8))
            ctk.CTkLabel(tf,text=t,font=ctk.CTkFont(size=10, weight="bold"),text_color=c,fg_color="transparent").pack(side="left")
            ctk.CTkLabel(ci,text=d,font=ctk.CTkFont(9),text_color=T_DIM,fg_color="transparent",
                        anchor="w",justify="left").pack(anchor="w")

    def _build_launch(self,parent):
        p=ctk.CTkFrame(parent,fg_color=BG_PANEL,corner_radius=16,
                        border_width=1,border_color=BLUE_800)
        p.grid(row=0,column=1,sticky="nsew",padx=(6,0),pady=(0,6))
        pi=ctk.CTkFrame(p,fg_color="transparent")
        pi.pack(fill="both",expand=True,padx=22,pady=22)

        # Server info
        ctk.CTkLabel(pi,text="KONEKCIJA",font=ctk.CTkFont(size=10, weight="bold"),
                    text_color=NEON_SKY,fg_color="transparent",anchor="w").pack(anchor="w",pady=(0,6))
        cc=ctk.CTkFrame(pi,fg_color=BG_INPUT,corner_radius=10,border_width=1,border_color=BLUE_950)
        cc.pack(fill="x",pady=(0,12))
        ctk.CTkLabel(cc,text=f"  {SERVER_IP}:{SERVER_PORT}",font=ctk.CTkFont("Consolas",11),
                    text_color=NEON_SKY,fg_color="transparent",anchor="w").pack(anchor="w",padx=6,pady=(7,1))
        ctk.CTkLabel(cc,text=f"  {SERVER_NAME}",font=ctk.CTkFont("Consolas",9),
                    text_color=T_DIM,fg_color="transparent",anchor="w").pack(anchor="w",padx=6,pady=(0,7))

        ctk.CTkLabel(pi,text="GTA SAN ANDREAS",font=ctk.CTkFont(size=10, weight="bold"),
                    text_color=BLUE_300,fg_color="transparent",anchor="w").pack(anchor="w",pady=(4,6))
        pc=ctk.CTkFrame(pi,fg_color=BG_INPUT,corner_radius=10,border_width=1,border_color=BLUE_950)
        pc.pack(fill="x",pady=(0,4))
        pt=self.gta_path or "Nije pronadjen!"; ptc=GREEN_OK if self.gta_path else RED_ERR
        self.path_lbl=ctk.CTkLabel(pc,text=pt,font=ctk.CTkFont("Consolas",9),text_color=ptc,
                                   fg_color="transparent",anchor="w",wraplength=230)
        self.path_lbl.pack(anchor="w",padx=8,pady=7)

        ctk.CTkButton(pi,text="BROWSE",width=75,height=26,fg_color="#0f172a",hover_color=BG_HOVER,
                      text_color=NEON_SKY,font=ctk.CTkFont(size=9, weight="bold"),corner_radius=8,
                      border_width=1,border_color=BLUE_800,command=self.browse).pack(anchor="w",pady=(0,12))

        # Progress
        self.pbar=GlowBar(pi,height=5); self.pbar.pack(fill="x",pady=(0,3))
        self.prog_lbl=ctk.CTkLabel(pi,text="",font=ctk.CTkFont(9),text_color=NEON_SKY,fg_color="transparent",anchor="w")
        self.prog_lbl.pack(anchor="w")
        self.prog_det=ctk.CTkLabel(pi,text="",font=ctk.CTkFont(8),text_color=T_DIM,fg_color="transparent",anchor="w")
        self.prog_det.pack(anchor="w")

        ctk.CTkFrame(pi,fg_color="transparent").pack(fill="both",expand=True)

        # Install
        self.install_btn=NeonButton(pi,text="AUTO-INSTALACIJA",height=38,
            font=ctk.CTkFont("Segoe UI",12,"bold"),fg_color="#0f172a",hover_color=BLUE_600,
            text_color=BLUE_400,corner_radius=10,border_width=1,border_color=BLUE_600,
            glow=BLUE_500,command=self.auto_install)
        if not self.status['ready']:
            self.install_btn.pack(fill="x",pady=(0,6),side="bottom")

        # LAUNCH
        self.launch_btn=NeonButton(pi,text="LAUNCH",height=62,
            font=ctk.CTkFont("Segoe UI",22,"bold"),fg_color="#0f172a",hover_color=NEON_SKY,
            text_color=NEON_SKY,corner_radius=14,border_width=2,border_color=NEON_SKY,
            glow=NEON_SKY,pulse=True,command=self.launch_game)
        self.launch_btn.pack(fill="x",pady=(0,4),side="bottom")

        self.launch_lbl=ctk.CTkLabel(pi,text="",font=ctk.CTkFont(9),text_color=T_DIM,fg_color="transparent")
        self.launch_lbl.pack(pady=(0,3),side="bottom")

    def _build_news(self,parent):
        p=ctk.CTkFrame(parent,fg_color=BG_PANEL,corner_radius=16,
                        border_width=1,border_color=BLUE_950)
        p.grid(row=1,column=0,sticky="nsew",padx=(0,6),pady=(6,0))
        pi=ctk.CTkFrame(p,fg_color="transparent")
        pi.pack(fill="both",expand=True,padx=18,pady=14)

        ctk.CTkLabel(pi,text="NOVOSTI",font=ctk.CTkFont(size=11, weight="bold"),
                    text_color=NEON_ICE,fg_color="transparent",anchor="w").pack(anchor="w",pady=(0,8))

        for t,d,c in [
            ("open.mp Server","Server je sad na open.mp sa ugradjenim CEF-om! Tablet, inventar i laptop rade iz boxa.",NEON_SKY),
            ("Auto-Instalacija","Launcher skida i instalira sve automatski - CEF, ASI loader, Chromium runtime.",BLUE_400),
            ("Bounty Sistem","Postavljanje nagrada za igrace putem tableta ili laptopa. /bounty",NEON_ELECTRIC)]:
            card=ctk.CTkFrame(pi,fg_color="#080e1e",corner_radius=10,border_width=1,border_color=BLUE_950)
            card.pack(fill="x",pady=2)
            inner=ctk.CTkFrame(card,fg_color="transparent"); inner.pack(fill="both",expand=True,padx=10,pady=7)
            ctk.CTkFrame(inner,fg_color=c,width=3,corner_radius=2).pack(side="left",fill="y",padx=(0,8),pady=1)
            tc=ctk.CTkFrame(inner,fg_color="transparent"); tc.pack(side="left",fill="both",expand=True)
            ctk.CTkLabel(tc,text=t,font=ctk.CTkFont(size=10, weight="bold"),text_color=T_WHITE,fg_color="transparent",anchor="w").pack(anchor="w")
            ctk.CTkLabel(tc,text=d,font=ctk.CTkFont(8),text_color=T_DIM,fg_color="transparent",anchor="w",wraplength=500).pack(anchor="w")

    def _build_status(self,parent):
        p=ctk.CTkFrame(parent,fg_color=BG_PANEL,corner_radius=16,
                        border_width=1,border_color=BLUE_950)
        p.grid(row=1,column=1,sticky="nsew",padx=(6,0),pady=(6,0))
        pi=ctk.CTkFrame(p,fg_color="transparent")
        pi.pack(fill="both",expand=True,padx=18,pady=14)

        ctk.CTkLabel(pi,text="KOMPONENTE",font=ctk.CTkFont(size=11, weight="bold"),
                    text_color=BLUE_300,fg_color="transparent",anchor="w").pack(anchor="w",pady=(0,8))

        comps=[
            ("SA-MP Client",self.status['has_samp']),
            ("ASI Loader",self.status['has_asi']),
            ("CEF Plugin",self.status['cef_ok']),
            ("Chromium RT",self.gta_path and os.path.exists(os.path.join(self.gta_path,"cef"))),
        ]
        self.comp_widgets=[]
        for name,ok in comps:
            r=ctk.CTkFrame(pi,fg_color="transparent"); r.pack(fill="x",pady=2)
            d=PulseDot(r,7,GREEN_OK if ok else ORANGE_WARN); d.pack(side="left",padx=(0,6))
            l=ctk.CTkLabel(r,text=name+(" ✓" if ok else " ✗"),font=ctk.CTkFont(10),
                          text_color=GREEN_OK if ok else ORANGE_WARN,fg_color="transparent")
            l.pack(side="left")
            self.comp_widgets.append((d,l,name))

        ctk.CTkFrame(pi,height=1,fg_color=BLUE_950).pack(fill="x",pady=10)

        ctk.CTkLabel(pi,text="BRZE VEZE",font=ctk.CTkFont(size=10, weight="bold"),
                    text_color=T_DIM,fg_color="transparent",anchor="w").pack(anchor="w",pady=(0,6))
        lf=ctk.CTkFrame(pi,fg_color="transparent"); lf.pack(fill="x")
        for nm,url,c in [("Website",WEBSITE_URL,NEON_SKY),("Discord",DISCORD_URL,BLUE_400)]:
            NeonButton(lf,text=nm,height=28,width=100,font=ctk.CTkFont(size=9, weight="bold"),
                      fg_color="#080e1e",hover_color=c,text_color=c,corner_radius=8,
                      border_width=1,border_color=c,glow=c,
                      command=lambda u=url:webbrowser.open(u)).pack(side="left",padx=(0,6))

    # ===== ACTIONS =====
    def browse(self):
        e=filedialog.askopenfilename(title="Pronadji gta_sa.exe",
            filetypes=[("GTA SA","gta_sa.exe"),("All","*.*")])
        if e:
            self.gta_path=os.path.dirname(e)
            self.path_lbl.configure(text=self.gta_path,text_color=GREEN_OK)
            save_settings(self.gta_path); self._refresh_status()

    def _refresh_status(self):
        self.status=get_status(self.gta_path)
        comps=[self.status['has_samp'],self.status['has_asi'],self.status['cef_ok'],
               self.gta_path and os.path.exists(os.path.join(self.gta_path,"cef"))]
        for (d,l,n),ok in zip(self.comp_widgets,comps):
            d.set_color(GREEN_OK if ok else ORANGE_WARN)
            l.configure(text=n+(" ✓" if ok else " ✗"),text_color=GREEN_OK if ok else ORANGE_WARN)
        if self.status['ready']:
            self.install_btn.pack_forget()
            self.pbar.set_progress(100)
            self.prog_lbl.configure(text="Sve spremno!",text_color=GREEN_OK)
        else:
            self.install_btn.pack(fill="x",pady=(0,6),side="bottom")

    def auto_install(self):
        if self.is_installing: return
        if not self.gta_path:
            messagebox.showerror("Greska","GTA SA nije pronadjen! Klikni Browse.")
            return
        self.is_installing=True
        self.install_btn.configure(text="INSTALIRAM...",state="disabled")
        self.launch_btn.stop_pulse(); self.launch_btn.configure(state="disabled")
        self.pbar.reset()
        self.prog_lbl.configure(text="Zapocinjem...",text_color=NEON_SKY)
        self.dl_mgr=DownloadManager(
            on_progress=lambda p,d,t,s: self.root.after(0,lambda:self._prog(p,d,t,s)),
            on_status=lambda t: self.root.after(0,lambda:self.prog_lbl.configure(text=t,text_color=NEON_SKY)),
            on_complete=lambda c: self.root.after(0,lambda:self._done()),
            on_error=lambda e: self.root.after(0,lambda:self._err(e)))
        threading.Thread(target=self.dl_mgr.install_all,args=(self.gta_path,self.status['missing']),daemon=True).start()

    def _prog(self,pct,dl,total,spd):
        self.pbar.set_progress(pct)
        if total>0: self.prog_det.configure(text=f"{dl:.1f}/{total:.1f} MB | {spd:.0f} KB/s")
        elif dl>0: self.prog_det.configure(text=f"{dl:.1f} MB")

    def _done(self):
        self.is_installing=False
        self.install_btn.configure(text="AUTO-INSTALACIJA",state="normal")
        self.launch_btn.configure(state="normal"); self.launch_btn.start_pulse()
        self.prog_lbl.configure(text="Sve instalirano!",text_color=GREEN_OK)
        self.pbar.set_progress(100); self._refresh_status()
        save_settings(self.gta_path,cef_installed=True)

    def _err(self,msg):
        self.is_installing=False
        self.install_btn.configure(text="AUTO-INSTALACIJA",state="normal")
        self.launch_btn.configure(state="normal"); self.launch_btn.start_pulse()
        self.prog_lbl.configure(text="Greska!",text_color=RED_ERR)
        messagebox.showerror("Greska",f"Instalacija neuspjesna:\n\n{msg}")

    # ===== SERVER =====
    def check_server(self):
        threading.Thread(target=self._do_check,daemon=True).start()

    def _do_check(self):
        info=query_samp_server(SERVER_IP,SERVER_PORT)
        self.server_info=info; self.root.after(0,self._update_status,info)

    def _update_status(self,info):
        if info and info.get('online'):
            self.dot_status.set_color(GREEN_OK); self.dot_nav.set_color(GREEN_OK)
            self.lbl_status.configure(text=f"Online | {info.get('name',SERVER_NAME)}",text_color=GREEN_OK)
            self.lbl_players.configure(text=f"Igraci: {info['players']}/{info['max_players']}")
            self.lbl_mode.configure(text=f"Gamemode: {info.get('gamemode','N/A')}")
            self.online_nav.configure(text=f"{info['players']}/{info['max_players']}",text_color=GREEN_OK)
        else:
            self.dot_status.set_color(RED_ERR); self.dot_nav.set_color(RED_ERR)
            self.lbl_status.configure(text="Server Offline",text_color=RED_ERR)
            self.lbl_players.configure(text="Nije dostupan")
            self.lbl_mode.configure(text="")
            self.online_nav.configure(text="Offline",text_color=RED_ERR)

    def _sched_server(self):
        self.check_server(); self.root.after(30000,self._sched_server)

    # ===== LAUNCH =====
    def launch_game(self):
        if self.is_launching or self.is_installing: return
        if not self.gta_path:
            messagebox.showerror("Greska","GTA SA nije pronadjen! Klikni Browse."); return
        if not os.path.exists(os.path.join(self.gta_path,"gta_sa.exe")):
            messagebox.showerror("Greska",f"gta_sa.exe nije pronadjen u:\n{self.gta_path}"); return
        samp=find_samp_exe(self.gta_path)
        if not samp:
            messagebox.showerror("Greska","SA-MP client nije pronadjen! Instaliraj SA-MP 0.3.7-R4."); return
        ok,msg=check_cef(self.gta_path)
        if not ok:
            if messagebox.askyesno("CEF",f"CEF nije instaliran!\n\n{msg}\n\nBez CEF-a necete moci koristiti Tablet/Inventar/Laptop.\n\nPokrenuti AUTO-INSTALACIJU?"):
                self.auto_install(); return
        self.is_launching=True
        self.launch_btn.stop_pulse()
        self.launch_btn.configure(text="POKRECEM...",fg_color=NEON_SKY,text_color=BG_VOID,state="disabled")
        self.launch_lbl.configure(text="Konektujem se...",text_color=NEON_SKY)
        threading.Thread(target=self._do_launch,args=(samp,),daemon=True).start()

    def _do_launch(self,exe):
        try:
            self.root.after(0,lambda:self.launch_lbl.configure(text=f"Pokrecem {SERVER_IP}:{SERVER_PORT}...",text_color=NEON_SKY))
            subprocess.Popen([exe,f"{SERVER_IP}:{SERVER_PORT}"],cwd=self.gta_path)
            time.sleep(2)
            self.root.after(0,lambda:self.launch_lbl.configure(text="Igra pokrenuta! Uzivaj!",text_color=GREEN_OK))
            time.sleep(5); self.root.after(0,self.root.destroy)
        except Exception as e:
            self.root.after(0,lambda:self._launch_err(str(e)))

    def _launch_err(self,msg):
        self.is_launching=False
        self.launch_btn.configure(text="LAUNCH",fg_color="#0f172a",text_color=NEON_SKY,state="normal")
        self.launch_btn.start_pulse()
        self.launch_lbl.configure(text=""); messagebox.showerror("Greska",f"Nije moguce pokrenuti igru!\n\n{msg}")

    def on_close(self):
        if self.dl_mgr: self.dl_mgr.cancel()
        self.bg.stop()
        if self.gta_path: save_settings(self.gta_path)
        self.root.destroy(); sys.exit(0)


if __name__=="__main__":
    UnicateGamingLauncher().root.mainloop()
