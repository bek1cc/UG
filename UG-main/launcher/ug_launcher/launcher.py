"""
Unicate Gaming Launcher v3.0
=============================
Next-level professional gaming launcher.
Auto-installs CEF, ASI loader, and UG samp.dll for all players.

Build .exe:
  pip install pyinstaller
  pyinstaller --onefile --windowed --name="Unicate Gaming" --icon=ug_icon.ico launcher.py
"""

import tkinter as tk
from tkinter import filedialog, messagebox
import os
import sys
import subprocess
import json
import shutil
import threading
import urllib.request
import time
import math

# ===== CONFIG =====
SERVER_IP = "194.163.159.118"
SERVER_PORT = "7777"
SERVER_NAME = "Unicate Gaming"
VERSION = "3.0.0"
GITHUB_RAW = "https://raw.githubusercontent.com/bek1cc/UG/main/launcher/ug_launcher"

# ===== COLOR PALETTE =====
C_BG              = "#080c16"
C_BG_SIDEBAR      = "#060a12"
C_BG_HEADER       = "#0a0e18"
C_BG_CARD         = "#0d1220"
C_BG_CARD_ALT     = "#0b101c"
C_BG_INPUT        = "#111828"
C_BG_INPUT_FOCUS  = "#152040"
C_BG_HOVER        = "#14203a"
C_BG_ACTIVE       = "#0c2d5e"
C_BORDER          = "#1a2540"
C_BORDER_LIGHT    = "#1e3050"
C_ACCENT          = "#0099ff"
C_ACCENT_HOVER    = "#0077dd"
C_NEON            = "#00d4ff"
C_NEON_DIM        = "#006699"
C_GREEN           = "#22c55e"
C_RED             = "#ef4444"
C_ORANGE          = "#f59e0b"
C_PURPLE          = "#a855f7"
C_TEXT            = "#f0f4f8"
C_TEXT_LIGHT      = "#c8d6e5"
C_TEXT_DIM        = "#64748b"
C_TEXT_DARK       = "#3a4560"
C_SEP             = "#141e30"

# ===== FONTS =====
F_TITLE      = ("Segoe UI", 26, "bold")
F_HERO_SUB   = ("Segoe UI", 11, "bold")
F_H2         = ("Segoe UI", 14, "bold")
F_H3         = ("Segoe UI", 11, "bold")
F_SECTION    = ("Segoe UI", 10, "bold")
F_BODY       = ("Segoe UI", 10)
F_SMALL      = ("Segoe UI", 9)
F_TINY       = ("Segoe UI", 8)
F_NAV        = ("Segoe UI", 10)
F_NAV_ACT    = ("Segoe UI", 10, "bold")
F_INPUT      = ("Consolas", 10)
F_INPUT_NICK = ("Consolas", 12, "bold")
F_PLAY       = ("Segoe UI", 15, "bold")
F_PLAY_ICON  = ("Segoe UI", 18)
F_STAT       = ("Segoe UI", 20, "bold")
F_STAT_LBL   = ("Segoe UI", 8)
F_COMP       = ("Segoe UI", 9)
F_TAG        = ("Segoe UI", 8, "bold")
F_NEWS_TITLE = ("Segoe UI", 10, "bold")
F_NEWS_BODY  = ("Segoe UI", 9)
F_VERSION    = ("Segoe UI", 8)


class UnicateLauncher:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("Unicate Gaming")
        self.root.geometry("1100x700")
        self.root.resizable(False, False)
        self.root.configure(bg=C_BG)

        # Frameless window
        self.root.overrideredirect(True)
        self._center()

        # Drag state
        self._drag_x = 0
        self._drag_y = 0

        # Load config
        self._cfg_path = self._get_cfg_path()
        self._cfg = self._load_cfg()

        # Tk variables
        self.gta_path    = tk.StringVar(value=self._cfg.get("gta_path", self._find_gta()))
        self.nickname    = tk.StringVar(value=self._cfg.get("nickname", ""))
        self.server_ip   = tk.StringVar(value=self._cfg.get("server_ip", f"{SERVER_IP}:{SERVER_PORT}"))
        self.status_var  = tk.StringVar(value="Spreman")
        self.srv_status  = tk.StringVar(value="Offline")
        self.online_var  = tk.StringVar(value="0")
        self.slots_var   = tk.StringVar(value="1000")

        # Component vars
        self.c_samp     = tk.StringVar(value="...")
        self.c_asi      = tk.StringVar(value="...")
        self.c_cef      = tk.StringVar(value="...")
        self.c_chrome   = tk.StringVar(value="...")

        # Active page
        self._page = "home"

        # Logo image
        self._logo_img = None
        self._hero_logo_img = None
        self._load_images()

        # Build UI
        self._build()

        # Check components after short delay
        self.root.after(600, self._auto_install_and_check)

    # ============================================================
    # IMAGES
    # ============================================================
    def _load_images(self):
        launcher_dir = self._launcher_dir()
        logo_path = os.path.join(launcher_dir, "ug_logo.png")
        if os.path.exists(logo_path):
            try:
                from PIL import Image, ImageTk
                img = Image.open(logo_path)
                img = img.resize((40, 40), Image.LANCZOS)
                self._logo_img = ImageTk.PhotoImage(img)

                hero_img = Image.open(logo_path)
                hero_img = hero_img.resize((110, 110), Image.LANCZOS)
                self._hero_logo_img = ImageTk.PhotoImage(hero_img)
            except Exception:
                self._logo_img = None
                self._hero_logo_img = None

    # ============================================================
    # UTILITY
    # ============================================================
    def _center(self):
        self.root.update_idletasks()
        w, h = 1100, 700
        x = (self.root.winfo_screenwidth() // 2) - (w // 2)
        y = (self.root.winfo_screenheight() // 2) - (h // 2)
        self.root.geometry(f"{w}x{h}+{x}+{y}")

    def _launcher_dir(self):
        if getattr(sys, 'frozen', False):
            return os.path.dirname(sys.executable)
        return os.path.dirname(os.path.abspath(__file__))

    def _get_cfg_path(self):
        if sys.platform == 'win32':
            d = os.path.join(os.environ.get('APPDATA', os.path.expanduser('~')), 'UnicateGaming')
        else:
            d = os.path.expanduser('~/.unicategaming')
        os.makedirs(d, exist_ok=True)
        return os.path.join(d, 'config.json')

    def _load_cfg(self):
        try:
            if os.path.exists(self._cfg_path):
                with open(self._cfg_path, 'r') as f:
                    return json.load(f)
        except:
            pass
        return {}

    def _save_cfg(self):
        self._cfg["gta_path"]  = self.gta_path.get()
        self._cfg["nickname"]  = self.nickname.get()
        self._cfg["server_ip"] = self.server_ip.get()
        try:
            with open(self._cfg_path, 'w') as f:
                json.dump(self._cfg, f, indent=2)
        except:
            pass

    def _find_gta(self):
        paths = [
            r"C:\Program Files (x86)\Rockstar Games\GTA San Andreas",
            r"C:\Program Files\Rockstar Games\GTA San Andreas",
            r"C:\GTA San Andreas",
            r"D:\GTA San Andreas",
            r"D:\Games\GTA San Andreas",
            r"E:\GTA San Andreas",
            r"C:\Program Files (x86)\Steam\steamapps\common\Grand Theft Auto San Andreas",
        ]
        for p in paths:
            if os.path.exists(p) and os.path.exists(os.path.join(p, "gta_sa.exe")):
                return p
        return ""

    def _start_drag(self, e):
        self._drag_x = e.x
        self._drag_y = e.y

    def _do_drag(self, e):
        x = self.root.winfo_x() + (e.x - self._drag_x)
        y = self.root.winfo_y() + (e.y - self._drag_y)
        self.root.geometry(f"+{x}+{y}")

    # ============================================================
    # UI CONSTRUCTION
    # ============================================================
    def _build(self):
        outer = tk.Frame(self.root, bg=C_BG)
        outer.pack(fill=tk.BOTH, expand=True)

        # Left sidebar
        self._build_sidebar(outer)

        # Right area
        right_area = tk.Frame(outer, bg=C_BG)
        right_area.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        self._build_titlebar(right_area)
        self._build_navbar(right_area)
        self._build_content(right_area)

    # ---------- SIDEBAR ----------
    def _build_sidebar(self, parent):
        sb = tk.Frame(parent, bg=C_BG_SIDEBAR, width=210)
        sb.pack(side=tk.LEFT, fill=tk.Y)
        sb.pack_propagate(False)

        # Logo area
        logo_area = tk.Frame(sb, bg=C_BG_SIDEBAR)
        logo_area.pack(fill=tk.X, pady=(22, 8), padx=18)

        if self._logo_img:
            logo_lbl = tk.Label(logo_area, image=self._logo_img, bg=C_BG_SIDEBAR)
            logo_lbl.pack(side=tk.LEFT, padx=(0, 10))
        else:
            logo_c = tk.Canvas(logo_area, width=42, height=42, bg=C_BG_SIDEBAR, highlightthickness=0)
            logo_c.pack(side=tk.LEFT, padx=(0, 10))
            logo_c.create_oval(2, 2, 40, 40, outline=C_NEON, width=2)
            logo_c.create_text(21, 21, text="UG", font=("Segoe UI", 13, "bold"), fill=C_NEON)

        txt_area = tk.Frame(logo_area, bg=C_BG_SIDEBAR)
        txt_area.pack(side=tk.LEFT, fill=tk.Y)
        tk.Label(txt_area, text="UNICATE", font=("Segoe UI", 16, "bold"),
                 fg=C_TEXT, bg=C_BG_SIDEBAR).pack(anchor=tk.W)
        tk.Label(txt_area, text="GAMING", font=("Segoe UI", 9, "bold"),
                 fg=C_NEON, bg=C_BG_SIDEBAR).pack(anchor=tk.W)

        # Separator
        self._sep(sb, padx=18, pady=(12, 8))

        # Nav buttons
        nav = [
            ("home",     "  Pocetna"),
            ("server",   "  Server Info"),
            ("news",     "  Novosti"),
            ("settings", "  Podesavanja"),
        ]
        self._nav_btns = {}
        for key, label in nav:
            f = tk.Frame(sb, bg=C_BG_SIDEBAR, cursor="hand2")
            f.pack(fill=tk.X, padx=6, pady=1)
            lbl = tk.Label(f, text=label, font=F_NAV,
                           fg=C_NEON if key == "home" else C_TEXT_DIM,
                           bg=C_BG_SIDEBAR, anchor=tk.W, padx=14, pady=8)
            lbl.pack(fill=tk.X)

            # Active indicator bar
            if key == "home":
                bar = tk.Frame(f, bg=C_NEON, width=3)
                bar.place(x=0, rely=0.1, relheight=0.8)
            else:
                bar = None

            self._nav_btns[key] = (f, lbl, bar)

            f.bind("<Enter>", lambda e, k=key: self._nav_hover(k, True))
            f.bind("<Leave>", lambda e, k=key: self._nav_hover(k, False))
            f.bind("<Button-1>", lambda e, k=key: self._set_page(k))
            lbl.bind("<Enter>", lambda e, k=key: self._nav_hover(k, True))
            lbl.bind("<Leave>", lambda e, k=key: self._nav_hover(k, False))
            lbl.bind("<Button-1>", lambda e, k=key: self._set_page(k))

        # Social links
        self._sep(sb, padx=18, pady=(16, 8))

        social_area = tk.Frame(sb, bg=C_BG_SIDEBAR)
        social_area.pack(fill=tk.X, padx=10)

        tk.Label(social_area, text="DRUSTVENE MREZE", font=F_TINY,
                 fg=C_TEXT_DARK, bg=C_BG_SIDEBAR).pack(anchor=tk.W, padx=8, pady=(0, 4))

        for name, color in [("Discord", C_ACCENT), ("Website", C_NEON)]:
            sf = tk.Frame(social_area, bg=C_BG_CARD, cursor="hand2",
                          highlightbackground=C_BORDER, highlightthickness=1)
            sf.pack(fill=tk.X, pady=2)
            sl = tk.Label(sf, text=name, font=F_SMALL, fg=color, bg=C_BG_CARD, padx=12, pady=6)
            sl.pack(anchor=tk.W)
            sf.bind("<Enter>", lambda e, f=sf, l=sl: (f.configure(bg=C_BG_HOVER), l.configure(bg=C_BG_HOVER)))
            sf.bind("<Leave>", lambda e, f=sf, l=sl: (f.configure(bg=C_BG_CARD), l.configure(bg=C_BG_CARD)))

        # Version at bottom
        bot = tk.Frame(sb, bg=C_BG_SIDEBAR)
        bot.pack(side=tk.BOTTOM, fill=tk.X, padx=18, pady=12)
        tk.Label(bot, text=f"v{VERSION}", font=F_VERSION,
                 fg=C_TEXT_DARK, bg=C_BG_SIDEBAR).pack(side=tk.LEFT)
        tk.Label(bot, text="SA-MP 0.3.7-R5", font=F_VERSION,
                 fg=C_TEXT_DARK, bg=C_BG_SIDEBAR).pack(side=tk.RIGHT)

    # ---------- TITLEBAR ----------
    def _build_titlebar(self, parent):
        tb = tk.Frame(parent, bg=C_BG_HEADER, height=40)
        tb.pack(fill=tk.X)
        tb.pack_propagate(False)

        tb.bind("<ButtonPress-1>", self._start_drag)
        tb.bind("<B1-Motion>", self._do_drag)

        # Status badge
        badge_frame = tk.Frame(tb, bg=C_BG_HEADER)
        badge_frame.pack(side=tk.LEFT, padx=16)

        self._status_dot_canvas = tk.Canvas(badge_frame, width=10, height=10,
                                             bg=C_BG_HEADER, highlightthickness=0)
        self._status_dot_canvas.pack(side=tk.LEFT, padx=(0, 6))
        self._status_dot_canvas.create_oval(1, 1, 9, 9, fill=C_ORANGE, outline="", tags="dot")

        self._status_dot_lbl = tk.Label(badge_frame, text="Provjera...",
                                         font=F_SMALL, fg=C_ORANGE, bg=C_BG_HEADER)
        self._status_dot_lbl.pack(side=tk.LEFT)

        # Window controls
        for text, cmd, color in [
            ("-", lambda e: self.root.iconify(), C_TEXT_DIM),
            ("X", lambda e: self._quit(), C_TEXT_DIM),
        ]:
            btn = tk.Label(tb, text=text, font=("Arial", 11), fg=C_TEXT_DIM,
                           bg=C_BG_HEADER, cursor="hand2", width=3)
            btn.pack(side=tk.RIGHT, pady=6)
            btn.bind("<Button-1>", cmd)
            if text == "X":
                btn.bind("<Enter>", lambda e, b=btn: b.configure(fg=C_RED))
                btn.bind("<Leave>", lambda e, b=btn: b.configure(fg=C_TEXT_DIM))
            else:
                btn.bind("<Enter>", lambda e, b=btn: b.configure(fg=C_TEXT))
                btn.bind("<Leave>", lambda e, b=btn: b.configure(fg=C_TEXT_DIM))
            btn.bind("<ButtonPress-1>", self._start_drag, add="+")
            btn.bind("<B1-Motion>", self._do_drag, add="+")

    # ---------- NAVBAR ----------
    def _build_navbar(self, parent):
        nb = tk.Frame(parent, bg=C_BG, height=42)
        nb.pack(fill=tk.X)
        nb.pack_propagate(False)

        tabs = [("home", "POCETNA"), ("server", "SERVER"), ("news", "NOVOSTI"), ("settings", "PODESAVANJA")]
        self._tab_btns = {}
        for key, label in tabs:
            is_active = key == "home"
            f = tk.Frame(nb, bg=C_BG, cursor="hand2")
            f.pack(side=tk.LEFT, padx=(0, 2))

            lbl = tk.Label(f, text=label, font=F_SECTION,
                           fg=C_NEON if is_active else C_TEXT_DIM,
                           bg=C_BG, padx=18, pady=10)
            lbl.pack()

            ind = tk.Frame(f, bg=C_NEON if is_active else C_BG, height=2)
            ind.pack(fill=tk.X, padx=8)

            self._tab_btns[key] = (f, lbl, ind)
            f.bind("<Button-1>", lambda e, k=key: self._set_page(k))
            lbl.bind("<Button-1>", lambda e, k=key: self._set_page(k))

    # ---------- CONTENT ----------
    def _build_content(self, parent):
        content = tk.Frame(parent, bg=C_BG)
        content.pack(fill=tk.BOTH, expand=True)

        # Create scrollable pages
        self._pages = {}

        container = tk.Frame(content, bg=C_BG)
        container.pack(fill=tk.BOTH, expand=True, padx=6, pady=4)

        # Center area + right panel
        center_right = tk.Frame(container, bg=C_BG)
        center_right.pack(fill=tk.BOTH, expand=True)

        # Center content
        self._center_frame = tk.Frame(center_right, bg=C_BG)
        self._center_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(6, 4))

        # Right panel
        self._build_right_panel(center_right)

        # Build pages
        self._build_home_page()
        self._build_server_page()
        self._build_news_page()
        self._build_settings_page()

    # ---------- HOME PAGE ----------
    def _build_home_page(self):
        page = tk.Frame(self._center_frame, bg=C_BG)
        self._pages["home"] = page

        # Hero card
        hero = tk.Frame(page, bg=C_BG_CARD, highlightbackground=C_BORDER, highlightthickness=1)
        hero.pack(fill=tk.X, pady=(0, 8))

        hero_inner = tk.Frame(hero, bg=C_BG_CARD)
        hero_inner.pack(fill=tk.X, padx=24, pady=20)

        # Logo + Info
        hero_left = tk.Frame(hero_inner, bg=C_BG_CARD)
        hero_left.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        logo_row = tk.Frame(hero_left, bg=C_BG_CARD)
        logo_row.pack(fill=tk.X)

        if self._hero_logo_img:
            tk.Label(logo_row, image=self._hero_logo_img, bg=C_BG_CARD).pack(side=tk.LEFT, padx=(0, 16))
        else:
            logo_c = tk.Canvas(logo_row, width=80, height=80, bg=C_BG_CARD, highlightthickness=0)
            logo_c.pack(side=tk.LEFT, padx=(0, 16))
            logo_c.create_oval(5, 5, 75, 75, outline=C_NEON, width=2)
            logo_c.create_text(40, 40, text="UG", font=("Segoe UI", 28, "bold"), fill=C_NEON)

        info = tk.Frame(logo_row, bg=C_BG_CARD)
        info.pack(side=tk.LEFT, fill=tk.Y)

        tk.Label(info, text="UNICATE", font=F_TITLE, fg=C_TEXT, bg=C_BG_CARD).pack(anchor=tk.W)
        title_row = tk.Frame(info, bg=C_BG_CARD)
        title_row.pack(anchor=tk.W)
        tk.Label(title_row, text="GAMING", font=("Segoe UI", 26, "bold"),
                 fg=C_NEON, bg=C_BG_CARD).pack(side=tk.LEFT)
        tk.Label(title_row, text="  SAMP RPG SERVER", font=F_HERO_SUB,
                 fg=C_ACCENT, bg=C_BG_CARD).pack(side=tk.LEFT, padx=(8, 0), pady=(8, 0))

        # Status line
        stat_line = tk.Frame(info, bg=C_BG_CARD)
        stat_line.pack(anchor=tk.W, pady=(6, 0))

        dot_c = tk.Canvas(stat_line, width=10, height=10, bg=C_BG_CARD, highlightthickness=0)
        dot_c.pack(side=tk.LEFT, padx=(0, 6))
        dot_c.create_oval(1, 1, 9, 9, fill=C_RED, outline="", tags="dot")

        self._hero_status_lbl = tk.Label(stat_line, textvariable=self.srv_status,
                                          font=F_SMALL, fg=C_RED, bg=C_BG_CARD)
        self._hero_status_lbl.pack(side=tk.LEFT)

        # Stat cards
        stats_row = tk.Frame(hero_left, bg=C_BG_CARD)
        stats_row.pack(fill=tk.X, pady=(16, 0))

        for val_var, label, color in [
            (self.online_var, "Igraca Online", C_NEON),
            (self.slots_var,  "Max Slotova",   C_TEXT),
        ]:
            card = tk.Frame(stats_row, bg=C_BG, highlightbackground=C_BORDER, highlightthickness=1)
            card.pack(side=tk.LEFT, padx=(0, 10), ipadx=12)
            tk.Label(card, textvariable=val_var, font=F_STAT, fg=color, bg=C_BG).pack(padx=8, pady=(8, 0))
            tk.Label(card, text=label, font=F_STAT_LBL, fg=C_TEXT_DIM, bg=C_BG).pack(padx=8, pady=(0, 6))

        # RPG gamemode badge
        rpg_card = tk.Frame(stats_row, bg=C_BG, highlightbackground=C_BORDER, highlightthickness=1)
        rpg_card.pack(side=tk.LEFT, padx=(0, 10), ipadx=12)
        tk.Label(rpg_card, text="RPG", font=F_STAT, fg=C_GREEN, bg=C_BG).pack(padx=8, pady=(8, 0))
        tk.Label(rpg_card, text="Gamemode", font=F_STAT_LBL, fg=C_TEXT_DIM, bg=C_BG).pack(padx=8, pady=(0, 6))

        # Connect button on hero right
        hero_right = tk.Frame(hero_inner, bg=C_BG_CARD)
        hero_right.pack(side=tk.RIGHT, padx=(20, 0))

        play_btn_frame = tk.Frame(hero_right, bg=C_ACCENT, cursor="hand2",
                                   highlightbackground=C_NEON, highlightthickness=2)
        play_btn_frame.pack(pady=(10, 4), ipadx=24, ipady=8)

        play_inner = tk.Frame(play_btn_frame, bg=C_ACCENT)
        play_inner.pack(padx=16, pady=6)
        tk.Label(play_inner, text="  PRIKLJUCI SE", font=F_PLAY, fg=C_TEXT, bg=C_ACCENT).pack(side=tk.LEFT)

        play_btn_frame.bind("<Button-1>", lambda e: self._play())
        play_btn_frame.bind("<Enter>", lambda e: play_btn_frame.configure(bg=C_ACCENT_HOVER))
        play_btn_frame.bind("<Leave>", lambda e: play_btn_frame.configure(bg=C_ACCENT))
        for w in play_btn_frame.winfo_children():
            w.bind("<Button-1>", lambda e: self._play())
            if isinstance(w, tk.Label):
                w.bind("<Enter>", lambda e, f=play_btn_frame, l=w: (f.configure(bg=C_ACCENT_HOVER), l.configure(bg=C_ACCENT_HOVER)))
                w.bind("<Leave>", lambda e, f=play_btn_frame, l=w: (f.configure(bg=C_ACCENT), l.configure(bg=C_ACCENT)))

        tk.Label(hero_right, text="SA-MP 0.3.7-R5", font=F_TINY,
                 fg=C_NEON_DIM, bg=C_BG_CARD).pack()

        # News section
        news_section = tk.Frame(page, bg=C_BG)
        news_section.pack(fill=tk.BOTH, expand=True)

        self._section_header(news_section, "NOVOSTI")

        news_grid = tk.Frame(news_section, bg=C_BG)
        news_grid.pack(fill=tk.BOTH, expand=True)

        news = [
            ("OBAVIJEST", C_ACCENT, "Dobrodosli na Unicate Gaming!",
             "Server je otvoren! Pridruzi se nasoj zajednici i uzivaj u RP iskustvu."),
            ("UPDATE", C_PURPLE, "CEF Sistemi Aktivni",
             "Tablet, inventar i laptop sistemi sada su dostupni na serveru."),
            ("EVENT", C_GREEN, "Happy Hours - Dupli Respekti",
             "Svaki dan od 18-22h dobijas duplo respekata za brzi napredak."),
        ]

        for i, (tag, tag_color, title, body) in enumerate(news):
            card = tk.Frame(news_grid, bg=C_BG_CARD, highlightbackground=C_BORDER, highlightthickness=1,
                           cursor="hand2")
            card.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(0, 6 if i < 2 else 0), pady=4)

            # Tag
            tag_f = tk.Frame(card, bg=C_BG_CARD)
            tag_f.pack(fill=tk.X, padx=12, pady=(10, 0))
            tk.Label(tag_f, text=tag, font=F_TAG, fg=C_TEXT, bg=tag_color,
                     padx=6, pady=2).pack(side=tk.LEFT)

            # Title
            tk.Label(card, text=title, font=F_NEWS_TITLE, fg=C_TEXT,
                     bg=C_BG_CARD, anchor=tk.W, wraplength=220).pack(fill=tk.X, padx=12, pady=(6, 2))

            # Body
            tk.Label(card, text=body, font=F_NEWS_BODY, fg=C_TEXT_DIM,
                     bg=C_BG_CARD, anchor=tk.W, wraplength=220, justify=tk.LEFT).pack(fill=tk.X, padx=12)

            # Date
            tk.Label(card, text="Jun 2026", font=F_TINY, fg=C_TEXT_DARK,
                     bg=C_BG_CARD).pack(anchor=tk.W, padx=12, pady=(6, 10))

            # Hover effect
            card.bind("<Enter>", lambda e, c=card: c.configure(highlightbackground=C_ACCENT))
            card.bind("<Leave>", lambda e, c=card: c.configure(highlightbackground=C_BORDER))

    # ---------- SERVER PAGE ----------
    def _build_server_page(self):
        page = tk.Frame(self._center_frame, bg=C_BG)
        self._pages["server"] = page

        self._section_header(page, "SERVER INFO")

        grid = tk.Frame(page, bg=C_BG)
        grid.pack(fill=tk.BOTH, expand=True, pady=4)

        info_items = [
            ("IP Adresa",    f"{SERVER_IP}:{SERVER_PORT}", C_NEON),
            ("Igraca Online", "0 / 1000",                  C_ACCENT),
            ("Gamemode",      "RPG",                       C_GREEN),
            ("Mapa",          "San Andreas",               C_TEXT),
            ("Host",          "Dedicated Server",          C_TEXT_LIGHT),
            ("Verzija",       "SA-MP 0.3.7-R5",           C_TEXT_LIGHT),
            ("Ping",          "--",                        C_NEON),
            ("Status",        "Offline",                   C_RED),
        ]

        for i, (label, value, color) in enumerate(info_items):
            row, col = divmod(i, 2)
            card = tk.Frame(grid, bg=C_BG_CARD, highlightbackground=C_BORDER, highlightthickness=1)
            card.grid(row=row, column=col, padx=4, pady=4, sticky="nsew")

            tk.Label(card, text=label, font=F_SMALL, fg=C_TEXT_DIM, bg=C_BG_CARD).pack(
                anchor=tk.W, padx=14, pady=(10, 2))
            tk.Label(card, text=value, font=F_H3, fg=color, bg=C_BG_CARD).pack(
                anchor=tk.W, padx=14, pady=(0, 10))

        grid.columnconfigure(0, weight=1)
        grid.columnconfigure(1, weight=1)

    # ---------- NEWS PAGE ----------
    def _build_news_page(self):
        page = tk.Frame(self._center_frame, bg=C_BG)
        self._pages["news"] = page

        self._section_header(page, "NOVOSTI")

        news = [
            ("OBAVIJEST", C_ACCENT, "Dobrodosli na Unicate Gaming!",
             "Server je otvoren! Pridruzi se nasoj zajednici i uzivaj u RP iskustvu. Novi sistemi, nove mogucnosti i novi pocetak te cekaju. Registriraj se odmah i postani dio nase price."),
            ("UPDATE", C_PURPLE, "Update v1.0 - CEF Sistemi",
             "CEF tablet, inventar i laptop su sada dostupni. Server sa ugradjenim CEF-om za najbolje iskustvo. Svi novi sistemi su testirani i spremni za koristenje."),
            ("EVENT", C_GREEN, "Happy Hours - Dupli Respekti",
             "Svaki dan od 18-22h dobijas duplo respekata! Iskoristi priliku za brzi napredak na serveru. Event vazi za sve igrace bez izuzetka."),
        ]

        for tag, tag_color, title, body in news:
            card = tk.Frame(page, bg=C_BG_CARD, highlightbackground=C_BORDER, highlightthickness=1)
            card.pack(fill=tk.X, pady=4)

            inner = tk.Frame(card, bg=C_BG_CARD)
            inner.pack(fill=tk.X, padx=16, pady=12)

            tag_f = tk.Frame(inner, bg=C_BG_CARD)
            tag_f.pack(anchor=tk.W)
            tk.Label(tag_f, text=tag, font=F_TAG, fg=C_TEXT, bg=tag_color,
                     padx=6, pady=2).pack(side=tk.LEFT)

            tk.Label(inner, text=title, font=F_H3, fg=C_TEXT,
                     bg=C_BG_CARD, anchor=tk.W, wraplength=500).pack(anchor=tk.W, pady=(6, 2))
            tk.Label(inner, text=body, font=F_BODY, fg=C_TEXT_DIM,
                     bg=C_BG_CARD, anchor=tk.W, wraplength=500, justify=tk.LEFT).pack(anchor=tk.W)
            tk.Label(inner, text="Jun 2026", font=F_TINY, fg=C_TEXT_DARK,
                     bg=C_BG_CARD).pack(anchor=tk.W, pady=(4, 0))

    # ---------- SETTINGS PAGE ----------
    def _build_settings_page(self):
        page = tk.Frame(self._center_frame, bg=C_BG)
        self._pages["settings"] = page

        self._section_header(page, "PODESAVANJA")

        # Server mode
        mode_card = tk.Frame(page, bg=C_BG_CARD, highlightbackground=C_BORDER, highlightthickness=1)
        mode_card.pack(fill=tk.X, pady=4)

        tk.Label(mode_card, text="SERVER MODE", font=F_SECTION, fg=C_ACCENT, bg=C_BG_CARD).pack(
            anchor=tk.W, padx=14, pady=(10, 6))

        mode_row = tk.Frame(mode_card, bg=C_BG_CARD)
        mode_row.pack(fill=tk.X, padx=14, pady=(0, 10))

        self._mode_btns = {}
        for key, label, ip in [("prod", "PRODUKCIJA", SERVER_IP), ("local", "LOKALNI TEST", "127.0.0.1")]:
            is_prod = key == "prod"
            f = tk.Frame(mode_row, bg=C_BG_ACTIVE if is_prod else C_BG_INPUT,
                         highlightbackground=C_NEON if is_prod else C_BORDER, highlightthickness=2,
                         cursor="hand2")
            f.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(0, 6), ipady=6)

            tk.Label(f, text=label, font=F_SECTION, fg=C_NEON if is_prod else C_TEXT_DIM,
                     bg=C_BG_ACTIVE if is_prod else C_BG_INPUT).pack(pady=(8, 0))
            tk.Label(f, text=ip, font=("Consolas", 9), fg=C_ACCENT if is_prod else C_TEXT_DARK,
                     bg=C_BG_ACTIVE if is_prod else C_BG_INPUT).pack(pady=(2, 8))

            self._mode_btns[key] = f
            f.bind("<Button-1>", lambda e, k=key: self._set_mode(k))
            for w in f.winfo_children():
                w.bind("<Button-1>", lambda e, k=key: self._set_mode(k))

        # GTA Path
        path_card = tk.Frame(page, bg=C_BG_CARD, highlightbackground=C_BORDER, highlightthickness=1)
        path_card.pack(fill=tk.X, pady=4)

        tk.Label(path_card, text="GTA:SA PUTANJA", font=F_SECTION, fg=C_ACCENT, bg=C_BG_CARD).pack(
            anchor=tk.W, padx=14, pady=(10, 4))

        path_row = tk.Frame(path_card, bg=C_BG_CARD)
        path_row.pack(fill=tk.X, padx=14, pady=(0, 10))

        path_entry = tk.Entry(path_row, textvariable=self.gta_path, font=F_INPUT,
                              fg=C_TEXT, bg=C_BG_INPUT, insertbackground=C_NEON,
                              relief=tk.FLAT, bd=6)
        path_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, ipady=3)

        browse = tk.Label(path_row, text="BROWSE", font=F_TAG, fg=C_NEON,
                          bg=C_BG_INPUT, cursor="hand2", padx=10, pady=6)
        browse.pack(side=tk.RIGHT, padx=(6, 0))
        browse.bind("<Button-1>", lambda e: self._browse_gta())
        browse.bind("<Enter>", lambda e: browse.configure(bg=C_BG_HOVER))
        browse.bind("<Leave>", lambda e: browse.configure(bg=C_BG_INPUT))

        # Nickname
        nick_card = tk.Frame(page, bg=C_BG_CARD, highlightbackground=C_BORDER, highlightthickness=1)
        nick_card.pack(fill=tk.X, pady=4)

        tk.Label(nick_card, text="NICKNAME", font=F_SECTION, fg=C_ACCENT, bg=C_BG_CARD).pack(
            anchor=tk.W, padx=14, pady=(10, 4))

        nick_entry = tk.Entry(nick_card, textvariable=self.nickname, font=F_INPUT_NICK,
                              fg=C_TEXT, bg=C_BG_INPUT, insertbackground=C_NEON,
                              relief=tk.FLAT, bd=6)
        nick_entry.pack(fill=tk.X, padx=14, pady=(0, 10), ipady=4)

        # Components
        comp_card = tk.Frame(page, bg=C_BG_CARD, highlightbackground=C_BORDER, highlightthickness=1)
        comp_card.pack(fill=tk.X, pady=4)

        tk.Label(comp_card, text="KOMPONENTE", font=F_SECTION, fg=C_ACCENT, bg=C_BG_CARD).pack(
            anchor=tk.W, padx=14, pady=(10, 4))

        comp_row_frame = tk.Frame(comp_card, bg=C_BG_CARD)
        comp_row_frame.pack(fill=tk.X, padx=14, pady=(0, 6))

        for name, var in [("SA-MP Client", self.c_samp), ("ASI Loader", self.c_asi),
                          ("CEF Plugin", self.c_cef), ("Chromium RT", self.c_chrome)]:
            row = tk.Frame(comp_row_frame, bg=C_BG_CARD)
            row.pack(fill=tk.X, pady=1)
            tk.Label(row, text=name, font=F_COMP, fg=C_TEXT_LIGHT, bg=C_BG_CARD).pack(side=tk.LEFT)
            tk.Label(row, textvariable=var, font=F_COMP, fg=C_GREEN, bg=C_BG_CARD).pack(side=tk.RIGHT)

        # Install button
        install_row = tk.Frame(comp_card, bg=C_BG_CARD)
        install_row.pack(fill=tk.X, padx=14, pady=(4, 10))

        install_btn = tk.Label(install_row, text="  AUTO-INSTALACIJA KOMPONENTI",
                               font=F_SMALL, fg=C_ACCENT, bg=C_BG_INPUT, cursor="hand2",
                               padx=8, pady=6)
        install_btn.pack(fill=tk.X)
        install_btn.bind("<Button-1>", lambda e: self._force_install())
        install_btn.bind("<Enter>", lambda e: install_btn.configure(bg=C_ACCENT, fg=C_TEXT))
        install_btn.bind("<Leave>", lambda e: install_btn.configure(bg=C_BG_INPUT, fg=C_ACCENT))

    # ---------- RIGHT PANEL ----------
    def _build_right_panel(self, parent):
        rp = tk.Frame(parent, bg=C_BG_CARD_ALT, width=270)
        rp.pack(side=tk.RIGHT, fill=tk.Y, padx=(4, 6), pady=4)
        rp.pack_propagate(False)

        # Connection header
        tk.Label(rp, text="KONEKCIJA", font=F_SECTION, fg=C_ACCENT,
                 bg=C_BG_CARD_ALT).pack(anchor=tk.W, padx=14, pady=(14, 6))
        self._sep(rp, padx=14, pady=(0, 6))

        # IP
        tk.Label(rp, text="IP Adresa", font=F_TINY, fg=C_TEXT_DIM,
                 bg=C_BG_CARD_ALT).pack(anchor=tk.W, padx=14)
        ip_e = tk.Entry(rp, textvariable=self.server_ip, font=F_INPUT,
                        fg=C_NEON, bg=C_BG_INPUT, insertbackground=C_NEON,
                        relief=tk.FLAT, bd=5)
        ip_e.pack(fill=tk.X, padx=14, pady=(2, 6), ipady=3)

        # GTA Path
        tk.Label(rp, text="GTA:SA Putanja", font=F_TINY, fg=C_TEXT_DIM,
                 bg=C_BG_CARD_ALT).pack(anchor=tk.W, padx=14)
        path_row = tk.Frame(rp, bg=C_BG_CARD_ALT)
        path_row.pack(fill=tk.X, padx=14, pady=(2, 6))

        path_e = tk.Entry(path_row, textvariable=self.gta_path, font=F_INPUT,
                          fg=C_TEXT_LIGHT, bg=C_BG_INPUT, insertbackground=C_NEON,
                          relief=tk.FLAT, bd=5)
        path_e.pack(side=tk.LEFT, fill=tk.X, expand=True, ipady=3)

        bb = tk.Label(path_row, text="...", font=F_BODY, fg=C_TEXT, bg=C_ACCENT,
                      cursor="hand2", padx=6)
        bb.pack(side=tk.RIGHT, padx=(4, 0), ipady=3)
        bb.bind("<Button-1>", lambda e: self._browse_gta())
        bb.bind("<Enter>", lambda e: bb.configure(bg=C_ACCENT_HOVER))
        bb.bind("<Leave>", lambda e: bb.configure(bg=C_ACCENT))

        # Nickname
        tk.Label(rp, text="Nickname", font=F_TINY, fg=C_TEXT_DIM,
                 bg=C_BG_CARD_ALT).pack(anchor=tk.W, padx=14)
        nick_e = tk.Entry(rp, textvariable=self.nickname, font=F_INPUT_NICK,
                          fg=C_TEXT, bg=C_BG_INPUT, insertbackground=C_NEON,
                          relief=tk.FLAT, bd=5)
        nick_e.pack(fill=tk.X, padx=14, pady=(2, 8), ipady=3)

        # PLAY button
        play_f = tk.Frame(rp, bg=C_ACCENT, cursor="hand2",
                          highlightbackground=C_NEON, highlightthickness=2)
        play_f.pack(fill=tk.X, padx=14, pady=(4, 4), ipady=6)

        play_lbl = tk.Label(play_f, text="  KONEKTUJ SE", font=F_PLAY, fg=C_TEXT, bg=C_ACCENT)
        play_lbl.pack(pady=4)

        play_f.bind("<Button-1>", lambda e: self._play())
        play_f.bind("<Enter>", lambda e: (play_f.configure(bg=C_ACCENT_HOVER), play_lbl.configure(bg=C_ACCENT_HOVER)))
        play_f.bind("<Leave>", lambda e: (play_f.configure(bg=C_ACCENT), play_lbl.configure(bg=C_ACCENT)))
        play_lbl.bind("<Button-1>", lambda e: self._play())
        play_lbl.bind("<Enter>", lambda e: (play_f.configure(bg=C_ACCENT_HOVER), play_lbl.configure(bg=C_ACCENT_HOVER)))
        play_lbl.bind("<Leave>", lambda e: (play_f.configure(bg=C_ACCENT), play_lbl.configure(bg=C_ACCENT)))

        # Status text
        self._status_lbl = tk.Label(rp, textvariable=self.status_var, font=F_SMALL,
                                     fg=C_TEXT_DIM, bg=C_BG_CARD_ALT)
        self._status_lbl.pack(pady=(0, 6))

        self._sep(rp, padx=14, pady=4)

        # Components
        tk.Label(rp, text="KOMPONENTE", font=F_SECTION, fg=C_ACCENT,
                 bg=C_BG_CARD_ALT).pack(anchor=tk.W, padx=14, pady=(4, 4))

        for name, var in [("SA-MP Client", self.c_samp), ("ASI Loader", self.c_asi),
                          ("CEF Plugin", self.c_cef), ("Chromium RT", self.c_chrome)]:
            row = tk.Frame(rp, bg=C_BG_CARD_ALT)
            row.pack(fill=tk.X, padx=14, pady=1)
            tk.Label(row, text=name, font=F_COMP, fg=C_TEXT_DIM, bg=C_BG_CARD_ALT).pack(side=tk.LEFT)
            tk.Label(row, textvariable=var, font=F_COMP, fg=C_GREEN, bg=C_BG_CARD_ALT).pack(side=tk.RIGHT)

        self._sep(rp, padx=14, pady=6)

        # Quick actions
        tk.Label(rp, text="BRZE AKCIJE", font=F_SECTION, fg=C_ACCENT,
                 bg=C_BG_CARD_ALT).pack(anchor=tk.W, padx=14, pady=(4, 4))

        act_frame = tk.Frame(rp, bg=C_BG_CARD_ALT)
        act_frame.pack(fill=tk.X, padx=14)

        for i, (name, key) in enumerate([("Ocisti Chat", "F9"), ("Reconnect", "F5"), ("Screenshot", "F8")]):
            row = tk.Frame(act_frame, bg=C_BG_CARD_ALT)
            row.pack(fill=tk.X, pady=1)
            tk.Label(row, text=name, font=F_SMALL, fg=C_TEXT_LIGHT, bg=C_BG_CARD_ALT).pack(side=tk.LEFT)
            tk.Label(row, text=key, font=F_TINY, fg=C_TEXT_DIM, bg=C_BG_INPUT, padx=4).pack(side=tk.RIGHT)

        # Radio at bottom
        radio_frame = tk.Frame(rp, bg=C_BG_CARD_ALT)
        radio_frame.pack(side=tk.BOTTOM, fill=tk.X, padx=14, pady=10)

        radio_row = tk.Frame(radio_frame, bg=C_BG_INPUT, cursor="hand2")
        radio_row.pack(fill=tk.X)
        tk.Label(radio_row, text=" Unicate Radio", font=F_SMALL, fg=C_ORANGE,
                 bg=C_BG_INPUT, pady=6, padx=8).pack(anchor=tk.W)
        radio_row.bind("<Enter>", lambda e: radio_row.configure(bg=C_BG_HOVER))
        radio_row.bind("<Leave>", lambda e: radio_row.configure(bg=C_BG_INPUT))

        # Repair button
        repair_btn = tk.Label(rp, text="POPRAVI / REINSTALUJ", font=F_TINY,
                              fg=C_TEXT_DARK, bg=C_BG_INPUT, cursor="hand2", pady=5)
        repair_btn.pack(side=tk.BOTTOM, fill=tk.X, padx=14, pady=(4, 6))
        repair_btn.bind("<Button-1>", lambda e: self._repair())
        repair_btn.bind("<Enter>", lambda e: repair_btn.configure(fg=C_TEXT_DIM, bg=C_BG_HOVER))
        repair_btn.bind("<Leave>", lambda e: repair_btn.configure(fg=C_TEXT_DARK, bg=C_BG_INPUT))

    # ============================================================
    # NAVIGATION
    # ============================================================
    def _set_page(self, key):
        self._page = key
        # Update nav buttons
        for k, (f, lbl, bar) in self._nav_btns.items():
            if k == key:
                lbl.configure(fg=C_NEON)
                if bar:
                    bar.configure(bg=C_NEON)
                else:
                    bar_new = tk.Frame(f, bg=C_NEON, width=3)
                    bar_new.place(x=0, rely=0.1, relheight=0.8)
                    self._nav_btns[k] = (f, lbl, bar_new)
            else:
                lbl.configure(fg=C_TEXT_DIM)
                if bar:
                    bar.destroy()
                    self._nav_btns[k] = (f, lbl, None)

        # Update tab bar
        for k, (f, lbl, ind) in self._tab_btns.items():
            if k == key:
                lbl.configure(fg=C_NEON)
                ind.configure(bg=C_NEON)
            else:
                lbl.configure(fg=C_TEXT_DIM)
                ind.configure(bg=C_BG)

        # Show page
        for k, page in self._pages.items():
            if k == key:
                page.pack(fill=tk.BOTH, expand=True)
            else:
                page.pack_forget()

    def _nav_hover(self, key, entering):
        f, lbl, bar = self._nav_btns[key]
        if entering:
            lbl.configure(fg=C_TEXT_LIGHT)
        else:
            if key == self._page:
                lbl.configure(fg=C_NEON)
            else:
                lbl.configure(fg=C_TEXT_DIM)

    def _set_mode(self, key):
        for k, f in self._mode_btns.items():
            if k == key:
                f.configure(bg=C_BG_ACTIVE, highlightbackground=C_NEON)
                for w in f.winfo_children():
                    if isinstance(w, tk.Label):
                        if w.cget("font") == F_SECTION:
                            w.configure(fg=C_NEON)
                        else:
                            w.configure(fg=C_ACCENT)
            else:
                f.configure(bg=C_BG_INPUT, highlightbackground=C_BORDER)
                for w in f.winfo_children():
                    if isinstance(w, tk.Label):
                        if w.cget("font") == F_SECTION:
                            w.configure(fg=C_TEXT_DIM)
                        else:
                            w.configure(fg=C_TEXT_DARK)

        if key == "prod":
            self.server_ip.set(f"{SERVER_IP}:{SERVER_PORT}")
        else:
            self.server_ip.set("127.0.0.1:7777")

    # ============================================================
    # COMPONENTS & AUTO-INSTALL
    # ============================================================
    def _auto_install_and_check(self):
        """Auto-install all missing components silently, then check status."""
        gta = self.gta_path.get()
        if not gta or not os.path.exists(os.path.join(gta, "gta_sa.exe")):
            self.c_samp.set("Nije pronadjen")
            self.c_asi.set("Nije pronadjen")
            self.c_cef.set("Nije pronadjen")
            self.c_chrome.set("Nije pronadjen")
            self.status_var.set("Postavi GTA SA lokaciju!")
            return

        self.status_var.set("Provjera i instalacija...")
        self.root.update()

        # Auto-install missing components
        self._install_missing_silent(gta)
        self._check_components(gta)

    def _install_missing_silent(self, gta):
        """Silently install any missing component from bundled files."""
        launcher_dir = self._launcher_dir()

        # 1. SA-MP Client (samp.dll)
        if not os.path.exists(os.path.join(gta, "samp.dll")):
            bundled_dll = os.path.join(launcher_dir, "ug_samp.dll")
            if os.path.exists(bundled_dll):
                try:
                    shutil.copy2(bundled_dll, os.path.join(gta, "samp.dll"))
                except:
                    pass
            # Also try original samp.dll backup
            backup = os.path.join(gta, "samp.dll.ug_backup")
            if not os.path.exists(os.path.join(gta, "samp.dll")) and os.path.exists(backup):
                try:
                    shutil.copy2(backup, os.path.join(gta, "samp.dll"))
                except:
                    pass

        # 2. ASI Loader
        has_asi = (os.path.exists(os.path.join(gta, "vorbisFile.dll")) or
                   os.path.exists(os.path.join(gta, "vorbisHooked.dll")))
        if not has_asi:
            asi_dir = os.path.join(launcher_dir, "asi_files")
            if os.path.exists(asi_dir):
                try:
                    for f in os.listdir(asi_dir):
                        src = os.path.join(asi_dir, f)
                        if os.path.isfile(src):
                            shutil.copy2(src, os.path.join(gta, f))
                except:
                    pass

        # 3. CEF Plugin (samp_cef.dll + cef/ folder)
        has_cef_dll = os.path.exists(os.path.join(gta, "samp_cef.dll"))
        has_cef_dir = os.path.exists(os.path.join(gta, "cef"))

        if not has_cef_dll:
            src_dll = os.path.join(launcher_dir, "cef_files", "samp_cef.dll")
            if os.path.exists(src_dll):
                try:
                    shutil.copy2(src_dll, os.path.join(gta, "samp_cef.dll"))
                except:
                    pass

        if not has_cef_dir or not os.path.exists(os.path.join(gta, "cef", "libcef.dll")):
            src_cef = os.path.join(launcher_dir, "cef_files", "cef")
            if os.path.exists(src_cef):
                try:
                    dest = os.path.join(gta, "cef")
                    if os.path.exists(dest):
                        shutil.rmtree(dest)
                    shutil.copytree(src_cef, dest)
                except:
                    pass

        # 4. CEF content (scriptfiles/cef/ for the server-side CEF pages)
        cef_content_src = os.path.join(launcher_dir, "cef_content")
        if os.path.exists(cef_content_src):
            # These go to GTA/cef/ subfolders - for server-side they should be in scriptfiles
            pass

    def _check_components(self, gta=None):
        if not gta:
            gta = self.gta_path.get()

        if not gta or not os.path.exists(os.path.join(gta, "gta_sa.exe")):
            self.c_samp.set("Nije pronadjen")
            self.c_asi.set("Nije pronadjen")
            self.c_cef.set("Nije pronadjen")
            self.c_chrome.set("Nije pronadjen")
            self.status_var.set("Postavi GTA SA lokaciju!")
            return

        # SAMP Client
        if os.path.exists(os.path.join(gta, "samp.dll")):
            self.c_samp.set("Instaliran")
        else:
            self.c_samp.set("Nedostaje")

        # ASI Loader
        if os.path.exists(os.path.join(gta, "vorbisFile.dll")) or os.path.exists(os.path.join(gta, "vorbisHooked.dll")):
            self.c_asi.set("Instaliran")
        else:
            self.c_asi.set("Nedostaje")

        # CEF Plugin
        if os.path.exists(os.path.join(gta, "samp_cef.dll")):
            self.c_cef.set("Instaliran")
        else:
            self.c_cef.set("Nedostaje")

        # Chromium Runtime
        if os.path.exists(os.path.join(gta, "cef", "libcef.dll")):
            self.c_chrome.set("Instaliran")
        else:
            self.c_chrome.set("Nedostaje")

        # Overall status
        all_ok = all(v.get() == "Instaliran" for v in [self.c_samp, self.c_asi, self.c_cef, self.c_chrome])
        if all_ok:
            self.status_var.set("Sve spremno - klikni KONEKTUJ SE!")
        else:
            missing = []
            if self.c_samp.get() != "Instaliran": missing.append("SAMP")
            if self.c_asi.get() != "Instaliran": missing.append("ASI")
            if self.c_cef.get() != "Instaliran": missing.append("CEF")
            if self.c_chrome.get() != "Instaliran": missing.append("Chromium")
            self.status_var.set(f"Nedostaje: {', '.join(missing)}")

    # ============================================================
    # FILE OPERATIONS
    # ============================================================
    def _browse_gta(self):
        path = filedialog.askdirectory(title="Izaberi GTA San Andreas folder")
        if path:
            self.gta_path.set(path)
            self._save_cfg()
            self._auto_install_and_check()

    def _force_install(self):
        """Force reinstall all components from bundled files."""
        gta = self.gta_path.get()
        if not gta or not os.path.exists(os.path.join(gta, "gta_sa.exe")):
            messagebox.showerror("Greska", "Prvo postavi GTA SA lokaciju!")
            return

        self.status_var.set("Instalacija u toku...")
        self.root.update()

        launcher_dir = self._launcher_dir()

        # Install UG samp.dll
        ug_dll = os.path.join(launcher_dir, "ug_samp.dll")
        if os.path.exists(ug_dll):
            try:
                original = os.path.join(gta, "samp.dll")
                backup = os.path.join(gta, "samp.dll.ug_backup")
                if os.path.exists(original) and not os.path.exists(backup):
                    shutil.copy2(original, backup)
                shutil.copy2(ug_dll, original)
            except Exception as e:
                messagebox.showerror("Greska", f"Greska pri instalaciji samp.dll:\n{e}")
                return

        # Install CEF
        cef_dir = os.path.join(launcher_dir, "cef_files")
        if os.path.exists(cef_dir):
            try:
                src_dll = os.path.join(cef_dir, "samp_cef.dll")
                if os.path.exists(src_dll):
                    shutil.copy2(src_dll, os.path.join(gta, "samp_cef.dll"))

                src_cef = os.path.join(cef_dir, "cef")
                if os.path.exists(src_cef):
                    dest = os.path.join(gta, "cef")
                    if os.path.exists(dest):
                        shutil.rmtree(dest)
                    shutil.copytree(src_cef, dest)
            except Exception as e:
                messagebox.showerror("Greska", f"Greska pri instalaciji CEF-a:\n{e}")
                return

        # Install ASI loader
        asi_dir = os.path.join(launcher_dir, "asi_files")
        if os.path.exists(asi_dir):
            try:
                for f in os.listdir(asi_dir):
                    src = os.path.join(asi_dir, f)
                    if os.path.isfile(src):
                        shutil.copy2(src, os.path.join(gta, f))
            except Exception as e:
                messagebox.showerror("Greska", f"Greska pri instalaciji ASI-a:\n{e}")
                return

        self._check_components(gta)
        self.status_var.set("Sve komponente instalirane!")
        messagebox.showinfo("Uspjeh", "Sve komponente su uspjesno instalirane!")

    def _repair(self):
        if messagebox.askyesno("Popravka",
            "Ovo ce reinstalirati:\n"
            "- CEF plugin\n"
            "- UG samp.dll\n"
            "- ASI Loader\n\n"
            "Nastavi?"):
            self._force_install()

    # ============================================================
    # PLAY
    # ============================================================
    def _play(self):
        gta = self.gta_path.get()
        if not gta or not os.path.exists(os.path.join(gta, "gta_sa.exe")):
            messagebox.showerror("Greska", "GTA San Andreas lokacija nije ispravna!\nKlikni '...' da pronadjes folder.")
            return

        nick = self.nickname.get().strip()
        if not nick:
            messagebox.showerror("Greska", "Unesi nadimak (nickname)!")
            return

        self._save_cfg()

        # Auto-install if anything is missing
        self._install_missing_silent(gta)
        self._check_components(gta)

        # Warn about missing components
        missing = []
        if self.c_cef.get() != "Instaliran": missing.append("CEF Plugin")
        if self.c_asi.get() != "Instaliran": missing.append("ASI Loader")
        if self.c_samp.get() != "Instaliran": missing.append("SA-MP Client")
        if self.c_chrome.get() != "Instaliran": missing.append("Chromium RT")

        if missing:
            if not messagebox.askyesno("Upozorenje",
                f"Nedostaju komponente: {', '.join(missing)}\n\n"
                "Neki featurei mozda necete raditi.\n"
                "Nastavi ipak?"):
                return

        # Setup SAMP config
        self._setup_samp_cfg(gta, nick)

        # Launch
        self.status_var.set("Pokretanje igre...")
        self.root.update()

        try:
            ip_raw = self.server_ip.get().strip()
            if ":" in ip_raw:
                ip, port = ip_raw.split(":", 1)
            else:
                ip = ip_raw
                port = SERVER_PORT

            samp_exe = os.path.join(gta, "samp.exe")
            if os.path.exists(samp_exe):
                subprocess.Popen([samp_exe, ip, port], cwd=gta)
            else:
                gta_exe = os.path.join(gta, "gta_sa.exe")
                subprocess.Popen([gta_exe, f"-c -h {ip} -p {port} -n {nick}"], cwd=gta)

            self.status_var.set("Igra pokrenuta! Uzivaj!")
            self.root.iconify()

        except Exception as e:
            messagebox.showerror("Greska", f"Greska pri pokretanju:\n{e}")
            self.status_var.set("Greska pri pokretanju")

    def _setup_samp_cfg(self, gta_path, nick):
        cfg_dir = os.path.join(gta_path, "SAMP")
        os.makedirs(cfg_dir, exist_ok=True)
        cfg_file = os.path.join(cfg_dir, "sa-mp.cfg")
        try:
            with open(cfg_file, 'w') as f:
                f.write(f"nametag {nick}\n")
                f.write(f"pagesize 50\n")
                f.write(f"fpslimit 90\n")
        except:
            pass

    # ============================================================
    # HELPERS
    # ============================================================
    def _sep(self, parent, padx=0, pady=0):
        tk.Frame(parent, bg=C_SEP, height=1).pack(fill=tk.X, padx=padx, pady=pady)

    def _section_header(self, parent, text):
        f = tk.Frame(parent, bg=C_BG)
        f.pack(fill=tk.X, pady=(4, 8))

        tk.Label(f, text=text, font=F_H2, fg=C_NEON, bg=C_BG).pack(side=tk.LEFT)
        # Line
        line = tk.Frame(f, bg=C_BORDER, height=1)
        line.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(12, 0), pady=8)

    # ============================================================
    # QUIT
    # ============================================================
    def _quit(self):
        self._save_cfg()
        self.root.quit()
        self.root.destroy()

    def run(self):
        self.root.mainloop()


if __name__ == "__main__":
    app = UnicateLauncher()
    app.run()
